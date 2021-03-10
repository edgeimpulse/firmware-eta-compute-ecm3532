/***************************************************************************//**
 *
 * @file eta_bsp.h
 *
 * @brief This file contains eta_bsp module definitions.
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
 * @addtogroup ecm3532boards-aisensor
 * @addtogroup ecm3532aisensor-bsp
 *
 * @defgroup ecm3532aisensor-bsp BSP (Board Support Package)
 *
 * @ingroup ecm3532boards-aisensor
 * @ingroup ecm3532aisensor-bsp
 * @{
 ******************************************************************************/

#ifndef __ETA_BSP_H__
#define __ETA_BSP_H__

#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "config.h"
// SoC files.
#include "eta_chip.h"
#include "eta_csp_inc.h"

// BSP files.
#include "eta_bsp_button.h"
#include "eta_bsp_led.h"
#include "eta_bsp_power.h"

// Device files.
#ifdef USE_BMP388
#include "eta_devices_bmp388.h"
#endif
#ifdef USE_ICM20602
#include "eta_devices_icm20602.h"
#endif
#ifdef USE_SPIFLASH
//#include "eta_devices_spiflash.h"
#endif
#ifdef USE_BLE
//#include "eta_devices_a31r118.h"
#endif

// Utils
// #include "eta_utils_stdio.h"
// #ifdef ETA_UTILS_USE_TERMINAL
// #include "eta_utils_terminal.h"
// #endif


#ifdef __cplusplus
extern "C" {
#endif

#define ETA_BSP_ABOV_RST_PIN eGpioBit16
#define ETA_BSP_I2C_PWR_PIN  eGpioBit21
#define ETA_BSP_PDM_PWR_PIN  eGpioBit6
#define ETA_BSP_SPI_PWR_PIN  eGpioBit11

#if (CONFIG_AI_VISION_BOARD == 1)
#define ETA_BSP_NUM_LEDS (2U)
#define ETA_BSP_LED0     (eGpioBit27)
#define ETA_BSP_LED1     (eGpioBit28)
#define ETA_BS_LED_MASK ((1 << ETA_BSP_LED0) | (1 << ETA_BSP_LED1))
#else
#define ETA_BSP_NUM_LEDS (5U)
#define ETA_BSP_LED0     (eGpioBit2)
#define ETA_BSP_LED1     (eGpioBit1)
#define ETA_BSP_LED2     (eGpioBit18)
#define ETA_BSP_LED3     (eGpioBit0)
#define ETA_BSP_LED4     (eGpioBit17)

#define ETA_BS_LED_MASK ((1 << ETA_BSP_LED0) | (1 << ETA_BSP_LED1) | \
                         (1 << ETA_BSP_LED2) | (1 << ETA_BSP_LED3) | \
                         (1 << ETA_BSP_LED4))
#endif

#define ETA_BSP_LED_POLARITY_LOW_IS_ON true

#define ETA_BSP_NUM_BUTTONS (1U)
#define ETA_BSP_BUTTON0     (eGpioBit10)      // GPIO #

#define ETA_BSP_ICM20602_SPI_NUM    (1U)
#define ETA_BSP_ICM20602_SPI_SPEED  eSpiClk2MHz
#define ETA_BSP_ICM20602_CLK        eGpioBit29
#define ETA_BSP_ICM20602_MOSI       eGpioBit28
#define ETA_BSP_ICM20602_MISO       eGpioBit27
#define ETA_BSP_ICM20602_SPI_CS     eGpioBit30
#define ETA_BSP_ICM20602_SPI_CS_NUM eSpiChipSel0

#define ETA_BSP_SPIFLASH_SPI_NUM  ETA_BSP_ICM20602_SPI_NUM
#define ETA_BSP_SPIFLASHSPI_SPEED ETA_BSP_ICM20602_SPI_SPEED
#define ETA_BSP_SPIFLASH_CLK      ETA_BSP_ICM20602_CLK
#define ETA_BSP_SPIFLASH_MOSI     ETA_BSP_ICM20602_MOSI
#define ETA_BSP_SPIFLASH_MISO     ETA_BSP_ICM20602_MISO
#define ETA_BSP_SPIFLASH_CS       eGpioBit31
#define ETA_BSP_SPIFLASH_CS_NUM   eSpiChipSel1

#define ETA_BSP_BMP388_I2C_NUM   (1U)
#define ETA_BSP_BMP388_I2C_ADDR  0x76 << 1
#define ETA_BSP_BMP388_I2C_SPEED eI2cClk100kHz
#define ETA_BSP_BMP388_SDA       eGpioBit8
#define ETA_BSP_BMP388_SCL       eGpioBit9

#define ETA_BSP_USE_UART0    (1U)
#define ETA_BSP_USE_UART1    (1U)
#define ETA_BSP_USE_UART_DSP (1U)

#define ETA_BSP_GPIO_UART1_RX  (eGpioBit3)    // GPIO #
#define ETA_BSP_GPIO_UART1_TX  (eGpioBit7)    // GPIO #
#define ETA_BSP_GPIO_UART1_CTS (eGpioBit5) // GPIO #
#define ETA_BSP_GPIO_UART1_RTS (eGpioBit4) // GPIO #

//
// Define Legal Values for UART Selections
//
typedef enum
{
    eBspPrintfNone         = -1,
    eBspPrintfUart0Primary =  0,
    eBspPrintfUart1Primary =  1, // UART_TX on GPIO[20], UART_RX on
                                 // GPIO[19], UART_RTS GPIO[22], UART_CTS
                                 // GPIO[21]
    eBspPrintfUart1Secondary =  2, // UART_TX on GPIO[7], UART_RX on
                                   // GPIO[3], UART_RTS GPIO[4], UART_CTS
                                   // GPIO[5]
    eBspPrintfUartDspPrimary   =  3,   // UART_TX on GPIO[20]
    eBspPrintfUartDspSecondary =  4, // UART_TX on GPIO[7]
}
tBspPrintfUart;

//
// The global UART structures.
//
extern tUart g_sUart0;
extern tUart g_sUart1;

//
// Define which UART should be used for printf (UART0, UART1, UARTDSP
//
#define ETA_BSP_PRINTF_UART     &g_sUart1
#define ETA_BSP_DSP_PRINTF_UART (eBspPrintfNone)

//
// Use this function from startup code.
//
extern void *startup_get_my_pc(void);

//
// Return true if executing from flash.
//
#define ETA_BSP_IS_EXEC_FROM_FLASH() \
    ((((uint32_t)startup_get_my_pc()) & 0xFF000000u) == 0x01000000)

//
//! Global BSP cold start variable.
//
extern bool g_bBspIsColdStart;

//
//! Global BSP run from flash variable.
//
extern bool g_bBspIsRunInFlash;

/*******************************************************************************
 *
 * External function definitions
 *
 ******************************************************************************/

//
// Initialize the BSP.
//
extern void EtaBspInit(void);

//
// Initialize the UARTs.
//
extern void EtaBspUartInit(tUartBaud iBaud);

//
// Deinitialize the BSP.
//
extern void EtaBspDeInit(void);

#ifdef __cplusplus
}
#endif

#endif // __ETA_BSP_H__

