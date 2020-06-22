/***************************************************************************//**
 *
 * @file eta_csp_spi.c
 *
 * @brief This file contains eta_csp_spi module implementations.
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
 * @addtogroup ecm3532spi-m3 Serial Peripheral Interface (SPI)
 * @ingroup ecm3532csp-m3
 * @{
 ******************************************************************************/

#include <stdint.h>
#include "eta_status.h"
#include "eta_chip.h"
#include "eta_csp_spi.h"
#include "eta_csp_socctrl.h"

// #define ETA_CSP_SPI_DEBUG

//#define ETA_CSP_SPI_DETAILED_DEBUG

#define ETA_CSP_SPI_EN_GREEDY_INT       1 // If 1, we'll do an additional read per interrupt
                                          // to determine how much TX FIFO avail or RX FIFO ready
                                          // and read that amount. If 0, we do one write/read
                                          // per interrupt. Greedy tends to be more efficient, but does
                                          // add one read which is not strictly required since we know
                                          // the interrupt watermarks.

/***************************************************************************//**
 *
 * EtaCspSpiDonePoll - Poll for done.
 *
 * @param iNum SPI instance number
 * @return Return the status.
 *
 ******************************************************************************/
tEtaStatus
EtaCspSpiDonePoll(tSpiNum iNum)
{
    // In the extremes, this should
    // be 10,000 bit times.
    uint32_t ui32Timeout = 10000000;

    while((--ui32Timeout) && (REG_SPI_STATUS(iNum).BF.START_BUSY == 1))
    {
    }

    if(ui32Timeout == 0)
    {
#ifdef ETA_CSP_SPI_DEBUG
        ETA_DIAGS_PRINTF("ERROR: Timed out waiting for SPI%d CMD (SR=%x)\r\n",
                          iNum, REG_SPI_STATUS(iNum).V);
#endif
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
 * @param iNum SPI instance number
 * @return Return the status.
 *
 ******************************************************************************/
tEtaStatus
EtaCspSpiTxReadyPoll(tSpiNum iNum)
{
    // In the extremes, this should
    // be 10,000 bit times.
    uint32_t ui32Timeout = 10000000;

    while((--ui32Timeout) && (REG_SPI_STATUS(iNum).BF.TXFIFO_HAS_1SPACE == 0))
    {
    }

    if(ui32Timeout == 0)
    {
#ifdef ETA_CSP_SPI_DEBUG
        ETA_DIAGS_PRINTF("ERROR: Time out waiting for SPI%d TX (STATUS=%x)" \
                          "\r\n", !!iNum, REG_SPI_STATUS(iNum).V);
#endif
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
 * @param iNum SPI instance number
 * @return Return the status.
 *
 ******************************************************************************/
tEtaStatus
EtaCspSpiRxDataPoll(tSpiNum iNum)
{
    // In the extremes, this should
    // be 10,000 bit times.
    uint32_t ui32Timeout = 10000000;

    while((--ui32Timeout) && (REG_SPI_STATUS(iNum).BF.RXFIFO_HAS_1DATA == 0))
    {
    }

    if(ui32Timeout == 0)
    {
#ifdef ETA_CSP_SPI_DEBUG
        ETA_DIAGS_PRINTF("ERROR: Time out waiting for SPI%d RX to be ready " \
                          "(STATUS=%x)\r\n", !!iNum, REG_SPI_STATUS(iNum).V);
#endif
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
 * @param iNum SPI instance number
 * @return Return the status.
 *
 ******************************************************************************/
tEtaStatus
EtaCspSpiXmitDonePoll(tSpiNum iNum)
{
    // In the extremes, this should
    // be 10,000 bit times.
    uint32_t ui32Timeout = 10000000;

    while((--ui32Timeout) && (REG_SPI_STATUS(iNum).BF.XMIT_COMPLETE == 0))
    {
    }

    if(ui32Timeout == 0)
    {
#ifdef ETA_CSP_SPI_DEBUG
        ETA_DIAGS_PRINTF("ERROR: Time out waiting for SPI%d XMIT to be done " \
                          "(STATUS=%x)\r\n", !!iNum, REG_SPI_STATUS(iNum).V);
#endif
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
 * @param iNum SPI instance number
 * @param pui8TxData Pointer to the data to transmit.
 * @param ui32TxLen Transmit length in bytes.
 * @param pui8RxData Point to the received data.
 * @param ui32RxLen Receive length in bytes.
 * @param spiChipSel Chip Select to use
 * @param iSpiSequence Sequence field allowing multiple functions in a single SPI command
 * @return Return the status.
 *
 ******************************************************************************/
tEtaStatus
EtaCspSpiTransferPoll(tSpiNum iNum, uint8_t *pui8TxData, uint32_t ui32TxLen,
                       uint8_t *pui8RxData, uint32_t ui32RxLen,
                       tSpiChipSel spiChipSel, tSpiSequence iSpiSequence)
{
  tEtaStatus localStatus;
  uint32_t ui32Timeout;
  uint8_t tx_byte;
  uint8_t rx_byte;
  // fixme remove uint32_t rx_word;


  if (ui32TxLen > (64*1024)-1)
  {
#ifdef ETA_CSP_SPI_DEBUG
    ETA_DIAGS_PRINTF ("ERROR: EtaCspSpiTransferPoll for iNum=%d exceeded max ui32TxLen of (64*1024)-1 with %d", iNum, ui32TxLen);
#endif
    return eEtaSpiInvalidConfig;
  }

  if (ui32RxLen > (64*1024)-1)
  {
#ifdef ETA_CSP_SPI_DEBUG
    ETA_DIAGS_PRINTF ("ERROR: EtaCspSpiTransferPoll for iNum=%d exceeded max ui32RxLen of (64*1024)-1 with %d", iNum, ui32RxLen);
#endif
    return eEtaSpiInvalidConfig;
  }

  if ((iNum == eSpi1) && ((spiChipSel == eSpiChipSel2) || (spiChipSel == eSpiChipSel3)))
  {
#ifdef ETA_CSP_SPI_DEBUG
    ETA_DIAGS_PRINTF ("ERROR: EtaCspSpiTransferPoll for iNum=%d invalid chipselect", iNum, spiChipSel);
#endif
    return eEtaSpiInvalidConfig;
  }

  // If it is first, start command
  if (iSpiSequence & eSpiSequenceFirstOnly)
  {
    // Make sure previous command is done (optional)
    if ( ( localStatus = EtaCspSpiDonePoll(iNum) ) )
      return localStatus;

    // Start command
    REG_SPI_STATUS(iNum).V = BM_SPI_STATUS_START_BUSY | (spiChipSel<<BP_SPI_STATUS_CS_SEL) | BM_SPI_STATUS_HOLD_CS;
  }

  // Deal with TX first
  // We'll drop ui32TxLen of data (garbage receive data during TX)
  REG_SPI_RXDROP_CNT(iNum).V = ui32TxLen;

  ui32Timeout=10000;
  while ((--ui32Timeout) && (ui32TxLen))
  {
    if ( ( localStatus = EtaCspSpiTxReadyPoll(iNum) ) )
      return localStatus;

    tx_byte = *pui8TxData++;

    REG_SPI_TXRX_1BYTE_FIFO_DATA(iNum).V = tx_byte;
    ui32TxLen--;
  }
  if (ui32Timeout == 0) return 1;


  // Now deal with RX.

  // Write number of dummy bytes to send to get rx data we want
  REG_SPI_TX_DUMMY_BYTE_CNT(iNum).V = ui32RxLen;

  // If it is Last, end command
  if (iSpiSequence & eSpiSequenceLastOnly)
  {
    // At this point, all TX is in the queue. Remove hold
    REG_SPI_STATUS(iNum).V = 0;
  }

  ui32Timeout=10000;
  // 1 Byte version
  while ((--ui32Timeout) && (ui32RxLen))
  {
    localStatus = EtaCspSpiRxDataPoll(iNum);
    if (localStatus)
      return localStatus;

    // pop data off the read stack
    rx_byte = REG_SPI_TXRX_1BYTE_FIFO_DATA(iNum).V;
    *pui8RxData++ = rx_byte;

    // If we have data to send, ignore receive data.
    ui32RxLen --;
  }

  // This ui32Timeout is for general programming reasons to prevent infinite loop
  if (ui32Timeout == 0)
    return(eEtaFailure);

  return (eEtaSuccess);

}

/***************************************************************************//**
 *
 * EtaCspSpiInit - Initialize the SPI module.
 *
 *  @param iNum the SPI number.
 *  @param EtaCspSpiConfig_t SPI Configurtation struct
 *  @return The result.
 *
 ******************************************************************************/
tEtaStatus
EtaCspSpiInit(tSpiNum iNum, EtaCspSpiConfig_t iSpiConfig)
{

  uint32_t spi_config;
  uint32_t spi_config2;
  uint32_t spi_divclk;
  uint32_t hfo_freq;

  // Configurations
  spi_config          = (iSpiConfig.ui8IntSize          << BP_SPI_CONFIG_FIFO_LEVEL_TRIG) |
                        (iSpiConfig.spiBitEndian        << BP_SPI_CONFIG_END)             |
                        (iSpiConfig.spiClkPhase         << BP_SPI_CONFIG_CPHA)            |
                        (iSpiConfig.spiClkPol           << BP_SPI_CONFIG_CPOL)            ;

  spi_config2         = (iSpiConfig.spiByteEndian       << BP_SPI_CONFIG2_RX_BYTE2_END)    |
                        (iSpiConfig.spiByteEndian       << BP_SPI_CONFIG2_TX_BYTE2_END)    |
                        (0x1                            << BP_SPI_CONFIG2_AGGR_CLK_GATING) |
                        (0x0                            << BP_SPI_CONFIG2_CLK_ON)          |
                        (0x1                            << BP_SPI_CONFIG2_STALL_IF_RXFULL) ;

  // Determine what our source clock is
  switch (EtaCspSocCtrlUstFreqGet())
  {
    case (eSocCtrlUSTSel8MHz) : hfo_freq = 8000; break;
    case (eSocCtrlUSTSel4MHz) : hfo_freq = 4000; break;
    case (eSocCtrlUSTSel2MHz) : hfo_freq = 2000; break;
    default :
         return (eEtaSpiInvalidConfig);
         break;
  }

  // Check if desired close is within range
  if ((iSpiConfig.spiClkFreq*2) > hfo_freq) // Cannot achieve freq (max)
    return (eEtaSpiInvalidConfig);

  if ((iSpiConfig.spiClkFreq << 4) < hfo_freq) // Cannot achieve freq (min)
    return (eEtaSpiInvalidConfig);

  spi_divclk = ((hfo_freq/iSpiConfig.spiClkFreq)-1) & BM_SPI_CLKDIV_CLKDIV;

  // Add some subtle tweaks for corner cases
  switch (spi_divclk)
  {
    case (0) : return (eEtaSpiInvalidConfig); break; // Illegal
    case (1) : spi_config2 |= (1 << BP_SPI_CONFIG2_RX_SAMPLE_TIME) | BM_SPI_CONFIG2_DISABLE_MISO_METAFLOP; break;
    case (2) : spi_config2 |= (2 << BP_SPI_CONFIG2_RX_SAMPLE_TIME); break;
    default  : break;
  }

  // Add in inter command gap
  if (iSpiConfig.ui8ICG == 0xff)
    spi_divclk |= (spi_divclk & BM_SPI_CLKDIV_CLKDIV)     << BP_SPI_CLKDIV_ICG;
  else
    spi_divclk |= (iSpiConfig.ui8ICG & BM_SPI_CLKDIV_ICG) << BP_SPI_CLKDIV_ICG;

  // Add in inter byte gap
  if (iSpiConfig.ui8IBG == 0xff)
    spi_divclk |= (spi_divclk & BM_SPI_CLKDIV_CLKDIV)     << BP_SPI_CLKDIV_IBG;
  else
    spi_divclk |= (iSpiConfig.ui8IBG & BM_SPI_CLKDIV_IBG) << BP_SPI_CLKDIV_IBG;

  REG_SPI_CONFIG(iNum).V  = spi_config;
  REG_SPI_CONFIG2(iNum).V = spi_config2;
  REG_SPI_CLKDIV(iNum).V  = spi_divclk;

  return (eEtaSuccess);
}


////////////////////////////////////////////////////////////////////////////////////
// Interrupts

// Global pointer to command structs for SPI0 and SPI1
volatile EtaCspSpiCmd_t * g_pEtaCspSpiCmd[2];


/***************************************************************************//**
 *
 *  EtaCspSpiTransferInt - Start the SPI using Interrupt
 *
 *  @param instance the SPI number.
 *  @param pui8TxData pointer to TX Data Array
 *  @param ui32TxLen TX Data Length
 *  @param pui8RxData pointer to RX Data Array
 *  @param ui32RxLen RX Data Length
 *  @param chipsel select chip select 0-3.
 *
 ******************************************************************************/
tEtaStatus EtaCspSpiTransferInt(tSpiNum iNum, uint8_t * pui8TxData, uint32_t ui32TxLen,
                        uint8_t * pui8RxData, uint32_t ui32RxLen,
                        tSpiChipSel spiChipSel, tSpiSequence iSpiSequence)
{

  tEtaStatus localStatus;
  // fixme remove uint32_t ui32Timeout;
  // fixme remove uint8_t  lastcmd;

  if (ui32TxLen > (64*1024)-1)
  {
#ifdef ETA_CSP_SPI_DEBUG
    ETA_DIAGS_PRINTF ("ERROR: EtaCspSpiTransferPoll for iNum=%d exceeded max ui32TxLen of (64*1024)-1 with %d", iNum, ui32TxLen);
#endif
    return eEtaSpiInvalidConfig;
  }

  if (ui32RxLen > (64*1024)-1)
  {
#ifdef ETA_CSP_SPI_DEBUG
    ETA_DIAGS_PRINTF ("ERROR: EtaCspSpiTransferPoll for iNum=%d exceeded max ui32RxLen of (64*1024)-1 with %d", iNum, ui32RxLen);
#endif
    return eEtaSpiInvalidConfig;
  }

  if ((iNum == eSpi1) && ((spiChipSel == eSpiChipSel2) || (spiChipSel == eSpiChipSel3)))
  {
#ifdef ETA_CSP_SPI_DEBUG
    ETA_DIAGS_PRINTF ("ERROR: EtaCspSpiTransferPoll for iNum=%d invalid chipselect", iNum, spiChipSel);
#endif
    return eEtaSpiInvalidConfig;
  }

  // If it is first, start command
  if (iSpiSequence & eSpiSequenceFirstOnly)
  {
    // Make sure previous command is done (optional)
    if ( (localStatus = EtaCspSpiDonePoll(iNum) ) )
      return localStatus;

    // Start command
    REG_SPI_STATUS(iNum).V = BM_SPI_STATUS_START_BUSY | (spiChipSel<<BP_SPI_STATUS_CS_SEL) | BM_SPI_STATUS_HOLD_CS;
  }

  // Setup global array
  g_pEtaCspSpiCmd[iNum]->pui8TxData = pui8TxData;
  g_pEtaCspSpiCmd[iNum]->pui8RxData = pui8RxData;
  g_pEtaCspSpiCmd[iNum]->ui32TxLen  = ui32TxLen;
  g_pEtaCspSpiCmd[iNum]->ui32RxLen  = ui32RxLen;
  g_pEtaCspSpiCmd[iNum]->ui8RxIntSize = 14;
  g_pEtaCspSpiCmd[iNum]->ui8TxIntSize = 14;
  g_pEtaCspSpiCmd[iNum]->bCmdActive = 1;
  g_pEtaCspSpiCmd[iNum]->iSpiSequence = iSpiSequence;
  g_pEtaCspSpiCmd[iNum]->spiStatus = eEtaFailure;

  // fixme remove if (iSpiSequence & eSpiSequenceLastOnly)
    // fixme remove lastcmd = 1;
  // fixme remove else
    // fixme remove lastcmd = 0;

  if (ui32TxLen) // If we have TX, lets process it.
  {
    // Deal with TX first
    // We'll drop ui32TxLen of data (garbage receive data during TX)
    REG_SPI_RXDROP_CNT(iNum).V = ui32TxLen;

    REG_SPI_CONFIG(iNum).BF.FIFO_LEVEL_TRIG = g_pEtaCspSpiCmd[iNum]->ui8TxIntSize;

  }
  else if (ui32RxLen) // No TX, just RX
  {

    REG_SPI_TX_DUMMY_BYTE_CNT(iNum).V = ui32RxLen;

    // If it is Last, end command
    if (iSpiSequence & eSpiSequenceLastOnly)
    {
      // At this point, all TX is in the queue. Remove hold
      REG_SPI_STATUS(iNum).V = 0;
    }

    REG_SPI_CONFIG(iNum).BF.FIFO_LEVEL_TRIG = g_pEtaCspSpiCmd[iNum]->ui8RxIntSize;

  }
  else // No TX, No RX
  {

    // If it is Last, end command
    if (iSpiSequence & eSpiSequenceLastOnly)
    {
      // At this point, all TX is in the queue. Remove hold
      REG_SPI_STATUS(iNum).V = 0;
    }

  }

  // Setup next expected interrupt
  REGN_W4(iNum, SPI_INT_EN, TXFIFO_HAS_LEVEL_SPACE_EN0,       (ui32TxLen != 0),
                            RX_FIFO_HIT_LEVEL_EN0,            (ui32RxLen != 0),
                            XMIT_COMPLETE_EN0,                ((iSpiSequence & eSpiSequenceLastOnly) ? 0 : 1) ,
                            CMD_IS_DONE_EN0,                  ((iSpiSequence & eSpiSequenceLastOnly) ? 1 : 0) );


  return eEtaSuccess;
}

/***************************************************************************//**
 *
 *  EtaCspSpiProcessIntTxData - Process SPI Interrupt for TX Space Available
 *
 *  @param iNum the SPI number.
 *
 ******************************************************************************/
tEtaStatus EtaCspSpiProcessIntTxData (tSpiNum iNum)
{
  uint8_t ui8SpaceInFifo;

  if ((g_pEtaCspSpiCmd[iNum]->bCmdActive == 0) || (g_pEtaCspSpiCmd[iNum]->ui32TxLen == 0))
  {
#ifdef ETA_CSP_SPI_DEBUG
    ETA_DIAGS_PRINTF("ERROR: received spi_int_tx but command is done. bCmdActive=%x  ui32TxLen=%x\r\n", g_pEtaCspSpiCmd[iNum]->bCmdActive, g_pEtaCspSpiCmd[iNum]->ui32TxLen);
#endif
    return eEtaSpiCmdSequenceErr;
  }

  if (ETA_CSP_SPI_EN_GREEDY_INT) // This allows for burst writes, which is more efficient, but not requires an extra read.
  {
    ui8SpaceInFifo = 16-(MEMIO8(REG_SPI_TX_FIFO_DEBUG_ADDR (iNum)) & BM_SPI_TX_FIFO_DEBUG_ENTRIES);
  }
  else
  {
    ui8SpaceInFifo = g_pEtaCspSpiCmd[iNum]->ui8TxIntSize; // Do work that interrupt said we have space for
  }

#ifdef ETA_CSP_SPI_DETAILED_DEBUG
  ETA_DIAGS_PRINTF("DETAILED_SPI:SPI TX interrupt: Space for %d bytes starting with %x\r\n", ui8SpaceInFifo, *g_pEtaCspSpiCmd[iNum]->pui8TxData);
#endif

  do
  {
    MEMIO8(REG_SPI_TXRX_1BYTE_FIFO_DATA_ADDR (iNum)) = *g_pEtaCspSpiCmd[iNum]->pui8TxData++;
    g_pEtaCspSpiCmd[iNum]->ui32TxLen--;
    if (g_pEtaCspSpiCmd[iNum]->ui32TxLen == 0)
    {
      break; // Exit loop, we're done.
    }
  }
  while (--ui8SpaceInFifo);


  if (g_pEtaCspSpiCmd[iNum]->ui32TxLen == 0) // Just sent last byte. Lets turn off TX has space interrupt and send dummy bytes
  {
    REG_SPI_TX_DUMMY_BYTE_CNT(iNum).V = g_pEtaCspSpiCmd[iNum]->ui32RxLen;

    // If it is Last, end command
    if (g_pEtaCspSpiCmd[iNum]->iSpiSequence & eSpiSequenceLastOnly)
    {
      // At this point, all TX is in the queue. Remove hold
      REG_SPI_STATUS(iNum).V = 0;
    }

    REGN_M1(iNum, SPI_INT_EN, TXFIFO_HAS_LEVEL_SPACE_EN0, 0);

    REG_SPI_CONFIG(iNum).BF.FIFO_LEVEL_TRIG = g_pEtaCspSpiCmd[iNum]->ui8RxIntSize;

#ifdef ETA_CSP_SPI_DETAILED_DEBUG
    ETA_DIAGS_PRINTF("DETAILED_SPI:Last SPI TX data interrupt\r\n");
#endif
  }

  return eEtaSuccess;
}

/***************************************************************************//**
 *
 *  EtaCspSpiProcessIntRxData - Process SPI Interrupt for RX Data Available
 *
 *  @param iNum the SPI number.
 *
 ******************************************************************************/
tEtaStatus EtaCspSpiProcessIntRxData (tSpiNum iNum)
{
  uint8_t ui8SpaceInFifo;
  uint32_t ui32RxWord;

  if ((g_pEtaCspSpiCmd[iNum]->bCmdActive == 0) || (g_pEtaCspSpiCmd[iNum]->ui32RxLen == 0))
  {
#ifdef ETA_CSP_SPI_DEBUG
    ETA_DIAGS_PRINTF("ERROR: received spi_int_rx but command is done. bCmdActive=%x  ui32RxLen=%x\r\n", g_pEtaCspSpiCmd[iNum]->bCmdActive, g_pEtaCspSpiCmd[iNum]->ui32RxLen);
#endif
    return eEtaSpiCmdSequenceErr;
  }

  if (ETA_CSP_SPI_EN_GREEDY_INT) // This allows for burst writes, which is more efficient, but not requires an extra read.
  {
    ui8SpaceInFifo = (MEMIO8(REG_SPI_RX_FIFO_DEBUG_ADDR (iNum)) & BM_SPI_RX_FIFO_DEBUG_ENTRIES);
  }
  else
  {
    ui8SpaceInFifo = g_pEtaCspSpiCmd[iNum]->ui8RxIntSize; // Do work that interrupt said we have space for
  }

#ifdef ETA_CSP_SPI_DETAILED_DEBUG
  ETA_DIAGS_PRINTF("DETAILED_SPI:SPI RX data interrupt. entries:%d\r\n", ui8SpaceInFifo);
#endif

  do
  {
    if (ui8SpaceInFifo >= 4)
    {
      ui32RxWord = REG_SPI_RX_4BYTE_FIFO_DATA(iNum).V;
#ifdef ETA_CSP_SPI_DETAILED_DEBUG
      ETA_DIAGS_PRINTF("DETAILED_SPI:SPI RX 4data interrupt. rx:%x rxlen:%d space:%d\r\n", ui32RxWord, g_pEtaCspSpiCmd[iNum]->ui32RxLen, ui8SpaceInFifo);
#endif

      *g_pEtaCspSpiCmd[iNum]->pui8RxData++ = ((ui32RxWord >> 0) & 0xff);
      *g_pEtaCspSpiCmd[iNum]->pui8RxData++ = ((ui32RxWord >> 8) & 0xff);
      *g_pEtaCspSpiCmd[iNum]->pui8RxData++ = ((ui32RxWord >> 16) & 0xff);
      *g_pEtaCspSpiCmd[iNum]->pui8RxData++ = ((ui32RxWord >> 24) & 0xff);
      g_pEtaCspSpiCmd[iNum]->ui32RxLen -= 4;
      ui8SpaceInFifo -= 4;
    }
    else if (ui8SpaceInFifo >= 1)
    {
      ui32RxWord = MEMIO8(REG_SPI_TXRX_1BYTE_FIFO_DATA_ADDR (iNum));
#ifdef ETA_CSP_SPI_DETAILED_DEBUG
      ETA_DIAGS_PRINTF("DETAILED_SPI:SPI RX 1data interrupt. rx:%x rxlen:%d space:%d\r\n", ui32RxWord, g_pEtaCspSpiCmd[iNum]->ui32RxLen, ui8SpaceInFifo);
#endif

      *g_pEtaCspSpiCmd[iNum]->pui8RxData++ = ((ui32RxWord >> 0) & 0xff);
      g_pEtaCspSpiCmd[iNum]->ui32RxLen --;
      ui8SpaceInFifo --;
    }

    if ((ui8SpaceInFifo < 4) & (g_pEtaCspSpiCmd[iNum]->ui32RxLen >= 4))
    {
      break; // If we drop low enough that we cannot bulk transfer, exit loop, wait for next int.
    }

    if (g_pEtaCspSpiCmd[iNum]->ui32RxLen == 0)
    {
      break; // Exit loop, we're done.
    }
  }
  while (ui8SpaceInFifo);

  if (g_pEtaCspSpiCmd[iNum]->ui32RxLen == 0) // Just sent last byte. Lets turn off TX has space interrupt and send dummy bytes
  {
    REGN_M1(0, SPI_INT_EN, RX_FIFO_HIT_LEVEL_EN0, 0);
#ifdef ETA_CSP_SPI_DETAILED_DEBUG
    ETA_DIAGS_PRINTF("DETAILED_SPI:Last SPI RX data interrupt\r\n");
#endif
  }
  else if (g_pEtaCspSpiCmd[iNum]->ui32RxLen < g_pEtaCspSpiCmd[iNum]->ui8RxIntSize) // RX Length less than interrupt size.
  {
    REG_SPI_CONFIG(iNum).BF.FIFO_LEVEL_TRIG = g_pEtaCspSpiCmd[iNum]->ui32RxLen;
#ifdef ETA_CSP_SPI_DETAILED_DEBUG
    ETA_DIAGS_PRINTF("DETAILED_SPI:SPI RX dropped below level, set to %d\r\n", g_pEtaCspSpiCmd[iNum]->ui32RxLen);
#endif
  }
  return eEtaSuccess;
}

/***************************************************************************//**
 *
 *  EtaCspSpiProcessIntXmitDone - Process SPI Interrupt for All Transmit/Recieve complete
 *
 *  @param iNum the SPI number.
 *
 ******************************************************************************/
tEtaStatus EtaCspSpiProcessIntXmitDone (tSpiNum iNum)
{
  if ((g_pEtaCspSpiCmd[iNum]->bCmdActive == 0) || (g_pEtaCspSpiCmd[iNum]->ui32TxLen != 0) || (g_pEtaCspSpiCmd[iNum]->ui32RxLen != 0))
  {
#ifdef ETA_CSP_SPI_DEBUG
    ETA_DIAGS_PRINTF("ERROR: received spi_xmit_done but command is already done or has work left to do. bCmdActive=%x  ui32TxLen=%x  ui32RxLen=%x\r\n", g_pEtaCspSpiCmd[iNum]->bCmdActive, g_pEtaCspSpiCmd[iNum]->ui32TxLen, g_pEtaCspSpiCmd[iNum]->ui32RxLen);
#endif
    return eEtaSpiCmdSequenceErr;
  }

#ifdef ETA_CSP_SPI_DETAILED_DEBUG
  ETA_DIAGS_PRINTF("DETAILED_SPI:SPI command interrupt xmit complete\r\n");
#endif

  g_pEtaCspSpiCmd[iNum]->bCmdActive = 0;
  g_pEtaCspSpiCmd[iNum]->spiStatus = eEtaSuccess;
  REGN_M1(iNum, SPI_INT_EN, XMIT_COMPLETE_EN0, 0);

  return eEtaSuccess;
}

/***************************************************************************//**
 *
 *  EtaCspSpiProcessIntCmdDone - Process SPI Interrupt for Command Done
 *
 *  @param iNum the SPI number.
 *
 ******************************************************************************/
tEtaStatus EtaCspSpiProcessIntCmdDone (tSpiNum iNum)
{
  if ((g_pEtaCspSpiCmd[iNum]->bCmdActive == 0) || (g_pEtaCspSpiCmd[iNum]->ui32TxLen != 0) || (g_pEtaCspSpiCmd[iNum]->ui32RxLen != 0))
  {
#ifdef ETA_CSP_SPI_DEBUG
    ETA_DIAGS_PRINTF("ERROR: received spi_int_done but command is already done or has work left to do. bCmdActive=%x  ui32TxLen=%x  ui32RxLen=%x\r\n", g_pEtaCspSpiCmd[iNum]->bCmdActive, g_pEtaCspSpiCmd[iNum]->ui32TxLen, g_pEtaCspSpiCmd[iNum]->ui32RxLen);
#endif
    return eEtaSpiCmdSequenceErr;
  }

#ifdef ETA_CSP_SPI_DETAILED_DEBUG
  ETA_DIAGS_PRINTF("DETAILED_SPI:SPI command interrupt done\r\n");
#endif

  g_pEtaCspSpiCmd[iNum]->bCmdActive = 0;
  g_pEtaCspSpiCmd[iNum]->spiStatus = eEtaSuccess;
  REGN_M1(iNum, SPI_INT_EN, CMD_IS_DONE_EN0, 0);

  return eEtaSuccess;
}

/***************************************************************************//**
 *
 *  EtaCspSpiProcessIntEntry - Entry for Interrupts for Spi
 *
 *  @param iNum the Spi number.
 *
 ******************************************************************************/
tEtaStatus EtaCspSpiProcessIntEntry (tSpiNum iNum)
{
  uint32_t active_interrupt;
  tEtaStatus spiStatus = eEtaSuccess;

  // Do one read per interrupt. The INT_ACT field may change during the interrupt
  active_interrupt = MEMIO16(REG_SPI_INT_ACT_ADDR (iNum));

  if (active_interrupt & BM_SPI_INT_ACT_TXFIFO_HAS_LEVEL_SPACE_ACT ) spiStatus |= EtaCspSpiProcessIntTxData  (iNum) ;
  if (active_interrupt & BM_SPI_INT_ACT_RX_FIFO_HIT_LEVEL_ACT      ) spiStatus |= EtaCspSpiProcessIntRxData  (iNum) ;
  if (active_interrupt & BM_SPI_INT_ACT_XMIT_COMPLETE_ACT          ) spiStatus |= EtaCspSpiProcessIntXmitDone (iNum) ;
  if (active_interrupt & BM_SPI_INT_ACT_CMD_IS_DONE_ACT            ) spiStatus |= EtaCspSpiProcessIntCmdDone (iNum) ;

  // FENCE TO MAKE SURE ALL WRITES GET FLUSHED
  REG_GPIO_DEBUG1_READ_ONLY.V = REG_SPI_CONFIG(iNum).V;

//  // Do one read per interrupt. The INT_ACT field may change during the interrupt
//  if (USE_MEMIO_CALLS)
//    active_interrupt = MEMIO16(REG_SPI_INT_ACT_ADDR (instance));
//  else
//    active_interrupt = REG_SPI_INT_ACT(instance).V;
//
//  if (active_interrupt & BM_SPI_INT_ACT_TXFIFO_HAS_LEVEL_SPACE_ACT )  EtaCspSpiProcessIntTxData(instance);
//  if (active_interrupt & BM_SPI_INT_ACT_RX_FIFO_HIT_LEVEL_ACT )       EtaCspSpiProcessIntRxData(instance);
//  if (active_interrupt & BM_SPI_INT_ACT_CMD_IS_DONE_ACT )             EtaCspSpiProcessIntCmdDone(instance);
//
//  // Fence to make sure all writes get flushed
//  REG_GPIO_DEBUG1_READ_ONLY.V = REG_SPI_CONFIG(instance).V;
  return spiStatus;
}

// Interrupt Processing
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 *
 * EtaCspSpiFence - Set up a fence for writes to the SPI0 & SPI1 regs.
 *
 * Add a fence on the last bus operation to SPI registers.
 *
 *
 ******************************************************************************/
void
EtaCspSpiFence(void)
{
    EtaCspSpiFenceFast();
}

