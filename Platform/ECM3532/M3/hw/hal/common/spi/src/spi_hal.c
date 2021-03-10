/*******************************************************************************
*
* @file spi_hal.c
*
* Copyright (C) 2019 Eta Compute, Inc
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************************/
#include "config.h"
#include <stddef.h>
#include "eta_csp_socctrl.h"
#include "eta_csp_spi.h"
#include "spi_hal.h"
#include "errno.h"
#ifdef CONFIG_ECM3531
#include "ecm3531.h"
#elif CONFIG_ECM3532
#include "ecm3532.h"
#endif
#include "cm3.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "print_util.h"

#define ECM3531_SPI_OP_NONE         0
#define ECM3531_SPI_OP_BLOCKING     1
#define ECM3531_SPI_OP_NONBLOCKING  2
#define SPI_FIFO_DEPTH 16
#define SPI_TX_INTR (BM_SPI_INT_EN_TX_DATA_EN0 |\
                        BM_SPI_INT_EN_TXFIFO_HAS_LEVEL_SPACE_EN0 |\
                        BM_SPI_INT_EN_XMIT_COMPLETE_EN0)

#define SPI_SRC_CLK_IN_KHZ (8000)

/* SPI State*/
typedef enum {
    /* Idle state */
    HalSpiCmdDone= 0,
    /* Tx in Progress */
    HalSpiInTx,
    /* Rx in Progress */
    HalSpiInRx,
    /* TxRx in Progress */
    HalSpiInTxRx,
    /* Wait for bus to go Idle */
    HalSpiWaitForCmdDone,
}tHalSpiState;

/**
 * SPI Bus Driver structure
 * Latch Driver state, Tx and Rx buffer
 * Tx and Rx count
 * Synchronization primitives
 * Callback function
 */
struct sDevSpi {
    uint8_t ui8FifoDepth;
    uint8_t *ui8TxBuf;
    uint8_t *ui8RxBuf;
    uint8_t ui8SpiState;
    uint16_t ui16TxByteCnt;
    uint16_t ui16RxByteCnt;
    uint8_t ui8Spi;
    uint8_t CSLevel;
    tHalSpiXferDoneCb fXferDoneCb;
    void *vSpiCbArg;
    SemaphoreHandle_t SpiLock;
    SemaphoreHandle_t SpiCmdDoneLock;
};

#ifdef CONFIG_SPI_0_MASTER
static struct sDevSpi sDevSpi0 = {0};
#endif
#ifdef CONFIG_SPI_1_MASTER
static struct sDevSpi sDevSpi1 = {0};
#endif

static struct sDevSpi* SpiGetDev(uint8_t ui8Spi)
{
    switch (ui8Spi) {
#ifdef CONFIG_SPI_0_MASTER
    case 0:
        return &sDevSpi0;
#endif
#ifdef CONFIG_SPI_1_MASTER
    case 1:
        return &sDevSpi1;
#endif
    default:
        return NULL;
    }
}

static void EnableSpiIntr(uint8_t ui8Spi, uint32_t  ui32IntrSet)
{
    REG_SPI_INT_EN(ui8Spi).V |= ui32IntrSet;
}

static void DisableSpiIntr(uint8_t ui8Spi, uint32_t ui32IntrSet)
{
    REG_SPI_INT_EN(ui8Spi).V &= (~ui32IntrSet);
}


/** Assert CS Gpio pin of SPI Slave,
 * GPIO pin should have configured to OUTPUT pin
 *
 * @param ui8SpiNum SPI bus number
 * @param ui32CSGpio Gpio pin connected to slave CS pin
 * @param CSLevel CS active level
 *
 */
void HalSpiAssertCS(uint8_t ui8Spi, uint32_t ui32CSGpio, tSpiCSActiveLevel CSLevel)
{
    struct sDevSpi *sDev;
    sDev = SpiGetDev(ui8Spi);
    if (sDev == NULL) {
        ecm35xx_printf("Invalid SPIDEV\r\n");
        configASSERT(0);
    }
    xSemaphoreTake(sDev->SpiLock, portMAX_DELAY);
    sDev->CSLevel = (CSLevel & 1);
    HalGpioWrite(ui32CSGpio, sDev->CSLevel);
}

