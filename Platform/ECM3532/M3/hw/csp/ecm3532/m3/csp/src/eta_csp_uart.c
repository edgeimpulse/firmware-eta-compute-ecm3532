/***************************************************************************//**
 *
 * @file eta_csp_uart.c
 *
 * @brief This file contains eta_csp_uart module implementations.
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
 * This is part of revision 1.0.0b1 of the Tensai Chip Support Package.
 *
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup ecm3532uart-m3 Universal Asynchronous Receiver/Transmitter (UART)
 * @ingroup ecm3532csp-m3
 * @{
 ******************************************************************************/

#include "eta_chip.h"
#include "eta_csp_uart.h"
#include "eta_csp_socctrl.h"


/***************************************************************************//**
 *
 *  EtaCspUartInit - Initialize the UART.
 *
 *  @param psUart is a pointer to a UART structure.
 *  @param iNum is the UART number.
 *  @param iBaud is the UART baud.
 *  @param iFlowControl is the flow control setting.
 *
 ******************************************************************************/
void
EtaCspUartInit(tUart *psUart, tUartNum iNum, tUartBaud iBaud,
               tUartFlowControl iFlowControl)
{
    bool bFlowControl =
        (iFlowControl == eUartFlowControlHardware) ? TRUE : FALSE;

    //
    // Set the UART number and baud.
    //
    psUart->iNum = iNum;
    psUart->iBaud = iBaud;

    //
    // Write to the UART configuration.
    //
    REGN_W6(psUart->iNum, UART_CFG_STATUS, CTS_POLARITY, 1,
            CTS, bFlowControl,
            TX_EN, TRUE,
            RTS_AUTO, TRUE,
            RTS_POLARITY, TRUE,
            RX_EN, TRUE);
    EtaCspUartBaudSet(psUart, iBaud);
}

/***************************************************************************//**
 *
 *  EtaCspUartBaudSet - Set the baud rate.
 *
 *  @param psUart is a pointer to a UART structure.
 *  @param iBaud is the UART baud.
 *
 ******************************************************************************/
void
EtaCspUartBaudSet(tUart *psUart, tUartBaud iBaud)
{
    uint32_t uBaud, uPhase;
    // Setup UART baud and phase based on UST clock:
    switch (EtaCspSocCtrlUstFreqGet())
    {
      default: // pick one
      case (eSocCtrlUSTSel8MHz) :
           uBaud = UART_INT_DIV(iBaud,8000000)-1;  // baud rate derived from 8MHz
           uPhase = UART_DEC_DIV_PHSE(iBaud,8000000);
           break;
      case (eSocCtrlUSTSel4MHz) :
           uBaud = UART_INT_DIV(iBaud,4000000)-1;  // baud rate derived from 4MHz
           uPhase = UART_DEC_DIV_PHSE(iBaud,4000000);
           break;
      case (eSocCtrlUSTSel2MHz) :
           uBaud = UART_INT_DIV(iBaud,2000000)-1;  // baud rate derived from 2MHz
           uPhase = UART_DEC_DIV_PHSE(iBaud,2000000);
           break;
    }

    REGN_W1(psUart->iNum, UART_BAUDRATE, DIVISOR, uBaud);
    REGN_W2(psUart->iNum, UART_PHASE, SHIFT_TX, uPhase, SHIFT_RX, uPhase);
}

/***************************************************************************//**
 *
 *  EtaCspUartPutc - Send a character.
 *
 *  @param psUart is a pointer to a UART structure.
 *  @param cData is the character to send.
 *
 ******************************************************************************/
void
EtaCspUartPutc(tUart *psUart, char cData)
{

    //
    // Wait for space in TX FIFO.
    //
    while(REG_UART_TX_FIFO(psUart->iNum).BF.COUNT == UART_TX_FIFO_DEPTH)
    {
    }

    //
    // Write to TX FIFO.
    //
    REGN_W1(psUart->iNum, UART_TX_FIFO, DATA, cData);
}

/***************************************************************************//**
 *
 *  EtaCspUartPuts - Send a string.
 *
 *  @param psUart is a pointer to a UART structure.
 *  @param pcData is a pointer to the string.
 *
 ******************************************************************************/
