/***************************************************************************//**
 *
 * @file eta_csp_i2c.c
 *
 * @brief This file contains eta_csp_i2c module implementations.
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
 * This is part of revision 1.0.0a6 of the Tensai Software Development Kit.
 *
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup ecm3532i2c-m3 Inter-integrated Circuit (I2C)
 * @ingroup ecm3532csp-m3
 * @{
 ******************************************************************************/

// FIXME, missing Dynamic Pull Up Control....
#include <stdint.h>
#include "eta_status.h"
#include "eta_chip.h"
#include "eta_csp_i2c.h"

// #define ETA_CSP_I2C_DEBUG
// #define ETA_CSP_I2C_INT_DEBUG   // If set, we enable some extremely verbose debug info


#define ETA_CSP_I2C_EN_GREEDY_INT       1 // If 1, we'll do an additional read per interrupt
                                          // to determine how much TX FIFO avail or RX FIFO ready
                                          // and read that amount. If 0, we do one write/read
                                          // per interrupt. Greedy tends to be more efficient, but does
                                          // add one read which is not strictly required since we know
                                          // the interrupt watermarks.

/***************************************************************************//**
 *
 * EtaCspI2cSpeedConfig - Configure the speed for the I2C module.
 *
 * @param iNum the I2C number.
 * @param iClockSpeed the I2C clock speed
 * @return the status.
 *
 ******************************************************************************/
tEtaStatus
EtaCspI2cSpeedConfig(tI2cNum iNum, tI2cClk iClockSpeed)
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

/***************************************************************************//**
 *
 * EtaCspI2cDonePoll - Poll for done.
 *
 * @param iNum I2C instance number
 * @return Return the status.
 *
 ******************************************************************************/
tEtaStatus
EtaCspI2cDonePoll(tI2cNum iNum)
{
    uint32_t ui32Timeout = 1000000;

    while((--ui32Timeout) && (REG_I2C_STATUS(iNum).BF.START_BUSY == 1) && (REG_I2C_STATUS(iNum).BF.SM_IN_NACK_HOLD == 0))
    {
    }

    if(ui32Timeout <= 0)
    {
#ifdef ETA_CSP_I2C_DEBUG
        ETA_DIAGS_PRINTF("ERROR: Timed out waiting for I2C%d CMD (SR=%x)\r\n",
                          iNum, REG_I2C_STATUS(iNum).V);
#endif
        return(eEtaI2cDoneTimeout);
    }
    else
    {
        if (REG_I2C_STATUS(iNum).BF.SM_IN_NACK_HOLD == 1) // We encountered a NACK, stop command, report back.
        {
            REG_I2C_CONFIG2(iNum).BF.SOFT_RST = 1;

            // FENCE TO MAKE SURE ALL WRITES GET FLUSHED
            REG_GPIO_DEBUG1_READ_ONLY.V = REG_I2C_CONFIG2(iNum).V;

            REG_I2C_CONFIG2(iNum).BF.SOFT_RST = 0;
            return(eEtaI2cUnexpectedNack);
        }
        else
            return(eEtaSuccess);
    }
}

/***************************************************************************//**
 *
 * EtaCspI2cRxDataPoll - Poll for RX data ready.
 *
 * @param iNum I2C instance number
 * @return Return the status.
 *
 ******************************************************************************/
tEtaStatus
EtaCspI2cRxDataPoll(tI2cNum iNum)
{
    uint32_t ui32Timeout = 1000000;

    while((--ui32Timeout) && (REG_I2C_STATUS(iNum).BF.RXFIFO_HAS_1DATA == 0))
    {
    }

    if(ui32Timeout <= 0)
    {
#ifdef ETA_CSP_I2C_DEBUG
        ETA_DIAGS_PRINTF("ERROR: Time out waiting for I2C%d RX to be ready " \
                          "(STATUS=%x)\r\n", !!iNum, REG_I2C_STATUS(iNum).V);
#endif
        return(eEtaI2cRecTimeout);
    }
    else
    {
        return(eEtaSuccess);
    }
}

/***************************************************************************//**
 *
 * EtaCspI2cTxReadyPoll - Poll for TX ready.
 *
 * @param iNum I2C instance number
 * @return Return the status.
 *
 ******************************************************************************/
tEtaStatus
EtaCspI2cTxReadyPoll(tI2cNum iNum)
{
    uint32_t ui32Timeout = 10000000;

    while((--ui32Timeout) && (REG_I2C_STATUS(iNum).BF.TXFIFO_HAS_1SPACE == 0))
    {
    }

    if(ui32Timeout <= 0)
    {
#ifdef ETA_CSP_I2C_DEBUG
        ETA_DIAGS_PRINTF("ERROR: Time out waiting for I2C%d TX (STATUS=%x)" \
                          "\r\n", !!iNum, REG_I2C_STATUS(iNum).V);
#endif
        return(eEtaI2cSendTimeout);
    }
    else
    {
        return(eEtaSuccess);
    }
}

/***************************************************************************//**
 *
 * EtaCspI2cLineClear - Triggers an I2C clearing event to clear any residual commands
 *                      issued before a soft reset
 *
 * @param iNum I2C instance number
 * @return Return the status.
 *
 ******************************************************************************/
tEtaStatus
EtaCspI2cLineClear(tI2cNum iNum)
{
    tEtaStatus iResult;

    REGN_M1(iNum, I2C_CONFIG, IGNORE_NACK, 1);

    REG_I2C_TXRX_1BYTE_FIFO_DATA(iNum).V = 0xff;
    REG_I2C_TXRX_1BYTE_FIFO_DATA(iNum).V = 0xff;
    REG_I2C_CMD_FIFO_DATA(iNum).V = I2C_CMD_STOP;
    REGN_W1(iNum, I2C_STATUS, START_BUSY, 1);

    iResult = EtaCspI2cDonePoll(iNum);
    if(iResult != eEtaSuccess)
    {
        return(iResult);
    }

    REGN_M1(iNum, I2C_CONFIG, IGNORE_NACK, 0);

    //
    // Return the result.
    //
    return(iResult);
}

