/*******************************************************************************
*
* @file spi_slave_hal.c
*
* Copyright (C) 2020 Eta Compute, Inc
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
#include "eta_csp_spi.h"
#include "spi_slave_hal.h"
#include "errno.h"
#include "ecm3532.h"
#include "cm3.h"
#include "FreeRTOS.h"
#include "semphr.h"

#define SPI_FIFO_DEPTH 16
#define SPI_TX_INTR (BM_SPI_INT_EN_TX_DATA_EN0 |\
                        BM_SPI_INT_EN_TXFIFO_HAS_LEVEL_SPACE_EN0 |\
                        BM_SPI_INT_EN_XMIT_COMPLETE_EN0)

/* SPI Slave State*/
typedef enum {
    /* Idle state */
    HalSpiCmdDone= 0,
    /* Tx in Progress */
    HalSpiInTx,
    /* Rx in Progress */
    HalSpiInRx,
    /* Wait for bus to go Idle */
    HalSpiWaitForCmdDone,
}tHalSpiSlvSt;

/**
 * SPI Slave Driver structure
 * Latch Driver state, Tx and Rx buffer
 * Tx and Rx count
 * Synchronization primitives
 * Callback function
 */
struct sDevSpiSlv {
    uint8_t ui8FifoDepth;
    uint8_t *ui8TxBuf;
    uint8_t *ui8RxBuf;
    uint8_t ui8SpiState;
    uint32_t ui32TxByteCnt;
    uint32_t ui32RxByteCnt;
    uint8_t ui8Spi;
    tHalSpiXferDoneCb fXferDoneCb;
    void *vSpiCbArg;
    SemaphoreHandle_t SpiLock;
    SemaphoreHandle_t SpiCmdDoneLock;
};

#ifdef CONFIG_SPI_0_SLAVE
static struct sDevSpiSlv sDevSpiSlv0 = {0};
#endif
#ifdef CONFIG_SPI_1_SLAVE
static struct sDevSpiSlv sDevSpiSlv1 = {0};
#endif

static inline struct sDevSpiSlv* SpiGetDev(uint8_t ui8Spi)
{
    switch (ui8Spi) {
#ifdef CONFIG_SPI_0_SLAVE
    case 0:
        return &sDevSpiSlv0;
#endif
#ifdef CONFIG_SPI_1_SLAVE
    case 1:
        return &sDevSpiSlv1;
#endif
    default:
        return NULL;
    }
}
/**
 * Enable interrupt helper func
 */
static inline void EnableSpiIntr(uint8_t ui8Spi, uint32_t  ui32IntrSet)
{
    REG_SPI_INT_EN(ui8Spi).V |= ui32IntrSet;
}

/**
 * Disable Interrupt helper func
 */
static inline void DisableSpiIntr(uint8_t ui8Spi, uint32_t ui32IntrSet)
{
    REG_SPI_INT_EN(ui8Spi).V &= (~ui32IntrSet);
}

/**
 * Push in Tx Fifo based on curr Fifo level
 */
static void SpiSlaveXmit(struct sDevSpiSlv *sDev)
{
    uint8_t ui8CurrLevel, ui8Avail;
    if (sDev->ui32TxByteCnt)
    {
        ui8CurrLevel = MEMIO8(REG_SPI_TX_FIFO_DEBUG_ADDR(sDev->ui8Spi)) &
                    BM_SPI_TX_FIFO_DEBUG_ENTRIES;
        ui8Avail = SPI_FIFO_DEPTH - ui8CurrLevel;
        while(ui8Avail && sDev->ui32TxByteCnt)
        {
            REG_SPI_TXRX_1BYTE_FIFO_DATA(sDev->ui8Spi).V =
                *sDev->ui8TxBuf++;
            sDev->ui32TxByteCnt--;
            ui8Avail--;
        }
    }
}

/**
 * SPI Slave send and receive routine, works in half-duplex mode.
 *
 * For Send should be called with valid Xmit buffer and count
 * For Receive should be called with valid Receive buffer and count.
 *
 *
 * In send mode TX buffer contain data to be send, transfer count specify the send
 * data length in bytes,
 * In receive mode RX count specify the byte length to be  receive in receive buffer
 *
 * If called with valid (non zero) callback function, it will be a non blocking
 * call and callback function will be called after completion from interrupt
 * context, callback function must only be used for signal of completion as it
 * is called from interrupt context
 * if callback function is NULL, API will be blocking API and will retun after
 * comletetion of SPI transfer
 *
 * @param ui8Spi        SPI interface to use
 * @param ui8TxBuf      Pointer to buffer where values to transmit are stored.
 * @param ui32TxCnt     Transmit byte count.
 * @param ui8Rxbuf      Pointer to buffer to store values received from slave.
 * @param ui32TxCnt     Receive byte count.
 * @param fXferDoneCb   callback function pointer, for async call.
 * @param vCbArg        Argument for callback funct pointer
 *
 * @return int 0 on success, non-zero error code on failure.
 */
