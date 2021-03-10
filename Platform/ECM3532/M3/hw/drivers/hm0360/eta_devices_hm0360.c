/***************************************************************************//**
 *
 * @file eta_devices_hm0360.c
 *
 * @brief Sensor driver for the Himax HM0360 camera.
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
 * This is part of revision ${version}, of the Tensai Software Development Kit.
 *
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup etadevices-hm01b0 Himax HM0360 QVGA Camera
 * @ingroup etadevices
 * @{
 ******************************************************************************/

#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "eta_devices_hm0360.h"
#include "i2c_hal.h"
#include "gpio_hal.h"
#include "timer_hal.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "print_util.h"

#ifdef CONFIG_QQVGA_MODE
#include "eta_devices_hm0360_raw8_qqvga_5fps.h"
#elif CONFIG_QVGA_MODE
#include "eta_devices_hm0360_raw8_qvga_5fps.h"
#else
#include "eta_devices_hm0360_raw8_vga_5fps.h"
#endif

#define STARTING_COLUMN (0)
#define END_COLUMN  (CONFIG_IMAGE_COLUMN_COUNT)

#define STARTING_ROW (0)
#define END_ROW  (CONFIG_IMAGE_ROW_COUNT)

#define VSYNC_LOW_GPIOIRQ 0
#define VSYNC_HIGH_GPIOIRQ 1

SemaphoreHandle_t xVSHSem =  NULL;
SemaphoreHandle_t xVSLSem =  NULL;

//
// The I2C address.
//
static uint8_t g_ui8I2cAddress = CONFIG_HM0360_I2C_ADDRESS;
static uint8_t g_ui8I2cInstance = CONFIG_HM0360_I2C_INSTANCE;

//
// Pin assignments.
//
tHm0360Pins g_sPins = {
    .iD0 = CONFIG_HM0360_D0_PIN,
    .ui32Hsync = (1  << CONFIG_HM0360_HSYNC_GPIO),
    .ui32Vsync = (1 << CONFIG_HM0360_VSYNC_GPIO),
    .ui32Pclk = (1 << CONFIG_HM0360_PCLK_GPIO),
};

/***************************************************************************//**
 *
 * Hm0360RegWrite - TODO
 *
 * @param ui8Address TODO
 * @param ui8Value TODO
 *
 ******************************************************************************/
static void
Hm0360RegWrite(uint16_t ui16Reg, uint8_t ui8Value)
{
    uint8_t ui8TxData = ui8Value;

    HalI2cWrite(CONFIG_HM0360_I2C_INSTANCE, CONFIG_HM0360_I2C_ADDRESS,
            ui16Reg, 2, &ui8TxData, 1, NULL, NULL);
}

/***************************************************************************//**
 *
 * Hm0360RegRead - TODO
 *
 * @param ui8Address TODO
 *
 ******************************************************************************/
static uint8_t
Hm0360RegRead(uint16_t ui16Reg)
{
    uint8_t ui8RxData;
    HalI2cRead(g_ui8I2cInstance, g_ui8I2cAddress, ui16Reg, 2, &ui8RxData, 1, NULL, NULL);
    return ui8RxData;
}