/***************************************************************************//**
 *
 * EtaCspI2cInit - Initialize the I2C module.
 *
 *  @param iNum the I2C number.
 *  @param iClockSpeed the I2C clock speed
 * @return The result.
 *
 ******************************************************************************/
tEtaStatus
EtaCspI2cInit(tI2cNum iNum, tI2cClk iClockSpeed)
{
    tEtaStatus iResult;

    iResult = EtaCspI2cSpeedConfig(iNum, iClockSpeed);
    if(iResult != eEtaSuccess)
    {
        return(iResult);
    }

    REGN_W2(iNum, I2C_CONFIG, STALL_IF_RXFULL, 1, HOLD_AT_ACK, 1);
    REGN_M1(iNum, I2C_CONFIG2, SOFT_RST, 1);
    REGN_M1(iNum, I2C_CONFIG2, SOFT_RST, 0);

    //
    // I2C setup.
    //
#ifdef ETA_CSP_I2C_DEBUG
    ETA_DIAGS_PRINTF("Configuring I2C0 to %dHz.\r\n", iClockSpeed);
#endif
    EtaCspI2cDonePoll(iNum);
    EtaCspI2cTxReadyPoll(iNum);

    //
    // Clear the Line from any aborted previous command.
    //
#ifdef ETA_CSP_I2C_DEBUG
    ETA_DIAGS_PRINTF("Clearing the line\r\n");
#endif
    EtaCspI2cLineClear(iNum);

    //
    // Return the result.
    //
    return(iResult);
}

/***************************************************************************//**
 *
 * EtaCspI2cTransferPoll - Read and or write to an I2C device.
 *
 * @param iNum I2C instance number
 * @param pui8TxData Pointer to the data to transmit.
 * @param ui32TxLen Transmit length in bytes.
 * @param pui8RxData Point to the received data.
 * @param ui32RxLen Receive length in bytes.
 * @param ui8RestartAfterByte Perform a repeated start after this mayn bytes.
 * @return Return the status.
 *
 ******************************************************************************/
