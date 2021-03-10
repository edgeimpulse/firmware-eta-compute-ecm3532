/*******************************************************************************
*
* @file i2c_hal.h
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
#include "eta_csp_i2c.h"
#include "i2c_hal.h"
#include "errno.h"
#ifdef CONFIG_ECM3531
#include "ecm3531.h"
#elif CONFIG_ECM3532
#include "ecm3532.h"
#endif
#include "cm3.h"
#include "FreeRTOS.h"
#include "semphr.h"
#define TX_FIFO_DEPTH           (16)

/* I2C Address, 8 bit */
#define WRITE_MODE              (0)
#define READ_MODE               (1)

/* State machine*/
#define I2C_IDLE        (0)
#define WRITE_OP        (1)
#define READ_OP         (2)
#define WAIT_CMD_DONE   (3)

#define SPACE_REQ_FOR_CMD_BYTES_IN_WR (1)
/*
 * I2c HAL Data structure
 * holding state, buffer pointer
 * and callbacks
 */
struct sDevI2c {
    volatile uint8_t ui8State;
    uint8_t ui8SlaveAddr;
    uint8_t ui8PendingTxByte;
    uint8_t *ui8TxBuf;
    uint8_t *ui8RxBuf;
    uint32_t ui32TxByteCnt;
    uint32_t ui32RxByteCnt;
    tHalI2cCb fCb;
    void *vPtr;
    SemaphoreHandle_t I2cLock;
    SemaphoreHandle_t I2cCmdDone;
};

#ifdef CONFIG_I2C_0
static struct sDevI2c sDev0;
#endif
#ifdef CONFIG_I2C_1
static struct sDevI2c sDev1;
#endif

static struct sDevI2c *
I2cGetDev(uint8_t ui8I2c)
{
    switch (ui8I2c) {
#ifdef CONFIG_I2C_0
        case 0:
            return &sDev0;
#endif
#ifdef CONFIG_I2C_1
        case 1:
            return &sDev1;
#endif
        default:
            return NULL;
    }
}

/*
 * I2c interrupt enable and disable func
 */
static void EnableI2cIntr(uint8_t ui8I2c, uint32_t  ui32IntrSet)
{
    REG_I2C_INT_EN(ui8I2c).V |= ui32IntrSet;
}

static void DisableI2cIntr(uint8_t ui8I2c, uint32_t ui32IntrSet)
{
    REG_I2C_INT_EN(ui8I2c).V &= (~ui32IntrSet);
}

