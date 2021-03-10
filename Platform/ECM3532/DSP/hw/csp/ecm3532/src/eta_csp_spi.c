/***************************************************************************//**
 *
 * @file eta_csp_spi.c
 *
 * @brief This file contains eta_csp_spi module implementations.
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

#include "reg.h"
#include "eta_csp_spi.h"
#include "reg_eta_ecm3532_dsp.h"
#include "eta_csp_mailbox.h"
#include "eta_csp_io.h"
#include "eta_csp_isr.h"
#include "eta_csp_dma.h"


// If 1, we'll do an additional read per
// interrupt
// to determine how much TX FIFO avail or RX
// FIFO ready
// and read that amount. If 0, we do one
// write/read
// per interrupt. Greedy tends to be more
// efficient, but does
// add one read which is not strictly
// required since we know
// the interrupt watermarks.
#define ETA_CSP_SPI_EN_GREEDY_INT 0

/***************************************************************************//**
 *
 * EtaCspSpiDonePoll - Poll for done.
 *
 * @return Return the status.
 *
 ******************************************************************************/
tEtaStatus
EtaCspSpiDonePoll()
{
    // In the extremes, this should
    // be 10,000 bit times.
    uint32_t ui32Timeout = 10000000;

    while((--ui32Timeout) &&
          ((REG_SPI2_STATUS & BM_SPI2_STATUS_START_BUSY) ==
           BM_SPI2_STATUS_START_BUSY))
    {
    }

    if(ui32Timeout == 0)
    {
        return(eEtaSpiDoneTimeout);
    }
    else
    {
        return(eEtaSuccess);
    }
}

/***************************************************************************//**
 *
 * EtaCspSpiTxReadyPoll - Poll for TX ready.
 *
 * @return Return the status.
 *
 ******************************************************************************/
tEtaStatus
EtaCspSpiTxReadyPoll()
{
    // In the extremes, this should
    // be 10,000 bit times.
    uint32_t ui32Timeout = 10000000;

    while((--ui32Timeout) &&
          ((REG_SPI2_STATUS & BM_SPI2_STATUS_TXFIFO_HAS_1SPACE) == 0))
    {
    }

    if(ui32Timeout == 0)
    {
        return(eEtaSpiSendTimeout);
    }
    else
    {
        return(eEtaSuccess);
    }
}

/***************************************************************************//**
 *
 * EtaCspSpiRxDataPoll - Poll for RX data ready.
 *
 * @return Return the status.
 *
 ******************************************************************************/
tEtaStatus
EtaCspSpiRxDataPoll()
{
    // In the extremes, this should
    // be 10,000 bit times.
    uint32_t ui32Timeout = 10000000;

    while((--ui32Timeout) &&
          ((REG_SPI2_STATUS & BM_SPI2_STATUS_RXFIFO_HAS_1DATA) == 0))
    {
    }

    if(ui32Timeout == 0)
    {
        return(eEtaSpiRecTimeout);
    }
    else
    {
        return(eEtaSuccess);
    }
}

/***************************************************************************//**
 *
 * EtaCspSpiXmitDonePoll - Poll for All Transmit / Receive to be Done
 *
 * @return Return the status.
 *
 ******************************************************************************/
tEtaStatus
EtaCspSpiXmitDonePoll()
{
    // In the extremes, this should
    // be 10,000 bit times.
    uint32_t ui32Timeout = 10000000;

    while((--ui32Timeout) &&
          ((REG_SPI2_STATUS & BM_SPI2_STATUS_XMIT_COMPLETE) == 0))
    {
    }

    if(ui32Timeout == 0)
    {
        return(eEtaSpiDoneTimeout);
    }
    else
    {
        return(eEtaSuccess);
    }
}

/***************************************************************************//**
 *
 * EtaCspSpiTransferPoll - Read and or write to an SPI device.
 *
 * @param pui8TxData Pointer to the data to transmit.
 * @param ui16TxLen Transmit length in bytes.
 * @param pui8RxData Point to the received data.
 * @param ui16RxLen Receive length in bytes.
 * @param spiChipSel Chip Select to use
 * @param iSpiSequence Sequence field allowing multiple functions in a single
 * SPI command
 * @return Return the status.
 *
 ******************************************************************************/