tEtaStatus
EtaCspI2cTransferPoll(tI2cNum iNum, uint8_t *pui8TxData, uint32_t ui32TxLen,
                      uint8_t *pui8RxData, uint32_t ui32RxLen,
                      uint32_t ui8RestartAfterByte)
{
    uint32_t ui32ByteCount, ui32Timeout;
    uint8_t ui8TxByte, ui8RxByte;
    tEtaStatus localI2cStatus;

    //
    // Check that the command is acceptable.
    //
    if(ui32TxLen == 0)
    {
#ifdef ETA_CSP_I2C_DEBUG
        ETA_DIAGS_PRINTF("ERROR: i2c_transfer_poll, inst %d, no data " \
                          "(ui32TxLen %d)\r\n", iNum, ui32TxLen);
#endif
        return(eEtaI2cInvalidConfig);
    }

    if(ui32TxLen < ui8RestartAfterByte)
    {
#ifdef ETA_CSP_I2C_DEBUG
        ETA_DIAGS_PRINTF("ERROR: i2c_transfer_poll, inst %d, invalid " \
                          "ui32TxLen %d, ui8RestartAfterByte %d)\r\n", iNum,
                          ui32TxLen, ui8RestartAfterByte);
#endif
        return(eEtaI2cInvalidConfig);
    }

    if(ui32TxLen > 64 * 1024)
    {
#ifdef ETA_CSP_I2C_DEBUG
        ETA_DIAGS_PRINTF("ERROR: i2c_transfer_poll, inst %d exceeds max " \
                          "ui32TxLen=%d\r\n", iNum, ui32TxLen);
#endif
        return(eEtaI2cInvalidConfig);
    }

    if(ui32RxLen > 64 * 1024)
    {
#ifdef ETA_CSP_I2C_DEBUG
        ETA_DIAGS_PRINTF("ERROR: i2c_transfer_poll, inst %d exceeds max " \
                          "ui32RxLen=%d\r\n", iNum, ui32RxLen);
#endif
        return(eEtaI2cInvalidConfig);
    }

    //
    // Make sure previous command is done (optional).
    //
    if( ( localI2cStatus = EtaCspI2cDonePoll(iNum) ) )
    {
        return(localI2cStatus);
    }

    //
    // Start command (waits for first byte).
    //
    REGN_W1(iNum, I2C_STATUS, START_BUSY, 1);
    ui32ByteCount = 0;
#ifdef ETA_CSP_I2C_DEBUG
    ETA_DIAGS_PRINTF("i2c_transfer_poll beginning tx\r\n");
#endif
    ui32Timeout = 10000;
    while((--ui32Timeout) && (ui32TxLen))
    {
        if( ( localI2cStatus = EtaCspI2cTxReadyPoll(iNum) ) )
        {
            return(localI2cStatus);
        }

        ui8TxByte = *pui8TxData++;

        REG_I2C_TXRX_1BYTE_FIFO_DATA(iNum).V = ui8TxByte;
        ui32TxLen--;
#ifdef ETA_CSP_I2C_DEBUG
        ETA_DIAGS_PRINTF("i2c_transfer_poll tx data sent (%d left)\r\n",
                          ui32TxLen);
#endif

        ui32ByteCount = ui32ByteCount + 1;
        if(ui32ByteCount == ui8RestartAfterByte)
        {
            if( ( localI2cStatus = EtaCspI2cTxReadyPoll(iNum) ) )
            {
                return(localI2cStatus);
            }

            REG_I2C_CMD_FIFO_DATA(iNum).V = I2C_CMD_RESTART;
#ifdef ETA_CSP_I2C_DEBUG
            ETA_DIAGS_PRINTF("i2c_transfer_poll restart issued\r\n");
#endif
        }
    }

    // This timeout is for general programming reasons to prevent infinite loop
    if(ui32Timeout == 0)
    {
        return(eEtaFailure);
    }
#ifdef ETA_CSP_I2C_DEBUG
    ETA_DIAGS_PRINTF("i2c_transfer_poll done with tx\r\n");
#endif

    //
    // If no rx, end write.
    //
    if(ui32RxLen == 0)
    {
        if( ( localI2cStatus = EtaCspI2cTxReadyPoll(iNum) ) )
        {
            return(localI2cStatus);
        }

        REG_I2C_CMD_FIFO_DATA(iNum).V = I2C_CMD_END_WRITE;
#ifdef ETA_CSP_I2C_DEBUG
        ETA_DIAGS_PRINTF("i2c_transfer_poll corner case with ui32RxLen=0\r\n");
#endif
        return(eEtaSuccess);
    }

    if(ui32RxLen == 1)
    {
#ifdef ETA_CSP_I2C_DEBUG
        ETA_DIAGS_PRINTF("i2c_transfer_poll beginning, ui32RxLen=1\r\n");
#endif
        if( ( localI2cStatus = EtaCspI2cTxReadyPoll(iNum) ) )
        {
            return(localI2cStatus);
        }
        REG_I2C_TXRX_1BYTE_FIFO_DATA(iNum).V = 0xff;
        if( ( localI2cStatus = EtaCspI2cTxReadyPoll(iNum) ) )
        {
            return(localI2cStatus);
        }
        REG_I2C_CMD_FIFO_DATA(iNum).V = I2C_CMD_BEGIN_READ
                                        | I2C_CMD_END_READ;
    }
    else if(ui32RxLen == 2)
    {
#ifdef ETA_CSP_I2C_DEBUG
        ETA_DIAGS_PRINTF("i2c_transfer_poll beginning, ui32RxLen=2\r\n");
#endif
        if( ( localI2cStatus = EtaCspI2cTxReadyPoll(iNum) ) )
        {
            return(localI2cStatus);
        }
        REG_I2C_TXRX_1BYTE_FIFO_DATA(iNum).V = 0xff;
        if( ( localI2cStatus = EtaCspI2cTxReadyPoll(iNum) ) )
        {
            return(localI2cStatus);
        }
        REG_I2C_CMD_FIFO_DATA(iNum).V = I2C_CMD_BEGIN_READ;
        if( ( localI2cStatus = EtaCspI2cTxReadyPoll(iNum) ) )
        {
            return(localI2cStatus);
        }
        REG_I2C_TXRX_1BYTE_FIFO_DATA(iNum).V = 0xff;
        if( ( localI2cStatus = EtaCspI2cTxReadyPoll(iNum) ) )
        {
            return(localI2cStatus);
        }
        REG_I2C_CMD_FIFO_DATA(iNum).V = I2C_CMD_END_READ;
    }
    else
    {
#ifdef ETA_CSP_I2C_DEBUG
        ETA_DIAGS_PRINTF("i2c_transfer_poll beginning, ui32RxLen>2\r\n");
#endif
        if(( localI2cStatus = EtaCspI2cTxReadyPoll(iNum) ) )
        {
            return(localI2cStatus);
        }
        REG_I2C_TXRX_1BYTE_FIFO_DATA(iNum).V = 0xff;
        if( ( localI2cStatus = EtaCspI2cTxReadyPoll(iNum) ) )
        {
            return(localI2cStatus);
        }
        REG_I2C_CMD_FIFO_DATA(iNum).V = I2C_CMD_BEGIN_READ;

        REG_I2C_DUMMY_BYTES(iNum).V = ui32RxLen - 2;
        if( ( localI2cStatus = EtaCspI2cTxReadyPoll(iNum) ) )
        {
            return(localI2cStatus);
        }
        REG_I2C_CMD_FIFO_DATA(iNum).V = I2C_CMD_DUMMYBYTE;

        if( ( localI2cStatus = EtaCspI2cTxReadyPoll(iNum) ) )
        {
            return(localI2cStatus);
        }
        REG_I2C_TXRX_1BYTE_FIFO_DATA(iNum).V = 0xff;
        if( ( localI2cStatus = EtaCspI2cTxReadyPoll(iNum) ) )
        {
            return(localI2cStatus);
        }
        REG_I2C_CMD_FIFO_DATA(iNum).V = I2C_CMD_END_READ;
    }

#ifdef ETA_CSP_I2C_DEBUG
    ETA_DIAGS_PRINTF("i2c_transfer_poll waiting for rx data\r\n");
#endif
    ui32Timeout = 10000;
    while((--ui32Timeout) && (ui32RxLen))
    {
        if( ( localI2cStatus = EtaCspI2cRxDataPoll(iNum) ) )
        {
            return(localI2cStatus);
        }

        //
        // Pop data off the read stack.
        //
        ui8RxByte = REG_I2C_TXRX_1BYTE_FIFO_DATA(iNum).V;
        *pui8RxData++ = ui8RxByte;

        ui32RxLen--;
#ifdef ETA_CSP_I2C_DEBUG
        ETA_DIAGS_PRINTF("i2c_transfer_poll rx data received (%d left)\r\n",
                          ui32RxLen);
#endif
    }

#ifdef ETA_CSP_I2C_DEBUG
    ETA_DIAGS_PRINTF("i2c_transfer_poll done waiting for rx data\r\n");
#endif

    // This timeout is for general programming reasons to prevent infinite loop
    if(ui32Timeout == 0)
    {
        return(eEtaFailure);
    }

    return(eEtaSuccess);
}


/***************************************************************************//**
 *
 * EtaCspI2cDeviceAck - Read and or write to an I2C device.
 *
 * @param iNum I2C instance number
 * @param ui8TxData DevAddr and Read(write not) of device to poll for ack
 * @return Return the status.
 *
 ******************************************************************************/
