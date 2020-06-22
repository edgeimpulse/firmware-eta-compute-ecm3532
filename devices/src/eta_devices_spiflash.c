/***************************************************************************//**
 *
 * @file eta_devices_spiflash.c
 *
 * @brief Generic SPI Flash interface
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
 * @addtogroup etadevices-spiflash Serial Flash
 * @ingroup etadevices
 * @{
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include "eta_devices_spiflash.h"

// #define ETA_DEVICES_SPIFLASH_DEBUG
#ifdef ETA_DEVICES_SPIFLASH_DEBUG
#include "eta_diags.h"
#endif

//
// Is not compatable with the ecm3531 CSP.
//
#ifndef ECM3531

/***************************************************************************//**
 *
 * EtaDevicesSpiFlashPoll4ProgDone - Polls SR1 command until Write In Progress
 *is done.
 *
 *  @param iNum the SPI number.
 *  @param tSpiChipSel SPI chip select
 *  @return Return the status.
 *
 ******************************************************************************/
tEtaStatus
EtaDevicesSpiFlashPoll4ProgDone(tSpiNum iNum, tSpiChipSel spiChipSel)
{
    tEtaStatus result;
    uint8_t ui8RxByte;
#ifdef ETA_DEVICES_SPIFLASH_DEBUG
    uint8_t ui8PrevRxByte = 0x55; // unlikely...
#endif
    uint32_t ui32Timeout = 1000000;

    do
    {
        ui8RxByte = 0xD7;

        result = EtaCspSpiTransferPoll(iNum, &ui8RxByte, 1, &ui8RxByte, 1,
                                       spiChipSel, eSpiSequenceFirstLast);

#ifdef ETA_DEVICES_SPIFLASH_DEBUG
        if((ui8PrevRxByte != ui8RxByte) || (ui32Timeout == 1))
        {
            ETA_DIAGS_PRINTF("Current SPI SR1:%x\r\n", ui8RxByte);
            ui8PrevRxByte = ui8RxByte;
        }
#endif
    }
    while(((ui8RxByte & 0x80) == 0x0) && (result == eEtaSuccess) &&
          (--ui32Timeout));

    if(result)
    {
        return(result);
    }
    else if(ui32Timeout == 0)
    {
        return(eEtaFailure);
    }
    else
    {
        return(eEtaSuccess);
    }
}

/***************************************************************************//**
 *
 * EtaDevicesSpiFlashReadID - Reads ID (RDID command)
 *
 *  @param iNum the SPI number.
 *  @param tSpiChipSel SPI chip select
 *  @param ui32Rdid Pointer to 32 bit uint for rdid. Only bits [23:0] will be
 *filled.
 *  @return Return the status.
 *
 ******************************************************************************/
tEtaStatus
EtaDevicesSpiFlashReadID(tSpiNum iNum, tSpiChipSel spiChipSel,
                         uint32_t *ui32Rdid)
{
    tEtaStatus result;
    uint8_t ui8RxByte [3];

    ui8RxByte [0] = 0x9f;

    result = EtaCspSpiTransferPoll(iNum, &ui8RxByte[0], 1, &ui8RxByte[0], 3,
                                   spiChipSel, eSpiSequenceFirstLast);

    *ui32Rdid = (ui8RxByte[0] << 16) | (ui8RxByte[1] << 8) |
                (ui8RxByte[2] << 0);

    return(result);
}

/***************************************************************************//**
 *
 * EtaDevicesSpiFlashEraseSector - Erases the sector belonging to ui32Address
 *
 *  @param iNum the SPI number.
 *  @param tSpiChipSel SPI chip select
 *  @param uiAddress Byte address to program in SPI Flash. Only bits [23:0] are
 *used.
 *  @return Return the status.
 *
 ******************************************************************************/
