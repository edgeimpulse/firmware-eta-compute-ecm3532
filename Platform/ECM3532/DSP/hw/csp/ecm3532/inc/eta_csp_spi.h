/***************************************************************************//**
 *
 * @file eta_csp_spi.h
 *
 * @brief This file contains eta_csp_spi module definitions.
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
 * @addtogroup ecm3532spi-dsp Serial Peripheral Interface (SPI)
 * @ingroup ecm3532csp-dsp
 * @{
 ******************************************************************************/

//
// SPI CSP
//
#ifndef ETA_CSP_SPI_H
#define ETA_CSP_SPI_H

#include "eta_csp_status.h"
#include "eta_csp_isr.h"
#include "eta_csp_dma.h"



//
//! SPI clock typedef.
//
typedef enum
{
    //
    //! 500kHz
    //
    eSpiClk500kHz = 500U,

    //
    //! 1MHz
    //
    eSpiClk1MHz = 1000U,

    //
    //! 2MHz
    //
    eSpiClk2MHz = 2000U,

    //
    //! 4MHz
    //
    eSpiClk4MHz = 4000U,
}
tSpiClkFreq;

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
    uint8_t ui8IntSize; // Default Interrupt Watermark
    uint8_t ui8ICG; // Cycles between successive
                    // bytes (0xFF means 1 bit time)
    uint8_t ui8IBG; // Cycles between successive
                    // commands (0xFF means 1 bit
                    // time)
} tSpiConfig;

//
// Poll for done.
//
extern tEtaStatus EtaCspSpiDonePoll(void);

//
// Poll for done.
//
extern tEtaStatus EtaCspSpiXmitDonePoll(void);

//
// Poll for RX data ready.
//
extern tEtaStatus EtaCspSpiRxDataPoll(void);

//
// Poll for TX ready.
//
extern tEtaStatus EtaCspSpiTxReadyPoll(void);

//
// Main Polling Transfer Function
//
extern tEtaStatus EtaCspSpiTransferPoll(uint8_t *pui8TxData, uint16_t ui16TxLen,
                                        uint8_t *pui8RxData, uint16_t ui16RxLen,
                                        tSpiChipSel spiChipSel,
                                        tSpiSequence iSpiSequence);

//
// Main SPI Init
//
tEtaStatus EtaCspSpiInit(tSpiConfig iSpiConfig);

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
    volatile uint8_t bCmdActive  : 1; // Command is bCmdActive
    uint8_t rsvd        : 7; //
    tEtaStatus spiStatus; // Current Status of the Command
    tSpiSequence iSpiSequence; // SPI Command Sequence
} tSpiCmd;

// Global pointer to command structs for SPI2
//extern volatile EtaCspSpiCmd_t *g_pEtaCspSpiCmd[1];
extern volatile tSpiCmd gsSpiCmd;

//
// Read and or write to an Spi device via DMA
//
tEtaStatus
EtaCspSpiTransferDmaConfig(uint16_t ui16TxLen, uint16_t ui16RxLen,
                           tSpiChipSel spiChipSel, tSpiSequence iSpiSequence);

//
// Read and or write to an Spi device via interrupt
//
tEtaStatus EtaCspSpiTransferInt(uint8_t *pui8TxData, uint16_t ui16TxLen,
                                uint8_t *pui8RxData, uint16_t ui16RxLen,
                                tSpiChipSel spiChipSel,
                                tSpiSequence iSpiSequence);

/***************************************************************************//**
 *
 *  EtaCspSpiIsr- ISR handler for SPI interrupts.
 *
 *  @param iIrqNum is the interrupt number.
 *  @param int_line is the DSP interrupt line number.
 *
 ******************************************************************************/
extern void EtaCspSpiIsr(tIrqNum iIrqNum, intline_T int_line);

//
// Interrupt Process for Tx Data
//
extern tEtaStatus EtaCspSpiProcessIntTxData(void);

//
// Interrupt Process for Rx Data
//
extern tEtaStatus EtaCspSpiProcessIntRxData(void);

//
// Interrupt Process for Transmit/Receive FIFO Empty
//
extern tEtaStatus EtaCspSpiProcessIntXmitDone(void);

//
// Interrupt Process for Command Done
//
extern tEtaStatus EtaCspSpiProcessIntCmdDone(void);

//
// Top Level Interrupt Process Routine
//
extern tEtaStatus EtaCspSpiProcessIntEntry(void);

// Interrupts
////////////////////////////////////////////////////////////////////////////////////

#endif // ETA_CSP_SPI_H

