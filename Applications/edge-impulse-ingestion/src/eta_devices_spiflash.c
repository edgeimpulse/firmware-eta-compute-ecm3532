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
 * This is part of revision ${version} of the Tensai Software Development Kit.
 *
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "eta_csp_spi.h"
#include "spi_hal.h"
#include "gpio_hal.h"
#include "print_util.h"


//#include "eta_devices_spiflash.h"

//
// Is not compatable with the ecm3531 CSP.
//
#ifndef CONFIG_ECM3531
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
    uint8_t ui8RxByte[2];
    //uint8_t ui8PrevRxByte = 0x55; // unlikely...
    uint32_t ui32Timeout = 1000000;

    do
    {
        ui8RxByte[0] = 0xD7;

        result = HalSpiXfer(iNum, &ui8RxByte[0], 1,
                                &ui8RxByte[0], 2, NULL, NULL);

#ifdef ETA_DEVICES_SPIFLASH_DEBUG
        if((ui8PrevRxByte != ui8RxByte) || (ui32Timeout == 1))
        {
            ETA_DIAGS_PRINTF("Current SPI SR1:%x\r\n", ui8RxByte[0]);
            ui8PrevRxByte = ui8RxByte[0];
        }
#endif
        if(!(ui8RxByte[0] & 0x80))
           vTaskDelay(1);
        else
            break;
    }
    while(--ui32Timeout);

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
    result = HalSpiXfer(iNum, &ui8RxByte[0], 1,
                        &ui8RxByte[0], 3, NULL, NULL);

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

    result = HalSpiXfer(iNum, &ui8TxByte[0], 4,
                        &ui8TxByte[0], 0, NULL, NULL);

#ifdef ETA_DEVICES_SPIFLASH_DEBUG
    ui8TxByte[0] = 0x5; // Read SR1

    result = HalSpiXfer(iNum, &ui8TxByte[0], 1,
                        &ui8TxByte[0], 1, NULL, NULL);

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
    if ((result = HalSpiXfer(iNum, &ui8TxByte[0], 4,
                                &ui8TxByte[0], 0, NULL, NULL)))
    {
        return(result);
    }

        // Continue with Program
    result = HalSpiXfer(iNum, pui8TxData,
                        ui32TxLen, pui8TxData, 0, NULL, NULL);

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

#if 0
    // Send Command
    if((result =
           HalSpiXfer(iNum, spiChipSel, &ui8TxByte[0], 4,
                        NULL, 0, NULL, NULL)))
    {
        return(result);
    }

    if((result = EtaCspSpiXmitDonePoll(iNum)))
    {
        return(result);
    }


    result = HalSpiXfer(iNum, spiChipSel, NULL,
                            0, pui8RxData, ui32RxLen, NULL, NULL);

#endif
       // Send Command
    if((result =
           EtaCspSpiTransferPoll(iNum, &ui8TxByte[0], 4, &ui8TxByte[0], 0,
                                 spiChipSel,
                                 eSpiSequenceFirstOnly)))
    {
        return(result);
    }

    if((result = EtaCspSpiXmitDonePoll(iNum)))
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
uint32_t SpiFlashReadId(tSpiNum iNum, uint32_t CSGpio)
{
    uint32_t ui32Id;
    uint8_t  ui8Byte[4];

    ui8Byte[0] = 0x9f;
    HalGpioWrite(CSGpio, 0);
    HalSpiXfer(iNum, &ui8Byte[0], 1, &ui8Byte[1], 3, NULL, NULL);
    HalGpioWrite(CSGpio, 1);
    ui32Id = (ui8Byte[1] << 16) | (ui8Byte[2] << 8) |
        (ui8Byte[3] << 0);
    return ui32Id;
}

void waitForReady(tSpiNum iNum, uint32_t CSGpio)
{
    uint8_t txBuffer = 0xD7;
    uint8_t status;

    HalGpioWrite(CSGpio, 0);
	do
	{
		HalSpiXfer(iNum, &txBuffer, 1, &status, 1, NULL, NULL);
		if (status & 0x80)
			break;
		else
			vTaskDelay(1);
	} while(1);

    HalGpioWrite(CSGpio, 1);
}


/*
 *  ======== SpiFlashWriteBuffer ========
 */
int SpiFlashWriteBuffer(tSpiNum iNum, uint32_t CSGpio, uint8_t *data, uint16_t data_size)
{
    uint8_t txBuffer[4];
    //uint8_t *txBuffer = data;
	uint8_t boff = 0;

    /* Using SRAM Buffer 1 */
    txBuffer[0] = 0x84;
    txBuffer[1] = 0x00;
    txBuffer[2] = (unsigned char)(0x1 & (boff >> 8));
    txBuffer[3] = (unsigned char)(0xFF & boff);

    HalSpiXfer(iNum, &txBuffer[0], 4, NULL, 0, NULL, NULL);

	HalSpiXfer(iNum, data, data_size, NULL, 0, NULL, NULL);

    return 0;
}

/*
 *  ======== SpiFlashWriteBufferToPage ========
 */