tEtaStatus
EtaCspSpiTransferPoll(uint8_t *pui8TxData, uint16_t ui16TxLen,
                      uint8_t *pui8RxData, uint16_t ui16RxLen,
                      tSpiChipSel spiChipSel, tSpiSequence iSpiSequence)
{
    tEtaStatus localStatus;
    uint32_t ui32Timeout;
    uint8_t tx_byte;
    uint8_t rx_byte;

// Impossible if we use 16 bit lengths...
// but keep incase we standardize to 32 bit lengths.
// if (ui32TxLen > (64*1024)-1)
// {
// #ifdef ETA_CSP_SPI_DEBUG
// etaPrintf ("ERROR: EtaCspSpiTransferPoll exceeded max ui32TxLen of
// (64*1024)-1 with %d", ui32TxLen);
// #endif
// return eEtaSpiInvalidConfig;
// }
//
// if (ui32RxLen > (64*1024)-1)
// {
// #ifdef ETA_CSP_SPI_DEBUG
// etaPrintf ("ERROR: EtaCspSpiTransferPoll exceeded max ui32RxLen of
// (64*1024)-1 with %d", ui32RxLen);
// #endif
// return eEtaSpiInvalidConfig;
// }

    // If it is first, start command
    if(iSpiSequence & eSpiSequenceFirstOnly)
    {
        // Make sure previous command is done (optional)
        if(localStatus = EtaCspSpiDonePoll())
        {
            return(localStatus);
        }

        // Start command
        REG_SPI2_STATUS = BM_SPI2_STATUS_START_BUSY |
                          (spiChipSel << BP_SPI2_STATUS_CS_SEL) |
                          BM_SPI2_STATUS_HOLD_CS;
    }

    //
    // If this is not the start of the command wait till
    // dummy bytes registers are 0. If this is the start
    // of the command these are 0 anyway.
    //
    while(REG_SPI2_RXDROP_CNT)
    {
    }
    while(REG_SPI2_TX_DUMMY_BYTE_CNT)
    {
    }

    // Deal with TX first
    // We'll drop ui16TxLen of data (garbage receive data during TX)
    REG_SPI2_RXDROP_CNT = ui16TxLen;

    ui32Timeout = 10000;
    while((--ui32Timeout) && (ui16TxLen))
    {
        if(localStatus = EtaCspSpiTxReadyPoll())
        {
            return(localStatus);
        }

        tx_byte = *pui8TxData++;

        REG_SPI2_TXRX_1BYTE_FIFO_DATA = tx_byte;
        ui16TxLen--;
    }
    if(ui32Timeout == 0)
    {
        return(1);
    }

    // Now deal with RX.

    // Write number of dummy bytes to send to get rx data we want
    REG_SPI2_TX_DUMMY_BYTE_CNT = ui16RxLen;

    // If it is Last, end command
    if(iSpiSequence & eSpiSequenceLastOnly)
    {
        // At this point, all TX is in the queue. Remove hold
        REG_SPI2_STATUS = 0;
    }

    ui32Timeout = 10000;

    // 1 Byte version
    while((--ui32Timeout) && (ui16RxLen))
    {
        localStatus = EtaCspSpiRxDataPoll();
        if(localStatus)
        {
            return(localStatus);
        }

        // pop data off the read stack
        rx_byte = REG_SPI2_TXRX_1BYTE_FIFO_DATA;
        *pui8RxData++ = rx_byte;

        // If we have data to send, ignore receive data.
        ui16RxLen--;
    }

    // This ui32Timeout is for general programming reasons to prevent infinite
    // loop
    if(ui32Timeout == 0)
    {
        return(eEtaFailure);
    }

    return(eEtaSuccess);
}

/***************************************************************************//**
 *
 * EtaCspSpiInit - Initialize the SPI module.
 *
 * @param tSpiConfig is the SPI Configurtation struct
 *
 * The function takes the parameters set in the struct and initializes the
 * SPI module. The source clock to SPI has to be set at 8MHz. Note that the
 * source clock speed configurable, and can be set to either 8MHz (default),
 * 4MHz, or 2MHz. SPI will function correctly only when the source clock is
 * 8MHz.
 *
 * > The source clock has to be 8MHz. Make sure you set this correctly before
 *   initializing the SPI. Don't change the source clock after initializing the
 *   SPI.
 *
 * @return Returns eEtaSuccess if initialization was successful.
 *
 ******************************************************************************/