static tEtaStatus I2cSpeedConfig(tI2cNum iNum, tI2cClk iClockSpeed)
{
    char CYCLE_TIME;
    char IBG_TIME;
    char PE_TIME;
    char NE_TIME;
    char TX_TIME;
    char RX_TIME;
    char MIN_SCL_HI;
    char SCL_HYSTERSIS_LEVEL;
    char SDA_HYSTERSIS_LEVEL;
    char TEMP_TIME;

    //
    // Use algorithm for lower speed interfaces.
    //
    if(iClockSpeed < 500)
    {
        CYCLE_TIME = (4000 / iClockSpeed) - 1; // 4MHz source clock.
        IBG_TIME = CYCLE_TIME;
        TEMP_TIME = ((400 - iClockSpeed) / 3);
        PE_TIME = 4 + ((TEMP_TIME * (16 - 4))) / 100;
        NE_TIME = 8 + ((TEMP_TIME * (31 - 8))) / 100;
        RX_TIME = 5 + ((TEMP_TIME * (25 - 5))) / 100;
        TX_TIME = 1 + ((TEMP_TIME * (1  - 1))) / 100;
        MIN_SCL_HI = 0 + ((TEMP_TIME * (4 - 0))) / 100;
        SCL_HYSTERSIS_LEVEL = 1 + ((TEMP_TIME * (8 - 1))) / 100;
        SDA_HYSTERSIS_LEVEL = 1 + ((TEMP_TIME * (8 - 1))) / 100;

        //
        // Enable hysteresis.
        //
        if(SCL_HYSTERSIS_LEVEL > 0)
        {
            REGN_M1(iNum, I2C_CONFIG2, SCL_HYSTERISIS_LEVEL,
                    (16 - SCL_HYSTERSIS_LEVEL));
        }
        else
        {
            REGN_M1(iNum, I2C_CONFIG2, DISABLE_SCL_HYSTERSIS, 1);
        }

        //
        // Enable hysteresis.
        //
        if(SDA_HYSTERSIS_LEVEL > 0)
        {
            REGN_M1(iNum, I2C_CONFIG2, SDA_HYSTERISIS_LEVEL,
                    (16 - SDA_HYSTERSIS_LEVEL));
        }
        else
        {
            REGN_M1(iNum, I2C_CONFIG2, DISABLE_SDA_HYSTERSIS, 1);
        }
    }

    //
    // If (iClockSpeed >= 500) Use prerecorded values.
    //
    else
    {
        CYCLE_TIME = 3;
        IBG_TIME = CYCLE_TIME;
        PE_TIME = 1;
        NE_TIME = 2;
        RX_TIME = 3;
        TX_TIME = 0;
        MIN_SCL_HI = 0;

        //
        // Disable Hysteresis.
        //
        SCL_HYSTERSIS_LEVEL = 0;
        SDA_HYSTERSIS_LEVEL = 0;
        REGN_M1(iNum, I2C_CONFIG2, DISABLE_SCL_HYSTERSIS, 1);
        REGN_M1(iNum, I2C_CONFIG2, DISABLE_SDA_HYSTERSIS, 1);
    }

    REGN_M2(iNum, I2C_CLK_CONFIG1, IBG_TIME, IBG_TIME, CYCLE_TIME,
            CYCLE_TIME);
    REGN_M2(iNum, I2C_CLK_CONFIG2, TX_TIME, TX_TIME, RX_TIME, RX_TIME);
    REGN_M2(iNum, I2C_CLK_CONFIG3, PE_TIME, PE_TIME, NE_TIME, NE_TIME);
    REGN_M1(iNum, I2C_SCL_CONFIG, MIN_SCL_HIGH, MIN_SCL_HI);

    return(eEtaSuccess);
}

/**
 * Set i2c bus frequency
 *
 */
int32_t HalI2cSetBusFreq(uint8_t ui8I2c, tHalI2cClk iFreq)
{
    struct sDevI2c *sDev;

    sDev = I2cGetDev(ui8I2c);

    if (!sDev || (iFreq > HalI2cClk1MHz))
        return -EINVAL;

    if(sDev->ui8State != I2C_IDLE)
        return -EBUSY;
    xSemaphoreTake(sDev->I2cLock, portMAX_DELAY);
    I2cSpeedConfig(ui8I2c, iFreq);
    xSemaphoreGive(sDev->I2cLock);
    return 0;
}

/**
 * I2c Write to Slave
 *
 */