tEtaStatus
EtaCspI2cDeviceAck (tI2cNum iNum, uint8_t ui8TxData){
  tEtaStatus result = eEtaSuccess;

  // Turn off NACK hold
  REG_I2C_CONFIG(iNum).BF.IGNORE_NACK = 1;

  // upper 7 is device id, lower bit is Read (write not)
  result |= EtaCspI2cTransferPoll(iNum, &ui8TxData, 1, &ui8TxData, 0, 1);

  result |= EtaCspI2cDonePoll(iNum);

  // Turn back on NACK hold
  REG_I2C_CONFIG(iNum).BF.IGNORE_NACK = 0;

  if (result)
  {
    return result;
  }

  if (REG_I2C_GENERAL_DEBUG(iNum).BF.LAST_BYTE_HAD_NACK)
    result = eEtaI2cUnexpectedNack;
  else
    result = eEtaSuccess;

  return result;
}



////////////////////////////////////////////////////////////////////////////////////
// Interrupts

// Global pointer to command structs for I2C0 and I2C1
volatile EtaCspI2cCmd_t * g_pEtaCspI2cCmd[2];


/***************************************************************************//**
 *
 *  EtaCspI2cTransferInt - Start the I2C using Interrupt
 *
 *  @param iNum the I2C number.
 *  @param pui8TxData pointer to TX Data Array
 *  @param ui32TxLen TX Data Length
 *  @param pui8RxData pointer to RX Data Array
 *  @param ui32RxLen RX Data Length
 *  @param ui32RestartAfterByte Indicates when restart will be asserted (set to 0 if no restart needed).
 *
 ******************************************************************************/

tEtaStatus EtaCspI2cTransferInt(tI2cNum iNum, uint8_t * pui8TxData, uint32_t ui32TxLen, uint8_t * pui8RxData, uint32_t ui32RxLen, uint32_t ui32RestartAfterByte)
{
  tEtaStatus localI2cStatus;

  // Check that the command is acceptable
  // These can be removed if onerous
  if (ui32TxLen == 0)
  {
  // users should send at least device id.
#ifdef ETA_CSP_I2C_DEBUG
    ETA_DIAGS_PRINTF ("ERROR: EtaCspI2cTransferInt for iNum=%d has no tx data (ui32TxLen %d). Not supported.", iNum, ui32TxLen);
#endif
    return eEtaI2cInvalidConfig;
  }

  if (ui32TxLen < ui32RestartAfterByte)
  {
#ifdef ETA_CSP_I2C_DEBUG
    ETA_DIAGS_PRINTF ("ERROR: EtaCspI2cTransferInt for iNum=%d has restart after tx data (ui32TxLen %d, ui32RestartAfterByte %d). Not supported.", iNum, ui32TxLen, ui32RestartAfterByte);
#endif
    return eEtaI2cInvalidConfig;
  }

  if (ui32TxLen > (64*1024)-1)
  {
#ifdef ETA_CSP_I2C_DEBUG
    ETA_DIAGS_PRINTF ("ERROR: EtaCspI2cTransferInt for iNum=%d exceeded max ui32TxLen of 64*1024-1 with %d", iNum, ui32TxLen);
#endif
    return eEtaI2cInvalidConfig;
  }

  if (ui32RxLen > (64*1024)-1)
  {
#ifdef ETA_CSP_I2C_DEBUG
    ETA_DIAGS_PRINTF ("ERROR: EtaCspI2cTransferInt for iNum=%d exceeded max ui32RxLen of 64*1024-1 with %d", iNum, ui32RxLen);
#endif
    return eEtaI2cInvalidConfig;
  }
  // Check that the command is acceptable



  // Make sure previous command is done (optional)
  if ( ( localI2cStatus = EtaCspI2cDonePoll(iNum) ) )
    return localI2cStatus;

  // Start command (start could be done later, but it will wait for first byte sent to it).
  REG_I2C_STATUS(iNum).V = BM_I2C_STATUS_START_BUSY;

  if (ui32RxLen != 0) REG_I2C_DUMMY_BYTES(iNum).V = ui32RxLen-1;

  // Setup global array
  g_pEtaCspI2cCmd[iNum]->pui8TxData = pui8TxData;
  g_pEtaCspI2cCmd[iNum]->pui8RxData = pui8RxData;

  if (ui32RxLen == 0)
  {
    g_pEtaCspI2cCmd[iNum]->bRxLenIs0 = 1;       // Corner case of ui32RxLen=0 requires 1 extra transmit
    g_pEtaCspI2cCmd[iNum]->bRxLenIs1 = 0;
    g_pEtaCspI2cCmd[iNum]->bRxLenIsGt1 = 0;
    g_pEtaCspI2cCmd[iNum]->ui32TxLen = ui32TxLen+1;
  }
  else if (ui32RxLen == 1)
  {
    g_pEtaCspI2cCmd[iNum]->bRxLenIs0 = 0;
    g_pEtaCspI2cCmd[iNum]->bRxLenIs1 = 1;       // Corner case of ui32RxLen=1 requires 2 extra transmit
    g_pEtaCspI2cCmd[iNum]->bRxLenIsGt1 = 0;
    g_pEtaCspI2cCmd[iNum]->ui32TxLen = ui32TxLen+2;
  }
  else
  {
    g_pEtaCspI2cCmd[iNum]->bRxLenIs0 = 0;
    g_pEtaCspI2cCmd[iNum]->bRxLenIs1 = 0;
    g_pEtaCspI2cCmd[iNum]->bRxLenIsGt1 = 1;     // Common case of ui32RxLen>1 requires 4 extra transmit
    g_pEtaCspI2cCmd[iNum]->ui32TxLen = ui32TxLen+4;
  }

  g_pEtaCspI2cCmd[iNum]->ui8TxIntSize = 8;
  g_pEtaCspI2cCmd[iNum]->ui8RxIntSize = 8;
  g_pEtaCspI2cCmd[iNum]->ui32RxLen = ui32RxLen;
  g_pEtaCspI2cCmd[iNum]->bCmdActive = 1;
  g_pEtaCspI2cCmd[iNum]->ui32CurrByteCount = 0;
  g_pEtaCspI2cCmd[iNum]->ui32RestartAfterByte = ui32RestartAfterByte;
  g_pEtaCspI2cCmd[iNum]->i2cStatus = eEtaSuccess;

  REG_I2C_CONFIG(iNum).BF.RX_FIFO_INT_LEVEL = g_pEtaCspI2cCmd[iNum]->ui8TxIntSize;

  #ifdef ETA_CSP_I2C_INT_DEBUG
    ETA_DIAGS_PRINTF("DETAILED_I2C:Kicked off command. ui32TxLen=%x ui32RxLen=%x\r\n", ui32TxLen, ui32RxLen);
  #endif

  REG_I2C_INT_EN(iNum).V = BM_I2C_INT_EN_TXFIFO_HAS_LEVEL_SPACE_EN | BM_I2C_INT_EN_UNACK_EN;
  return eEtaSuccess;

}

