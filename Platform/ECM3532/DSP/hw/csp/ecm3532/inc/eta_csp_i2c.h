/***************************************************************************//**
 *
 * @file eta_csp_i2c.h
 *
 * @brief This file contains eta_csp_i2c module implementations for the DSP.
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
 * This is part of revision 1.0.0b1 of the Tensai Chip Support Package.
 *
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup ecm3532i2c-dsp Inter-integrated Circuit (I2C)
 * @ingroup ecm3532csp-dsp
 * @{
 ******************************************************************************/
#ifndef __ETA_CSP_I2C_H__
#define __ETA_CSP_I2C_H__

#ifdef __cplusplus
extern "C" {
#endif

#define I2C_START_CMD   0x1
#define I2C_STOP_CMD    0x2
#define I2C_RESTART_CMD 0x4
#define I2C_NOACK_CMD   0x8

#define I2C_RESULT_ERR   -1
#define I2C_RESULT_ACK   1
#define I2C_RESULT_NOACK 0

#define I2C_SPEED_100K  1
#define I2C_SPEED_400K  4
#define I2C_SPEED_800K  8
#define I2C_SPEED_1000K 10

//
//! I2C source clock typedef.
//
typedef enum
{
    //
    //! I2C 8MHz source clock.
    //
    eI2cSrcClk8MHz = 8000,

    //
    //! I2C 4MHz source clock.
    //
    eI2cSrcClk4MHz = 4000,

    //
    //! I2C 2MHz source clock.
    //
    eI2cSrcClk2MHz = 2000,
}
tI2cSrcClk;

//
//! I2C clock typedef.
//
typedef enum
{
    //
    //! I2C standard (100kHz).
    //
    eI2cClk100kHz = 100,

    //
    //! I2C fast (400kHz).
    //
    eI2cClk400kHz = 400,

    //
    //! I2C FastPlus (1MHz).
    //
    eI2cClk1MHz = 1000,
}
tI2cClk;

/*******************************************************************************
 *
 * I2C Commands
 *
 ******************************************************************************/
#define I2C_CMD_ENABLE_RXFIFO  (BFV_I2C2_CMD_FIFO_DATA_CMD_RXFIFO_ENABLE_RXFIFO \
                                << BP_I2C2_CMD_FIFO_DATA_CMD_RXFIFO)
#define I2C_CMD_DISABLE_RXFIFO (BFV_I2C2_CMD_FIFO_DATA_CMD_RXFIFO_DISABLE_RXFIFO \
                                << BP_I2C2_CMD_FIFO_DATA_CMD_RXFIFO)
#define I2C_CMD_NACK_LOW       (BFV_I2C2_CMD_FIFO_DATA_CMD_NACK_DRIVE_NACK_LOW \
                                << BP_I2C2_CMD_FIFO_DATA_CMD_NACK)
#define I2C_CMD_NACK_HIGH      (BFV_I2C2_CMD_FIFO_DATA_CMD_NACK_DRIVE_NACK_HIGH \
                                << BP_I2C2_CMD_FIFO_DATA_CMD_NACK)
#define I2C_CMD_EXP_1NACK      (BM_I2C2_CMD_FIFO_DATA_EXP_1NACK)
#define I2C_CMD_STOP           (BFV_I2C2_CMD_FIFO_DATA_CMD_SRD_STOP << \
                                BP_I2C2_CMD_FIFO_DATA_CMD_SRD)
#define I2C_CMD_DUMMYBYTE      (BFV_I2C2_CMD_FIFO_DATA_CMD_SRD_DUMMYBYTE << \
                                BP_I2C2_CMD_FIFO_DATA_CMD_SRD)

#define I2C_CMD_RESTART    (BFV_I2C2_CMD_FIFO_DATA_CMD_SRD_RESTART << \
                            BP_I2C2_CMD_FIFO_DATA_CMD_SRD)
#define I2C_CMD_BEGIN_READ (I2C_CMD_ENABLE_RXFIFO | I2C_CMD_NACK_LOW)
#define I2C_CMD_END_WRITE  I2C_CMD_STOP
#define I2C_CMD_END_READ   (I2C_CMD_NACK_HIGH | I2C_CMD_STOP | \
                            I2C_CMD_EXP_1NACK)

