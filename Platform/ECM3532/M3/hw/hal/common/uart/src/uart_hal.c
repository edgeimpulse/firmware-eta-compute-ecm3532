/*******************************************************************************
*
* @file uart_hal.c
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
#include "eta_csp_uart.h"
#include "uart_hal.h"
#include "errno.h"
#ifdef CONFIG_ECM3531
#include "ecm3531.h"
#include "reg_eta_ecm3531_m3.h"
#elif CONFIG_ECM3532
#include "ecm3532.h"
#endif
#include "cm3.h"
#include "FreeRTOS.h"
#include "semphr.h"

#define TX_FIFO_SZ 16
#ifndef CONFIG_UART_CNT
#define CONFIG_UART_CNT (0)
#endif

#define UART_INDEX(x) (x - CONFIG_UART_INDEX_OFFSET)

/*
 * Uart Struct
 */
typedef struct {
    uint8_t *ui8RxBuf;
    uint8_t *ui8TxBuf;
    int32_t ui32UTxCnt;
    int32_t ui32URxCnt;
    tHalUartTxDoneCb fTxDone;
    tHalUartRxDoneCb fRxDone;
    void *vRxCbArg;
    void *vTxCbArg;
    tHalUartAsyncRecvCh fRxAsyncCb[CONFIG_UART_ASYNC_RECV_MAX_CNT];
    uint8_t ui8AsyncRecvCnt;
    tUart sCspUdev;
    SemaphoreHandle_t xTxSem;
    SemaphoreHandle_t xRxSem;
    SemaphoreHandle_t xUartSem;
    uint8_t u8PortInit:1;
    volatile uint8_t u8TxStarted:1;
    volatile uint8_t u8RxStarted:1;
} tECM3531Uart;

static tECM3531Uart sUPort[CONFIG_UART_CNT];

/*
 * Uart TX Interrupt enable function
 */
static inline void
ECM3531UartEnTxInt(uint8_t ui8Port)
{
    EtaCspUartIntEnable(&sUPort[UART_INDEX(ui8Port)].sCspUdev,
            BM_UART_INT_ENABLE_TX_IDLE | BM_UART_INT_ENABLE_TX_FIFO_LWM);
}

/*
 * Uart TX Interrupt disable function
 */
static inline void
ECM3531UartDisTxInt(uint8_t ui8Port)
{
    EtaCspUartIntDisable(&sUPort[UART_INDEX(ui8Port)].sCspUdev,
            BM_UART_INT_ENABLE_TX_IDLE | BM_UART_INT_ENABLE_TX_FIFO_LWM);
}

/*
 * Uart RX Interrupt enable function
 */
static inline void
ECM3531UartEnRxInt(uint8_t ui8Port)
{
    EtaCspUartIntEnable(&sUPort[UART_INDEX(ui8Port)].sCspUdev,
            BM_UART_INT_ENABLE_RX_FIFO_TO | BM_UART_INT_ENABLE_RX_FIFO_HWM);
}

/*
 * Uart RX Interrupt disable function
 */
static inline void
ECM3531UartDisRxInt(uint8_t ui8Port)
{
    tECM3531Uart *sUp;
    sUp = &sUPort[UART_INDEX(ui8Port)];
    if (!sUp->ui8AsyncRecvCnt)
    {
        EtaCspUartIntDisable(&sUp->sCspUdev,
                                BM_UART_INT_ENABLE_RX_FIFO_TO |
                                BM_UART_INT_ENABLE_RX_FIFO_HWM);
    }
}

/*
 * UART Async Cb Caller
 * will call registered cb functions from ISR
 */
void ECM3531UartCallAsyncRecv(uint8_t ui8Port, uint8_t ui8Data)
{
    uint8_t ui8Cnt = 0, ui8Idx = 0;
    tECM3531Uart *sUp;

    sUp = &sUPort[UART_INDEX(ui8Port)];
    if(sUp->ui8AsyncRecvCnt)
        ui8Cnt = sUp->ui8AsyncRecvCnt;

    while(ui8Cnt)
    {

        if(sUp->fRxAsyncCb[ui8Idx])
        {
            sUp->fRxAsyncCb[ui8Idx](ui8Data);
            ui8Cnt--;
            ui8Idx++;
        }

    }

}