void
EtaCspUartPuts(tUart *psUart, char *pcData)
{
    //
    // Write characters until the NULL.
    //
    while(*pcData != 0)
    {
        EtaCspUartPutc(psUart, *pcData++);
    }
}

/***************************************************************************//**
 *
 *  EtaCspUartTx - Transmit until the requested size.
 *
 *  @param psUart is a pointer to a UART structure.
 *  @param pcData pointer to the string.
 *  @param ui32Size the number of characters to send.
 *
 *  This function will block and transmit until the requested ui32Size is hit.
 *
 ******************************************************************************/
void
EtaCspUartTx(tUart *psUart, char *pcData, uint32_t ui32Size)
{
    uint32_t ui32Sent;

    //
    // Continue to transmit until the requested size is hit.
    //
    ui32Sent = 0;
    while(ui32Sent < ui32Size)
    {
        EtaCspUartPutc(psUart, *pcData++);
        ui32Sent++;
    }
}

/***************************************************************************//**
 *
 *  EtaCspUartGetc - Receive and return a character.
 *
 *  @param psUart is a pointer to a UART structure.
 *  @param bPop boolean to pop the character after reading.
 *  @return Return the character.
 *
 ******************************************************************************/
char
EtaCspUartGetc(tUart *psUart, bool bPop)
{
    char cChar;

    //
    // Get the character.
    //
    cChar = (char)(REG_UART_RX_FIFO(psUart->iNum).BF.DATA & 0x000000ff);

    //
    // Pop it?
    //
    if(bPop)
    {
        REG_UART_RX_FIFO(psUart->iNum).V = 0;
    }

    //
    // Return the character.
    //
    return(cChar);
}

/***************************************************************************//**
 *
 *  EtaCspUartGetcWait - Wait for a character to be available then read and
 *                       return it.
 *
 *  @param psUart is a pointer to a UART structure.
 *  @param bPop boolean to pop the character after reading.
 *  @return Return the character.
 *
 ******************************************************************************/
char
EtaCspUartGetcWait(tUart *psUart, bool bPop)
{
    char cChar;

    //
    // Poll the UART waiting for the FIFO to be not empty.
    //
    while(REG_UART_CFG_STATUS(psUart->iNum).BF.RX_FIFO_EMPTY)
    {
    }

    //
    // Get the character.
    //
    cChar = (char)(REG_UART_RX_FIFO(psUart->iNum).BF.DATA & 0x000000ff);

    //
    // Pop it?
    //
    if(bPop)
    {
        REG_UART_RX_FIFO(psUart->iNum).V = 0;
    }

    //
    // Return the charcter.
    //
    return(cChar);
}

/***************************************************************************//**
 *
 *  EtaCspUartTxWait - Wait (block) until the transmit is complete.
 *
 *  @param psUart is a pointer to a UART structure.
 *
 ******************************************************************************/
void
EtaCspUartTxWait(tUart *psUart)
{
    uint32_t ui32Data;

    //
    // Wait for the UART to be empty.
    //
    do
    {
        ui32Data = REG_UART_TX_FIFO(psUart->iNum).V ^
                   BM_UART_TX_FIFO_FIFO_EMPTY;
    }
    while(ui32Data & (BM_UART_TX_FIFO_TX_BUSY | BM_UART_TX_FIFO_FIFO_EMPTY));
}

/***************************************************************************//**
 *
 *  EtaCspUartOff - Turn the UART off.
 *
 *  @param psUart is a pointer to a UART structure.
 *  @param iTxPin which gpio pin to turn off UART1 TX, if appropriate.
 *
 * NOTE: use the BSP to determine which UART TX pin to turn off.
 ******************************************************************************/