/***************************************************************************//**
 *
 *  EtaCspI2cProcessIntTxData - Process I2C Interrupt for TX Space Available
 *
 *  @param iNum the I2C number.
 *
 ******************************************************************************/
tEtaStatus EtaCspI2cProcessIntTxData (tI2cNum iNum)
{
  uint8_t space_in_fifo;

  if ((g_pEtaCspI2cCmd[iNum]->bCmdActive == 0) || (g_pEtaCspI2cCmd[iNum]->ui32TxLen == 0))
  {
#ifdef ETA_CSP_I2C_DEBUG
    ETA_DIAGS_PRINTF("ERROR: received i2c_int_tx but command is done. bCmdActive=%x  ui32TxLen=%x\r\n", g_pEtaCspI2cCmd[iNum]->bCmdActive, g_pEtaCspI2cCmd[iNum]->ui32TxLen);
#endif
    return eEtaI2cCmdSequenceErr;
  }

  if (ETA_CSP_I2C_EN_GREEDY_INT) // This allows for burst writes, which is more efficient, but not requires an extra read.
  {
    space_in_fifo = 16-(MEMIO8(REG_I2C_TX_FIFO_DEBUG_ADDR (iNum)) & BM_I2C_TX_FIFO_DEBUG_ENTRIES);
  }
  else
  {
    space_in_fifo = g_pEtaCspI2cCmd[iNum]->ui8TxIntSize; // Do work that interrupt said we have space for
  }

  #ifdef ETA_CSP_I2C_INT_DEBUG
    ETA_DIAGS_PRINTF("DETAILED_I2C:I2C TX interrupt: Space for %d bytes starting with %x\r\n", space_in_fifo, *g_pEtaCspI2cCmd[iNum]->pui8TxData);
  #endif

  do
  {
    // Deal with restart
    // First we check if we have a restart, then we check if it is ours. If not, continue with transactions.
    if (g_pEtaCspI2cCmd[iNum]->ui32RestartAfterByte != 0)
    {
      if (g_pEtaCspI2cCmd[iNum]->ui32CurrByteCount == g_pEtaCspI2cCmd[iNum]->ui32RestartAfterByte)
      {
        g_pEtaCspI2cCmd[iNum]->ui32RestartAfterByte = 0; // This should "disable" restart since 0 is illegal for restart position
        MEMIO8(REG_I2C_CMD_FIFO_DATA_ADDR(iNum)) = I2C_CMD_RESTART;

        #ifdef ETA_CSP_I2C_INT_DEBUG
          ETA_DIAGS_PRINTF("DETAILED_I2C:I2C TX interrupt: I2C TX restart\r\n");
        #endif

        break; // Exit loop if we entered here.
      }
    }

    // This is the more common case, so we'll check this first and only go to the exceptions if this common case is not true.

    // These are various exceptions
    g_pEtaCspI2cCmd[iNum]->ui32CurrByteCount = g_pEtaCspI2cCmd[iNum]->ui32CurrByteCount + 1;

    if (g_pEtaCspI2cCmd[iNum]->ui32TxLen > 4) // Note, this is the same as the "else" clause at the bottom below.
    {
      #ifdef ETA_CSP_I2C_INT_DEBUG
        ETA_DIAGS_PRINTF("DETAILED_I2C:I2C TX interrupt: Sending: %x txleft: %x part A\r\n", (*g_pEtaCspI2cCmd[iNum]->pui8TxData) & 0xff, g_pEtaCspI2cCmd[iNum]->ui32TxLen-1);
      #endif

      MEMIO8(REG_I2C_TXRX_1BYTE_FIFO_DATA_ADDR(iNum)) = *g_pEtaCspI2cCmd[iNum]->pui8TxData++;
      g_pEtaCspI2cCmd[iNum]->ui32TxLen--;
      if (g_pEtaCspI2cCmd[iNum]->ui32TxLen == 0) // Just sent last byte.
      {
        REG_I2C_INT_EN(iNum).V = BM_I2C_INT_EN_RX_HIT_LEVEL_EN | BM_I2C_INT_EN_UNACK_EN;
        REG_I2C_CONFIG(iNum).BF.RX_FIFO_INT_LEVEL = g_pEtaCspI2cCmd[iNum]->ui8RxIntSize;

        #ifdef ETA_CSP_I2C_INT_DEBUG
          ETA_DIAGS_PRINTF("DETAILED_I2C:Last I2C TX data interrupt\r\n");
        #endif
      }
    }
    // Case where RX Length == 0
    else if ((g_pEtaCspI2cCmd[iNum]->bRxLenIs0==1) && (g_pEtaCspI2cCmd[iNum]->ui32TxLen == 1))
    {
      MEMIO8(REG_I2C_CMD_FIFO_DATA_ADDR(iNum)) = I2C_CMD_END_WRITE;
      REG_I2C_INT_EN(iNum).V = BM_I2C_INT_EN_CMD_DONE | BM_I2C_INT_EN_UNACK_EN;
      g_pEtaCspI2cCmd[iNum]->ui32TxLen--;
      REG_I2C_CONFIG(iNum).BF.RX_FIFO_INT_LEVEL = g_pEtaCspI2cCmd[iNum]->ui8RxIntSize;

      #ifdef ETA_CSP_I2C_INT_DEBUG
        ETA_DIAGS_PRINTF("DETAILED_I2C:I2C TX interrupt: rxlen=0,txleft=0\r\n");
      #endif
      break; // Exit loop, we're done.
    }
    // Cases where RX Length == 1
    else if ((g_pEtaCspI2cCmd[iNum]->bRxLenIs1==1) && (g_pEtaCspI2cCmd[iNum]->ui32TxLen == 2))
    {
      MEMIO8(REG_I2C_TXRX_1BYTE_FIFO_DATA_ADDR(iNum)) = 0xff;
      g_pEtaCspI2cCmd[iNum]->ui32TxLen--;

      #ifdef ETA_CSP_I2C_INT_DEBUG
       ETA_DIAGS_PRINTF("DETAILED_I2C:I2C TX interrupt: rxlen=1,txleft=1\r\n");
      #endif
    }
    else if ((g_pEtaCspI2cCmd[iNum]->bRxLenIs1==1) && (g_pEtaCspI2cCmd[iNum]->ui32TxLen == 1))
    {
      MEMIO8(REG_I2C_CMD_FIFO_DATA_ADDR(iNum)) = I2C_CMD_BEGIN_READ | I2C_CMD_END_READ;
      g_pEtaCspI2cCmd[iNum]->ui32TxLen--;
      REG_I2C_INT_EN(iNum).V = BM_I2C_INT_EN_RX_HIT_LEVEL_EN | BM_I2C_INT_EN_UNACK_EN; // Go to RX
      REG_I2C_CONFIG(iNum).BF.RX_FIFO_INT_LEVEL = g_pEtaCspI2cCmd[iNum]->ui8RxIntSize;

      #ifdef ETA_CSP_I2C_INT_DEBUG
       ETA_DIAGS_PRINTF("DETAILED_I2C:I2C TX interrupt: rxlen=1,txleft=0\r\n");
      #endif
      break; // Exit loop, we're done.
    }
    // Cases where RX Length >1
    else if ((g_pEtaCspI2cCmd[iNum]->bRxLenIsGt1==1) && (g_pEtaCspI2cCmd[iNum]->ui32TxLen == 4))
    {
      MEMIO8(REG_I2C_TXRX_1BYTE_FIFO_DATA_ADDR(iNum)) = 0xff;
      g_pEtaCspI2cCmd[iNum]->ui32TxLen--;

      #ifdef ETA_CSP_I2C_INT_DEBUG
       ETA_DIAGS_PRINTF("DETAILED_I2C:I2C TX interrupt: rxlen>1,txleft=3\r\n");
      #endif
    }
    else if ((g_pEtaCspI2cCmd[iNum]->bRxLenIsGt1==1) && (g_pEtaCspI2cCmd[iNum]->ui32TxLen == 3))
    {
      MEMIO8(REG_I2C_CMD_FIFO_DATA_ADDR(iNum)) = I2C_CMD_BEGIN_READ;
      g_pEtaCspI2cCmd[iNum]->ui32TxLen--;

      #ifdef ETA_CSP_I2C_INT_DEBUG
       ETA_DIAGS_PRINTF("DETAILED_I2C:I2C TX interrupt: rxlen>1,txleft=2\r\n");
      #endif
    }
    else if ((g_pEtaCspI2cCmd[iNum]->bRxLenIsGt1==1) && (g_pEtaCspI2cCmd[iNum]->ui32TxLen == 2))
    {
      MEMIO8(REG_I2C_CMD_FIFO_DATA_ADDR(iNum)) = I2C_CMD_DUMMYBYTE;
      g_pEtaCspI2cCmd[iNum]->ui32TxLen--;

      #ifdef ETA_CSP_I2C_INT_DEBUG
       ETA_DIAGS_PRINTF("DETAILED_I2C:I2C TX interrupt: rxlen>1,txleft=1\r\n");
      #endif
    }
    else if ((g_pEtaCspI2cCmd[iNum]->bRxLenIsGt1==1) && (g_pEtaCspI2cCmd[iNum]->ui32TxLen == 1))
    {
      MEMIO8(REG_I2C_CMD_FIFO_DATA_ADDR(iNum)) = I2C_CMD_END_READ;
      g_pEtaCspI2cCmd[iNum]->ui32TxLen--;
      REG_I2C_INT_EN(iNum).V = BM_I2C_INT_EN_RX_HIT_LEVEL_EN | BM_I2C_INT_EN_UNACK_EN; // Go to RX
      REG_I2C_CONFIG(iNum).BF.RX_FIFO_INT_LEVEL = g_pEtaCspI2cCmd[iNum]->ui8RxIntSize;

      #ifdef ETA_CSP_I2C_INT_DEBUG
       ETA_DIAGS_PRINTF("DETAILED_I2C:I2C TX interrupt: rxlen>1,txleft=0\r\n");
      #endif
      break; // Exit loop, we're done.
    }
    else
    {
      #ifdef ETA_CSP_I2C_INT_DEBUG
       ETA_DIAGS_PRINTF("DETAILED_I2C:I2C TX interrupt: Sending: %x txleft: %x part B\r\n", (*g_pEtaCspI2cCmd[iNum]->pui8TxData) & 0xff, g_pEtaCspI2cCmd[iNum]->ui32TxLen-1);
      #endif

      MEMIO8(REG_I2C_TXRX_1BYTE_FIFO_DATA_ADDR(iNum)) = *g_pEtaCspI2cCmd[iNum]->pui8TxData++;
      g_pEtaCspI2cCmd[iNum]->ui32TxLen--;
      if (g_pEtaCspI2cCmd[iNum]->ui32TxLen == 0) // Just sent last byte.
      {
        REG_I2C_INT_EN(iNum).V = BM_I2C_INT_EN_RX_HIT_LEVEL_EN | BM_I2C_INT_EN_UNACK_EN;
        REG_I2C_CONFIG(iNum).BF.RX_FIFO_INT_LEVEL = g_pEtaCspI2cCmd[iNum]->ui8RxIntSize;

        #ifdef ETA_CSP_I2C_INT_DEBUG
         ETA_DIAGS_PRINTF("DETAILED_I2C:Last I2C TX data interrupt\r\n");
        #endif
        break; // Exit loop, we're done.
      }
    }
  } while (--space_in_fifo);

  return eEtaSuccess;
}