/** De-Assert CS Gpio pin of SPI Slave,
 * GPIO pin should have configured to OUTPUT pin
 *
 * @param ui8SpiNum SPI bus number
 * @param ui32CSGpio Gpio pin connected to slave CS pin
 *
 */
void HalSpiDeAssertCS(uint8_t ui8Spi, uint32_t ui32CSGpio)
{
    struct sDevSpi *sDev;
    sDev = SpiGetDev(ui8Spi);
    tSpiCSActiveLevel CSLevel;
    if (sDev == NULL) {
        ecm35xx_printf("Invalid SPIDEV\r\n");
        configASSERT(0);
    }
    CSLevel = (sDev->CSLevel + 1 ) & 1;
    HalGpioWrite(ui32CSGpio, CSLevel);
    xSemaphoreGive(sDev->SpiLock);
}

/**
 * Configure the spi.
 * @param ui8Spi The number of the SPI to configure.
 * @param sCfgSpi SPI configuration settings to be set
 *
 * @return int 0 on success, non-zero error code on failure.
 */
int32_t HalSpiSetCfg(uint8_t ui8Spi, tHalSpiCfg *sCfgSpi)
{
    uint8_t ui8ClkDiv;
    struct sDevSpi *sDev;

    sDev = SpiGetDev(ui8Spi);
    if (sDev == NULL) {
        ecm35xx_printf("Invalid SPIDEV\r\n");
        configASSERT(0);
    }

    if (sCfgSpi->ui16BRateKHz >= SPI_SRC_CLK_IN_KHZ)
        return -EINVAL;

    xSemaphoreTake(sDev->SpiLock, portMAX_DELAY);
    ui8ClkDiv = (SPI_SRC_CLK_IN_KHZ / sCfgSpi->ui16BRateKHz) - 1;
    REG_SPI_CLKDIV(ui8Spi).BF.CLKDIV = ui8ClkDiv;

    REG_SPI_CONFIG(ui8Spi).BF.CPOL =
        sCfgSpi->ui8Cpol;
    REG_SPI_CONFIG(ui8Spi).BF.CPHA =
        sCfgSpi->ui8Cpha;
    REG_SPI_CONFIG2(ui8Spi).BF.DISABLE_MISO_METAFLOP =
        sCfgSpi->ui8MetaFlopsDis;
    xSemaphoreGive(sDev->SpiLock);
    return 0;
}

/**
 * Get Spi Configuration
 * @param ui8Spi The number of the SPI to configure.
 * @param sCfgSpi SPI configuration settings to be filled
 *
 * @return int 0 on success, non-zero error code on failure.
 */
int32_t HalSpiGetCfg(uint8_t ui8Spi, tHalSpiCfg *sCfgSpi)
{
    uint8_t ui8ClkDiv;
    struct sDevSpi *sDev;

    sDev = SpiGetDev(ui8Spi);
    if (sDev == NULL) {
        return -EINVAL;
    }

    xSemaphoreTake(sDev->SpiLock, portMAX_DELAY);

    ui8ClkDiv = REG_SPI_CLKDIV(ui8Spi).BF.CLKDIV;
    sCfgSpi->ui16BRateKHz =
        (SPI_SRC_CLK_IN_KHZ / (ui8ClkDiv + 1));

    sCfgSpi->ui8Cpol =
        REG_SPI_CONFIG(ui8Spi).BF.CPOL;
    sCfgSpi->ui8Cpha =
        REG_SPI_CONFIG(ui8Spi).BF.CPHA;
    sCfgSpi->ui8MetaFlopsDis =
        REG_SPI_CONFIG2(ui8Spi).BF.DISABLE_MISO_METAFLOP;
    xSemaphoreGive(sDev->SpiLock);
    return 0;
}