void vSyncReady(void *ptr)
{
    long lHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(xVSHSem, &lHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(lHigherPriorityTaskWoken);
    HalGpioIntDisable(VSYNC_HIGH_GPIOIRQ, CONFIG_HM0360_VSYNC_GPIO);
}

void vSyncLow(void *ptr)
{
    long lHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(xVSLSem, &lHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(lHigherPriorityTaskWoken);
    HalGpioIntDisable(VSYNC_LOW_GPIOIRQ, CONFIG_HM0360_VSYNC_GPIO);
}

/***************************************************************************//**
 *
 * EtaDevicesHm0360Init - Initialize the device.
 *
 * @param psPins Pointer to the pin assignments.
 * @param iI2cAddress is the 7bit I2C address of the device.
 * @param pfnI2cRead is a pointer to the function to use for reads.
 * @param pfnI2cWrite is a pointer to the function to use for writes.
 *
 ******************************************************************************/

void EtaDevicesHm0360Init(void)
{
    uint32_t ui32Idx;

    // Reset the camera.
    //
    Hm0360RegWrite(0x0100, 0x0);
    HalTmrDelay(0, 50);
    Hm0360RegWrite(0x0103, 0x1);
    HalTmrDelay(0, 500);
    Hm0360RegWrite(0x0103, 0x0);
    HalTmrDelay(0, 500);

    ecm35xx_printf("Cam Id %x%x \r\n", Hm0360RegRead(0), Hm0360RegRead(1));

    //
    // Loop through predefined config.
    //
    for(ui32Idx = 0; ui32Idx < (sizeof(HM0360_HIMAX_INIT) / sizeof(HM0360_HIMAX_INIT[0]));
        ui32Idx++)
    {
        //
        // Write register.
        //
        Hm0360RegWrite(HM0360_HIMAX_INIT[ui32Idx][0], HM0360_HIMAX_INIT[ui32Idx][1]);

    }

    if (!xVSHSem)
    {
        xVSHSem = xSemaphoreCreateBinary();
        HalGpioIntInit(CONFIG_HM0360_VSYNC_GPIO, VSYNC_HIGH_GPIOIRQ, vSyncReady, NULL,
                        HalGpioTrigHigh, HalGpioPullNone);
        HalGpioIntDisable(VSYNC_HIGH_GPIOIRQ, CONFIG_HM0360_VSYNC_GPIO);
    }
    if (!xVSLSem)
    {
        xVSLSem = xSemaphoreCreateBinary();
        HalGpioIntInit(CONFIG_HM0360_VSYNC_GPIO, VSYNC_LOW_GPIOIRQ, vSyncLow, NULL,
                        HalGpioTrigLow, HalGpioPullNone);
        HalGpioIntDisable(VSYNC_LOW_GPIOIRQ, CONFIG_HM0360_VSYNC_GPIO);
    }

}

/***************************************************************************//**
 *
 * EtaDevicesHm0360FrameStream - Stream ui8Frames and enter standby.
 *
 *
 ******************************************************************************/
void
EtaDevicesHm0360FrameStream(void)
{
    //
    // Write number of frames to request.
    //
    Hm0360RegWrite(HM0360_FRAME_COUNT_H, 0);
    Hm0360RegWrite(HM0360_FRAME_COUNT_L, 1);

    //
    // Set streaming mode to 3.
    //
    Hm0360RegWrite(HM0360_MODE_SELECT, eHm0360ModeStreaming3);

}

/***************************************************************************//**
 *
 * _OneFrameRead - Read in one frame.
 *
 ******************************************************************************/

static inline uint32_t
_OneFrameRead(uint8_t *pui8Frame)
{
    bool bExit;
    uint32_t ui32Pixels;
    int32_t i32Ret = -1;
    uint16_t row = 0 , col = 0;
    uint32_t lscycle = 0, lecycle = 0;
    uint32_t pscycle = 0, pecycle = 0;
    int32_t iVal;

    //
    // Initialize exit and pixel count.
    //
    bExit = false;
    ui32Pixels = 0;

    if(REG_GPIO_DATA_IN.V & g_sPins.ui32Vsync)
    {
        HalGpioIntClear(VSYNC_LOW_GPIOIRQ, CONFIG_HM0360_VSYNC_GPIO);
        HalGpioIntEnable(VSYNC_LOW_GPIOIRQ, CONFIG_HM0360_VSYNC_GPIO);
        do {
            i32Ret  = xSemaphoreTake(xVSLSem, 1);
        } while((!i32Ret));
        HalGpioIntDisable(VSYNC_LOW_GPIOIRQ, CONFIG_HM0360_VSYNC_GPIO);
    }

    HalGpioIntClear(VSYNC_HIGH_GPIOIRQ, CONFIG_HM0360_VSYNC_GPIO);
    HalGpioIntEnable(VSYNC_HIGH_GPIOIRQ, CONFIG_HM0360_VSYNC_GPIO);
    do {
        i32Ret  = xSemaphoreTake(xVSHSem, 1);
    } while  (!i32Ret);

    __asm volatile( "cpsid i" ::: "memory" );
    HalGpioIntDisable(VSYNC_HIGH_GPIOIRQ, CONFIG_HM0360_VSYNC_GPIO);

    /*  Wait for Hsync */
    while(!(REG_GPIO_DATA_IN.V & g_sPins.ui32Hsync));

    //
    // Wait for exit.
    do
    {
        //
        // Save pixels while HSYNC is high.
        //
        col = 0;
        do
        {
            //
            // Wait for PCLK to go high.
            //
            do
            {
                iVal = REG_GPIO_DATA_IN.V;
            } while (!(iVal & g_sPins.ui32Pclk));

            //
            // Save pixel value.
            //
            if ( col < END_COLUMN)
                pui8Frame[ui32Pixels++] = (iVal >> CONFIG_HM0360_D0_PIN);

            ++col;

            //
            // Wait for PCLK to go low.
            //
            //while(REG_GPIO_DATA_IN.V & g_sPins.ui32Pclk);
            do
            {
                iVal = REG_GPIO_DATA_IN.V;
            } while ((iVal & g_sPins.ui32Pclk));


        } while ((iVal & g_sPins.ui32Hsync));


        //
        // Wait for HSYNC to go high.
        //
        while((REG_GPIO_DATA_IN.V & g_sPins.ui32Hsync) == 0)
        {
            //
            // Is VSYNC low as well?
            //
            if((REG_GPIO_DATA_IN.V & g_sPins.ui32Vsync) == 0)
            {
                //
                // Exit.
                //
                bExit = true;
                break;
            }
        }
        ++row;
        if (row >= END_ROW)
            bExit = true;

    } while(!bExit);

    __asm volatile( "cpsie i" ::: "memory" );

    //
    // Return pixels saved.
    //
    return(ui32Pixels);
}

/***************************************************************************//**
 *
 * EtaDevicesHm0360OneFrameReadBlocking - TODO
 *
 * @param pui8Frame TODO
 * @param ui32DelayMs TODO
 *
 * @return The number of pixels captured.
 *
 ******************************************************************************/
uint32_t
EtaDevicesHm0360OneFrameReadBlocking(uint8_t *pui8Frame)
{
    uint32_t ui32Pixels;

#ifndef CONFIG_STREAMING_MODE
    //
    // Trigger a frame.
    //
    while(REG_GPIO_DATA_IN.V & g_sPins.ui32Vsync);
    EtaDevicesHm0360FrameStream();

#endif

    //
    // Read the frame.
    //
    ui32Pixels = _OneFrameRead(pui8Frame);

#ifndef CONFIG_STREAMING_MODE
    Hm0360RegWrite(HM0360_MODE_SELECT, 0);
#endif
    //
    // Return the number of pixels captured.
    //
    return(ui32Pixels);
}