/***************************************************************************//**
 *
 *  EtaCspI2cProcessIntRxData - Process I2C Interrupt for RX Data Available
 *
 *  @param iNum the I2C number.
 *
 ******************************************************************************/
tEtaStatus EtaCspI2cProcessIntRxData (tI2cNum iNum)
{
  uint8_t space_in_fifo;
  uint32_t rx_word;

  #ifdef ETA_CSP_I2C_INT_DEBUG
     ETA_DIAGS_PRINTF("DETAILED_I2C:entered EtaCspI2cProcessIntRxData\r\n");
  #endif

  if ((g_pEtaCspI2cCmd[iNum]->bCmdActive == 0) || (g_pEtaCspI2cCmd[iNum]->ui32RxLen == 0))
  {
#ifdef ETA_CSP_I2C_DEBUG
    ETA_DIAGS_PRINTF("ERROR: received i2c_int_rx but command is done. bCmdActive=%x  ui32RxLen=%x\r\n", g_pEtaCspI2cCmd[iNum]->bCmdActive, g_pEtaCspI2cCmd[iNum]->ui32RxLen);
#endif
    return eEtaI2cCmdSequenceErr;
  }

  if (ETA_CSP_I2C_EN_GREEDY_INT) // This allows for burst writes, which is more efficient, but not requires an extra read.
  {
    space_in_fifo = (MEMIO8(REG_I2C_RX_FIFO_DEBUG_ADDR (iNum)) & BM_I2C_RX_FIFO_DEBUG_ENTRIES);
  }
  else
  {
    space_in_fifo = g_pEtaCspI2cCmd[iNum]->ui8RxIntSize; // Do work that interrupt said we have space for
    if (space_in_fifo > g_pEtaCspI2cCmd[iNum]->ui32RxLen) space_in_fifo = g_pEtaCspI2cCmd[iNum]->ui32RxLen;
  }

  #ifdef ETA_CSP_I2C_INT_DEBUG
     ETA_DIAGS_PRINTF("DETAILED_I2C:I2C RX data interrupt. space_in_fifo:%d\r\n", space_in_fifo);
  #endif

  do
  {
    if (space_in_fifo >= 4)
    {
      rx_word = REG_I2C_RX_4BYTE_FIFO_DATA(iNum).V;

      #ifdef ETA_CSP_I2C_INT_DEBUG
       ETA_DIAGS_PRINTF("DETAILED_I2C:I2C RX 4data interrupt. rx:%x rxlen:%d space:%d\r\n", rx_word, g_pEtaCspI2cCmd[iNum]->ui32RxLen, space_in_fifo);
      #endif

      *g_pEtaCspI2cCmd[iNum]->pui8RxData++ = ((rx_word >> 0) & 0xff);
      *g_pEtaCspI2cCmd[iNum]->pui8RxData++ = ((rx_word >> 8) & 0xff);
      *g_pEtaCspI2cCmd[iNum]->pui8RxData++ = ((rx_word >> 16) & 0xff);
      *g_pEtaCspI2cCmd[iNum]->pui8RxData++ = ((rx_word >> 24) & 0xff);
      g_pEtaCspI2cCmd[iNum]->ui32RxLen -= 4;
      space_in_fifo -= 4;
    }
    else if (space_in_fifo >= 2)
    {
      rx_word = MEMIO16(REG_I2C_RX_2BYTE_FIFO_DATA_ADDR (iNum));

      #ifdef ETA_CSP_I2C_INT_DEBUG
       ETA_DIAGS_PRINTF("DETAILED_I2C:I2C RX 2data interrupt. rx:%x rxlen:%d space:%d\r\n", rx_word, g_pEtaCspI2cCmd[iNum]->ui32RxLen, space_in_fifo);
      #endif

      *g_pEtaCspI2cCmd[iNum]->pui8RxData++ = ((rx_word >> 0) & 0xff);
      *g_pEtaCspI2cCmd[iNum]->pui8RxData++ = ((rx_word >> 8) & 0xff);
      g_pEtaCspI2cCmd[iNum]->ui32RxLen -= 2;
      space_in_fifo -= 2;
    }
    else
    {
      rx_word = MEMIO8(REG_I2C_TXRX_1BYTE_FIFO_DATA_ADDR (iNum));

      #ifdef ETA_CSP_I2C_INT_DEBUG
        ETA_DIAGS_PRINTF("DETAILED_I2C:I2C RX 1data interrupt. rx:%x rxlen:%d space:%d\r\n", rx_word, g_pEtaCspI2cCmd[iNum]->ui32RxLen, space_in_fifo);
      #endif

      *g_pEtaCspI2cCmd[iNum]->pui8RxData++ = ((rx_word >> 0) & 0xff);

      // If we have data to send, ignore receive data.
      g_pEtaCspI2cCmd[iNum]->ui32RxLen --;
      space_in_fifo --;

    }

    if (g_pEtaCspI2cCmd[iNum]->ui32RxLen == 0) // Just received last byte.
    {
      REG_I2C_INT_EN(iNum).V = BM_I2C_INT_EN_CMD_DONE | BM_I2C_INT_EN_UNACK_EN;
      #ifdef ETA_CSP_I2C_INT_DEBUG
       ETA_DIAGS_PRINTF("DETAILED_I2C:Last I2C RX data interrupt\r\n");
      #endif
    }

    if ((space_in_fifo < 4) & (g_pEtaCspI2cCmd[iNum]->ui32RxLen >= 4))
    {
      break; // If we drop low enough that we cannot bulk transfer, exit loop, wait for next int.
    }

    if ((space_in_fifo < 2) & (g_pEtaCspI2cCmd[iNum]->ui32RxLen >= 2))
    {
      break; // If we drop low enough that we cannot bulk transfer, exit loop, wait for next int.
    }

    if (g_pEtaCspI2cCmd[iNum]->ui32RxLen == 0)
    {
      break; // Exit loop, we're done.
    }
  }
  while (space_in_fifo);

  if (g_pEtaCspI2cCmd[iNum]->ui8RxIntSize > g_pEtaCspI2cCmd[iNum]->ui32RxLen)
    REG_I2C_CONFIG(iNum).BF.RX_FIFO_INT_LEVEL = g_pEtaCspI2cCmd[iNum]->ui32RxLen;
    #ifdef ETA_CSP_I2C_INT_DEBUG
     ETA_DIAGS_PRINTF("DETAILED_I2C:exiting EtaCspI2cProcessIntRxData\r\n");
    #endif

  return eEtaSuccess;
}