tEtaStatus
EtaCspSpiInit(tSpiConfig iSpiConfig)
{
    uint16_t spi_config;
    uint16_t spi_config2;
    uint16_t spi_divclk;
    uint16_t hfo_freq;
    uint16_t spi_clk_freq;

    // Configurations
    spi_config = (iSpiConfig.ui8IntSize << BP_SPI2_CONFIG_FIFO_LEVEL_TRIG) |
                 (iSpiConfig.spiBitEndian << BP_SPI2_CONFIG_END)           |
                 (iSpiConfig.spiClkPhase << BP_SPI2_CONFIG_CPHA)           |
                 (iSpiConfig.spiClkPol << BP_SPI2_CONFIG_CPOL);

    spi_config2 =
        (iSpiConfig.spiByteEndian << BP_SPI2_CONFIG2_RX_BYTE2_END) |
        (iSpiConfig.spiByteEndian << BP_SPI2_CONFIG2_TX_BYTE2_END) |
        (0x1 << BP_SPI2_CONFIG2_AGGR_CLK_GATING)                   |
        (0x1 << BP_SPI2_CONFIG2_STALL_IF_RXFULL);

    // assume 8MHz, this may not be true at all times and could be 4Mhz (maybe
    // 2MHz too?)
    // FIXME (need much more complex config with M3 sending literally one word
    // to make this work
    // or some kind of compile option)
    hfo_freq = 8000;

    spi_clk_freq = iSpiConfig.spiClkFreq;

    // Check if desired close is within range
    if((spi_clk_freq * 2) > hfo_freq) // Cannot achieve freq (max)
    {
        return(eEtaSpiInvalidConfig);
    }

    if((spi_clk_freq << 4) < hfo_freq) // Cannot achieve freq (min)
    {
        return(eEtaSpiInvalidConfig);
    }

    spi_divclk = ((hfo_freq / spi_clk_freq) - 1) & BM_SPI2_CLKDIV_CLKDIV;

    // Add some subtle tweaks for corner cases
    switch(spi_divclk)
    {
        case (0):
        {
            return(eEtaSpiInvalidConfig);
            break; // Illegal
        }

        case (1):
        {
            spi_config2 |= (1 << BP_SPI2_CONFIG2_RX_SAMPLE_TIME) |
                           BM_SPI2_CONFIG2_DISABLE_MISO_METAFLOP;
            break;
        }

        case (2):
        {
            spi_config2 |= (2 << BP_SPI2_CONFIG2_RX_SAMPLE_TIME);
            break;
        }

        default:
        { }
        break;
    }

    // Add in inter command gap
    if(iSpiConfig.ui8ICG == 0xff)
    {
        spi_divclk |= (spi_divclk & BM_SPI2_CLKDIV_CLKDIV) <<
                      BP_SPI2_CLKDIV_ICG;
    }
    else
    {
        spi_divclk |= (iSpiConfig.ui8ICG & BM_SPI2_CLKDIV_ICG) <<
                      BP_SPI2_CLKDIV_ICG;
    }

    // Add in inter byte gap
    if(iSpiConfig.ui8IBG == 0xff)
    {
        spi_divclk |= (spi_divclk & BM_SPI2_CLKDIV_CLKDIV) <<
                      BP_SPI2_CLKDIV_IBG;
    }
    else
    {
        spi_divclk |= (iSpiConfig.ui8IBG & BM_SPI2_CLKDIV_IBG) <<
                      BP_SPI2_CLKDIV_IBG;
    }

    REG_SPI2_CONFIG = spi_config;
    REG_SPI2_CONFIG2 = spi_config2;
    REG_SPI2_CLKDIV = spi_divclk;

    return(eEtaSuccess);
}

////////////////////////////////////////////////////////////////////////////////
// Interrupts

// Global pointer to command structs for SPI2
// Note, this doesn't need to be an array of 1 deep, but doing so
// makes the structure more like the M3, so value in similarity.
//volatile EtaCspSpiCmd_t *g_pEtaCspSpiCmd[1];
volatile tSpiCmd gsSpiCmd;

/***************************************************************************//**
 *
 * EtaCspSpiTransferDma - Start the SPI using DMA
 *
 * @param ui16TxLen TX Data Length.
 * @param ui16RxLen RX Data Length.
 * @param spiChipSel is the chip select (0-3).
 * @param iSpiSequence is the SPI command sequence.
 *
 * >You cannot pass a non-zero uiTxLen and uiRxLen in a call. If you do,
 *  uiRxLen will be ignored. If you are setting up a TX DMA tranfer
 *  iSpiSequence has no effect. If you want to hold chip select active, do
 *  it explicitly. If you are setting up a RX DMA transfer, passing
 *  eSpiSequenceLastOnly will deassert the chip select signal.
 *
 ******************************************************************************/