int32_t HalSpiSlaveXfer(uint8_t ui8Spi, uint8_t *ui8TxBuf,
                    uint32_t ui32TxCnt, uint8_t *ui8RxBuf, uint32_t ui32RxCnt,
                    tHalSpiXferDoneCb fSpiCb, void *vCbArg)
{
    struct sDevSpiSlv *sDev;
    int32_t i32Ret = 0;
    uint8_t bFlush ;
    uint8_t ui8Cnt= 0, ui8Data;

    (void)ui8Data;
    sDev = SpiGetDev(ui8Spi);
    if (sDev == NULL) {
        return -EINVAL;
    }

    xSemaphoreTake(sDev->SpiLock, portMAX_DELAY);

    if (sDev->ui8SpiState != HalSpiCmdDone)
    {
        i32Ret = -EBUSY;
        goto err_exit;
    }
    else if (ui8TxBuf && ui32TxCnt)
    {
        sDev->ui8SpiState = HalSpiInTx;
        sDev->ui32TxByteCnt = ui32TxCnt;
        sDev->ui8TxBuf = ui8TxBuf;
    }
    else if (ui8RxBuf || ui32RxCnt)
    {
        sDev->ui8SpiState = HalSpiInRx;
        sDev->ui32RxByteCnt = ui32RxCnt;
        sDev->ui8RxBuf = ui8RxBuf;
    }
    else
    {
        i32Ret = -EBUSY;
        goto err_exit;

    }

    if (fSpiCb)
    {
        sDev->fXferDoneCb = fSpiCb;
        sDev->vSpiCbArg = vCbArg;
    }

    if (sDev->ui8SpiState == HalSpiInTx)
    {
        SpiSlaveXmit(sDev);
        if (sDev->ui32TxByteCnt > (sDev->ui8FifoDepth - 2))
                REG_SPI_CONFIG(ui8Spi).BF.FIFO_LEVEL_TRIG =
                            sDev->ui8FifoDepth - 2 ;
            else
                REG_SPI_CONFIG(ui8Spi).BF.FIFO_LEVEL_TRIG =
                            sDev->ui32TxByteCnt;

        EnableSpiIntr(ui8Spi,SPI_TX_INTR);
        bFlush = 1;
    }
    else if (sDev->ui8SpiState == HalSpiInRx)
    {
        if (sDev->ui32RxByteCnt > sDev->ui8FifoDepth)
            REG_SPI_CONFIG(ui8Spi).BF.FIFO_LEVEL_TRIG =
                sDev->ui8FifoDepth;
        else
            REG_SPI_CONFIG(ui8Spi).BF.FIFO_LEVEL_TRIG =
                sDev->ui32RxByteCnt;

        sDev->ui8SpiState = HalSpiInRx;
        /* Enable RX interrupt to get notified of RX */
        EnableSpiIntr(ui8Spi, BM_SPI_INT_EN_RX_DATA_EN0);
    }

    if(!sDev->fXferDoneCb)
    {
        if (sDev->ui8SpiState == HalSpiInRx)
        {
            xSemaphoreTake(sDev->SpiCmdDoneLock, portMAX_DELAY);
        }
        else
        {
            do {

            } while (pdTRUE != xSemaphoreTake(sDev->SpiCmdDoneLock, 1));
        }
    }

    if (bFlush)
    {
        ui8Cnt = MEMIO8(REG_SPI_RX_FIFO_DEBUG_ADDR(sDev->ui8Spi)) &
                BM_SPI_RX_FIFO_DEBUG_ENTRIES;
            while(ui8Cnt)
            {
                ui8Data = REG_SPI_TXRX_1BYTE_FIFO_DATA(ui8Spi).V;
                ui8Cnt--;
            }
    }
err_exit:
    xSemaphoreGive(sDev->SpiLock);
    return i32Ret;
}


/*
 * SPI interrupt handler
 */
