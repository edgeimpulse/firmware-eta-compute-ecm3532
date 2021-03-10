/***************************************************************************//**
 *
 * @file eta_devices_nhd0216.c
 *
 * @brief NHD_OLED (2 rows)
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
 * This is part of revision 1.0.0a-110-g026da8fb8 of the Tensai Software Development Kit.
 *
 ******************************************************************************/
#include "i2c_hal.h"
#include "eta_devices_nhd0216.h"
#include <stdlib.h>

//
// Device configuration structure.
//
tNhd0216Cfg g_sNhd0216Cfg;

// ##### INTERNAL BEGIN #####
/***************************************************************************//**
 *
 * _Nhd0216ReadI2c - Function for reading over I2C.
 *
 * @param pui8TxData TODO
 * @param pui8RxData TODO
 * @param ui8Count TODO
 *
 ******************************************************************************/
static void
_Nhd0216ReadI2c(uint8_t *pui8TxData, uint8_t ui8TxCount, uint8_t *pui8RxData,
                uint8_t ui8RxCount)
{
    EtaCspI2cTransferPoll(g_sNhd0216Cfg.iI2cInstance, pui8TxData,
                          ui8TxCount, pui8RxData, ui8RxCount, 2);
}
// ##### INTERNAL END #####

/***************************************************************************//**
 *
 * Nhd0216WriteI2c - Function for writing over I2C.
 *
 * @param pui8Data Data to write.
 * @param ui8Count Number of bytes to send.
 *
 ******************************************************************************/
static void
_Nhd0216WriteI2c(uint8_t *pui8Data, uint8_t ui8Count)
{
    EtaCspI2cTransferPoll(g_sNhd0216Cfg.iI2cInstance, pui8Data, ui8Count, 0,
                          0, 0);


}

/***************************************************************************//**
 *
 * _Nhd0216ReadSpi - Function for reading over SPI.
 *
 * @param pui8TxData TODO
 * @param pui8RxData TODO
 * @param ui8Count TODO
 *
 ******************************************************************************/
static void
_Nhd0216ReadSpi(uint8_t *pui8TxData, uint8_t ui8TxCount, uint8_t *pui8RxData,
                uint8_t ui8RxCount)
{
    EtaCspSpiTransferPoll(g_sNhd0216Cfg.iSpiInstance, pui8TxData,
                          ui8TxCount, pui8RxData, ui8RxCount,
                          g_sNhd0216Cfg.iChipSel, eSpiSequenceMiddle);
}

/***************************************************************************//**
 *
 * _Nhd0216WriteSpi - Function for writing over SPI.
 *
 * @param pui8Data Data to write.
 * @param ui8Count Number of bytes to send.
 *
 ******************************************************************************/
static void
_Nhd0216WriteSpi(uint8_t *pui8Data, uint8_t ui8Count)
{
    EtaCspSpiTransferPoll(g_sNhd0216Cfg.iSpiInstance, pui8Data, ui8Count, 0,
                          0, g_sNhd0216Cfg.iChipSel, eSpiSequenceMiddle);
}

/***************************************************************************//**
 *
 * EtaDevicesNhd0216Init - Initialize the display.
 *
 * @param psCfg device configuration.
 *
 ******************************************************************************/
void
EtaDevicesNhd0216Init(tNhd0216Cfg *psCfg)
{
    g_sNhd0216Cfg = *psCfg;

    EtaDevicesNhd0216CommandSend(0x2A); // function set (extended command set)
    EtaDevicesNhd0216CommandSend(0x71); // function selection A, disable
                                        // internal Vdd regualtor
    EtaDevicesNhd0216DataSend(0x00);

    EtaDevicesNhd0216CommandSend(0x28); // function set(fundamental command set)
    EtaDevicesNhd0216CommandSend(0x08); // display off, cursor off, blink off
    EtaDevicesNhd0216CommandSend(0x2A); // function set (extended command set)
    EtaDevicesNhd0216CommandSend(0x79); // OLED command set enabled
    EtaDevicesNhd0216CommandSend(0xD5); // set display clock divide
                                        // ratio/oscillator frequency

    EtaDevicesNhd0216CommandSend(0x70); // set display clock divide
                                        // ratio/oscillator frequency

    EtaDevicesNhd0216CommandSend(0x78); // OLED command set disabled
    EtaDevicesNhd0216CommandSend(0x09); // extended function set (4-lines)
    EtaDevicesNhd0216CommandSend(0x06); // COM SEG direction
    EtaDevicesNhd0216CommandSend(0x72); // function selection B, disable
                                        // internal Vdd regualtor

    EtaDevicesNhd0216DataSend(0x00); // ROM CGRAM selection

    EtaDevicesNhd0216CommandSend(0x2A); // function set (extended command set)
    EtaDevicesNhd0216CommandSend(0x79); // OLED command set enabled
    EtaDevicesNhd0216CommandSend(0xDA); // set SEG pins hardware configuration
    EtaDevicesNhd0216CommandSend(0x00); // set SEG pins hardware configuration
    //////////////////////////////////////0x10 on other slim
    // char OLEDs
    EtaDevicesNhd0216CommandSend(0xDC); // function selection C
    EtaDevicesNhd0216CommandSend(0x00); // function selection C
    EtaDevicesNhd0216CommandSend(0x81); // set contrast control
    EtaDevicesNhd0216CommandSend(0x7F); // set contrast control
    EtaDevicesNhd0216CommandSend(0xD9); // set phase length
    EtaDevicesNhd0216CommandSend(0xF1); // set phase length
    EtaDevicesNhd0216CommandSend(0xDB); // set VCOMH deselect level
    EtaDevicesNhd0216CommandSend(0x40); // set VCOMH deselect level
    EtaDevicesNhd0216CommandSend(0x78); // OLED command set disabled
    EtaDevicesNhd0216CommandSend(0x28); // function set (fundamental command
                                        // set)

    EtaDevicesNhd0216CommandSend(0x01); // clear display
    EtaDevicesNhd0216CommandSend(0x80); // set DDRAM address to 0x00
    EtaDevicesNhd0216CommandSend(0x0C); // display ON

    //
    // Delay.
    //
    EtaCspRtcTmrDelayMs(100);
}

