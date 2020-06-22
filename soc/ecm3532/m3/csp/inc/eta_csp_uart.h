/***************************************************************************//**
 *
 * @file eta_csp_uart.h
 *
 * @brief This file contains eta_csp_uart module definitions.
 *
 * Copyright (C) 2018 Eta Compute, Inc
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
 * This is part of revision 1.0.0a6 of the Tensai Software Development Kit.
 *
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup ecm3532uart-m3 Universal Asynchronous Receiver/Transmitter (UART)
 * @ingroup ecm3532csp-m3
 * @{
 ******************************************************************************/

#ifndef __ETA_CSP_UART_H__
#define __ETA_CSP_UART_H__

#include <stdint.h>
#include "eta_csp.h"
#include "eta_csp_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UART_TX_FIFO_DEPTH          16U
#define UART_RX_FIFO_DEPTH          16U

//FIXME remove #define ETA_CSP_UART_CLK_HZ       8000000ULL
//FIXME remove #define UART_BAUD_DIVISOR(ui32Baud) ((ETA_CSP_UART_CLK_HZ / ui32Baud) - 1U)
#define UART_INT_DIV(target_baud,src_clk_freq)          (src_clk_freq / target_baud)
#define UART_DEC_DIV(int_div, target_baud,src_clk_freq) (((((src_clk_freq*100) / target_baud) - (int_div*100))+5)/10)
#define UART_DEC_PHSE(dec_div)                          (dec_div == 0 ? 0x00 :             \
                                                         dec_div == 1 ? 0x08 :             \
                                                         dec_div == 2 ? 0x88 :             \
                                                         dec_div == 3 ? 0xa8 :             \
                                                         dec_div == 4 ? 0xaa :             \
                                                         dec_div == 5 ? 0xba :             \
                                                         dec_div == 6 ? 0xbb :             \
                                                         dec_div == 7 ? 0xfb :             \
                                                         dec_div == 8 ? 0xff :             \
                                                         dec_div == 9 ? 0xff : 0xff) // Note, 9 is same as 8.

#define UART_DEC_DIV_PHSE(target_baud,src_clk_freq) UART_DEC_PHSE(UART_DEC_DIV(UART_INT_DIV(target_baud,src_clk_freq), target_baud, src_clk_freq))

//
//! UART number typedef.
//
typedef enum
{
    //
    //! UART 0.
    //
    eUartNum0 = 0L,

    //
    //! UART 1.
    //
    eUartNum1 = 1L,
}
tUartNum;

//
//! UART number typedef.
//
typedef enum
{
    //
    //! UART baud of 9,600.
    //
    eUartBaud9600 = 9600L,

    //
    //! UART baud of 57,600.
    //
    eUartBaud57600 = 57600L,

    //
    //! UART baud of 115,200.
    //
    eUartBaud115200 = 115200L,

    //
    //! UART baud of 234,000.
    //
    eUartBaud234000 = 234000L,

    //
    //! UART baud of 460,800.
    //
    eUartBaud460800 = 460800L,
    //
    //! UART baud of 921,600.
    //
    eUartBaud921600 = 921600L,
}
tUartBaud;

//
//! UART flow control typedef.
//
typedef enum
{
    //
    //! UART flow control off.
    //
    eUartFlowControlNone = 0L,

    //
    //! UART hardware flow control.
    //
    eUartFlowControlHardware = 1L,
}
tUartFlowControl;

//
//! UART Interrupt Status Enums (one hot encoded)
//
typedef enum
{
    //
    //! UART INT RX FIFO HIGH WATER MARK
    //
    eUartIntRxFifoHwm = 0x0001L,

    //
    //! UART INT RX FIFO TIME OUT
    //
    eUartIntRxTO = 0x0002L,

    //
    //! UART INT END OF BREAK
    //
    eUartIntEOB = 0x0004L,

    //
    //! UART INT TX FIFO LOW WATER MARK
    //
    eUartIntTxFifoLwm = 0x0008L,

    //
    //! UART INT TX IDLE
    //
    eUartIntTxFifoIdle = 0x0010L,

    //
    //! UART INT RX ERROR
    //
    eUartIntRxError = 0x0020L,

}
tUartIntStatus;

//
//! UART structure.
//
typedef struct
{
    //
    //! The UART number.
    //
    tUartNum iNum;

    //
    //! The UART baud rate.
    //
    tUartBaud iBaud;

    //
    // Which pin to use for TX
    //
    tGpioBit  iTxPin;

    //
    // Which pin to use for RX
    //
    tGpioBit  iRxPin;

    //
    // Which pin to use for RTS
    //
    tGpioBit  iRtsPin;

    //
    // Which pin to use for CTS
    //
    tGpioBit  iCtsPin;

}
tUart;

//
// This fence will compile inline and do only one byte reads from the UART block.
//
#define EtaCspUartFenceFast() \
                             {REG_GPIO_DEBUG0_READ_ONLY.V = MEMIO8(REG_UART_CFG_STATUS_ADDR(0));}

//
// Add a fence on the last bus operation to SOC Ctrl registers.
//
extern void EtaCspUartFence(void);


//
// Initialize the UART.
//
extern void EtaCspUartInit(tUart *psUart, tUartNum iNum, tUartBaud iBaud,
                           tUartFlowControl iFlowControl);

//
// Set the baud.
//
extern void EtaCspUartBaudSet(tUart *psUart, tUartBaud iBaud);

//
//  EtaCspUartTx - Transmit until the requested size.
//
extern void EtaCspUartTx(tUart *psUart, char *cData, uint32_t ui32Len);

//
// Send a character.
//
extern void EtaCspUartPutc(tUart *psUart, char cData);

//
// Send a string.
//
extern void EtaCspUartPuts(tUart *psUart, char *cData);

//
// Receive a character.
//
extern char EtaCspUartGetc(tUart *psUart, bool bPop);

//
// Wait for a character to be available then read and return it.
//
extern char EtaCspUartGetcWait(tUart *psUart, bool bPop);

//
// Wait for Tx done.
//
extern void EtaCspUartTxWait(tUart *psUart);

//
// Turn the UART off to save power.
//
extern void EtaCspUartOff(tUart *psUart, tGpioBit iTxPin);

//
// Enable interrupts.
//
extern void EtaCspUartIntEnable(tUart *psUart, uint8_t ui8Interrupts);

//
// Disable interrupts.
//
extern void EtaCspUartIntDisable(tUart *psUart, uint8_t ui8Interrupts);

//
// Get the interrupt status.
//
extern tUartIntStatus EtaCspUartIntStatusGet(tUart *psUart, bool bClearAfterRead);

//
// Clear the interrupt status.
//
extern void EtaCspUartIntStatusClear(tUart *psUart, uint8_t ui8Interrupts);

//
// Get the RX FIFO depth.
//
extern uint32_t EtaCspUartRxFifoDepthGet(tUart *psUart);

//
// Enable UART signals to/from chip pads/pins.
//
extern tEtaStatus EtaCspUartPinSelect(tUart *psUart,
                                      tGpioBit  iTxPin,
                                      tGpioBit  iRxPin,
                                      tGpioBit  iRtsPin,
                                      tGpioBit  iCtsPin);

#ifdef __cplusplus
}
#endif

#endif // __ETA_CSP_UART_H__