/*
 * UART ISR handler
 */
void
ECM3531UartIsr(uint8_t ui8Port)
{
    uint32_t ui32Data;
    tECM3531Uart *sUp;
    uint8_t ui8Cnt, ui8data;
    uint8_t tx_fifo_cur_level = 0;
    uint8_t tx_fifo_space = 0;
    long lHigherPriorityTaskWoken = pdFALSE;

    sUp = &sUPort[UART_INDEX(ui8Port)];

    ui32Data = REG_UART_INT_STAT(ui8Port).V;
    if (ui32Data & (BM_UART_INT_STAT_RX_FIFO_HWM |
                BM_UART_INT_STAT_RX_FIFO_TO)) {
        /* async receive along with readbuf */
        if (sUp->u8RxStarted)
        {
            while((REG_UART_RX_FIFO(ui8Port).BF.DEPTH) &&
                    (sUp->ui32URxCnt))
            {
                ui8data = REG_UART_RX_FIFO(ui8Port).BF.DATA;
                if(sUp->ui8AsyncRecvCnt)
                    ECM3531UartCallAsyncRecv(ui8Port, ui8data);

                *sUp->ui8RxBuf++ = ui8data;
                /* drain the RX_FIFO */
                REG_UART_RX_FIFO(ui8Port).BF.DATA = 0;
                sUp->ui32URxCnt--;
                if (!sUp->ui32URxCnt) {
                    sUp->u8RxStarted = 0;
                    break;
                }
            }

          if (!sUp->ui32URxCnt) {
            if (sUp->fRxDone)
                    sUp->fRxDone(sUp->vRxCbArg);
            else
                xSemaphoreGiveFromISR(sUp->xRxSem,
                        &lHigherPriorityTaskWoken);
                portYIELD_FROM_ISR(lHigherPriorityTaskWoken);
            }
        }
        /* only async receive no readbuf */
        else if(sUp->ui8AsyncRecvCnt) {
            while(REG_UART_RX_FIFO(ui8Port).BF.DEPTH) {
                ui8data = REG_UART_RX_FIFO(ui8Port).BF.DATA;
                /* drain the RX_FIFO */
                REG_UART_RX_FIFO(ui8Port).BF.DATA = 0;
                ECM3531UartCallAsyncRecv(ui8Port, ui8data);
            }
        }
        /* no one is listning for RX ui8data */
        else {
            ECM3531UartDisRxInt(ui8Port);
        }
        // clear interrupt status bit
        REGN_W2(ui8Port, UART_INT_STAT_CLEAR,
                    RX_FIFO_TO, 1, RX_FIFO_HWM, 1);
    }
    if ((ui32Data & BM_UART_INT_STAT_TX_IDLE) ||
            (ui32Data & BM_UART_INT_STAT_TX_FIFO_LWM))
    {
        if(sUp->u8TxStarted)
        {
            REGN_W2(ui8Port, UART_INT_STAT_CLEAR,
                    TX_IDLE, 1, TX_FIFO_LWM, 1);

            if (sUp->ui32UTxCnt) {
                tx_fifo_cur_level =
                    REG_UART_TX_FIFO(ui8Port).BF.COUNT;
                tx_fifo_space =
                    TX_FIFO_SZ - tx_fifo_cur_level;
                if (sUp->ui32UTxCnt > tx_fifo_space)
                    ui8Cnt = tx_fifo_space;
                else
                    ui8Cnt = sUp->ui32UTxCnt;
                EtaCspUartTx(&sUp->sCspUdev,
                        (char *)sUp->ui8TxBuf, ui8Cnt);
                sUp->ui8TxBuf = &sUp->ui8TxBuf[ui8Cnt];
                sUp->ui32UTxCnt -= ui8Cnt;

            } else {
                /* TX_DONE */
                ECM3531UartDisTxInt(ui8Port);
                if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)
                {
                    if (sUp->fTxDone)
                        sUp->fTxDone(sUp->vTxCbArg);
                    sUp->u8TxStarted = 0;

                }
                else
                {
                    if (sUp->fTxDone)
                    {
                        sUp->fTxDone(sUp->vTxCbArg);
                        sUp->u8TxStarted = 0;
                    }
                    else
                    {
                        xSemaphoreGiveFromISR(sUp->xTxSem,
                                &lHigherPriorityTaskWoken);
                    }
                }
            }
        }
    }

    if(lHigherPriorityTaskWoken == pdTRUE)
        portYIELD_FROM_ISR(lHigherPriorityTaskWoken);
}