static void SpiIntrHandler(uint8_t ui8Spi)
{
    uint32_t ui32IntrSt;
    uint8_t ui8Cnt;

    struct sDevSpiSlv *sDev;
    long lHigherPriorityTaskWoken = pdFALSE;

    sDev = SpiGetDev(ui8Spi);

    ui32IntrSt = REG_SPI_STATUS(ui8Spi).V;

    if((sDev->ui8SpiState == HalSpiInTx) &&
            (ui32IntrSt & SPI_TX_INTR))
    {
       DisableSpiIntr(ui8Spi, SPI_TX_INTR);
       SpiSlaveXmit(sDev);
        /* check all TX bytes are pushed */
        if (sDev->ui32TxByteCnt)
        {
            if (sDev->ui32TxByteCnt > (sDev->ui8FifoDepth - 2))
                REG_SPI_CONFIG(ui8Spi).BF.FIFO_LEVEL_TRIG =
                            sDev->ui8FifoDepth - 2 ;
            else
                REG_SPI_CONFIG(ui8Spi).BF.FIFO_LEVEL_TRIG =
                            sDev->ui32TxByteCnt;

            EnableSpiIntr(ui8Spi,SPI_TX_INTR);
        }
        else
        { /* no more byte to XMIT */

            DisableSpiIntr(ui8Spi, SPI_TX_INTR);
            EnableSpiIntr(ui8Spi,
                            BM_SPI_INT_EN_CMD_IS_DONE_EN0);
            sDev->ui8SpiState = HalSpiWaitForCmdDone;
        }
    }
    /* RX Interrupt */
    if(REG_SPI_STATUS(ui8Spi).BF.RXFIFO_HAS_1DATA)
    {
        if ((sDev->ui8SpiState == HalSpiInRx) && (sDev->ui32RxByteCnt))
        {
            ui8Cnt = MEMIO8(REG_SPI_RX_FIFO_DEBUG_ADDR(sDev->ui8Spi)) &
                BM_SPI_RX_FIFO_DEBUG_ENTRIES;
            while(ui8Cnt && sDev->ui32RxByteCnt)
            {
                *sDev->ui8RxBuf =
                    REG_SPI_TXRX_1BYTE_FIFO_DATA(ui8Spi).V;
                sDev->ui8RxBuf++;
                sDev->ui32RxByteCnt--;
                ui8Cnt--;
            }

            if (sDev->ui32RxByteCnt)
            {
                if (sDev->ui32RxByteCnt > sDev->ui8FifoDepth)
                    REG_SPI_CONFIG(ui8Spi).BF.FIFO_LEVEL_TRIG =
                        sDev->ui8FifoDepth;
                else
                    REG_SPI_CONFIG(ui8Spi).BF.FIFO_LEVEL_TRIG =
                        sDev->ui32RxByteCnt;

                EnableSpiIntr(ui8Spi,
                        BM_SPI_INT_EN_RX_DATA_EN0);
            }
            else
            {
                sDev->ui8SpiState = HalSpiWaitForCmdDone;
                EnableSpiIntr(ui8Spi,
                        BM_SPI_INT_EN_CMD_IS_DONE_EN0);
            }

        }
        else
        {
            DisableSpiIntr(ui8Spi, BM_SPI_INT_EN_RX_DATA_EN0);
        }

    }

    if((sDev->ui8SpiState == HalSpiWaitForCmdDone) &&
                (REG_SPI_STATUS(ui8Spi).BF.CMD_IS_DONE))
    {
            sDev->ui8SpiState = HalSpiCmdDone;
            DisableSpiIntr(ui8Spi,
                            BM_SPI_INT_EN_CMD_IS_DONE_EN0);
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

#ifdef CONFIG_SPI_0_SLAVE
void SPI0_ISR(void)
{
    SpiIntrHandler(0);
}
#endif

#ifdef CONFIG_SPI_1_SLAVE
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

/*
 * Initialize SPI Slave
 */
__attribute__((section(".initSection"))) int32_t HalSpiSlaveInit(uint8_t ui8Spi)
{
    struct sDevSpiSlv *sDev;
    uint8_t ui8ClkDiv;

    tSpiConfig iSpiConfig;
    sDev = SpiGetDev(ui8Spi);
    if (sDev == NULL) {
        return -EINVAL;
    }

    SpiSetNvic(ui8Spi);
    sDev->ui8Spi = ui8Spi;
    sDev->SpiLock = xSemaphoreCreateMutex();
    sDev->SpiCmdDoneLock = xSemaphoreCreateBinary();

    REG_SPI_SLAVE_MODE(ui8Spi).BF.SLAVE_EN = 1;
    iSpiConfig.spiByteEndian = eSpiByteEndianLSB ;
        iSpiConfig.spiBitEndian  = eSpiBitEndianMSb;
        iSpiConfig.spiClkPhase   = CONFIG_SPI_SLAVE_CLK_PHASE;
        iSpiConfig.spiClkPol     = CONFIG_SPI_SLAVE_CLK_POLARITY;
        iSpiConfig.bSpiSlave     = TRUE;
        iSpiConfig.ui8IntSize    = 1;
        iSpiConfig.ui8ICG        = 0x0;
        iSpiConfig.ui8IBG        = 0xff;
        EtaCspSpiInit(ui8Spi, iSpiConfig);

    return 0;
}