void
EtaCspUartOff(tUart *psUart, tGpioBit iTxPin)
{
    //
    // Turn the UART off.
    //
    REGN_W6(psUart->iNum, UART_CFG_STATUS,
            CTS_POLARITY, 0,
            CTS, 0,
            TX_EN, 0,
            RTS_AUTO, 0,
            RTS_POLARITY, 0,
            RX_EN, 0);
    REGN_W1(psUart->iNum, UART_BAUDRATE, DIVISOR, 0);
    REGN_W2(psUart->iNum, UART_PHASE, SHIFT_TX, 0, SHIFT_RX, 0);

    //
    // Disable the GPIO pin for UART1.
    //
    if(psUart->iNum == eUartNum1)
    {
        EtaCspSocCtrlPadMuxSet(iTxPin, eSocCtrlPadMux0);
    }
}

/***************************************************************************//**
 *
 * EtaCspUartIntEnable - Enable UART interrutps.
 *
 * @param psUart is a pointer to a UART structure.
 * @param ui8Interrupts interrupt(s) to enable.
 *
 ******************************************************************************/
void
EtaCspUartIntEnable(tUart *psUart, uint8_t ui8Interrupts)
{
    REG_UART_INT_ENABLE(psUart->iNum).V |= ui8Interrupts;
}

/***************************************************************************//**
 *
 * EtaCspUartIntDisable - Disable UART interrutps.
 *
 * @param psUart is a pointer to a UART structure.
 * @param ui8Interrupts interrupt(s) to disable.
 *
 ******************************************************************************/
void
EtaCspUartIntDisable(tUart *psUart, uint8_t ui8Interrupts)
{
    REG_UART_INT_ENABLE(psUart->iNum).V &= ~ui8Interrupts;
}

/***************************************************************************//**
 *
 * EtaCspUartIntStatusGet - Get UART interrupt status.
 *
 * @param psUart is a pointer to a UART structure.
 * @param bClearAfterRead bool to clear the status after reading.
 *
 * @return a potentially multiple value enumeration of interrupts present.
 *
 ******************************************************************************/
tUartIntStatus
EtaCspUartIntStatusGet(tUart *psUart, bool bClearAfterRead)
{
    uint32_t ui32Status;

    //
    // Get the status.
    //
    ui32Status = REG_UART_INT_STAT(psUart->iNum).V;

    //
    // Clear the status?
    //
    if(bClearAfterRead)
    {
        REG_UART_INT_STAT_CLEAR(psUart->iNum).V = ui32Status;
    }

    //
    // Return the status.
    //
    return((tUartIntStatus)ui32Status);
}

/***************************************************************************//**
 *
 * EtaCspUartIntStatusClear - Clear UART interrupt status.
 *
 * @param psUart is a pointer to a UART structure.
 * @param ui8Interrupts interrupts to clear.
 *
 ******************************************************************************/
void
EtaCspUartIntStatusClear(tUart *psUart, uint8_t ui8Interrupts)
{
    REG_UART_INT_STAT_CLEAR(psUart->iNum).V = ui8Interrupts;
}

/***************************************************************************//**
 * EtaCspUartRxFifoDepthGet - Get the RX FIFO depth.
 *
 * @param psUart is a pointer to a UART structure.
 * @return RX fifo depth.
 *
 ******************************************************************************/
uint32_t
EtaCspUartRxFifoDepthGet(tUart *psUart)
{
    return(REG_UART_RX_FIFO(psUart->iNum).BF.DEPTH);
}

/***************************************************************************//**
 *
 * EtaCspUartPinSelect - Select Chip Pins for UART signals to use
 *
 * NOTE: if an unusable pin is requested this function will return bad status.
 * NOTE: Either supply a valid tGpioBit or
 * NOTE: UART0 pins are not shared. Use any GPIO designator.
 * NOTE: You can't use this function to disable a PAD MUX decision. Use a
 *       different function to reassing the pin to some other use.
 *
 * @param psUart is a pointer to a UART structure.
 * @param ui8Interrupts interrupts to clear.
 * @return Eta Csp Status
 *
 ******************************************************************************/