int32_t HalI2cWrite(uint8_t ui8I2c, uint8_t ui8SlaveAddr, uint16_t ui16Offset,
                uint8_t ui8OffsetLen, uint8_t *i8Txbuf, uint32_t ui32TxLen,
                tHalI2cCb fI2cCb, void *vPtr)
{

    uint8_t ui8Addr;
    uint8_t *ui8Wbuf;
    struct sDevI2c *sDev;
    uint8_t ui8TxCurrFifoLevel;
    uint8_t ui8MinReqTxFifo;
    uint32_t ui8Wlen = 0, ui8Cnt = 0;
    volatile BaseType_t ret = 0;

    sDev = I2cGetDev(ui8I2c);

    if (!sDev)
        return -ENODEV;
    if(sDev->ui8State != I2C_IDLE)
        return -EBUSY;
    if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING)
    {
        xSemaphoreTake(sDev->I2cLock, portMAX_DELAY);
    }
    else
    {
        NVIC_SetPriority(I2C0_IRQn + ui8I2c, (1 << __NVIC_PRIO_BITS));
    }
    sDev->ui8SlaveAddr = ui8SlaveAddr;
    sDev->ui8State = WRITE_OP;
    sDev->ui8RxBuf = NULL;
    sDev->ui32RxByteCnt = 0;

    /* 7 bit address */
    ui8Addr = (ui8SlaveAddr << 1) | WRITE_MODE;

    REG_I2C_TXRX_1BYTE_FIFO_DATA(ui8I2c).V = ui8Addr;
    ui8Wbuf = i8Txbuf;
    sDev->ui32TxByteCnt = ui32TxLen + ui8OffsetLen;
    ui8TxCurrFifoLevel =
                REG_I2C_TX_FIFO_DEBUG(ui8I2c).BF.ENTRIES;

    /** ui8MinReqTxFifo will make sure atleast 1 byte of data
     *  along with followup CMD byte, to avoid fifo starvation
     */
    ui8MinReqTxFifo = (TX_FIFO_DEPTH - SPACE_REQ_FOR_CMD_BYTES_IN_WR) -
        ui8TxCurrFifoLevel;
    /* byte remaing is less then FIFO space, push all */
    if (sDev->ui32TxByteCnt < ui8MinReqTxFifo)
    {
        ui8Wlen =  sDev->ui32TxByteCnt;
    }
    /* byte remaing is more then FIFO space,
     * push ui8MinReqTxFifo only bytes*/
    else
    {
        ui8Wlen =  ui8MinReqTxFifo;
    }

    sDev->ui32TxByteCnt = sDev->ui32TxByteCnt - ui8Wlen;

    while(ui8Wlen)
    {
        if(ui8OffsetLen)
        {
            if (ui8OffsetLen == 1)
            {
                REG_I2C_TXRX_1BYTE_FIFO_DATA(ui8I2c).V =
                            (ui16Offset & 0xFF);
            }
            else if (ui8OffsetLen == 2)
            {
                /* for 16 bit offset, 1st MSB followed by LSB */
                REG_I2C_TXRX_1BYTE_FIFO_DATA(ui8I2c).V =
                            (uint8_t)((ui16Offset & 0xFF00) >> 8);
                REG_I2C_TXRX_1BYTE_FIFO_DATA(ui8I2c).V =
                            (uint8_t)(ui16Offset);
            }
            ui8Wlen = ui8Wlen - ui8OffsetLen;
            ui8OffsetLen = 0;

        }
        REG_I2C_TXRX_1BYTE_FIFO_DATA(ui8I2c).V = ui8Wbuf[ui8Cnt];
        ui8Cnt++;
        ui8Wlen--;
    }

    REGN_W1(ui8I2c, I2C_STATUS, START_BUSY, 1);
    /*  NO byte reamining Enable TX Complete interrupt */
    if (!sDev->ui32TxByteCnt)
    {
        REG_I2C_CMD_FIFO_DATA(ui8I2c).V = I2C_CMD_END_WRITE;
        EnableI2cIntr(ui8I2c, BM_I2C_INT_EN_TX_COMPLETE_EN);
    }
    /**
     * Set FIFO level for TX FIFO Spave interrupt
     * to Push remaining bytes in FIFO
     */
    else
    {
        sDev->ui8TxBuf = &ui8Wbuf[ui8Cnt];
        /* TX RX FIFO interrupt level 1 Byte + SPACE_REQ_FOR_WR_CMD_BYTES*/
        REGN_M1(ui8I2c, I2C_CONFIG, RX_FIFO_INT_LEVEL,
                1 + SPACE_REQ_FOR_CMD_BYTES_IN_WR);
        EnableI2cIntr(ui8I2c,
                BM_I2C_INT_EN_TXFIFO_HAS_LEVEL_SPACE_EN);
    }
    sDev->fCb = fI2cCb;
    /**
     * If No callback, block mode wait on semaphore to be release
     * from ISR once TX done
     */
   if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING)
    {
        if(!fI2cCb)
        {
            do {
                ret = xSemaphoreTake(sDev->I2cCmdDone, 1);
            } while (!ret);
            sDev->ui8State = I2C_IDLE;
            ret = 0;
        }
        /**
         * Non blocking updated pointers and return
         */
        else
        {
            sDev->vPtr = vPtr;
        }

        if(REG_I2C_GENERAL_DEBUG(ui8I2c).BF.LAST_BYTE_HAD_NACK)
        {
            ret = -EREMOTEIO;
        }
    }
   else
   {
        while(sDev->ui8State != I2C_IDLE);
        NVIC_SetPriority(I2C0_IRQn + ui8I2c, (1 << __NVIC_PRIO_BITS) - 1);
   }
    xSemaphoreGive(sDev->I2cLock);
    return ret;

}

/**
 * I2c Read from Slave
 *
 */