/***************************************************************************//**
 *
 * EtaDevicesNhd0216CommandSend - TODO
 *
 * @param cCmd Command to send.
 *
 ******************************************************************************/
void
EtaDevicesNhd0216CommandSend(char cCmd)
{
    uint8_t pui8Package[3];

    switch(g_sNhd0216Cfg.iMode)
    {
        case eNhd0216ModeSpi:
        {
            pui8Package[0] = 0xF8; /* write the EtaDevicesNhd0216CommandSend */
            pui8Package[1] = ((cCmd & 0x01) << 7) | ((cCmd & 0x02) << 5) |
                             ((cCmd & 0x04) << 3) |
                             ((cCmd & 0x8) << 1);
            pui8Package[2] = ((cCmd & 0x10) << 3) | ((cCmd & 0x20) << 1) |
                             ((cCmd & 0x40) >> 1) |
                             ((cCmd & 0x80) >> 3);

            _Nhd0216WriteSpi(pui8Package, 3);
            break;
        }

        case eNhd0216ModeI2c:
        {
            pui8Package[0] = cCmd;

            HalI2cWrite(g_sNhd0216Cfg.iI2cInstance, g_sNhd0216Cfg.ui8I2cAddress,
                        0,  1, &pui8Package[0], 1, NULL, NULL);
            break;
        }
    }
}

/***************************************************************************//**
 *
 * EtaDevicesNhd0216DataSend - TODO
 *
 * @param cData TODO
 *
 ******************************************************************************/
void
EtaDevicesNhd0216DataSend(char cData)
{
    uint8_t pui8Package[3];

    switch(g_sNhd0216Cfg.iMode)
    {
        case eNhd0216ModeI2c:
        {
            pui8Package[0] = cData;
            HalI2cWrite(g_sNhd0216Cfg.iI2cInstance, g_sNhd0216Cfg.ui8I2cAddress,
                        0x40,  1, pui8Package, 1, NULL, NULL);
            break;
        }

        case eNhd0216ModeSpi:
        {
            pui8Package[0] = 0xFA; /* write the EtaDevicesNhd0216DataSend */
            pui8Package[1] = ((cData & 0x01) << 7) | ((cData & 0x02) << 5) |
                             ((cData & 0x04) << 3) |
                             ((cData & 0x8) << 1);
            pui8Package[2] = ((cData & 0x10) << 3) | ((cData & 0x20) << 1) |
                             ((cData & 0x40) >> 1) |
                             ((cData & 0x80) >> 3);

            _Nhd0216WriteSpi(pui8Package, 3);

            //
            // Delay
            //
            EtaCspRtcTmrDelayMs(5);
            return;
        }
    }
}

/***************************************************************************//**
 *
 * EtaDevicesNhd0216Clear - Clear the display.
 *
 ******************************************************************************/
void
EtaDevicesNhd0216Clear(void)
{
    EtaDevicesNhd0216CommandSend(0x01);
}

/***************************************************************************//**
 *
 * EtaDevicesNhd0216DataRead - TODO
 *
 * @param ui8Data TODO
 * @return TODO
 *
 ******************************************************************************/
uint8_t
EtaDevicesNhd0216DataRead(uint8_t ui8Data)
{
    uint8_t pui8Package[3];
    uint8_t ui8Rev;

    switch(g_sNhd0216Cfg.iMode)
    {
        case eNhd0216ModeSpi:
        {
            pui8Package[0] = 0xFE; /* write the EtaDevicesNhd0216DataSend */
            pui8Package[1] = ((ui8Data & 0x01) << 7) | ((ui8Data & 0x02) << 5) |
                             ((ui8Data & 0x04) << 3) |
                             ((ui8Data & 0x8) << 1);
            pui8Package[2] = ((ui8Data & 0x10) << 3) | ((ui8Data & 0x20) << 1) |
                             ((ui8Data & 0x40) >> 1) |
                             ((ui8Data & 0x80) >> 3);

            _Nhd0216ReadSpi(&pui8Package[0], 1, &ui8Rev, 1);

            //
            // Delay
            //
            EtaCspRtcTmrDelayMs(5);
        }
				case eNhd0216ModeI2c:
				default:
				{
					break;
				}
    }

    //
    // Return.
    //
    return(ui8Rev);
}

/***************************************************************************//**
 *
 * EtaDevicesNhd0216StringPrint - Print a string to the display
 *
 * @param pcStr String to print.
 * @param ui8X ui8X location.
 * @param ui8Y ui8Y location.
 *
 ******************************************************************************/
void
EtaDevicesNhd0216StringPrint(char *pcStr, uint8_t ui8X, uint8_t ui8Y)
{
    uint8_t ui8I, ui8Cmd;

    //
    // Configure position.
    //
    if(ui8X == 0)
    {
        ui8Cmd = 0x01 | ui8Y;
    }
    else
    {
        ui8Cmd = 0xA0 | ui8Y;
    }

    //
    // Send the command.
    //
    EtaDevicesNhd0216CommandSend(ui8Cmd);

    //
    // Send the data.
    //
    ui8I = 0;
    while(pcStr[ui8I] != 0)
    {
        EtaDevicesNhd0216DataSend(pcStr[ui8I++]);
    }
}