void UART0_ISR(void)
{
#ifdef CONFIG_UART_0
    ECM3531UartIsr(0);
#endif
}

void UART1_ISR(void)
{
#ifdef CONFIG_UART_1
    ECM3531UartIsr(1);
#endif
}

/*
 * UART Register receive callback
 * Client Id once registered need to be de register in order to re register
 * Client id is limited to (CONFIG_UART_ASYNC_RECV_MAX_CNT - 1)
 *
 */
int32_t HalUartRegisterAsyncRecvCb(uint8_t ui8Port, uint8_t ui8ReceiverId,
                                    tHalUartAsyncRecvCh fAsyncRCb)
{
    int32_t i32Ret = 0;
    tECM3531Uart *sUp;

    if ((ui8Port > CONFIG_UART_CNT) ||
        (ui8ReceiverId >= CONFIG_UART_ASYNC_RECV_MAX_CNT))
    {
        return -EINVAL;
    }

    sUp = &sUPort[UART_INDEX(ui8Port)];

    xSemaphoreTake(sUp->xUartSem, portMAX_DELAY);
    if(sUp->fRxAsyncCb[ui8ReceiverId])
    {
       i32Ret = -EBUSY;
       goto exit;
    }
    sUp->fRxAsyncCb[ui8ReceiverId] = fAsyncRCb;
    sUp->ui8AsyncRecvCnt++;
    ECM3531UartEnRxInt(ui8Port);
exit:
    xSemaphoreGive(sUp->xUartSem);
    return i32Ret;
}

/*
 * UART De Register receive callback
 * will remove client callback from list
 */
int32_t HalUartDeRegisterAsyncRecvCb(uint8_t ui8Port, uint8_t ui8ReceiverId)
{
    int32_t i32Ret = 0;
    tECM3531Uart *sUp;

    if ((ui8Port > CONFIG_UART_CNT) ||
        (ui8ReceiverId >= CONFIG_UART_ASYNC_RECV_MAX_CNT))
    {
        return -EINVAL;
    }

    sUp = &sUPort[UART_INDEX(ui8Port)];

    xSemaphoreTake(sUp->xUartSem, portMAX_DELAY);
    if((!sUp->fRxAsyncCb[ui8ReceiverId]) ||
            !sUp->ui8AsyncRecvCnt)
    {
       i32Ret = -EINVAL;
       goto exit;
    }
    sUp->fRxAsyncCb[ui8ReceiverId] = NULL;
    sUp->ui8AsyncRecvCnt--;
    ECM3531UartDisRxInt(ui8Port);
exit:
    xSemaphoreGive(sUp->xUartSem);
    return i32Ret;
}

/*
 * Write buffer to UART
 * Non blocking if callback is not NULL
 * blocking if callback is NULL
 */