int32_t HalI2cRead(uint8_t ui8I2c, uint8_t ui8SlaveAddr, uint16_t ui16Offset,
                uint8_t ui8OffsetLen, uint8_t *ui8Rxbuf, uint32_t ui32RxLen,
                tHalI2cCb fI2cCb, void *vPtr)
{

    uint8_t ui8Addr;
    volatile int32_t ret = 0;
    uint32_t rlen = 0;
    struct sDevI2c *sDev;

    sDev = I2cGetDev(ui8I2c);

    if (!sDev)
        return -ENODEV;

    if(sDev->ui8State != I2C_IDLE)
        return -EBUSY;

    if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING)
    {
        xSemaphoreTake(sDev->I2cLock, portMAX_DELAY);
    }

    else
    {
        NVIC_SetPriority(I2C0_IRQn + ui8I2c, (1 << __NVIC_PRIO_BITS));
    }

    sDev->ui8SlaveAddr = ui8SlaveAddr;
    sDev->ui8State = READ_OP;

    if (ui8OffsetLen)
    {
        /* 7 bit address */
        ui8Addr = (ui8SlaveAddr << 1) | WRITE_MODE;
        REG_I2C_TXRX_1BYTE_FIFO_DATA(ui8I2c).V = ui8Addr;

        if (ui8OffsetLen == 1)
        {
            REG_I2C_TXRX_1BYTE_FIFO_DATA(ui8I2c).V =
                            (ui16Offset & 0xFF);
        }
        else if (ui8OffsetLen == 2)
        {
            /* for 16 bit offset, 1st MSB followed by LSB */
            REG_I2C_TXRX_1BYTE_FIFO_DATA(ui8I2c).V =
                            (uint8_t)((ui16Offset & 0xFF00) >> 8);
            REG_I2C_TXRX_1BYTE_FIFO_DATA(ui8I2c).V =
                            (uint8_t)(ui16Offset);
        }

        REG_I2C_CMD_FIFO_DATA(ui8I2c).V = I2C_CMD_RESTART;
    }

    ui8Addr = (ui8SlaveAddr << 1) | READ_MODE;
    REG_I2C_TXRX_1BYTE_FIFO_DATA(ui8I2c).V = ui8Addr;
    rlen = ui32RxLen;
    sDev->ui8RxBuf = ui8Rxbuf;
    sDev->ui32RxByteCnt = ui32RxLen;
    /* RX FIFO interrupt level 1 Byte*/
    REGN_M1(ui8I2c, I2C_CONFIG, RX_FIFO_INT_LEVEL, 1);

    if(rlen == 1)
    {
        REG_I2C_TXRX_1BYTE_FIFO_DATA(ui8I2c).V = 0xff;
        REG_I2C_CMD_FIFO_DATA(ui8I2c).V = I2C_CMD_BEGIN_READ |
                                            I2C_CMD_END_READ;
    }
    else if(rlen == 2)
    {
        REG_I2C_TXRX_1BYTE_FIFO_DATA(ui8I2c).V = 0xff;
        REG_I2C_CMD_FIFO_DATA(ui8I2c).V = I2C_CMD_BEGIN_READ;
        REG_I2C_TXRX_1BYTE_FIFO_DATA(ui8I2c).V = 0xff;
        REG_I2C_CMD_FIFO_DATA(ui8I2c).V = I2C_CMD_END_READ;
    }
    else if (rlen > 2)
    {
        REG_I2C_TXRX_1BYTE_FIFO_DATA(ui8I2c).V = 0xff;
        REG_I2C_CMD_FIFO_DATA(ui8I2c).V = I2C_CMD_BEGIN_READ;
        REG_I2C_DUMMY_BYTES(ui8I2c).V = rlen - 2;
        REG_I2C_CMD_FIFO_DATA(ui8I2c).V = I2C_CMD_DUMMYBYTE;
        REG_I2C_TXRX_1BYTE_FIFO_DATA(ui8I2c).V = 0xff;
        REG_I2C_CMD_FIFO_DATA(ui8I2c).V = I2C_CMD_END_READ;
    }
    REGN_W1(ui8I2c, I2C_STATUS, START_BUSY, 1);
    EnableI2cIntr(ui8I2c, BM_I2C_INT_EN_RX_DATA_EN);
    sDev->fCb = fI2cCb;
    if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING)
    {
        if(!fI2cCb)
        {
            do
            {
                ret = xSemaphoreTake(sDev->I2cCmdDone, 1);
            } while (!ret);
            sDev->ui8State = I2C_IDLE;
        }
        else
        {
            sDev->vPtr = vPtr;
        }

    }
    else
    {
        while(sDev->ui8State != I2C_IDLE);
        NVIC_SetPriority(I2C0_IRQn + ui8I2c, (1 << __NVIC_PRIO_BITS) - 1);
    }
    xSemaphoreGive(sDev->I2cLock);

    return ret;
}