/**
 * SPI Master send and receive to a slave device selected by slave sel
 * send buffer contain data to be send , transfer count specify the send
 * data length in bytes, for receive rx count specify the byte length be
 * receive in receive buffer
 * If called with valid (non zero) callback function, it will be a non blocking
 * call and callback function will be called after completion from interrupt
 * context, callback function must only be used for signal of completion as it
 * is called from interrupt context
 * if callback function is NULL, API will be blocking API and will retun after
 * comletetion of SPI transfer
 *
 * @param ui8Spi     SPI interface to use
 * @param ui8TxBuf      Pointer to buffer where values to transmit are stored.
 * @param ui16TxCnt     Transmit byte count.
 * @param ui8Rxbuf      Pointer to buffer to store values received from slave.
 * @param ui16TxCnt     Receive byte count.
 * @param fXferDoneCb   callback function pointer, for async call.
 * @param vCbArg        Argument for callback funct pointer
 *
 * @return int 0 on success, non-zero error code on failure.
 */
int32_t HalSpiXfer(uint8_t ui8Spi, void *ui8TxBuf,
                    uint16_t ui16TxCnt, void *ui8RxBuf, uint16_t ui16RxCnt,
                    tHalSpiXferDoneCb fSpiCb, void *vCbArg)
{
    struct sDevSpi *sDev;
    int32_t i32Ret = 0;

    sDev = SpiGetDev(ui8Spi);
    if (sDev == NULL) {
        return -EINVAL;
    }

    if (sDev->ui8SpiState != HalSpiCmdDone)
    {
        i32Ret = -EBUSY;
        goto err_exit;
    }
    else if (!ui8TxBuf && ui16TxCnt)
    {
        i32Ret = -EINVAL;
        goto err_exit;
    }

    sDev->ui8SpiState = HalSpiInTx;
    sDev->fXferDoneCb = fSpiCb;
    sDev->vSpiCbArg = vCbArg;
    sDev->ui8TxBuf = ui8TxBuf;
    sDev->ui8RxBuf = ui8RxBuf;
    sDev->ui16TxByteCnt = ui16TxCnt;
    sDev->ui16RxByteCnt = ui16RxCnt;
    if (ui16TxCnt)
    {
        /* drop garbage receive data during TX */
        REG_SPI_RXDROP_CNT(ui8Spi).V = ui16TxCnt;

        /* set fifo level */
        if (ui16TxCnt > sDev->ui8FifoDepth)
            REG_SPI_CONFIG(ui8Spi).BF.FIFO_LEVEL_TRIG =
                            sDev->ui8FifoDepth;
        else
            REG_SPI_CONFIG(ui8Spi).BF.FIFO_LEVEL_TRIG =
                            ui16TxCnt;
    }

    /* Start command */
    REGN_W2(ui8Spi, SPI_STATUS, START_BUSY, 1, HOLD_CS, 1);
    if (ui16TxCnt)
    {
        /* enable TX interrupt */
        EnableSpiIntr(ui8Spi, BM_SPI_INT_EN_TX_DATA_EN0 |
                    BM_SPI_INT_EN_TXFIFO_HAS_LEVEL_SPACE_EN0);
    }
    else if (ui16RxCnt && sDev->ui8RxBuf)
    {
        if (sDev->ui16RxByteCnt > sDev->ui8FifoDepth)
                REG_SPI_CONFIG(ui8Spi).BF.FIFO_LEVEL_TRIG =
                                    sDev->ui8FifoDepth;
                else
                    REG_SPI_CONFIG(ui8Spi).BF.FIFO_LEVEL_TRIG =
                                    sDev->ui16RxByteCnt;

                /* Write number of dummy bytes to get rx data */
                REG_SPI_TX_DUMMY_BYTE_CNT(ui8Spi).V = sDev->ui16RxByteCnt;
                sDev->ui8SpiState = HalSpiInRx;
                /* Enable RX interrupt to get notified of RX */
                EnableSpiIntr(ui8Spi,
                        BM_SPI_INT_EN_RX_DATA_EN0);

                REGN_W2(ui8Spi, SPI_STATUS,
                            START_BUSY, 0, HOLD_CS, 0);
    }

    if(!sDev->fXferDoneCb)
        xSemaphoreTake(sDev->SpiCmdDoneLock, portMAX_DELAY);
err_exit:
    return i32Ret;
}