int32_t HalUartWriteBuf(uint8_t ui8Port, uint8_t *ui8TxBuf,
                        uint32_t ui32TxCnt, tHalUartTxDoneCb fTxDoneCb,
                        void *vCbArg)
{
    uint32_t ui32Cnt;
    tECM3531Uart *sUp;
    volatile int32_t i32Ret = -1;
    uint32_t ui32TxFifoSpace = 0;
    uint32_t ui32TxFifoCurLevel = 0;

    if (ui8Port > CONFIG_UART_CNT)
    {
        return -EINVAL;
    }
    if ((!ui8TxBuf) || (!ui32TxCnt))
        return 0;

    sUp = &sUPort[UART_INDEX(ui8Port)];
    if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING)
        xSemaphoreTake(sUp->xUartSem, portMAX_DELAY);
    else
        NVIC_SetPriority(UART0_IRQn + ui8Port, (1 << __NVIC_PRIO_BITS));

    if (!sUp->u8TxStarted) {
        ui32TxFifoCurLevel = REG_UART_TX_FIFO(ui8Port).BF.COUNT;
        ui32TxFifoSpace =  TX_FIFO_SZ - ui32TxFifoCurLevel;
        if (ui32TxCnt > ui32TxFifoSpace)
        {
            ui32Cnt = ui32TxFifoSpace;
            /* for next xfer */
            sUp->ui8TxBuf = &ui8TxBuf[ui32Cnt];
            sUp->ui32UTxCnt = ui32TxCnt - ui32Cnt;
        }
        else
        {
            sUp->ui32UTxCnt = 0;
            ui32Cnt = ui32TxCnt;
        }
        EtaCspUartTx(&sUp->sCspUdev, (char *)ui8TxBuf, ui32Cnt);
        sUp->u8TxStarted = 1;

        if (fTxDoneCb)
        {
            sUp->fTxDone = fTxDoneCb;
            sUp->vTxCbArg = vCbArg;
            ECM3531UartEnTxInt(ui8Port);
        }
        else
        {
            sUp->fTxDone = NULL;
            ECM3531UartEnTxInt(ui8Port);
            if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING)
            {
                do {
                    i32Ret  = xSemaphoreTake(sUp->xTxSem, 1);
                } while  (!i32Ret);
                sUp->u8TxStarted = 0;
            }
            else
            {
                while(sUp->u8TxStarted);
                NVIC_SetPriority(UART0_IRQn + ui8Port, (1 << __NVIC_PRIO_BITS) - 1);
            }
        }

        i32Ret = 0;
    }
    else
    {
        i32Ret = -EBUSY;
    }
    if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING)
        xSemaphoreGive(sUp->xUartSem);

    return i32Ret;
}

/*
 * Read buffer from UART
 * Non blocking if callback is not NULL
 * blocking if callback is NULL
 */
int32_t HalUartReadBuf(uint8_t ui8Port, uint8_t *ui8RxB,
                        uint32_t ui32RxCnt, tHalUartRxDoneCb fRxCb,
                        void *vCbArg)
{
    tECM3531Uart *sUp;

    if (ui8Port > CONFIG_UART_CNT)
    {
        return -EINVAL;
    }

    sUp = &sUPort[UART_INDEX(ui8Port)];
    if (!sUp->u8RxStarted)
    {
        sUp->ui8RxBuf = ui8RxB;
        sUp->ui32URxCnt = ui32RxCnt;
        sUp->u8RxStarted = 1;
        ECM3531UartEnRxInt(ui8Port);
        /* Blocking case */
        if(fRxCb == NULL)
        {
            sUp->fRxDone = NULL;
            xSemaphoreTake(sUp->xRxSem, portMAX_DELAY);
            ECM3531UartDisRxInt(ui8Port);
            sUp->u8RxStarted = 0;
        }
        else
        {
            sUp->fRxDone = fRxCb;
            sUp->vRxCbArg = vCbArg;
        }
    }
    return 0;
}
#if 0
/*
 * Get current UART port coniiguration
 */
int32_t HalUartGetCfg(int8_t ui8Port, tHalUartPortCfg *sCfg)
{
    tECM3531Uart *sUp;
    tUartParityType iPty;
    tUartFlowControl iFc;
    bool bParityEnable;

    if ((ui8Port > CONFIG_UART_CNT) || (!sCfg))
    {
        return -EINVAL;
    }

    sUp = &sUPort[ui8Port];
    xSemaphoreTake(sUp->xUartSem, portMAX_DELAY);
    sCfg->iBRate = sUp->sCspUdev.iBaud;
    /*
     *  ECM3531 support only 2 Stop bits
     *  Ignore input value
     */
    sCfg->iSBits = HalUartTwoStopBits;

    bParityEnable = REG_UART_CFG_STATUS(ui8Port).BF.RX_TX_PARITY;
    if (!bParityEnable)
    {
        sCfg->iParity = HalUartParityNone;
    }
    else
    {
        iPty = REG_UART_CFG_STATUS(ui8Port).BF.TX_PARITY_TYPE;
        if (iPty)
                sCfg->iParity = HalUartParityEven;
        else
                sCfg->iParity = HalUartParityOdd;
    }
    iFc = REG_UART_CFG_STATUS(ui8Port).BF.CTS;
    switch (iFc) {
        case eUartFlowControlNone:
            sCfg->iFCtl = HalUartFlowCtlNone;
            break;
        case eUartFlowControlHardware:
            sCfg->iFCtl = HalUartFlowCtlRtsCts;
            break;
        default :
            sCfg->iFCtl = HalUartFlowCtlNone;
            break;
    }

    xSemaphoreGive(sUp->xUartSem);
    return 0;
}