tEtaStatus
EtaCspSpiTransferDmaConfig(uint16_t ui16TxLen, uint16_t ui16RxLen,
                           tSpiChipSel spiChipSel, tSpiSequence iSpiSequence)
{
    tEtaStatus localStatus;
    uint32_t ui32Timeout;
    uint8_t lastcmd;

// Impossible if we use 16 bit lengths...
// but keep incase we standardize to 32 bit lengths.
// if (ui32TxLen > (64*1024)-1)
// {
// #ifdef ETA_CSP_SPI_DEBUG
// etaPrintf ("ERROR: EtaCspSpiTransferPoll exceeded max ui32TxLen of
// (64*1024)-1 with %d", ui32TxLen);
// #endif
// return eEtaSpiInvalidConfig;
// }
//
// if (ui32RxLen > (64*1024)-1)
// {
// #ifdef ETA_CSP_SPI_DEBUG
// etaPrintf ("ERROR: EtaCspSpiTransferPoll exceeded max ui32RxLen of
// (64*1024)-1 with %d", ui32RxLen);
// #endif
// return eEtaSpiInvalidConfig;
// }

    if(ui16TxLen && REG_SPI2_RXDROP_CNT)
    {
        return eEtaSpiBusy;
    }
    else if(ui16RxLen && REG_SPI2_TX_DUMMY_BYTE_CNT)
    {
        return eEtaSpiBusy;
    }

    // If it is first, start command
    if(iSpiSequence & eSpiSequenceFirstOnly)
    {
        // Make sure previous command is done (optional)
        //if(localStatus = EtaCspSpiDonePoll())
        //{
        //    return(localStatus);
        //}
        if((REG_SPI2_STATUS & BM_SPI2_STATUS_START_BUSY) ==
              BM_SPI2_STATUS_START_BUSY)
        {
            return eEtaSpiBusy;
        }

        // Start command
        if(iSpiSequence & eSpiSequenceLastOnly)
        {
            REG_SPI2_STATUS = BM_SPI2_STATUS_START_BUSY |
                              (spiChipSel << BP_SPI2_STATUS_CS_SEL);
        }
        else
        {
            REG_SPI2_STATUS = BM_SPI2_STATUS_START_BUSY |
                              (spiChipSel << BP_SPI2_STATUS_CS_SEL) |
                              BM_SPI2_STATUS_HOLD_CS;
        }
    }

    if(ui16TxLen) // If we have TX, lets process it.
    {
        // Deal with TX first
        // We'll drop ui16TxLen of data (garbage receive data during TX)
        REG_SPI2_RXDROP_CNT = ui16TxLen;

        REG_SPI2_CONFIG = (REG_SPI2_CONFIG & ~BM_SPI2_CONFIG_FIFO_LEVEL_TRIG) |
                          (2 << BP_SPI2_CONFIG_FIFO_LEVEL_TRIG);
    }
    else if(ui16RxLen) // No TX, just RX
    {
        REG_SPI2_TX_DUMMY_BYTE_CNT = ui16RxLen;

        // If it is Last, end command
        if(iSpiSequence & eSpiSequenceLastOnly)
        {
            // At this point, all TX is in the queue. Remove hold
            REG_SPI2_STATUS = 0;
        }

        REG_SPI2_CONFIG = (REG_SPI2_CONFIG & ~BM_SPI2_CONFIG_FIFO_LEVEL_TRIG) |
                          (2 << BP_SPI2_CONFIG_FIFO_LEVEL_TRIG);
    }
    else // No TX, No RX
    {
        // If it is Last, end command
        if(iSpiSequence & eSpiSequenceLastOnly)
        {
            // At this point, all TX is in the queue. Remove hold
            REG_SPI2_STATUS = 0;
        }
    }

    // Setup next expected interrupt
    REG_SPI2_INT_EN =
        ((ui16TxLen != 0) ? BM_SPI2_INT_EN_TXFIFO_HAS_LEVEL_SPACE_EN2 : 0)    |
        ((ui16RxLen != 0) ? BM_SPI2_INT_EN_RX_FIFO_HIT_LEVEL_EN1 : 0);

    return(eEtaSuccess);
}