/**
 * SPI Xfer, send and receive on SPI bus in Full Duplex mode i.e. both TX and RX
 * simultaneously
 * If called with valid callback function, it will be a non blocking
 * call and callback function will be called after completion from interrupt
 * context, callback function must only be used for signal of completion as it
 * is called from interrupt context.In case of non blockng mode, last xfer should
 * be completed before next call other wise BUSY error code will be returned.
 * If callback function is NULL, API will be blocking API and will retun after
 * completion of SPI transfer
 *
 * @param ui8SpiNum     SPI bus number
 * @param ui8TxBuf      Pointer transmit buffer.
 * @param ui16TxCnt     Transmit byte count.
 * @param ui8RxBuf      Received buffer to store data from slave.
 * @param ui16RxCnt     Receive byte count.
 * @param fXferDoneCb   Callback function pointer, for async call.
 * @param vCbArg        Argument with which callback func will be called after
 * completion.
 *
 * @return int 0 on success, non-zero error code on failure.
 */
int32_t HalSpiXferFD(uint8_t ui8Spi, void *ui8TxBuf,
                    uint16_t ui16TxCnt, void *ui8RxBuf, uint16_t ui16RxCnt,
                    tHalSpiXferDoneCb fSpiCb, void *vCbArg)
{
    struct sDevSpi *sDev;
    int32_t i32Ret = 0;

    sDev = SpiGetDev(ui8Spi);
    if (sDev == NULL) {
        return -EINVAL;
    }

    if (sDev->ui8SpiState != HalSpiCmdDone)
    {
        i32Ret = -EBUSY;
        goto err_exit;
    }
    else if (!ui8TxBuf || !ui16TxCnt)
    {
        i32Ret = -EINVAL;
        goto err_exit;
    }
    else if (!ui8RxBuf || !ui16RxCnt)
    {
        i32Ret = -EINVAL;
        goto err_exit;
    }
    else if (ui16TxCnt != ui16RxCnt)
    {
        i32Ret = -EINVAL;
        goto err_exit;
    }

    sDev->ui8SpiState = HalSpiInTxRx;
    sDev->fXferDoneCb = fSpiCb;
    sDev->vSpiCbArg = vCbArg;
    sDev->ui8TxBuf = ui8TxBuf;
    sDev->ui8RxBuf = ui8RxBuf;
    sDev->ui16TxByteCnt = ui16TxCnt;
    sDev->ui16RxByteCnt = ui16RxCnt;

    /* set fifo level */
    if (ui16TxCnt > sDev->ui8FifoDepth)
        REG_SPI_CONFIG(ui8Spi).BF.FIFO_LEVEL_TRIG =
                            sDev->ui8FifoDepth;
    else
        REG_SPI_CONFIG(ui8Spi).BF.FIFO_LEVEL_TRIG =
                            ui16TxCnt;

    /* Start command */
    REGN_W2(ui8Spi, SPI_STATUS, START_BUSY, 1, HOLD_CS,  1);
    /* enable TX interrupt */
    EnableSpiIntr(ui8Spi, BM_SPI_INT_EN_TX_DATA_EN0 |
                    BM_SPI_INT_EN_TXFIFO_HAS_LEVEL_SPACE_EN0);
    if(!sDev->fXferDoneCb)
        xSemaphoreTake(sDev->SpiCmdDoneLock, portMAX_DELAY);
err_exit:
    return i32Ret;
}
/*
 * SPI interrupt handler
 */
