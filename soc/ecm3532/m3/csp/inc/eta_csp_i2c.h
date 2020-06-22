/*******************************************************************************
 *
 * @file eta_csp_i2c.h
 *
 * @brief This file contains eta_csp_i2c module definitions.
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

#ifndef __ETA_CSP_I2C_H__
#define __ETA_CSP_I2C_H__

#include <stdint.h>
#include "eta_csp.h"
#include "eta_status.h"
#include "eta_chip.h"
#include "memio.h"

#ifdef __cplusplus
extern "C" {
#endif

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

//
//! I2C number typedef.
//
typedef enum
{
    //
    //! I2C 0.
    //
    eI2C0 = 0,

    //
    //! I2C 1.
    //
    eI2C1 = 1,
}
tI2cNum;



/*******************************************************************************
 *
 * I2C Commands
 *
 ******************************************************************************/
#define I2C_CMD_ENABLE_RXFIFO  (BFV_I2C_CMD_FIFO_DATA_CMD_RXFIFO_ENABLE_RXFIFO  << BP_I2C_CMD_FIFO_DATA_CMD_RXFIFO )
#define I2C_CMD_DISABLE_RXFIFO (BFV_I2C_CMD_FIFO_DATA_CMD_RXFIFO_DISABLE_RXFIFO << BP_I2C_CMD_FIFO_DATA_CMD_RXFIFO )
#define I2C_CMD_NACK_LOW       (BFV_I2C_CMD_FIFO_DATA_CMD_NACK_DRIVE_NACK_LOW   << BP_I2C_CMD_FIFO_DATA_CMD_NACK   )
#define I2C_CMD_NACK_HIGH      (BFV_I2C_CMD_FIFO_DATA_CMD_NACK_DRIVE_NACK_HIGH  << BP_I2C_CMD_FIFO_DATA_CMD_NACK   )
#define I2C_CMD_EXP_1NACK      (BM_I2C_CMD_FIFO_DATA_EXP_1NACK                                                     )
#define I2C_CMD_STOP           (BFV_I2C_CMD_FIFO_DATA_CMD_SRD_STOP              << BP_I2C_CMD_FIFO_DATA_CMD_SRD    )
#define I2C_CMD_DUMMYBYTE      (BFV_I2C_CMD_FIFO_DATA_CMD_SRD_DUMMYBYTE         << BP_I2C_CMD_FIFO_DATA_CMD_SRD    )
#define I2C_CMD_RESTART        (BFV_I2C_CMD_FIFO_DATA_CMD_SRD_RESTART           << BP_I2C_CMD_FIFO_DATA_CMD_SRD    )
#define I2C_CMD_BEGIN_READ     (I2C_CMD_ENABLE_RXFIFO | I2C_CMD_NACK_LOW)
#define I2C_CMD_END_WRITE      I2C_CMD_STOP
#define I2C_CMD_END_READ       (I2C_CMD_NACK_HIGH | I2C_CMD_STOP | I2C_CMD_EXP_1NACK)

/*******************************************************************************
 *
 * External function definitions
 *
 ******************************************************************************/

//
// Configure the speed for the I2C module.
//
tEtaStatus EtaCspI2cSpeedConfig(tI2cNum iNum, tI2cClk iClockSpeed);

//
// Poll for done.
//
tEtaStatus EtaCspI2cDonePoll(tI2cNum iNum);

//
// Poll for RX data ready.
//
tEtaStatus EtaCspI2cRxDataPoll(tI2cNum iNum);

//
// Poll for TX ready.
//
tEtaStatus EtaCspI2cTxReadyPoll(tI2cNum iNum);

//
// Triggers an I2C clearing event to clear any residual commands
// issued before a soft reset
//
tEtaStatus EtaCspI2cLineClear(tI2cNum iNum);

//
// Initialize the I2C module.
//
tEtaStatus EtaCspI2cInit(tI2cNum iNum, tI2cClk iClockSpeed);

//
// Read and or write to an I2C device.
//
tEtaStatus EtaCspI2cTransferPoll(tI2cNum iNum, uint8_t *pui8TxData, uint32_t ui32TxLen,
                      uint8_t *pui8RxData, uint32_t ui32RxLen,
                      uint32_t ui8RestartAfterByte);

//
// Returns Ack / No Ack response when we attempt to access device.
//
tEtaStatus EtaCspI2cDeviceAck (tI2cNum iNum, uint8_t ui8TxData);




////////////////////////////////////////////////////////////////////////////////////
// Interrupts

typedef struct
{
 uint8_t *        pui8TxData ;           // TX Data Array
 uint32_t         ui32TxLen ;            // Length of TX Data to Send
 uint8_t          ui8TxIntSize ;         // Watermark before you get TxData Available Interrupt
 uint8_t *        pui8RxData ;           // RX Data Array
 uint8_t          ui8RxIntSize ;         // Watermark before you get RxData Receieved Interrupt
 uint32_t         ui32RxLen ;            // Length of TX Data to Receive
 uint32_t         ui32RestartAfterByte ; // Byte to insert Restart bit in flow (0 if non needed)
 uint32_t         ui32CurrByteCount ;    // Current byte count. Used for restart (if needed)
 uint8_t          bRxLenIs0   :1 ;       // There are subtle differences between 0,1 and greather than 1 RX Length.
 uint8_t          bRxLenIs1   :1 ;       // These three fields define those.
 uint8_t          bRxLenIsGt1 :1 ;       //
 uint8_t          bCmdActive  :1 ;       // Command is bCmdActive
 uint8_t          rsvd        :4 ;       //
 tEtaStatus       i2cStatus ;            // Current Status of the Command
} EtaCspI2cCmd_t;

// Global pointer to command structs for I2C0 and I2C1
extern volatile EtaCspI2cCmd_t * g_pEtaCspI2cCmd[2];

//
// Read and or write to an I2C device via interrupt
//
tEtaStatus EtaCspI2cTransferInt(tI2cNum iNum, uint8_t * pui8TxData, uint32_t ui32TxLen,
                      uint8_t * pui8RxData, uint32_t ui32RxLen,
                      uint32_t ui32RestartAfterByte);

//
// Interrupt Process for Tx Data
//
tEtaStatus EtaCspI2cProcessIntTxData (tI2cNum iNum);

//
// Interrupt Process for Rx Data
//
tEtaStatus EtaCspI2cProcessIntRxData (tI2cNum iNum);

//
// Interrupt Process for Command Done
//
tEtaStatus EtaCspI2cProcessIntCmdDone (tI2cNum iNum);

//
// Interrupt Process for Unexpected NACK
//
tEtaStatus EtaCspI2cProcessIntNack (tI2cNum iNum);

//
// Top Level Interrupt Process Routine
//
tEtaStatus EtaCspI2cProcessIntEntry (tI2cNum iNum);


// Interrupts
////////////////////////////////////////////////////////////////////////////////////

//
// This fence will compile inline and do only one byte reads from the I2C0 & I2C1.
//
#define EtaCspI2cFenceFast() \
                    {REG_GPIO_DEBUG0_READ_ONLY.V = MEMIO8(REG_I2C_CONFIG_ADDR(0));}

//
// Add a fence on the last bus operation to I2C{0,1} registers.
//
extern void EtaCspI2cFence(void);


#ifdef __cplusplus
}
#endif

/** @}*/
/** @}*/
/** @}*/
/** @}*/


#endif // __ETA_CSP_I2C_H__