/***************************************************************************//**
 *
 *  EtaCspSpiTransferInt - Start the SPI using Interrupt
 *
 *  @param pui8TxData is the pointer to TX Data Array.
 *  @param ui16TxLen is the TX Data Array length.
 *  @param pui8RxData is the pointer to RX Data Array.
 *  @param ui16RxLen is the RX Data Array length.
 *  @param spiChipSel is the chip select (0-3).
 *  @param iSpiSequence is the SPI command sequence.
 *
 * > eSpiSequenceMiddle is not a valid SPI command sequence when using the
 *   interrupt mode.
 *
 ******************************************************************************/
tEtaStatus
EtaCspSpiTransferInt(uint8_t *pui8TxData, uint16_t ui16TxLen,
                     uint8_t *pui8RxData, uint16_t ui16RxLen,
                     tSpiChipSel spiChipSel, tSpiSequence iSpiSequence)
{
    tEtaStatus localStatus;
    uint32_t ui32Timeout;
    uint8_t lastcmd;

// Impossible if we use 16 bit lengths...
// but keep incase we standardize to 32 bit lengths.
// if (ui32TxLen > (64*1024)-1)
// {
// #ifdef ETA_CSP_SPI_DEBUG
// etaPrintf ("ERROR: EtaCspSpiTransferPoll exceeded max ui32TxLen of
// (64*1024)-1 with %d", ui32TxLen);
// #endif
// return eEtaSpiInvalidConfig;
// }
//
// if (ui32RxLen > (64*1024)-1)
// {
// #ifdef ETA_CSP_SPI_DEBUG
// etaPrintf ("ERROR: EtaCspSpiTransferPoll exceeded max ui32RxLen of
// (64*1024)-1 with %d", ui32RxLen);
// #endif
// return eEtaSpiInvalidConfig;
// }

    if(iSpiSequence == eSpiSequenceMiddle)
    {
         return eEtaSpiInvalidConfig;
    }

    // If it is first, start command
    if(iSpiSequence & eSpiSequenceFirstOnly)
    {
        // Make sure previous command is done (optional)
        if(localStatus = EtaCspSpiDonePoll())
        {
            return(localStatus);
        }

        // Start command
        REG_SPI2_STATUS = BM_SPI2_STATUS_START_BUSY |
                          (spiChipSel << BP_SPI2_STATUS_CS_SEL) |
                          BM_SPI2_STATUS_HOLD_CS;
    }

    //
    // If this is not the start of the command wait till
    // dummy bytes registers are 0. If this is the start
    // of the command these are 0 anyway.
    //
    if(ui16TxLen)
    {
        while(REG_SPI2_RXDROP_CNT)
        {
        }
    }
//    else if(ui16RxLen)
//    {
//        while(REG_SPI2_TX_DUMMY_BYTE_CNT)
//        {
//        }
//    }

    // Setup global array
    gsSpiCmd.pui8TxData = pui8TxData;
    gsSpiCmd.pui8RxData = pui8RxData;
    gsSpiCmd.ui16TxLen = ui16TxLen;
    gsSpiCmd.ui16RxLen = ui16RxLen;
    gsSpiCmd.ui8RxIntSize = 14;
    gsSpiCmd.ui8TxIntSize = 14;
    gsSpiCmd.bCmdActive = 1;
    gsSpiCmd.iSpiSequence = iSpiSequence;
    gsSpiCmd.spiStatus = eEtaFailure;

    if(iSpiSequence & eSpiSequenceLastOnly)
    {
        lastcmd = 1;
    }
    else
    {
        lastcmd = 0;
    }

    if(ui16TxLen) // If we have TX, lets process it.
    {
        // Deal with TX first
        // We'll drop ui16TxLen of data (garbage receive data during TX)
        REG_SPI2_RXDROP_CNT = ui16TxLen;

        REG_SPI2_CONFIG = (REG_SPI2_CONFIG & ~BM_SPI2_CONFIG_FIFO_LEVEL_TRIG) |
                          (gsSpiCmd.ui8TxIntSize <<
                           BP_SPI2_CONFIG_FIFO_LEVEL_TRIG);
    }
    else if(ui16RxLen) // No TX, just RX
    {
        REG_SPI2_TX_DUMMY_BYTE_CNT = ui16RxLen;

        // If it is Last, end command
        if(iSpiSequence & eSpiSequenceLastOnly)
        {
            // At this point, all TX is in the queue. Remove hold
            REG_SPI2_STATUS = 0;
        }
        else
        {
            REG_SPI2_INT_EN = REG_SPI2_INT_EN | BM_SPI2_INT_EN_XMIT_COMPLETE_EN0;
        }

        REG_SPI2_CONFIG = (REG_SPI2_CONFIG & ~BM_SPI2_CONFIG_FIFO_LEVEL_TRIG) |
                          (gsSpiCmd.ui8RxIntSize <<
                           BP_SPI2_CONFIG_FIFO_LEVEL_TRIG);
    }
    else // No TX, No RX
    {
        // If it is Last, end command
        if(iSpiSequence & eSpiSequenceLastOnly)
        {
            // At this point, all TX is in the queue. Remove hold
            REG_SPI2_STATUS = 0;
        }
    }


    // Setup next expected interrupt
    REG_SPI2_INT_EN =
        ((ui16TxLen != 0) ? BM_SPI2_INT_EN_TXFIFO_HAS_LEVEL_SPACE_EN0 : 0)               |
        ((ui16RxLen != 0) ? BM_SPI2_INT_EN_RX_FIFO_HIT_LEVEL_EN0 : 0)                    |
        //((iSpiSequence & eSpiSequenceLastOnly) ? 0 : BM_SPI2_INT_EN_XMIT_COMPLETE_EN0)   |
        ((iSpiSequence & eSpiSequenceLastOnly) ? BM_SPI2_INT_EN_CMD_IS_DONE_EN0 : 0);

// orig
// REGN_W4(iNum, SPI_INT_EN, TXFIFO_HAS_LEVEL_SPACE_EN0,       (ui16TxLen != 0),
// RX_FIFO_HIT_LEVEL_EN0,            (ui16RxLen != 0),
// XMIT_COMPLETE_EN0,                ((iSpiSequence & eSpiSequenceLastOnly) ? 0
// : 1) ,
// CMD_IS_DONE_EN0,                  ((iSpiSequence & eSpiSequenceLastOnly) ? 1
// : 0) );

    return(eEtaSuccess);
}