tEtaStatus
EtaCspUartPinSelect(tUart *psUart,
                    tGpioBit iTxPin,
                    tGpioBit iRxPin,
                    tGpioBit iRtsPin,
                    tGpioBit iCtsPin)
{
    //
    // Validate UART versus valid pad mux pins
    //
    if(psUart->iNum == eUartNum0)
    {
        //
        // Nothing to do for UART0 pads.
        //
    }
    else if(psUart->iNum == eUartNum1)
    {
        //
        // Check TX pins
        //
        if(iTxPin == eGpioBit7)
        {
            //
            // OK we can assign the request TX PIN to UART1
            //
            REG_M1(SOCCTRL_PMUXA, GPIO7, BFV_SOCCTRL_PMUXA_GPIO7_UART1TX);
        }
        else if(iTxPin == eGpioBit20)
        {
            //
            // OK we can assign the request TX PIN to UART1
            //
            REG_M1(SOCCTRL_PMUXC, GPIO20, BFV_SOCCTRL_PMUXC_GPIO20_UART1TX);
        }
        else if(iTxPin == eGpioBitNone)
        {
            //
            // Nothing to do but a valid case nonetheless.
            //
        }
        else
        {
            //
            // No valid pin selected for UART1 TX
            //
            return(eEtaUartInvalidPin);
        }

        //
        // Now check the request for the UART 1 RX pin
        //
        if(iRxPin == eGpioBit3)
        {
            //
            // OK we can assign the request RX PIN to UART1
            //
            REG_M1(SOCCTRL_PMUXA, GPIO3, BFV_SOCCTRL_PMUXA_GPIO3_UART1RX);
        }
        else if(iRxPin == eGpioBit19)
        {
            //
            // OK we can assign the request RX PIN to UART1
            //
            REG_M1(SOCCTRL_PMUXC, GPIO19, BFV_SOCCTRL_PMUXC_GPIO19_UART1RX);
        }
        else if(iRxPin == eGpioBitNone)
        {
            //
            // Nothing to do but a valid case nonetheless.
            //
        }
        else
        {
            //
            // No valid pin selected for UART1 RX
            //
            return(eEtaUartInvalidPin);
        }

        //
        // Now check the request for the UART 1 RTS pin
        //
        if(iRtsPin == eGpioBit4)
        {
            //
            // OK we can assign the request RTS PIN to UART1
            //
            REG_M1(SOCCTRL_PMUXA, GPIO4, BFV_SOCCTRL_PMUXA_GPIO4_UART1RTS);
        }
        else if(iRtsPin == eGpioBit22)
        {
            //
            // OK we can assign the request RTS PIN to UART1
            //
            REG_M1(SOCCTRL_PMUXC, GPIO22, BFV_SOCCTRL_PMUXC_GPIO22_UART1RTS);
        }
        else if(iRtsPin == eGpioBitNone)
        {
            //
            // Nothing to do but a valid case nonetheless.
            //
        }
        else
        {
            //
            // No valid pin selected for UART1 RTS
            //
            return(eEtaUartInvalidPin);
        }

        //
        // Now check the request for the UART 1 CTS pin
        //
        if(iCtsPin == eGpioBit5)
        {
            //
            // OK we can assign the request CTS PIN to UART1
            //
            REG_M1(SOCCTRL_PMUXA, GPIO5, BFV_SOCCTRL_PMUXA_GPIO5_UART1CTS);
        }
        else if(iCtsPin == eGpioBit21)
        {
            //
            // OK we can assign the request CTS PIN to UART1
            //
            REG_M1(SOCCTRL_PMUXC, GPIO21, BFV_SOCCTRL_PMUXC_GPIO21_UART1CTS);
        }
        else if(iCtsPin == eGpioBitNone)
        {
            //
            // Nothing to do but a valid case nonetheless.
            //
        }
        else
        {
            //
            // No valid pin selected for UART1 CTS
            //
            return(eEtaUartInvalidPin);
        }
    }
    else
    {
        //
        // So the UART number doesn't make sense for this chip.
        //
        return(eEtaUartInvalidPin);
    }

    //
    // If we get here then we are successful.
    //
    return(eEtaSuccess);
}

/***************************************************************************//**
 *
 * EtaCspUartFence - Set up a fence for writes to the TIMER registers.
 *
 *
 ******************************************************************************/
void
EtaCspUartFence(void)
{
    EtaCspUartFenceFast();
}