/***************************************************************************//**
 *
 *  EtaCspI2cProcessIntCmdDone - Process I2C Interrupt for Command Done
 *
 *  @param iNum the I2C number.
 *
 ******************************************************************************/
tEtaStatus EtaCspI2cProcessIntCmdDone (tI2cNum iNum)
{
  #ifdef ETA_CSP_I2C_INT_DEBUG
    ETA_DIAGS_PRINTF("DETAILED_I2C:entered EtaCspI2cProcessIntCmdDone\r\n");
  #endif

  if ((g_pEtaCspI2cCmd[iNum]->bCmdActive == 0) || (g_pEtaCspI2cCmd[iNum]->ui32TxLen != 0) || (g_pEtaCspI2cCmd[iNum]->ui32RxLen != 0))
  {
#ifdef ETA_CSP_I2C_DEBUG
    ETA_DIAGS_PRINTF("ERROR: received i2c_int_done but command is already done or has work left to do. bCmdActive=%x  ui32TxLen=%x  ui32RxLen=%x\r\n", g_pEtaCspI2cCmd[iNum]->bCmdActive, g_pEtaCspI2cCmd[iNum]->ui32TxLen, g_pEtaCspI2cCmd[iNum]->ui32RxLen);
#endif
    return eEtaI2cCmdSequenceErr;
  }

  #ifdef ETA_CSP_I2C_INT_DEBUG
   ETA_DIAGS_PRINTF("DETAILED_I2C:I2C command interrupt done\r\n");
  #endif

  g_pEtaCspI2cCmd[iNum]->i2cStatus = eEtaSuccess;
  g_pEtaCspI2cCmd[iNum]->bCmdActive = 0;
  REG_I2C_INT_EN(iNum).V = 0;
  return eEtaSuccess;
}