/***************************************************************************//**
 *
 *  EtaCspSpiProcessIntTxData - Process SPI Interrupt for TX Space Available
 *
 ******************************************************************************/
tEtaStatus
EtaCspSpiProcessIntTxData()
{
    uint8_t ui8SpaceInFifo;

    if((gsSpiCmd.bCmdActive == 0) ||
       (gsSpiCmd.ui16TxLen == 0))
    {
        return(eEtaSpiCmdSequenceErr);
    }

    if(ETA_CSP_SPI_EN_GREEDY_INT) // This allows for burst writes, which is more
                                  // efficient, but not requires an extra read.
    {
        ui8SpaceInFifo = 16 -
                         (REG_SPI2_TX_FIFO_DEBUG &
                          BM_SPI2_TX_FIFO_DEBUG_ENTRIES);
    }
    else
    {
        ui8SpaceInFifo = gsSpiCmd.ui8TxIntSize; // Do work that
                                                           // interrupt said we
                                                           // have space for
    }


    do
    {
        REG_SPI2_TXRX_1BYTE_FIFO_DATA = *gsSpiCmd.pui8TxData++;
        gsSpiCmd.ui16TxLen--;
        if(gsSpiCmd.ui16TxLen == 0)
        {
            break; // Exit loop, we're done.
        }
    }
    while(--ui8SpaceInFifo);

    if(gsSpiCmd.ui16TxLen == 0) // Just sent last byte. Lets turn off
                                           // TX has space interrupt and send
                                           // dummy bytes
    {
        REG_SPI2_TX_DUMMY_BYTE_CNT = gsSpiCmd.ui16RxLen;

        // If it is Last, end command
        if(gsSpiCmd.iSpiSequence & eSpiSequenceLastOnly)
        {
            // At this point, all TX is in the queue. Remove hold
            REG_SPI2_STATUS = 0;
        }
        else
        {
            REG_SPI2_INT_EN = REG_SPI2_INT_EN | BM_SPI2_INT_EN_XMIT_COMPLETE_EN0;
        }

        //
        // Disable TX fifo has space interrupt.
        // Reconfigure interrupt level to desired level for receiving.
        //
        REG_SPI2_INT_EN =
            (REG_SPI2_INT_EN & ~BM_SPI2_INT_EN_TXFIFO_HAS_LEVEL_SPACE_EN0);
        REG_SPI2_CONFIG = (REG_SPI2_CONFIG & ~BM_SPI2_CONFIG_FIFO_LEVEL_TRIG) |
                          (gsSpiCmd.ui8RxIntSize <<
                           BP_SPI2_CONFIG_FIFO_LEVEL_TRIG);

    }

    return(eEtaSuccess);
}

