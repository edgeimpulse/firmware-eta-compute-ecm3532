/***************************************************************************//**
 *
 * @file eta_devices_spiflash.h
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

#ifndef __ETA_DEVICES_SPIFLASH_H__
#define __ETA_DEVICES_SPIFLASH_H__

#include <stdint.h>
#include "eta_csp_inc.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 *
 * EtaDevicesSpiFlashPoll4ProgDone - Polls SR1 command until Write In Progress
 * is done.
 *
 *  @param iNum the SPI number.
 *  @param tSpiChipSel SPI chip select
 *  @return Return the status.
 *
 ******************************************************************************/
extern tEtaStatus EtaDevicesSpiFlashPoll4ProgDone(tSpiNum iNum,
                                                  tSpiChipSel spiChipSel);

/***************************************************************************//**
 *
 * EtaDevicesSpiFlashReadID - Reads ID (RDID command)
 *
 *  @param iNum the SPI number.
 *  @param tSpiChipSel SPI chip select
 *  @param ui32Rdid Pointer to 32 bit uint for rdid. Only bits [23:0] will be
 * filled.
 *  @return Return the status.
 *
 ******************************************************************************/
extern tEtaStatus EtaDevicesSpiFlashReadID(tSpiNum iNum, tSpiChipSel spiChipSel,
                                           uint32_t *ui32Rdid);

/***************************************************************************//**
 *
 * EtaDevicesSpiFlashEraseSector - Erases the sector belonging to ui32Address
 *
 *  @param iNum the SPI number.
 *  @param tSpiChipSel SPI chip select
 *  @param uiAddress Byte address to program in SPI Flash. Only bits [23:0] are
 * used.
 *  @return Return the status.
 *
 ******************************************************************************/
extern tEtaStatus EtaDevicesSpiFlashEraseSector(tSpiNum iNum,
                                                tSpiChipSel spiChipSel,
                                                uint32_t uiAddress);

/***************************************************************************//**
 *
 * EtaDevicesSpiFlashProgPage - Programs a page
 *
 *  @param iNum the SPI number.
 *  @param tSpiChipSel SPI chip select
 *  @param uiAddress Byte address to program in SPI Flash. Only bits [23:0] are
 * used.
 *  @param pui8TxData pointer array of data to program
 *  @param ui32TxLen length of array
 *  @return Return the status.
 *
 *  Note, the first 4 bytes of pui8TxData should be 0x02 A1 A2 A3 where
 *{A1,A2,A3} is
 *  the byte address to write and ui32TxLen should include these 4 bytes in its
 * calculation
 *
 *  FIXME: This is not smart enough to handle addresses / lengths that cross
 * page boundaries.
 *
 ******************************************************************************/
extern tEtaStatus EtaDevicesSpiFlashProgPage(tSpiNum iNum,
                                             tSpiChipSel spiChipSel,
                                             uint32_t uiAddress,
                                             uint8_t *pui8TxData,
                                             uint32_t ui32TxLen);

/***************************************************************************//**
 *
 * EtaDevicesSpiFlashReadData - Programs a page
 *
 *  @param iNum the SPI number.
 *  @param tSpiChipSel SPI chip select
 *  @param uiAddress Byte address to program in SPI Flash. Only bits [23:0] are
 * used.
 *  @param pui8RxData pointer array of data received
 *  @param ui32RxLen length of array
 *  @return Return the status.
 *
 *  Note, the first 4 bytes of pui8TxData should be 0x03 A1 A2 A3 where
 *{A1,A2,A3} is
 *  the byte address to read and ui32RxLen should include these 4 bytes in its
 * calculation
 *
 ******************************************************************************/
extern tEtaStatus EtaDevicesSpiFlashReadData(tSpiNum iNum,
                                             tSpiChipSel spiChipSel,
                                             uint32_t uiAddress,
                                             uint8_t *pui8RxData,
                                             uint32_t ui32RxLen);

/***************************************************************************//**
 *
 * EtaDevicesSpiFlashIsRDIDSupported - Returns 1 if SPI Flash is one of listed
 * supported models, 0 if not.
 *
 *  @param ui32Rdid SPI RDID. Only [23:0] are used.
 *  @return Return 1 for known, 0 for unknown device.
 *
 ******************************************************************************/
extern uint8_t EtaDevicesSpiFlashIsRDIDSupported(uint32_t ui32Rdid);

/***************************************************************************//**
 *
 * EtaDevicesSpiFlashAdestoUnlockSectors - Unlock sectors on Adesto SPI Flash
 *
 *  @param iNum the SPI number.
 *  @param tSpiChipSel SPI chip select
 *  @return Return the status.
 *
 ******************************************************************************/
extern tEtaStatus EtaDevicesSpiFlashAdestoUnlockSectors(tSpiNum iNum,
                                                        tSpiChipSel spiChipSel);

#ifdef __cplusplus
}
#endif

#endif // __ETA_DEVICES_SPIFLASH_H__