static void SpiIntrHandler(uint8_t ui8Spi)
{
    uint32_t ui32IntrSt;
    uint8_t ui8Cnt;

    struct sDevSpi *sDev;
    long lHigherPriorityTaskWoken = pdFALSE;

    sDev = SpiGetDev(ui8Spi);

    ui32IntrSt = REG_SPI_STATUS(ui8Spi).V;

    if(sDev->ui8SpiState == HalSpiInTxRx)
    {
        if(sDev->ui16RxByteCnt && sDev->ui8RxBuf &&
                REG_SPI_STATUS(ui8Spi).BF.RXFIFO_HAS_1DATA)
        {
            do {
                /* pop data from FIFO */
                *sDev->ui8RxBuf =
                    REG_SPI_TXRX_1BYTE_FIFO_DATA(ui8Spi).V;
                sDev->ui8RxBuf++;
                sDev->ui16RxByteCnt--;
            } while((sDev->ui16RxByteCnt ) &&
                    REG_SPI_STATUS(ui8Spi).BF.RXFIFO_HAS_1DATA);
        }

        if (sDev->ui16RxByteCnt) {
            if (sDev->ui16RxByteCnt > sDev->ui8FifoDepth)
                REG_SPI_CONFIG(ui8Spi).BF.FIFO_LEVEL_TRIG =
                    sDev->ui8FifoDepth;
            else
                REG_SPI_CONFIG(ui8Spi).BF.FIFO_LEVEL_TRIG =
                    sDev->ui16RxByteCnt;

            EnableSpiIntr(ui8Spi,
                    BM_SPI_INT_EN_RX_DATA_EN0);
        }
        else
        {
            DisableSpiIntr(ui8Spi, BM_SPI_INT_EN_RX_DATA_EN0);
        }

        if(ui32IntrSt & SPI_TX_INTR)
        {
            DisableSpiIntr(ui8Spi, SPI_TX_INTR);
            if (sDev->ui16TxByteCnt >= sDev->ui8FifoDepth)
                ui8Cnt = sDev->ui8FifoDepth;
            else
                ui8Cnt = sDev->ui16TxByteCnt;

            while (ui8Cnt)
            {
                if (!REG_SPI_STATUS(ui8Spi).BF.TXFIFO_HAS_1SPACE)
                    break;
                REG_SPI_TXRX_1BYTE_FIFO_DATA(ui8Spi).V =
                    *sDev->ui8TxBuf++;
                ui8Cnt--;
                sDev->ui16TxByteCnt--;
            }
        }

       /* check all TX bytes are pushed */
        if (sDev->ui16TxByteCnt)
        {
            /* If more to xmit, do aftet FIFO Empty*/
            EnableSpiIntr(ui8Spi,
                    BM_SPI_INT_EN_XMIT_COMPLETE_EN0);

        }
        else
        { /* no more byte to XMIT */

            DisableSpiIntr(ui8Spi, SPI_TX_INTR);
        }

        if (!(sDev->ui16TxByteCnt || sDev->ui16RxByteCnt))
        {
            sDev->ui8SpiState = HalSpiWaitForCmdDone;
            /* we received all byte */
            EnableSpiIntr(ui8Spi, BM_SPI_INT_EN_CMD_IS_DONE_EN0);
            REGN_W2(ui8Spi, SPI_STATUS,
                            START_BUSY, 0, HOLD_CS, 0);

        }
    }

    if((sDev->ui8SpiState == HalSpiInTx) &&
            (ui32IntrSt & SPI_TX_INTR))
    {
       DisableSpiIntr(ui8Spi, SPI_TX_INTR);
        while (sDev->ui16TxByteCnt)
        {
            if (!REG_SPI_STATUS(ui8Spi).BF.TXFIFO_HAS_1SPACE)
                break;
            REG_SPI_TXRX_1BYTE_FIFO_DATA(ui8Spi).V =
                        *sDev->ui8TxBuf++;
            sDev->ui16TxByteCnt--;
        }
        /* check all TX bytes are pushed */
        if (sDev->ui16TxByteCnt)
        {
            if (sDev->ui16TxByteCnt > sDev->ui8FifoDepth)
                    REG_SPI_CONFIG(ui8Spi).BF.FIFO_LEVEL_TRIG =
                                        sDev->ui8FifoDepth;
                else
                    REG_SPI_CONFIG(ui8Spi).BF.FIFO_LEVEL_TRIG =
                                sDev->ui16TxByteCnt;

            /* If more to xmit, do aftet FIFO Empty*/
            EnableSpiIntr(ui8Spi,
                    BM_SPI_INT_EN_XMIT_COMPLETE_EN0);

        }
        else
        { /* no more byte to XMIT */

            DisableSpiIntr(ui8Spi, SPI_TX_INTR);

            /* check for RX required */
            if (sDev->ui8RxBuf && sDev->ui16RxByteCnt)
            {

                if (sDev->ui16RxByteCnt > sDev->ui8FifoDepth)
                    REG_SPI_CONFIG(ui8Spi).BF.FIFO_LEVEL_TRIG =
                                    sDev->ui8FifoDepth;
                else
                    REG_SPI_CONFIG(ui8Spi).BF.FIFO_LEVEL_TRIG =
                                    sDev->ui16RxByteCnt;

                /* Write number of dummy bytes to get rx data */
                REG_SPI_TX_DUMMY_BYTE_CNT(ui8Spi).V = sDev->ui16RxByteCnt;
                sDev->ui8SpiState = HalSpiInRx;
                /* Enable RX interrupt to get notified of RX */
                EnableSpiIntr(ui8Spi,
                        BM_SPI_INT_EN_RX_DATA_EN0);
                REGN_W2(ui8Spi, SPI_STATUS,
                            START_BUSY, 0, HOLD_CS, 0);
            }
            else
            { /* No RX, we are done */
                sDev->ui8SpiState = HalSpiWaitForCmdDone;
                /* enable state mc completion intr */
                EnableSpiIntr(ui8Spi,
                                BM_SPI_INT_EN_CMD_IS_DONE_EN0);
                REGN_W2(ui8Spi, SPI_STATUS,
                            START_BUSY, 0, HOLD_CS, 0);
            }
        }
    }
    /* RX Interrupt */
    if((sDev->ui8SpiState == HalSpiInRx) && (sDev->ui16RxByteCnt) &&
            (REG_SPI_STATUS(ui8Spi).BF.RXFIFO_HAS_1DATA))
    {
        do {
            /* pop data from FIFO */
            *sDev->ui8RxBuf =
                        REG_SPI_TXRX_1BYTE_FIFO_DATA(ui8Spi).V;
            sDev->ui8RxBuf++;
            sDev->ui16RxByteCnt--;
        } while((sDev->ui16RxByteCnt ) &&
                    REG_SPI_STATUS(ui8Spi).BF.RXFIFO_HAS_1DATA);

        if (sDev->ui16RxByteCnt)
        {
            if (sDev->ui16RxByteCnt > sDev->ui8FifoDepth)
                REG_SPI_CONFIG(ui8Spi).BF.FIFO_LEVEL_TRIG =
                                    sDev->ui8FifoDepth;
            else
                REG_SPI_CONFIG(ui8Spi).BF.FIFO_LEVEL_TRIG =
                                    sDev->ui16RxByteCnt;

            EnableSpiIntr(ui8Spi,
                    BM_SPI_INT_EN_RX_DATA_EN0);
        }
        else
        {
            sDev->ui8SpiState = HalSpiWaitForCmdDone;
            /* we received all byte */
            EnableSpiIntr(ui8Spi, BM_SPI_INT_EN_CMD_IS_DONE_EN0);
            DisableSpiIntr(ui8Spi, BM_SPI_INT_EN_RX_DATA_EN0);
        }

    }

    if((sDev->ui8SpiState == HalSpiWaitForCmdDone) &&
                (REG_SPI_STATUS(ui8Spi).BF.CMD_IS_DONE))
    {
            DisableSpiIntr(ui8Spi,
                            BM_SPI_INT_EN_CMD_IS_DONE_EN0);
            sDev->ui8SpiState = HalSpiCmdDone;
            /* xfer complete, notify caller */
            if(sDev->fXferDoneCb)
            {
                sDev->fXferDoneCb(sDev->vSpiCbArg);
            }
            else
            {
                xSemaphoreGiveFromISR(sDev->SpiCmdDoneLock,
                                            &lHigherPriorityTaskWoken);
                portYIELD_FROM_ISR(lHigherPriorityTaskWoken);
            }
    }

}

