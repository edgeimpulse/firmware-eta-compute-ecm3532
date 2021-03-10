/***************************************************************************//**
 *
 * @file eta_csp_spi.h
 *
 * @brief This file contains eta_csp_spi module definitions.
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
 * This is part of revision 1.0.0b1 of the Tensai Chip Support Package.
 *
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup ecm3532spi-m3 Serial Peripheral Interface (SPI)
 * @ingroup ecm3532csp-m3
 * @{
 ******************************************************************************/

#ifndef __ETA_CSP_SPI_H__
#define __ETA_CSP_SPI_H__

#include <stdint.h>
#include "eta_csp.h"
#include "eta_status.h"
#include "eta_chip.h"
#include "memio.h"

#ifdef __cplusplus
extern "C" {
#endif

//
//! SPI clock typedef.
//
typedef enum
{
    //
    //! 500kHz
    //
    eSpiClk500kHz = 500,

    //
    //! 1MHz
    //
    eSpiClk1MHz = 1000,

    //
    //! 2MHz
    //
    eSpiClk2MHz = 2000,

    //
    //! 4MHz
    //
    eSpiClk4MHz = 4000,
}
tSpiClkFreq;

//
//! SPI number typedef.
//
typedef enum
{
    //
    // invalid
    //
    eSpiInvalid = -1,
    
    //
    //! SPI 0.
    //
    eSpi0 = 0,

    //
    // SPI 1.
    //
    eSpi1 = 1,
    
    //
    // SPI Max.
    //
    eSpiMax = 2,
}
tSpiNum;

//
//! SPI clock polarity typedef.
//
typedef enum
{
    //
    //! Negative clock polarity.
    //
    eSpiClkPolNeg = 1,

    //
    //! Positive clock polatity.
    //
    eSpiClkPolPos = 0,
}
tSpiClkPol;

//
//! SPI clock phase typedef.
//
typedef enum
{
    //
    //! Leading Clock Edge
    //
    eSpiClkLeadEdge = 0,

    //
    //! Training Clock Edge
    //
    eSpiClkTrailEdge = 1,
}
tSpiClkPhase;

//
//! SPI bit endian typedef.
//
typedef enum
{
    //
    //! MSb endianness.
    //
    eSpiBitEndianMSb = 0,

    //
    //! LSb endianness.
    //
    eSpiBitEndianLSb = 1,
}
tSpiBitEndian;

//
//! SPI byte endian typedef.
//
typedef enum
{
    //
    //! LSB endianness.
    //
    eSpiByteEndianLSB = 0,

    //
    //! MSB endianness.
    //
    eSpiByteEndianMSB = 1,
}
tSpiByteEndian;

//
//! SPI Sequence typedef
//
typedef enum
{
    //
    //! First and Last (aka Only), must be OR of First Only and Last Only
    //! Encoding
    //
    eSpiSequenceFirstLast = 3,

    //
    //! First Only
    //
    eSpiSequenceFirstOnly = 1,

    //
    //! Last Only
    //
    eSpiSequenceLastOnly = 2,

    //
    //! Middle
    //
    eSpiSequenceMiddle = 0,
}
tSpiSequence;

//
//! SPI chip select typedef.
//
typedef enum
{
    //
    // invalid
    //
    eSpiChipSelInvalid = -1,
    //
    //! Chip select 0.
    //
    eSpiChipSel0 = 0,

    //
    //! Chip select 1.
    //
    eSpiChipSel1 = 1,

    //
    //! Chip select 2.
    //
    eSpiChipSel2 = 2,

    //
    //! Chip select 3.
    //
    eSpiChipSel3 = 3,
}
tSpiChipSel;

typedef struct
{
    tSpiByteEndian spiByteEndian;
    tSpiBitEndian spiBitEndian;
    tSpiClkPhase spiClkPhase;
    tSpiClkPol spiClkPol;
    tSpiClkFreq spiClkFreq;
    bool bSpiSlave;
    uint8_t ui8IntSize;                         // Default Interrupt Watermark
    uint8_t ui8ICG;                             // Cycles between successive
                                                // bytes (0xFF means 1 bit time)
    uint8_t ui8IBG;                             // Cycles between successive
                                                // commands (0xFF means 1 bit
                                                // time)
} tSpiConfig;

//
// Poll for done.
//
tEtaStatus EtaCspSpiDonePoll(tSpiNum iNum);

//
// Poll for done.
//
tEtaStatus EtaCspSpiXmitDonePoll(tSpiNum iNum);

//
// Poll for RX data ready.
//
tEtaStatus EtaCspSpiRxDataPoll(tSpiNum iNum);

//
// Poll for TX ready.
//
tEtaStatus EtaCspSpiTxReadyPoll(tSpiNum iNum);

//
// Main Polling Transfer Function
//
tEtaStatus EtaCspSpiTransferPoll(tSpiNum iNum, uint8_t *pui8TxData,
                                 uint32_t ui32TxLen,
                                 uint8_t *pui8RxData, uint32_t ui32RxLen,
                                 tSpiChipSel spiChipSel,
                                 tSpiSequence iSpiSequence);

//
// Main SPI Init
//
tEtaStatus EtaCspSpiInit(tSpiNum iNum, tSpiConfig iSpiConfig);

////////////////////////////////////////////////////////////////////////////////
// Interrupts

typedef struct
{
    uint8_t *pui8TxData;                 // TX Data Array
    uint32_t ui32TxLen;                  // Length of TX Data to Send
    uint8_t ui8TxIntSize;                // Watermark before you get TxData
                                         // Available Interrupt
    uint8_t *pui8RxData;                 // RX Data Array
    uint8_t ui8RxIntSize;                // Watermark before you get RxData
                                         // Receieved Interrupt
    uint32_t ui32RxLen;                  // Length of TX Data to Receive
    uint8_t bCmdActive  : 1;             // Command is bCmdActive
    uint8_t rsvd        : 7;             //
    tEtaStatus spiStatus;                // Current Status of the Command
    tSpiSequence iSpiSequence;           // SPI Command Sequence
} tSpiCmd;


typedef struct
{
    bool bCmdIsDone      : 1;            // CS Deasserted
    bool bXmitComplete   : 1;            // All TX bytes are on the wire
    bool bRxFifoHitLevel : 1;            // RX FIFO has hit watermark
    bool bTxFifoHitLevel : 1;            // TX FIFO has hit watermark
    bool bRxFifoHasData  : 1;            // RX FIFO has data
    bool bTxFifoHasSpace : 1;            // TX FIFO has space
} tSpiInt;


// Global pointer to command structs for SPI0 and SPI1
extern volatile tSpiCmd *g_pEtaCspSpiCmd[2];

//
// Get interrupt status structure
//
extern tSpiInt EtaCspSpiIntStatusGet(tSpiNum iNum);

//
// Set interrupts enable bits
//
extern void EtaCspSpiIntSet(tSpiNum iNum, tSpiInt iSpiIntStatus);

//
// Enable interrupts with bits set to TRUE
//
extern void EtaCspSpiIntEnable(tSpiNum iNum, tSpiInt iSpiIntStatus);

//
// Disable interrupts with bits set to TRUE
//
extern void EtaCspSpiIntDisable(tSpiNum iNum, tSpiInt iSpiIntStatus);

//
// Set interrupts enable bits
//
extern tSpiInt EtaCspSpiIntGet(tSpiNum iNum);

//
// Read and or write to an Spi device via interrupt
//
extern tEtaStatus EtaCspSpiTransferInt(tSpiNum iNum, uint8_t *pui8TxData,
                                       uint32_t ui32TxLen,
                                       uint8_t *pui8RxData, uint32_t ui32RxLen,
                                       tSpiChipSel spiChipSel,
                                       tSpiSequence iSpiSequence);

//
// Interrupt Process for Tx Data
//
extern tEtaStatus EtaCspSpiProcessIntTxData(tSpiNum iNum);

//
// Interrupt Process for Rx Data
//
extern tEtaStatus EtaCspSpiProcessIntRxData(tSpiNum iNum);

//
// Interrupt Process for Transmit/Receive FIFO Empty
//
extern tEtaStatus EtaCspSpiProcessIntXmitDone(tSpiNum iNum);

//
// Interrupt Process for Command Done
//
extern tEtaStatus EtaCspSpiProcessIntCmdDone(tSpiNum iNum);

//
// Top Level Interrupt Process Routine
//
extern tEtaStatus EtaCspSpiProcessIntEntry(tSpiNum iNum);

// Interrupts
//////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////
// SPI Slave

//
// Receive bytes from the master. Function will return fewer than the requested
//   number of bytes if the RX buffer becomes empty.
//
tEtaStatus EtaCspSpiSlaveRxDataReceive(tSpiNum iNum, uint8_t * pui8RxData,
                        uint32_t ui32RxReadLen, uint32_t * pui32RxRecLen);

//
// Send bytes to the master. Function will send fewer than the requested number
//   of bytes if the TX buffer becomes full.
//
tEtaStatus EtaCspSpiSlaveTxDataSend(tSpiNum iNum, uint8_t * pui8TxData,
                        uint32_t ui32TxWriteLen, uint32_t * pui32TxSentLen);

// SPI Slave
////////////////////////////////////////////////////////////////////////////////

//
// This fence will compile inline and do only one byte reads from the SPI0,SPI1,
// UART0, UART1, and TIMER blocks.
//
#define EtaCspSpiFenceFast() \
                             {REG_GPIO_DEBUG0_READ_ONLY.V = MEMIO8(REG_SPI_CONFIG_ADDR(0));}

//
// Add a fence on the last bus operation to SPI{0,1} registers.
//
extern void EtaCspSpiFence(void);


#ifdef __cplusplus
}
#endif

#endif // __ETA_CSP_SPI_H__