tEtaStatus
EtaDevicesSpiFlashEraseSector(tSpiNum iNum, tSpiChipSel spiChipSel,
                              uint32_t uiAddress)
{
    tEtaStatus result;
    uint8_t ui8TxByte [4];

    ui8TxByte[0] = 0x7C; // Sector erase
    ui8TxByte[1] = (uiAddress >> 16) & 0xff;
    ui8TxByte[2] = (uiAddress >> 8)  & 0xff;
    ui8TxByte[3] = (uiAddress >> 0)  & 0xff;

    result = EtaCspSpiTransferPoll(iNum, &ui8TxByte[0], 4, &ui8TxByte[0], 0,
                                   spiChipSel, eSpiSequenceFirstLast);

#ifdef ETA_DEVICES_SPIFLASH_DEBUG
    ui8TxByte[0] = 0x5; // Read SR1

    result = EtaCspSpiTransferPoll(iNum, &ui8TxByte[0], 1, &ui8TxByte[0], 1,
                                   spiChipSel, eSpiSequenceFirstLast);

    if((ui8TxByte[0] & 0x80) == 0)
    {
        ETA_DIAGS_PRINTF("Write (Erase) In Progress Current SPI SR1:%x\r\n",
                         ui8TxByte[0]);
    }
    else
    {
        ETA_DIAGS_PRINTF(
                         "WARNING:  Erase did not start.  Current SPI SR1:%x\r\n",
                         ui8TxByte[0]);
    }
#endif

    return(result);
}

/***************************************************************************//**
 *
 * EtaDevicesSpiFlashProgPage - Programs a page
 *
 *  @param iNum the SPI number.
 *  @param tSpiChipSel SPI chip select
 *  @param uiAddress Byte address to program in SPI Flash. Only bits [23:0] are
 *used.
 *  @param pui8TxData pointer array of data to program
 *  @param ui32TxLen length of array
 *  @return Return the status.
 *
 *  FIXME: This is not smart enough to handle addresses / lengths that cross
 *page boundaries.
 *
 ******************************************************************************/
tEtaStatus
EtaDevicesSpiFlashProgPage(tSpiNum iNum, tSpiChipSel spiChipSel,
                           uint32_t uiAddress, uint8_t *pui8TxData,
                           uint32_t ui32TxLen)
{
    tEtaStatus result;

    uint8_t ui8TxByte [4];

    ui8TxByte[0] = 0x02; // Page program
    ui8TxByte[1] = (uiAddress >> 16) & 0xff;
    ui8TxByte[2] = (uiAddress >> 8)  & 0xff;
    ui8TxByte[3] = (uiAddress >> 0)  & 0xff;

    // Send Command
    if( ( result =
           EtaCspSpiTransferPoll(iNum, &ui8TxByte[0], 4, &ui8TxByte[0], 0,
                                 spiChipSel,
                                 eSpiSequenceFirstOnly) ) )
    {
        return(result);
    }

    if( ( result = EtaCspSpiXmitDonePoll(iNum)) )
    {
        return(result);
    }

    // Continue with Program
    result = EtaCspSpiTransferPoll(iNum, pui8TxData, ui32TxLen, pui8TxData, 0,
                                   spiChipSel, eSpiSequenceLastOnly);

    return(result);
}

/***************************************************************************//**
 *
 * EtaDevicesSpiFlashReadData - Programs a page
 *
 *  @param iNum the SPI number.
 *  @param tSpiChipSel SPI chip select
 *  @param uiAddress Byte address to program in SPI Flash. Only bits [23:0] are
 *used.
 *  @param pui8RxData pointer array of data received
 *  @param ui32RxLen length of array
 *  @return Return the status.
 *
 ******************************************************************************/
tEtaStatus
EtaDevicesSpiFlashReadData(tSpiNum iNum, tSpiChipSel spiChipSel,
                           uint32_t uiAddress, uint8_t *pui8RxData,
                           uint32_t ui32RxLen)
{
    tEtaStatus result;

    uint8_t ui8TxByte [4];

    ui8TxByte[0] = 0x03; // Read
    ui8TxByte[1] = (uiAddress >> 16) & 0xff;
    ui8TxByte[2] = (uiAddress >> 8)  & 0xff;
    ui8TxByte[3] = (uiAddress >> 0)  & 0xff;

    // Send Command
    if( ( result =
           EtaCspSpiTransferPoll(iNum, &ui8TxByte[0], 4, &ui8TxByte[0], 0,
                                 spiChipSel,
                                 eSpiSequenceFirstOnly) ) )
    {
        return(result);
    }

    if( ( result = EtaCspSpiXmitDonePoll(iNum) ) )
    {
        return(result);
    }

    result = EtaCspSpiTransferPoll(iNum, pui8RxData, 0, pui8RxData, ui32RxLen,
                                   spiChipSel, eSpiSequenceLastOnly);

    return(result);
}