/*
 * Set UART port coniiguration
 */
int32_t HalUartSetCfg(uint8_t ui8Port, tHalUartPortCfg *sCfg)
{
    tECM3531Uart *sUp;
    tUartFlowControl iFc;
    tUartParityType iPty;
    tHalUartStopBits iSBits;

    if (ui8Port > CONFIG_UART_CNT) {
        return -EINVAL;
    }

    sUp = &sUPort[ui8Port];
    xSemaphoreTake(sUp->xUartSem, portMAX_DELAY);

    switch (sCfg->iFCtl) {
        case HalUartFlowCtlNone:
            iFc = eUartFlowControlNone;
            break;
        case HalUartFlowCtlRtsCts:
            iFc = eUartFlowControlHardware;
            break;
        default :
            iFc = eUartFlowControlNone;
            break;
    }
    switch (sCfg->iParity) {
        case HalUartParityOdd:
            iPty = eUartParityOdd;
            break;
        case HalUartParityEven:
            iPty = eUartParityEven;
            break;
        case HalUartParityNone:
        default :
            iPty = eUartParityNone;
            break;
    }
    /*
     *  ECM3531 support only 2 Stop bits
     *  Ignore input value
     */
    iSBits = HalUartTwoStopBits;

    sUp->sCspUdev.iNum = ui8Port;
    sUp->sCspUdev.iBaud = sCfg->iBRate;

    EtaCspUartCfg(&sUp->sCspUdev, iFc, iPty, iSBits);

    ECM3531UartEnRxInt(ui8Port);

    sUp->u8TxStarted = 0;
    sUp->u8RxStarted = 0;

    xSemaphoreGive(sUp->xUartSem);
    return 0;
}
#endif
/**
 * Setup NVIC for UART Port
 */
static void
ECM3531UartSetIrq(uint8_t ui8Port)
{
    if (ui8Port) {
        NVIC_SetPriority(UART1_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
        NVIC_EnableIRQ(UART1_IRQn);
    } else {
        NVIC_SetPriority(UART0_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
        NVIC_EnableIRQ(UART0_IRQn);
    }

}

/*
 * Initialize UART port with
 * default coniiguration
 */
int32_t HalUartInit(uint8_t ui8Port)
{
    tHalUartPortCfg sPCfg;
    tECM3531Uart *sUp;

    if (ui8Port > CONFIG_UART_CNT)
        return -EINVAL;
    sUp = &sUPort[UART_INDEX(ui8Port)];

    sUp->xTxSem = xSemaphoreCreateBinary();
    sUp->xUartSem = xSemaphoreCreateMutex();
    sUp->xRxSem = xSemaphoreCreateBinary();
    if (ui8Port)
    {
#ifdef CONFIG_UART_1
        sPCfg.iBRate = CONFIG_UART1_DEFAULT_BAUD_RATE;
        sPCfg.iParity = CONFIG_UART1_DEFAULT_PARITY;
        sPCfg.iFCtl = CONFIG_UART1_DEFAULT_FLOW_CTRL;
        sPCfg.iSBits = CONFIG_UART1_DEFAULT_STOP_BITS;
#endif
    }
    else
    {
#ifdef CONFIG_UART_0
        sPCfg.iBRate = CONFIG_UART0_DEFAULT_BAUD_RATE;
        sPCfg.iParity = CONFIG_UART0_DEFAULT_PARITY;
        sPCfg.iFCtl = CONFIG_UART0_DEFAULT_FLOW_CTRL;
        sPCfg.iSBits = CONFIG_UART0_DEFAULT_STOP_BITS;
#endif
    }
    ECM3531UartSetIrq(ui8Port);

    EtaCspUartInit(&sUp->sCspUdev, ui8Port, sPCfg.iBRate, sPCfg.iFCtl);

    //HalUartSetCfg(ui8Port, &sPCfg);
    return 0;
}