bool SpiFlashWriteBufferToPage(tSpiNum iNum, tSpiChipSel spiChipSel, uint32_t page)
{
    unsigned char txBuffer[4];

    /* Using SRAM Buffer 2 w/ built in erase of page before write */
    txBuffer[0] = 0x83;
    txBuffer[1] = ((unsigned char)(page >> 7));
    txBuffer[2] = (unsigned char)(page << 1);
    txBuffer[3] = 0x00;

    HalSpiXfer(iNum, &txBuffer[0], 4, NULL, 0, NULL, NULL);
	return 0;
}
/*
 *  ======== SpiFlashWrite ========
 */
int SpiFlashWrite(tSpiNum iNum, tSpiChipSel spiChipSel,uint32_t page, uint8_t *buf, uint16_t len)
{

    //uint8_t boff = 0;
    //unsigned char txBuffer[4];
    HalGpioWrite(23, 0);
    waitForReady(iNum, spiChipSel);
    HalGpioWrite(23, 1);
    HalGpioWrite(23, 0);
    SpiFlashWriteBuffer(iNum, spiChipSel, buf, len);
    HalGpioWrite(23, 1);

    HalGpioWrite(23, 0);
    waitForReady(iNum, spiChipSel);
    HalGpioWrite(23, 1);
    HalGpioWrite(23, 0);
    /* Push the AT45DB Buffer to the flash page */
    SpiFlashWriteBufferToPage(iNum, spiChipSel, page);
    HalGpioWrite(23, 1);
#if 0
    waitForReady(iNum, spiChipSel);
    /* Write to the AT45DB Buffer */

    txBuffer[0] = 0x82;
    txBuffer[1] = (unsigned char)(page >> 7);
    txBuffer[2] = (unsigned char)((page << 1) | (0x1 & (boff >> 8)));
    txBuffer[3] = (unsigned char)(0xFF & (boff));

    HalSpiXfer(iNum, spiChipSel, &txBuffer[0], 4, NULL, 0, NULL, NULL);
#endif

    return (0);
}

int SpiFlashBufRead(tSpiNum iNum, tSpiChipSel spiChipSel, uint8_t *buf, uint16_t len)
{
    unsigned char txBuffer[4];
    uint8_t boff = 0;


	waitForReady(iNum, spiChipSel);


    /* Using SRAM Buffer 1 */
    txBuffer[0] = 0xD4;
    txBuffer[1] = 0x00;
    txBuffer[2] = (unsigned char)(0x1 & (boff >> 8));
    txBuffer[3] = (unsigned char)(0xFF & boff);

    HalSpiXfer(iNum, &txBuffer[0], 4, NULL, 0, NULL, NULL);
    HalSpiXfer(iNum, NULL, 0, buf, len, NULL, NULL);

     return 0;
}

/*
 *  ======== AT45DB_read ========
 */
int SpiFlashRead(tSpiNum iNum, tSpiChipSel spiChipSel, uint32_t page, uint8_t *buf, uint16_t len)
{
    uint8_t txBuffer[8]= {0}; /* last 4 bytes are needed, but have don't care values */
#if 1
    uint8_t boff = 0;
    HalGpioWrite(23, 0);
    waitForReady(iNum, spiChipSel);
    HalGpioWrite(23, 1);

    txBuffer[0] = 0xD2;
    txBuffer[1] = (unsigned char)(page >> 7);
    txBuffer[2] = (unsigned char)((page << 1) | (0x1 & (boff >> 8)));
    txBuffer[3] = (unsigned char)(0xFF & (boff));

    HalGpioWrite(23, 0);
    HalSpiXfer(iNum, &txBuffer[0], 8, buf, len, NULL, NULL);
    HalGpioWrite(23, 1);
	//waitForReady(iNum, spiChipSel);
    //HalSpiXfer(iNum, spiChipSel, NULL, 0, buf, len, NULL, NULL);
#else
    waitForReady(iNum, spiChipSel);

    /* Using SRAM Buffer 2 w/ built in erase of page before write */
    txBuffer[0] = 0x53;
    txBuffer[1] = ((unsigned char)(page >> 7));
    txBuffer[2] = (unsigned char)(page << 1);
    txBuffer[3] = 0x00;

    HalSpiXfer(iNum, spiChipSel, &txBuffer[0], 4, NULL, 0, NULL, NULL);
    SpiFlashBufRead(0, 0, buf,len);
#endif
	return 0;
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

    if((result =
           HalSpiXfer(iNum, &ui8TxRxByte[0],
                        1, &ui8TxRxByte[0], 1, NULL, NULL)))
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
               HalSpiXfer(iNum, &ui8TxRxByte[0], 4,
                            &ui8TxRxByte[0], 0, NULL, NULL)))
        {
            return(result);
        }

        ui8TxRxByte[0] = 0xD7;
        if((result =
               HalSpiXfer(iNum, &ui8TxRxByte[0], 1, &ui8TxRxByte[0],
                                     1, NULL, NULL)))
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