/*******************************************************************************
 *
 * External function definitions
 *
 ******************************************************************************/

//
// Configure the speed for the I2C module.
//
tEtaStatus EtaCspI2cSpeedConfig(tI2cClk iClockSpeed, tI2cSrcClk iSrcClockSpeed);

//
// Poll for done.
//
tEtaStatus EtaCspI2cDonePoll();

//
// Poll for RX data ready.
//
tEtaStatus EtaCspI2cRxDataPoll();

//
// Poll for TX ready.
//
tEtaStatus EtaCspI2cTxReadyPoll();

//
// Triggers an I2C clearing event to clear any residual commands
// issued before a soft reset
//
tEtaStatus EtaCspI2cLineClear();

//
// Initialize the I2C module.
//
tEtaStatus EtaCspI2cInit(tI2cClk iClockSpeed, tI2cSrcClk iSrcClockSpeed);

//
// Read and or write to an I2C device.
//
tEtaStatus EtaCspI2cTransferPoll(uint8_t *pui8TxData, uint16_t ui16TxLen,
                                 uint8_t *pui8RxData, uint16_t ui16RxLen,
                                 uint16_t ui8RestartAfterByte);

//
// Returns Ack / No Ack response when we attempt to access device.
//
tEtaStatus EtaCspI2cDeviceAck(uint8_t ui8TxData);

//
// Enable DMA tranfers
//
void EtaCspI2cTransferDmaConfig(void);

////////////////////////////////////////////////////////////////////////////////////
// Interrupts

typedef struct
{
    uint8_t *pui8TxData; // TX Data Array
    uint16_t ui16TxLen; // Length of TX Data to Send
    uint8_t ui8TxIntSize; // Watermark before you get TxData
                          // Available Interrupt
    uint8_t *pui8RxData; // RX Data Array
    uint8_t ui8RxIntSize; // Watermark before you get RxData
                          // Receieved Interrupt
    uint16_t ui16RxLen; // Length of TX Data to Receive
    uint16_t ui16RestartAfterByte; // Byte to insert Restart bit in flow
                                   // (0 if non needed)
    uint32_t ui32CurrByteCount; // Current byte count. Used for restart
                                // (if needed)
    uint8_t bRxLenIs0   : 1; // There are subtle differences between
                             // 0,1 and greather than 1 RX Length.
    uint8_t bRxLenIs1   : 1; // These three fields define those.
    uint8_t bRxLenIsGt1 : 1; //
    volatile uint8_t bCmdActive  : 1; // Command is bCmdActive
    uint8_t rsvd        : 4; //
    tEtaStatus i2cStatus; // Current Status of the Command
} tI2cCmd;

// Global pointer to command structs for I2C2
//extern volatile tI2cCmd *g_pEtaCspI2cCmd[1];
extern volatile tI2cCmd gsI2cCmd;

//
// Read and or write to an I2C device via interrupt
//
tEtaStatus EtaCspI2cTransferInt(uint8_t *pui8TxData, uint16_t ui16TxLen,
                                uint8_t *pui8RxData, uint16_t ui16RxLen,
                                uint16_t ui16RestartAfterByte);

//
// Interrupt Process for Tx Data
//
tEtaStatus EtaCspI2cProcessIntTxData();

//
// Interrupt Process for Rx Data
//
tEtaStatus EtaCspI2cProcessIntRxData();

//
// Interrupt Process for Command Done
//
tEtaStatus EtaCspI2cProcessIntCmdDone();

//
// Interrupt Process for Unexpected NACK
//
tEtaStatus EtaCspI2cProcessIntNack();

//
// Top Level Interrupt Process Routine
//
tEtaStatus EtaCspI2cProcessIntEntry();

//
// Top Level Interrupt Service Routine
//
void EtaCspI2cIsr(tIrqNum iIrqNum, intline_T int_line);



#endif // __ETA_CSP_I2C_H__