/***************************************************************************//**
 *
 * EtaDevicesSpiFlashCheckRDID - Unlock sectors on Adesto SPI Flash
 *
 *  @param ui32Rdid The RDID returned from a device.
 *  @return Return the status.
 *
 ******************************************************************************/
uint8_t
EtaDevicesSpiFlashIsRDIDSupported(uint32_t ui32Rdid)
{
    switch(ui32Rdid & 0x00ffffff)
    {
        case 0x010215:
        {
#ifdef ETA_DEVICES_SPIFLASH_DEBUG
            ETA_DIAGS_PRINTF("Info: RDID is Simulation Model RDID:0x%x\r\n",
                             ui32Rdid);
#endif
            return(1);
            break;
        }

        case 0x1f4402:
        {
#ifdef ETA_DEVICES_SPIFLASH_DEBUG
            ETA_DIAGS_PRINTF("Info: RDID is Adesto RDID :0x%x\r\n", ui32Rdid);
#endif
            return(1);
            break;
        }

        case 0x1f2800:
        {
#ifdef ETA_DEVICES_SPIFLASH_DEBUG
            ETA_DIAGS_PRINTF(
                             "Info: RDID is Fiji Validation Board Adesto RDID :0x%x\r\n",
                             ui32Rdid);
#endif
            return(1);
            break;
        }

        default:
#ifdef ETA_DEVICES_SPIFLASH_DEBUG
            ETA_DIAGS_PRINTF("ERROR:  RDID (%x) not recognized\r\n", ui32Rdid);
#endif
            return(0);
    }
}

/***************************************************************************//**
 *
 * EtaDevicesSpiFlashAdestoUnlockSectors - Unlock sectors on Adesto SPI Flash
 *
 *  @param iNum the SPI number.
 *  @param tSpiChipSel SPI chip select
 *  @return Return the status.
 *
 ******************************************************************************/
tEtaStatus
EtaDevicesSpiFlashAdestoUnlockSectors(tSpiNum iNum, tSpiChipSel spiChipSel)
{
    tEtaStatus result;
    uint8_t ui8TxRxByte [4];

#ifdef ETA_DEVICES_SPIFLASH_DEBUG
    ETA_DIAGS_PRINTF("Info: SPI%d Checking Adesto sector status... ", !!iNum);
#endif

    // Read status
    ui8TxRxByte[0] = 0xD7;

    if( ( result =
           EtaCspSpiTransferPoll(iNum, &ui8TxRxByte[0], 1, &ui8TxRxByte[0], 1,
                                 spiChipSel, eSpiSequenceFirstLast)) )
    {
        return(result);
    }

    if(ui8TxRxByte[0] & 0x02) // If any sectors are locked, unlock them all
    {
        // Disable protection
        ui8TxRxByte[0] = 0x3D;
        ui8TxRxByte[1] = 0x2A;
        ui8TxRxByte[2] = 0x7F;
        ui8TxRxByte[3] = 0x9A;
        if((result =
               EtaCspSpiTransferPoll(iNum, &ui8TxRxByte[0], 4, &ui8TxRxByte[0],
                                     0,
                                     spiChipSel, eSpiSequenceFirstLast)) )
        {
            return(result);
        }

        ui8TxRxByte[0] = 0xD7;
        if((result =
               EtaCspSpiTransferPoll(iNum, &ui8TxRxByte[0], 1, &ui8TxRxByte[0],
                                     1,
                                     spiChipSel, eSpiSequenceFirstLast)) )
        {
            return(result);
        }

        if(ui8TxRxByte[0] & 0x02)
        {
#ifdef ETA_DEVICES_SPIFLASH_DEBUG
            ETA_DIAGS_PRINTF("ERROR: sector protection still enabled.\r\n");
#endif
            return(-1);
        }

#ifdef ETA_DEVICES_SPIFLASH_DEBUG
        ETA_DIAGS_PRINTF("sectors were locked, so we unlocked them all.\r\n");
#endif
    }
    else
    {
#ifdef ETA_DEVICES_SPIFLASH_DEBUG
        ETA_DIAGS_PRINTF("sectors already unlocked.\r\n");
#endif
    }

    return(result);
}
#endif

