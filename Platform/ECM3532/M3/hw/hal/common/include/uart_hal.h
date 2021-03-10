/*******************************************************************************
*
* @file uart_hal.h
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
#ifndef H_UART_HAL_H_
#define H_UART_HAL_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/** Uart parity type */
typedef enum {
    /** Odd parity */
    HalUartParityOdd = 0,
    /** Even parity */
    HalUartParityEven = 1,
    /** No Parity */
    HalUartParityNone = 2,
}tHalUartParity;

/** Uart Flow Ctrl */
typedef enum {
    /** No Flow Control */
    HalUartFlowCtlNone = 0,
    /** HW Flow Control*/
    HalUartFlowCtlRtsCts = 1
}tHalUartFlowCtl;

/** Supported Uart Baud Rate */
typedef enum
{
    /** UART baud of 9,600 */
    HalUartBaud9600 = 9600,
    /** UART baud of 38,400 */
    HalUartBaud38400 = 38400,
    /** UART baud of 57,600 */
    HalUartBaud57600 = 57600,
    /** UART baud of 115,200 */
    HalUartBaud115200 = 115200,
    /** UART baud of 234,000 */
    HalUartBaud230400 = 234000,
    /** UART baud of 460,800 */
    HalUartBaud460800 = 460800,
}tHalUartBaud;

/**
 * Uart Stop Bits
 * ECM3531 support only 2 stop bits
 * place holder for future
 */
typedef enum {
    /** Two Stop Bits */
    HalUartTwoStopBits = 2,
}tHalUartStopBits;

/** Uart Port config structure
 * required to get and set Uart port config
 */
typedef struct {
    /** UART baud rate*/
    tHalUartBaud iBRate;
    /** UART parity*/
    tHalUartParity iParity;
    /** UART flow control*/
    tHalUartFlowCtl iFCtl;
    /** UART stop bits*/
    tHalUartStopBits iSBits;
}tHalUartPortCfg;

/**
 * Function prototype for transmission complete callback.
 * This will called when transmission is complete
 * Callback are called from interrupt context,no blocking
 * call should be done inside callaback.
 * print is not allowed inside callack, will result in
 * deadlock
 *
 */
typedef void (*tHalUartTxDoneCb)(void *);

/**
 * Function prototype for receive complete, required for
 * asynchronous read call API. This is passed
 * as argument of HalUartReadBuf, to receive number of bytes
 * in non blocking mode.
 * Callback will be called from Interrupt context, callback should
 * do only minimum to save input data and return, no blocking routinue
 * are allowed in asynchronous receive callback
 * print is not allowed inside callack, will result in
 * deadlock
 * tHalUartAsyncRecvCh if registered will be called before tHalUartRxDoneCb
 */
typedef void (*tHalUartRxDoneCb)(void *);

/**
 * Function prototype for UART driver to report incoming byte of data
 * for Asynchronous receive of data. Listener has to register callback
 * to listen to any input data without calling read API
 * Callback will be called from Interrupt context, callback should
 * do only minimum to save input data and return, no blocking routinue
 * are allowed in asynchronous receive callback
 * tHalUartAsyncRecvCh will be called before tHalUartRxDoneCb
 * print is not allowed inside callack, will result in
 * deadlock
 *
 * @return 0 on success, non-zero error code on failure
 */
typedef void (*tHalUartAsyncRecvCh)(uint8_t i8Ch);

/**
 * Register Asynchronous receive callback
 *
 * @param ui8Port       Uart port number
 * @param ui8ReceiverId Id of receiver registered to recevie callback,
 * id starts from 0,can go max up to (CONFIG_UART_ASYNC_RECV_MAX_CNT - 1)
 * @param fAsyncRCb     function to be called on received of each byte,
 * function will be called from ISR context, for each byte received, callback
 * should do only minimum
 *
 * @return 0 on success, non-zero error code on failure
 */
int32_t HalUartRegisterAsyncRecvCb(uint8_t ui8Port,
        uint8_t ui8ReceiverId, tHalUartAsyncRecvCh fAsyncRCb);

/**
 * De-Register Asynchronous receive callback
 *
 * @param ui8Port       Uart port number
 * @param ui8ReceiverId Id of receiver to be registered
 *
 * @return 0 on success, non-zero error code on failure
 */
int32_t HalUartDeRegisterAsyncRecvCb(uint8_t ui8Port, uint8_t ui8ReceiverId);
/**
 * Initialize the HAL uart, with default parameters
 *
 * @param ui8Port   Uart port number to be initialize
 *
 * @return 0 on success, non-zero error code on failure
 */
int32_t HalUartInit(uint8_t ui8Port);

/**
 * Applies given configuration to UART Port.
 *
 * @param ui8Port   UART port number to configure
 * @param sCfg      UART port configure structure
 *
 * @return 0 on success, non-zero error code on failure
 */
int32_t HalUartSetCfg(uint8_t ui8Port, tHalUartPortCfg *sCfg);

/**
 * Get configuration of UART Port.
 *
 * @param ui8Port The UART port number to configure
 * @param sCfg uart port configure structure
 *
 * @return 0 on success, non-zero error code on failure
 */
int32_t HalUartGetCfg(int8_t ui8Port, tHalUartPortCfg *sCfg);

/**
 * Read Data from UART Port, If receive callback is not NULL
 * it will be non blocking, if receive callback is NULL it will be
 * blocking till requested numbe of bytes are received
 *
 * @param ui8Port   UART port number to configure
 * @param ui8RxBuf  Buffer in which received data will be stored
 * @param ui32RxCnt Number of bytes to receive
 * @param fRxCb     Receive complete callback function
 * @param vCbArg    Argument to be called as parameter receive callback
 *
 * @return 0 on success, non-zero error code on failure
 */
int32_t HalUartReadBuf(uint8_t ui8Port, uint8_t *ui8RxBuf,
                        uint32_t ui32RxCnt, tHalUartRxDoneCb fRxCb,
                        void *vCbArg);

/**
 * Transmit Data from UART Port, If Transmit callback if NULL
 * it will wait for TX Done, if it is not NULL it will return
 * after writing data to FiFO, once TX is complete Callback will
 * be called to signal completion of TX
 *
 * @param ui8Port   UART port number to configure
 * @param ui8TxBuf  buffer in which received data will be stored
 * @param ui32TxCnt Number of bytes to xmit
 * @param fTxDoneCb receive complete callback function
 * @param vCbArg    Argument to be called as parameter receive callback
 *
 * @return 0 on success, non-zero error code on failure
 */
int32_t HalUartWriteBuf(uint8_t ui8Port, uint8_t *ui8TxBuf,
                        uint32_t ui32TxCnt, tHalUartTxDoneCb fTxDoneCb,
                        void *vCbArg);
#ifdef __cplusplus
}
#endif

#endif /* H_UART_HAL_H_ */
