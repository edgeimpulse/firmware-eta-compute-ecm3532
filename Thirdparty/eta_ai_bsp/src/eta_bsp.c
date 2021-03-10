/***************************************************************************//**
 *
 * @file eta_bsp.c
 *
 * @brief This file contains eta_bsp module implementation.
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
 * This is part of revision 1.0.0a6 of the Tensai Software Development Kit.
 *
 ******************************************************************************/

/***************************************************************************//**
 * @ingroup ecm3532aisensor-bsp
 * @{
 ******************************************************************************/

#include "eta_bsp.h"
#include "eta_csp_spi.h"

//
//! The global UART structures.
//
tUart g_sUart0;
tUart g_sUart1;

//
//! Global BSP cold start variable.
//
bool g_bBspIsColdStart;

//
//! Global BSP run from flash variable.
//
bool g_bBspIsRunInFlash;

/***************************************************************************//**
 *
 * EtaBspInit - Initialize the BSP.
 *
 ******************************************************************************/
void
EtaBspInit(void)
{
#ifdef USE_SPIFLASH
    tSpiConfig sSpiConfig;
#endif
#ifdef USE_ICM20602
    tIcm20602Cfg sIcm20602Config;
#endif
#ifdef USE_BMP388
    tBmp388Cfg sBmp388Config;
#endif

    //
    // Determine if this is the first time through the example.
    //
    g_bBspIsColdStart = (EtaCspRtcRestartModeGet() == eRestartCold);

    //
    // Is this running in Flash?
    //
    g_bBspIsRunInFlash = true;//ETA_BSP_IS_EXEC_FROM_FLASH();

    //
    // Do the standard CSP init.
    //
    EtaCspInit();

    //
    // Standard peripheral initializations
    //
    EtaCspGpioInit();

    //
    // Release the GPIOs after a power down.
    //
    EtaCspGpioRelease();

    //
    // Initialize the bucks.
    //
    EtaCspBuckInit();

    //
    // Initialize the timer.
    //
    EtaCspTimerInitMs();

#ifndef USE_SEGGER_RTT_PRINTF

    //
    // Initialize the UART to 115200 baud.
    //
    EtaBspUartInit(eUartBaud115200);
#endif


#ifdef USE_BLE

    //
    // Bring BLE out of reset.
    //
    EtaCspGpioPullBitEnable(ETA_BSP_ABOV_RST_PIN);
    EtaCspGpioPullUpBitSet(ETA_BSP_ABOV_RST_PIN);
    EtaCspGpioInputEnableBitSet(ETA_BSP_ABOV_RST_PIN);

    //
    // Initialize the BLE.
    //
    // EtaDevicesA31R118Init(&g_sUart0);
#endif

    //
    // Initialize the LEDs.
    //
    EtaBspLedsInit();

    //
    // Initialize the buttons.
    //
    EtaBspButtonInit();

    //
    // Enable the weak pull on SPIO0 MISO to keep from floating.
    //
    REG_W1(RTC_SPI0_CTRL, SPI0_MISO_RE, 1);

#ifdef USE_PDM_MIC
    //
    // Power on the PDM Mics.
    //
    EtaCspGpioOutputEnableBitSet(ETA_BSP_PDM_PWR_PIN);
    EtaCspGpioDriveHighSet(ETA_BSP_PDM_PWR_PIN);
    EtaCspGpioOutputBitSet(ETA_BSP_PDM_PWR_PIN);
#else
    //
    // Power down the PDM Mics.
    //
    EtaCspGpioOutputEnableBitSet(ETA_BSP_PDM_PWR_PIN);
    EtaCspGpioDriveHighSet(ETA_BSP_PDM_PWR_PIN);
    EtaCspGpioOutputBitClear(ETA_BSP_PDM_PWR_PIN);
#endif

#ifdef USE_BMP388

    //
    // Configure the I2C pins.
    //
    EtaCspSocCtrlPadMuxSet(ETA_BSP_BMP388_SCL, eSocCtrlPadMux1);
    EtaCspSocCtrlPadMuxSet(ETA_BSP_BMP388_SDA, eSocCtrlPadMux1);

    //
    // Power on the I2C Bus.
    //
    EtaCspGpioOutputEnableBitSet(ETA_BSP_I2C_PWR_PIN);
    EtaCspGpioOutputBitSet(ETA_BSP_I2C_PWR_PIN);

    //
    // Initialize the I2C.
    //
    if(EtaCspI2cInit(ETA_BSP_BMP388_I2C_NUM,
                     ETA_BSP_BMP388_I2C_SPEED) != eEtaSuccess)
    {
        EtaUtilsStdioPrintf("Error: I2C Init failed.\r\n");
    }

    //
    // Initialize the bmp388
    //
    sBmp388Config.iMode = eBmp388ModeI2c;
    sBmp388Config.iI2cInstance = ETA_BSP_BMP388_I2C_NUM;
    sBmp388Config.ui8I2cAddress = ETA_BSP_BMP388_I2C_ADDR;
    EtaDevicesBmp388Init(&sBmp388Config);
#else
    //
    // Power off the I2C Bus.
    //
    EtaCspGpioOutputEnableBitSet(ETA_BSP_I2C_PWR_PIN);
    EtaCspGpioOutputBitClear(ETA_BSP_I2C_PWR_PIN);
#endif

#if defined(USE_ICM20602) || defined(USE_SPIFLASH)

    //
    // Power on the sensors.
    //
    EtaCspGpioOutputEnableBitSet(ETA_BSP_SPI_PWR_PIN);
    EtaCspGpioOutputBitSet(ETA_BSP_SPI_PWR_PIN);

    //
    // Configure the SPI pins.
    //
    EtaCspSocCtrlPadMuxSet(ETA_BSP_ICM20602_CLK, eSocCtrlPadMux1);
    EtaCspSocCtrlPadMuxSet(ETA_BSP_ICM20602_MOSI, eSocCtrlPadMux1);
    EtaCspSocCtrlPadMuxSet(ETA_BSP_ICM20602_MISO, eSocCtrlPadMux1);
    EtaCspSocCtrlPadMuxSet(ETA_BSP_ICM20602_SPI_CS, eSocCtrlPadMux1);
    EtaCspSocCtrlPadMuxSet(ETA_BSP_SPIFLASH_CS, eSocCtrlPadMux1);

    //
    // Fill in the SPI config structure.
    //
    sSpiConfig.spiByteEndian = eSpiByteEndianLSB;
    sSpiConfig.spiBitEndian = eSpiBitEndianMSb;
    sSpiConfig.spiClkPhase = eSpiClkLeadEdge;
    sSpiConfig.spiClkPol = eSpiClkPolPos;
    sSpiConfig.spiClkFreq = ETA_BSP_ICM20602_SPI_SPEED;
    sSpiConfig.ui8IntSize = 0x4;
    sSpiConfig.ui8ICG = 0x0;
    sSpiConfig.ui8IBG = 0xff;

    //
    // Initialize the SPI.
    //
    if(EtaCspSpiInit(ETA_BSP_ICM20602_SPI_NUM, sSpiConfig) != eEtaSuccess)
    {
        printf("Error: SPI Init failed.\r\n");
    }
#else
    //
    // Power off the sensors.
    //
    EtaCspGpioOutputEnableBitSet(ETA_BSP_SPI_PWR_PIN);
    EtaCspGpioOutputBitClear(ETA_BSP_SPI_PWR_PIN);
#endif

#ifdef USE_ICM20602

    //
    // Initialize the icm20602
    //
    sIcm20602Config.iSpiInstance = ETA_BSP_ICM20602_SPI_NUM;
    sIcm20602Config.iSpiChipSel = ETA_BSP_ICM20602_SPI_CS_NUM;

    //
    // Initialize the ICM20602.
    //
    EtaDevicesIcm20602Init(&sIcm20602Config);
#endif

    //
    // Initialize the default HFO tune.
    //
    EtaCspHfoTuneDefaultInit();

#if defined(SRAM) || defined(FLASH_BOOT_SRAM)

    //
    // Indicate power up should be a warm start to SRAM.
    //
    EtaCspRtcRestartModeSet(eRestartWarmSramHfoTune);
#else

    //
    // Indicate power up should be a warm start to flash.
    //
    EtaCspRtcRestartModeSet(eRestartWarmFlashNoTrimNoHfoTune);
#endif
}