/**
 * Probes the i2c bus for a device with this address.
 */
int32_t HalI2cProbe(uint8_t ui8I2c, uint8_t ui8Addr)
{
    return HalI2cWrite(ui8I2c, ui8Addr, 0, 0, NULL, 0, NULL, NULL);
}

/**
 * I2C Interrupt Hanlder
 */
static void I2cIntrHandler(uint8_t ui8I2c)
{
    uint8_t *ui8Wbuf;
    uint32_t ui32IntrSt;
    struct sDevI2c *sDev;
    uint8_t ui8MinReqTxFifo;
    uint8_t ui8TxCurrFifoLevel;
    uint8_t ui8Wlen = 0, ui8Cnt = 0;
    long lHigherPriorityTaskWoken = pdFALSE;

    sDev = I2cGetDev(ui8I2c);
    ui32IntrSt = REG_I2C_STATUS(ui8I2c).V;

    if(sDev->ui8State == WRITE_OP)
    {
        if (!sDev->ui32TxByteCnt)
        {
            /* TX completed, now enable enalbe cmd done*/
            if (ui32IntrSt & BM_I2C_STATUS_XMIT_COMPLETE)
            {
                DisableI2cIntr(ui8I2c, BM_I2C_INT_EN_TX_COMPLETE_EN);
                EnableI2cIntr(ui8I2c, BM_I2C_INT_EN_CMD_DONE);
                sDev->ui8State = WAIT_CMD_DONE;
            }
        }
        /* push remaning byte to TX FIFO */
        else if (sDev->ui32TxByteCnt &&
                    (ui32IntrSt & BM_I2C_STATUS_TXFIFO_HAS_LEVEL_SPACE))
        {
            ui8TxCurrFifoLevel = REG_I2C_TX_FIFO_DEBUG(ui8I2c).BF.ENTRIES;
            /* ui8MinReqTxFifo will make sure atleast 1 byte of data
             * along with followup CMD byte, to avoid fifo starvation
             */
            ui8MinReqTxFifo = (TX_FIFO_DEPTH - SPACE_REQ_FOR_CMD_BYTES_IN_WR) -
                                ui8TxCurrFifoLevel;
            ui8Wbuf = sDev->ui8TxBuf;
            /* byte remaing is less then FIFO space, push all */
            if (sDev->ui32TxByteCnt < ui8MinReqTxFifo)
            {
                ui8Wlen =  sDev->ui32TxByteCnt;
            }
            /* byte remaing is more then FIFO space,
             * push ui8MinReqTxFifo only bytes*/
            else
            {
                ui8Wlen =  ui8MinReqTxFifo;
            }
            sDev->ui32TxByteCnt = sDev->ui32TxByteCnt - ui8Wlen;
            while(ui8Wlen)
            {
                REG_I2C_TXRX_1BYTE_FIFO_DATA(ui8I2c).V = ui8Wbuf[ui8Cnt];
                ui8Cnt++;
                ui8Wlen--;
            }

            /* All byte are pushed, enable TX Complete intr */
            if (!sDev->ui32TxByteCnt)
            {
                REG_I2C_CMD_FIFO_DATA(ui8I2c).V =
                        I2C_CMD_END_WRITE;
                DisableI2cIntr(ui8I2c,
                        BM_I2C_INT_EN_TXFIFO_HAS_LEVEL_SPACE_EN);
                EnableI2cIntr(ui8I2c, BM_I2C_INT_EN_TX_COMPLETE_EN);
            }
            /* Pending bytes for TX, Update buffer pointer for next cycle */
            else
            {
                sDev->ui8TxBuf = &ui8Wbuf[ui8Cnt];
            }

        }

    }
    /* I2C state machine complete interrupt */
    else if(sDev->ui8State == READ_OP)
    {
        if(sDev->ui32RxByteCnt &&
                (ui32IntrSt & BM_I2C_STATUS_RXFIFO_HAS_1DATA))
        {
            while(sDev->ui32RxByteCnt &&
                    (REG_I2C_STATUS(ui8I2c).BF.RXFIFO_HAS_1DATA))
            {
                *sDev->ui8RxBuf = REG_I2C_TXRX_1BYTE_FIFO_DATA(ui8I2c).V;
                sDev->ui32RxByteCnt--;
                sDev->ui8RxBuf++;
            }
            if(!sDev->ui32RxByteCnt)
            {
                EnableI2cIntr(ui8I2c, BM_I2C_INT_EN_CMD_DONE);
                sDev->ui8State = WAIT_CMD_DONE;
                DisableI2cIntr(ui8I2c, BM_I2C_INT_EN_RX_DATA_EN);
            }

        }
    }
    /* I2C state machine complete interrpt */
    else if(sDev->ui8State == WAIT_CMD_DONE)
    {
        if (ui32IntrSt & BP_I2C_STATUS_CMD_DONE)
        {
            DisableI2cIntr(ui8I2c, BM_I2C_INT_EN_CMD_DONE);

            if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)
            {
                sDev->ui8State = I2C_IDLE;
                if (sDev->fCb)
                    sDev->fCb(sDev->vPtr);
            }
            else
            {
                if (!sDev->fCb)
                {
                    xSemaphoreGiveFromISR(sDev->I2cCmdDone,
                            &lHigherPriorityTaskWoken);
                    if(lHigherPriorityTaskWoken == pdTRUE)
                        portYIELD_FROM_ISR(lHigherPriorityTaskWoken);
                }
                else
                {
                    sDev->fCb(sDev->vPtr);
                    sDev->ui8State = I2C_IDLE;
                }
            }

        }

    }
}