/***************************************************************************//**
 *
 *  EtaCspSpiProcessIntRxData - Process SPI Interrupt for RX Data Available
 *
 ******************************************************************************/
tEtaStatus
EtaCspSpiProcessIntRxData()
{
    uint8_t ui8SpaceInFifo;
    uint16_t ui16RxWord;

    if((gsSpiCmd.bCmdActive == 0) ||
       (gsSpiCmd.ui16RxLen == 0))
    {
        return(eEtaSpiCmdSequenceErr);
    }

    if(ETA_CSP_SPI_EN_GREEDY_INT) // This allows for burst writes, which is more
                                  // efficient, but not requires an extra read.
    {
        ui8SpaceInFifo =
            (REG_SPI2_RX_FIFO_DEBUG & BM_SPI2_RX_FIFO_DEBUG_ENTRIES);
    }
    else
    {
        ui8SpaceInFifo = gsSpiCmd.ui8RxIntSize; // Do work that
                                                           // interrupt said we
                                                           // have space for
    }


    do
    {
        if(ui8SpaceInFifo >= 2)
        {
            ui16RxWord = REG_SPI2_TXRX_2BYTE_FIFO_DATA;

            *gsSpiCmd.pui8RxData++ = ((ui16RxWord >> 0) & 0xff);
            *gsSpiCmd.pui8RxData++ = ((ui16RxWord >> 8) & 0xff);
            gsSpiCmd.ui16RxLen -= 2;
            ui8SpaceInFifo -= 2;
        }
        else if(ui8SpaceInFifo >= 1)
        {
            ui16RxWord = REG_SPI2_TXRX_1BYTE_FIFO_DATA;

            *gsSpiCmd.pui8RxData++ = ((ui16RxWord >> 0) & 0xff);
            gsSpiCmd.ui16RxLen--;
            ui8SpaceInFifo--;
        }

        if((ui8SpaceInFifo < 2) & (gsSpiCmd.ui16RxLen >= 2))
        {
            break; // If we drop low enough that we cannot bulk transfer, exit
                   // loop, wait for next int.
        }

        if(gsSpiCmd.ui16RxLen == 0)
        {
            break; // Exit loop, we're done.
        }
    }
    while(ui8SpaceInFifo);

    if(gsSpiCmd.ui16RxLen == 0) // Just sent last byte. Lets turn off
                                           // TX has space interrupt and send
                                           // dummy bytes
    {
        REG_SPI2_INT_EN =
            (REG_SPI2_INT_EN & ~BM_SPI2_INT_EN_RX_FIFO_HIT_LEVEL_EN0);
    }

    //
    // RX length less than interrupt size.
    //
    else if(gsSpiCmd.ui16RxLen < gsSpiCmd.ui8RxIntSize)
    {
        REG_SPI2_CONFIG = (REG_SPI2_CONFIG & ~BM_SPI2_CONFIG_FIFO_LEVEL_TRIG) |
                          (gsSpiCmd.ui16RxLen <<
                           BP_SPI2_CONFIG_FIFO_LEVEL_TRIG);
    }
    return(eEtaSuccess);
}

/***************************************************************************//**
 *
 *  EtaCspSpiProcessIntXmitDone - Process SPI Interrupt for All Transmit/Recieve
 *                                complete
 *
 ******************************************************************************/
tEtaStatus
EtaCspSpiProcessIntXmitDone()
{
    if((gsSpiCmd.bCmdActive == 0) ||
       (gsSpiCmd.ui16TxLen != 0) ||
       (gsSpiCmd.ui16RxLen != 0))
    {
        return(eEtaSpiCmdSequenceErr);
    }


    gsSpiCmd.bCmdActive = 0;
    gsSpiCmd.spiStatus = eEtaSuccess;
    REG_SPI2_INT_EN = (REG_SPI2_INT_EN & ~BM_SPI2_INT_EN_XMIT_COMPLETE_EN0);

    return(eEtaSuccess);
}

/***************************************************************************//**
 *
 *  EtaCspSpiProcessIntCmdDone - Process SPI Interrupt for Command Done
 *
 ******************************************************************************/
tEtaStatus
EtaCspSpiProcessIntCmdDone()
{
    if((gsSpiCmd.bCmdActive == 0) ||
       (gsSpiCmd.ui16TxLen != 0) ||
       (gsSpiCmd.ui16RxLen != 0))
    {
        return(eEtaSpiCmdSequenceErr);
    }


    gsSpiCmd.bCmdActive = 0;
    gsSpiCmd.spiStatus = eEtaSuccess;
    REG_SPI2_INT_EN = (REG_SPI2_INT_EN & ~BM_SPI2_INT_EN_CMD_IS_DONE_EN0);

    return(eEtaSuccess);
}