/***************************************************************************//**
 *
 * EtaBspUartInit - Initialize the BSP UARTs.
 *
 * @param iBaud is the desired baud rate for the UARTs.
 *
 ******************************************************************************/
void
EtaBspUartInit(tUartBaud iBaud)
{
    //
    // Init UART0 and UART1.
    //
    EtaCspUartInit(&g_sUart0, eUartNum0, iBaud, eUartFlowControlNone);
    EtaCspUartInit(&g_sUart1, eUartNum1, iBaud, eUartFlowControlNone);

    //
    // Setup pins for UART1.
    //
    EtaCspUartPinSelect(&g_sUart1, ETA_BSP_GPIO_UART1_TX, ETA_BSP_GPIO_UART1_RX,
                        ETA_BSP_GPIO_UART1_RTS, ETA_BSP_GPIO_UART1_CTS);

    //
    // Set the Printf UART.
    //
    // EtaCspIoPrintfRedirect(ETA_BSP_PRINTF_UART);

}

/***************************************************************************//**
 *
 * EtaBspDeInit - Deinitialize the BSP.
 *
 ******************************************************************************/
void
EtaBspDeInit(void)
{
    //
    // Wait for and turn the UARTs off.
    //
    EtaCspUartTxWait(&g_sUart0);
    EtaCspUartTxWait(&g_sUart1);
}