#ifdef CONFIG_SPI_0_MASTER
void SPI0_ISR(void)
{
    SpiIntrHandler(0);
}
#endif

#ifdef CONFIG_SPI_1_MASTER
void SPI1_ISR(void)
{
    SpiIntrHandler(1);
}
#endif

static void SpiSetNvic(uint8_t ui8Spi)
{
    if (ui8Spi)
    {
        NVIC_SetPriority(SPI1_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
        NVIC_EnableIRQ(SPI1_IRQn);
    }
    else
    {
        NVIC_SetPriority(SPI0_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
        NVIC_EnableIRQ(SPI0_IRQn);
    }

}

static tEtaStatus SpiInit(tSpiNum iNum, tSpiConfig iSpiConfig)
{

  uint32_t spi_config;
  uint32_t spi_config2;
  uint32_t spi_divclk;
  uint32_t hfo_freq;

  if (iSpiConfig.ui8IntSize > 0x1F) {
    return (eEtaSpiInvalidConfig);
  }

  // Configurations
  spi_config          = (iSpiConfig.ui8IntSize          << BP_SPI_CONFIG_FIFO_LEVEL_TRIG) |
                        (iSpiConfig.spiBitEndian        << BP_SPI_CONFIG_END)             |
                        (iSpiConfig.spiClkPhase         << BP_SPI_CONFIG_CPHA)            |
                        (iSpiConfig.spiClkPol           << BP_SPI_CONFIG_CPOL)            ;

  spi_config2         = (iSpiConfig.spiByteEndian       << BP_SPI_CONFIG2_RX_BYTE2_END)    |
                        (iSpiConfig.spiByteEndian       << BP_SPI_CONFIG2_TX_BYTE2_END)    |
                        (0x1                            << BP_SPI_CONFIG2_AGGR_CLK_GATING) |
                        (0x0                            << BP_SPI_CONFIG2_CLK_ON)          |
                        (0x1                            << BP_SPI_CONFIG2_STALL_IF_RXFULL) ;

  // Determine what our source clock is
  switch (EtaCspSocCtrlUstFreqGet())
  {
    case (eSocCtrlUSTSel8MHz) : hfo_freq = 8000; break;
    case (eSocCtrlUSTSel4MHz) : hfo_freq = 4000; break;
    case (eSocCtrlUSTSel2MHz) : hfo_freq = 2000; break;
    default :
         return (eEtaSpiInvalidConfig);
         break;
  }

  // Check if desired close is within range
  if ((iSpiConfig.spiClkFreq*2) > hfo_freq) // Cannot achieve freq (max)
    return (eEtaSpiInvalidConfig);

  if ((iSpiConfig.spiClkFreq << 4) < hfo_freq) // Cannot achieve freq (min)
    return (eEtaSpiInvalidConfig);

  spi_divclk = ((hfo_freq/iSpiConfig.spiClkFreq)-1) & BM_SPI_CLKDIV_CLKDIV;

  // Add some subtle tweaks for corner cases
  switch (spi_divclk)
  {
    case (0) : return (eEtaSpiInvalidConfig); break; // Illegal
    case (1) : spi_config2 |= (1 << BP_SPI_CONFIG2_RX_SAMPLE_TIME) | BM_SPI_CONFIG2_DISABLE_MISO_METAFLOP; break;
    case (2) : spi_config2 |= (2 << BP_SPI_CONFIG2_RX_SAMPLE_TIME); break;
    default  : break;
  }

  // Add in inter command gap
  if (iSpiConfig.ui8ICG == 0xff) {
    spi_divclk |= (spi_divclk & BM_SPI_CLKDIV_CLKDIV) << BP_SPI_CLKDIV_ICG;
  }
  else {
    if (iSpiConfig.ui8ICG <= 0xF) {
      spi_divclk |= (iSpiConfig.ui8ICG << BP_SPI_CLKDIV_ICG) & BM_SPI_CLKDIV_ICG;
    }
    else {
      return (eEtaSpiInvalidConfig);
    }
  }

  // Add in inter byte gap
  if (iSpiConfig.ui8IBG == 0xff) {
    spi_divclk |= (spi_divclk & BM_SPI_CLKDIV_CLKDIV) << BP_SPI_CLKDIV_IBG;
  }
  else {
    if (iSpiConfig.ui8IBG <= 0xF) {
      spi_divclk |= (iSpiConfig.ui8IBG << BP_SPI_CLKDIV_IBG) & BM_SPI_CLKDIV_IBG;
    }
    else {
      return (eEtaSpiInvalidConfig);
    }
  }

  REG_SPI_CONFIG(iNum).V  = spi_config;
  REG_SPI_CONFIG2(iNum).V = spi_config2;
  REG_SPI_CLKDIV(iNum).V  = spi_divclk;
  REG_SPI_SLAVE_MODE(iNum).BF.SLAVE_EN = iSpiConfig.bSpiSlave;

  return (eEtaSuccess);
}

/*
 * Initialize SPI
 */
__attribute__((section(".initSection"))) int32_t HalSpiInit(uint8_t ui8Spi)
{
    struct sDevSpi *sDev;
#ifdef CONFIG_ECM3532
    tSpiConfig iSpiConfig;
#else
    uint8_t ui8ClkDiv;
#endif
    sDev = SpiGetDev(ui8Spi);
    if (sDev == NULL) {
        return -EINVAL;
    }

    SpiSetNvic(ui8Spi);
    sDev->ui8Spi = ui8Spi;
    sDev->ui8FifoDepth = SPI_FIFO_DEPTH;
    sDev->SpiLock = xSemaphoreCreateMutex();
    sDev->SpiCmdDoneLock = xSemaphoreCreateBinary();

#ifdef CONFIG_ECM3532
    iSpiConfig.spiByteEndian = eSpiByteEndianLSB ;
    iSpiConfig.spiBitEndian  = eSpiBitEndianMSb  ;
    iSpiConfig.spiClkPhase   = eSpiClkLeadEdge   ;
    iSpiConfig.spiClkPol     = eSpiClkPolPos     ;
    if (ui8Spi)
    {
#ifdef CONFIG_SPI_1_BRATE_IN_KHZ
        iSpiConfig.spiClkFreq = CONFIG_SPI_1_BRATE_IN_KHZ;
#endif
    }
    else
    {
#ifdef CONFIG_SPI_0_BRATE_IN_KHZ
        iSpiConfig.spiClkFreq = CONFIG_SPI_0_BRATE_IN_KHZ;
#endif
    }
    iSpiConfig.ui8IntSize = 0x4; // Default Interrupt Watermark
    iSpiConfig.ui8ICG = 0x0; // No need for inter command gap
    iSpiConfig.ui8IBG = 0xff; // make IBG = 1 bit time.

    SpiInit(ui8Spi, iSpiConfig);
#else

    if (ui8Spi)
    {
#ifdef CONFIG_SPI_1_BRATE_IN_KHZ
        ui8ClkDiv = (SPI_SRC_CLK_IN_KHZ / CONFIG_SPI_1_BRATE_IN_KHZ) - 1;
#endif
    }
    else
    {
#ifdef CONFIG_SPI_0_BRATE_IN_KHZ
        ui8ClkDiv = (SPI_SRC_CLK_IN_KHZ / CONFIG_SPI_0_BRATE_IN_KHZ) - 1;
#endif
    }
    /*
     * Disable any odd configurations
     * Set the CPOL, CPHA bits to 11b
     *
     */
    REGN_W2(ui8Spi, SPI_CLKDIV, CLKDIV, ui8ClkDiv, IBG, 0);
    /* Perform soft reset of SM and set stall TX if RX FULL*/
    REGN_W1(ui8Spi, SPI_CONFIG2, SOFT_RST, 1);
    REGN_M3(ui8Spi, SPI_CONFIG2, SOFT_RST, 0,
                STALL_IF_RXFULL, 1, CLK_ON, 1);
    /* Disable Meta flop for Higher Brate */
    if (ui8ClkDiv <= 1)
        REGN_M1(ui8Spi, SPI_CONFIG2, DISABLE_MISO_METAFLOP, 1);
#endif
    return 0;
}