/***************************************************************************//**
 *
 *  EtaCspI2cProcessIntNack - Process I2C Interrupt for Unexpected Negative Acknowledgement
 *
 *  @param iNum the I2C number.
 *
 ******************************************************************************/
tEtaStatus EtaCspI2cProcessIntNack (tI2cNum iNum)
{
  #ifdef ETA_CSP_I2C_INT_DEBUG
    ETA_DIAGS_PRINTF("DETAILED_I2C:entered EtaCspI2cProcessIntNack\r\n");
  #endif

  // Disable Interrupts
  REG_I2C_INT_EN(iNum).V = 0;

  // Clear State machine / cancel command
  REG_I2C_CONFIG2(iNum).BF.SOFT_RST = 1;

  // FENCE TO MAKE SURE ALL WRITES GET FLUSHED
  REG_GPIO_DEBUG1_READ_ONLY.V = REG_I2C_CONFIG2(iNum).V;

  REG_I2C_CONFIG2(iNum).BF.SOFT_RST = 0;

  g_pEtaCspI2cCmd[iNum]->i2cStatus = eEtaI2cUnexpectedNack;
  g_pEtaCspI2cCmd[iNum]->bCmdActive = 0;
  REG_I2C_INT_EN(iNum).V = 0;
  return eEtaSuccess;
}

/***************************************************************************//**
 *
 *  EtaCspI2cProcessIntEntry - Entry for Interrupts for I2C
 *
 *  @param iNum the I2C number.
 *
 ******************************************************************************/
tEtaStatus EtaCspI2cProcessIntEntry (tI2cNum iNum)
{
  uint32_t bCmdActive_interrupt;
  tEtaStatus i2cStatus = eEtaSuccess;

  // Do one read per interrupt. The INT_ACT field may change during the interrupt
  bCmdActive_interrupt = MEMIO16(REG_I2C_INT_ACT_ADDR (iNum));

  if (bCmdActive_interrupt & BM_I2C_INT_EN_UNACK_EN                  ) i2cStatus |= EtaCspI2cProcessIntNack(iNum) ;
  if (bCmdActive_interrupt & BM_I2C_INT_EN_TXFIFO_HAS_LEVEL_SPACE_EN ) i2cStatus |= EtaCspI2cProcessIntTxData(iNum) ;
  if (bCmdActive_interrupt & BM_I2C_INT_EN_RX_HIT_LEVEL_EN           ) i2cStatus |= EtaCspI2cProcessIntRxData(iNum) ;
  if (bCmdActive_interrupt & BM_I2C_INT_EN_CMD_DONE                  ) i2cStatus |= EtaCspI2cProcessIntCmdDone(iNum) ;

  // FENCE TO MAKE SURE ALL WRITES GET FLUSHED
  REG_GPIO_DEBUG1_READ_ONLY.V = REG_I2C_CONFIG(iNum).V;

  return i2cStatus;
}

/***************************************************************************//**
 *
 * EtaCspSocCtrlFence - Set up a fence for writes to the I2C0 & I2C1 regs.
 *
 * Add a fence on the last bus operation to I2C{0,1} registers.
 *
 *
 ******************************************************************************/
void
EtaCspI2cFence(void)
{
    EtaCspI2cFenceFast();
}