/***************************************************************************//**
 *
 *  EtaCspSpiProcessIntEntry - Entry for Interrupts for SPI
 *
 ******************************************************************************/
tEtaStatus
EtaCspSpiProcessIntEntry()
{
    uint16_t active_interrupt;
    tEtaStatus spiStatus = eEtaSuccess;

    // Do one read per interrupt. The INT_ACT field may change during the
    // interrupt
    active_interrupt = REG_SPI2_INT_ACT;

    if(active_interrupt & BM_SPI2_INT_ACT_TXFIFO_HAS_LEVEL_SPACE_ACT)
    {
        spiStatus |= EtaCspSpiProcessIntTxData();
    }
    if(active_interrupt & BM_SPI2_INT_ACT_RX_FIFO_HIT_LEVEL_ACT)
    {
        spiStatus |= EtaCspSpiProcessIntRxData();
    }
    if(active_interrupt & BM_SPI2_INT_ACT_XMIT_COMPLETE_ACT)
    {
        spiStatus |= EtaCspSpiProcessIntXmitDone();
    }
    if(active_interrupt & BM_SPI2_INT_ACT_CMD_IS_DONE_ACT)
    {
        spiStatus |= EtaCspSpiProcessIntCmdDone();
    }

    return(spiStatus);
}

/***************************************************************************//**
 *
 *  EtaCspSpiIsr- ISR handler for SPI interrupts.
 *
 *  @param iIrqNum is the interrupt number.
 *  @param int_line is the DSP interrupt line number.
 *
 ******************************************************************************/
void
EtaCspSpiIsr(tIrqNum iIrqNum, intline_T int_line)
{
    tEtaStatus spiStatus = eEtaSuccess;
    spiStatus = EtaCspSpiProcessIntEntry();
    if (spiStatus != eEtaSuccess)
    {
        etaPrintf ("ERROR: Something went wrong while handling SPI interrupt.\r\n");
        while(1);
    }
    EtaCspIsrIntClear(iIrqNum, int_line);
}

// Interrupt Processing
////////////////////////////////////////////////////////////////////////////////


/***************************************************************************//**
 *
 * eta_csp_spi_initialize (LEGACY) This is the default SPI initialization
 * routine. 1MHz, CPOL=CPHA=0, msbit is transmitted first. No slave ready. All
 * CS are active low. If RX_FULL fills up, TX stalls. Includes a gratuitous soft
 * reset to make sure the IP is in good shape
 *
 * @return TODO
 *
 ******************************************************************************/
int
eta_csp_spi_initialize()
{
    volatile int tempdata;

    // Setup TX/RX FIFO level to be 2
    REG_SPI2_CONFIG = (2 << BP_SPI2_CONFIG_FIFO_LEVEL_TRIG);

#ifdef ETA_CSP_PERIPH_MIN_CLK_FREQ

    // Set divide by and IBG the same
    REG_SPI2_CLKDIV = (1 << BP_SPI2_CLKDIV_CLKDIV) | (1 << BP_SPI2_CLKDIV_IBG);
#else

    // Set divide by and IBG the same
    REG_SPI2_CLKDIV = (7 << BP_SPI2_CLKDIV_CLKDIV) | (7 << BP_SPI2_CLKDIV_IBG);
#endif

    // Perform soft reset of SM and set stall TX if RX FULL
    REG_SPI2_CONFIG2 = BM_SPI2_CONFIG2_SOFT_RST;
    tempdata = REG_SPI2_CONFIG2;
    REG_SPI2_CONFIG2 = BM_SPI2_CONFIG2_STALL_IF_RXFULL;

#ifdef ETA_CSP_PERIPH_MIN_CLK_FREQ

    // Set divide by and IBG the same
    REG_SPI2_CONFIG2 &= ~(BM_SPI2_CONFIG2_RX_SAMPLE_TIME);
    REG_SPI2_CONFIG2 |= (1 << BP_SPI2_CONFIG2_RX_SAMPLE_TIME) |
                        (1 << BP_SPI2_CONFIG2_DISABLE_MISO_METAFLOP);
#endif

    return(0);
}

// Lower Level SPI Commands (legacy)
////////////////////////////////////////////////////////////////////////////////