/*
 * ISR mapped to interrupt vector table
 */
void I2C0_ISR(void)
{
    I2cIntrHandler(0);
}

/*
 * ISR mapped to interrupt vector table
 */
void I2C1_ISR(void)
{
    I2cIntrHandler(1);
}

/*
 * Setup NVIC
 */
static void I2cSetNvic(uint8_t ui8I2c)
{
    if (ui8I2c) {
        NVIC_SetPriority(I2C1_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
        NVIC_EnableIRQ(I2C1_IRQn);
    } else {
        NVIC_SetPriority(I2C0_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
        NVIC_EnableIRQ(I2C0_IRQn);
    }

}
static tEtaStatus I2cInit(tI2cNum iNum, tI2cClk iClockSpeed)
{
    tEtaStatus iResult;

    iResult = I2cSpeedConfig(iNum, iClockSpeed);
    if(iResult != eEtaSuccess)
    {
        return(iResult);
    }
    REGN_M1(iNum, I2C_CONFIG2, SOFT_RST, 1);
    REGN_M1(iNum, I2C_CONFIG2, SOFT_RST, 0);
    return iResult;

}
/**
 * Initialize a new i2c device with the I2C number.
 *
 * @param ui8I2c The number of the I2C device being initialized
 * @param cfg The hardware specific configuration structure to configure
 *            the I2C with.  This includes things like pin configuration.
 *
 * @return 0 on success, and non-zero error code on failure
 */
__attribute__((section(".initSection"))) int32_t HalI2cInit(uint8_t ui8I2c)
{
    struct sDevI2c *sDev;

    sDev = I2cGetDev(ui8I2c);

    if (!sDev)
        return -ENODEV;

    sDev->I2cLock = xSemaphoreCreateMutex();
    sDev->I2cCmdDone = xSemaphoreCreateBinary();
    sDev->ui8PendingTxByte = 0;
    /* TX RX FIFO interrupt level 1 Byte*/
    REGN_M1(ui8I2c, I2C_CONFIG, RX_FIFO_INT_LEVEL, 1);
    if(ui8I2c) {
#ifdef CONFIG_I2C_1_FREQ
        I2cInit(ui8I2c, CONFIG_I2C_1_FREQ);
#else
        I2cInit(ui8I2c, eI2cClk400kHz);
#endif
    }
    else
    {
#ifdef CONFIG_I2C_0_FREQ
        I2cInit(ui8I2c, CONFIG_I2C_0_FREQ);
#else
        I2cInit(ui8I2c, eI2cClk400kHz);
#endif
    }
    I2cSetNvic(ui8I2c);
    REGN_M1(ui8I2c, I2C_CONFIG, IGNORE_NACK, 1);
    return 0;
}
