/***************************************************************************//**
 *
 * @file eta_devices_hm01b0.c
 *
 * @brief Sensor driver for the Himax HM01B0 camera.
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
 * This is part of revision ${version}, of the Tensai Software Development Kit.
 *
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup etadevices-hm01b0 Himax HM01B0 QVGA Camera
 * @ingroup etadevices
 * @{
 ******************************************************************************/

#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "eta_devices_hm01b0.h"
#include "i2c_hal.h"
#include "gpio_hal.h"
#include "timer_hal.h"
#include "FreeRTOS.h"
#include "semphr.h"

//#include "eta_devices_hm01b0_raw8_qvga.h"
#include "eta_devices_hm01b0_raw8_324x324_monochrome_5fps.h"

#define COLUMN_COUNT (CONFIG_IMAGE_COLUMN_COUNT)
/* center 256 col */
#define STARTING_COLUMN ((HM01B0_PIXEL_X_NUM - COLUMN_COUNT) >> 1)
#define END_COLUMN  (HM01B0_PIXEL_X_NUM - STARTING_COLUMN)

#define STARTING_ROW ((HM01B0_PIXEL_X_NUM - COLUMN_COUNT) >> 1)
#define END_ROW  (HM01B0_PIXEL_X_NUM - STARTING_COLUMN)

#define VSYNC_LOW_GPIOIRQ 0
#define VSYNC_HIGH_GPIOIRQ 1

SemaphoreHandle_t xVSHSem =  NULL;
SemaphoreHandle_t xVSLSem =  NULL;
//
// The I2C address.
//
static uint8_t g_ui8I2cAddress = CONFIG_HM01B0_I2C_ADDRESS;
static uint8_t g_ui8I2cInstance = CONFIG_HM01B0_I2C_INSTANCE;


//
// Pin assignments.
//
static tHm01b0Pins g_sPins = {
    .iD0 = CONFIG_HM01B0_D0_PIN,
    .ui32Hsync = (1  << CONFIG_HM01B0_HSYNC_GPIO),
    .ui32Vsync = (1 << CONFIG_HM01B0_VSYNC_GPIO),
    .ui32Pclk = (1 << CONFIG_HM01B0_PCLK_GPIO),
};

/***************************************************************************//**
 *
 * Hm01b0RegWrite - TODO
 *
 * @param ui8Address TODO
 * @param ui8Value TODO
 *
 ******************************************************************************/
static void
Hm01b0RegWrite(uint16_t ui16Reg, uint8_t ui8Value)
{
    uint8_t ui8TxData = ui8Value;

    HalI2cWrite(CONFIG_HM01B0_I2C_INSTANCE, CONFIG_HM01B0_I2C_ADDRESS,
            ui16Reg, 2, &ui8TxData, 1, NULL, NULL);
}

/***************************************************************************//**
 *
 * Hm01b0RegRead - TODO
 *
 * @param ui8Address TODO
 *
 ******************************************************************************/
static uint8_t
Hm01b0RegRead(uint16_t ui16Reg)
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
    HalGpioIntDisable(VSYNC_HIGH_GPIOIRQ, CONFIG_HM01B0_VSYNC_GPIO);
}

void vSyncLow(void *ptr)
{
    long lHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(xVSLSem, &lHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(lHigherPriorityTaskWoken);
    HalGpioIntDisable(VSYNC_LOW_GPIOIRQ, CONFIG_HM01B0_VSYNC_GPIO);
}

/***************************************************************************//**
 *
 * EtaDevicesHm01b0Init - Initialize the device.
 *
 * @param psPins Pointer to the pin assignments.
 * @param iI2cAddress is the 7bit I2C address of the device.
 * @param pfnI2cRead is a pointer to the function to use for reads.
 * @param pfnI2cWrite is a pointer to the function to use for writes.
 *
 ******************************************************************************/

void EtaDevicesHm01b0Init(void)
{
    uint32_t ui32Idx;

    // Reset the camera.
    //
    Hm01b0RegWrite(0x0103, 0x0);
    HalTmrDelay(0, 10);

    //
    // Loop through predefined config.
    //
    for(ui32Idx = 0; ui32Idx < (sizeof(HM01B0_INIT) / sizeof(HM01B0_INIT[0]));
        ui32Idx++)
    {
        //
        // Write register.
        //
        Hm01b0RegWrite(HM01B0_INIT[ui32Idx][0], HM01B0_INIT[ui32Idx][1]);

    }

    if (!xVSHSem)
    {
        xVSHSem = xSemaphoreCreateBinary();
        HalGpioIntInit(CONFIG_HM01B0_VSYNC_GPIO, VSYNC_HIGH_GPIOIRQ, vSyncReady, NULL,
                        HalGpioTrigHigh, HalGpioPullNone);
        HalGpioIntDisable(VSYNC_HIGH_GPIOIRQ, CONFIG_HM01B0_VSYNC_GPIO);
    }
    if (!xVSLSem)
    {
        xVSLSem = xSemaphoreCreateBinary();
        HalGpioIntInit(CONFIG_HM01B0_VSYNC_GPIO, VSYNC_LOW_GPIOIRQ, vSyncLow, NULL,
                        HalGpioTrigLow, HalGpioPullNone);
        HalGpioIntDisable(VSYNC_LOW_GPIOIRQ, CONFIG_HM01B0_VSYNC_GPIO);
    }

}

/***************************************************************************//**
 *
 * EtaDevicesHm01b0FramesStream - Stream ui8Frames and enter standby.
 *
 * @param ui8Frames Number of frames to stream.
 *
 ******************************************************************************/
void
EtaDevicesHm01b0FramesStream(uint8_t ui8Frames)
{
    //
    // Write number of frames to request.
    //
    Hm01b0RegWrite(HM01B0_PMU_AUTOSLEEP_FRAMECNT, ui8Frames);

    //
    // Set streaming mode to 2.
    //
    Hm01b0RegWrite(HM01B0_MODE_SELECT, eHm01b0ModeStreaming2);

    //
    // Update the frame timing.
    //
    Hm01b0RegWrite(HM01B0_GRP_PARAM_HOLD, 1);

}

/***************************************************************************//**
 *
 * _OneFrameRead - Read in one frame.
 *
 ******************************************************************************/
static inline uint32_t
_OneFrameRead(int8_t *pui8Frame)
{
    bool bExit;
    uint32_t ui32Pixels;
    int32_t i32Ret = -1;
    uint16_t row = 0 , col = 0;

    //
    // Initialize exit and pixel count.
    //
    bExit = false;
    ui32Pixels = 0;

    if(REG_GPIO_DATA_IN.V & g_sPins.ui32Vsync)
    {
        HalGpioIntClear(VSYNC_LOW_GPIOIRQ, CONFIG_HM01B0_VSYNC_GPIO);
        HalGpioIntEnable(VSYNC_LOW_GPIOIRQ, CONFIG_HM01B0_VSYNC_GPIO);
        do {
            i32Ret  = xSemaphoreTake(xVSLSem, 1);
        } while((!i32Ret));
        HalGpioIntDisable(VSYNC_LOW_GPIOIRQ, CONFIG_HM01B0_VSYNC_GPIO);
    }

    HalGpioIntClear(VSYNC_HIGH_GPIOIRQ, CONFIG_HM01B0_VSYNC_GPIO);
    HalGpioIntEnable(VSYNC_HIGH_GPIOIRQ, CONFIG_HM01B0_VSYNC_GPIO);
    do {
        i32Ret  = xSemaphoreTake(xVSHSem, 1);
    } while  (!i32Ret);

    __asm volatile( "cpsid i" ::: "memory" );
    HalGpioIntDisable(VSYNC_HIGH_GPIOIRQ, CONFIG_HM01B0_VSYNC_GPIO);

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
        while((REG_GPIO_DATA_IN.V & g_sPins.ui32Hsync))
        {
            //
            // Wait for PCLK to go high.
            //
            while(!(REG_GPIO_DATA_IN.V & g_sPins.ui32Pclk));

            //
            // Save pixel value.
            //
            if (((col >= STARTING_COLUMN) & (col < END_COLUMN)) && (row >= STARTING_ROW))
                pui8Frame[ui32Pixels++] = ((REG_GPIO_DATA_IN.V >> g_sPins.iD0) - 128);

            col++;

            //
            // Wait for PCLK to go low.
            //
            while(REG_GPIO_DATA_IN.V & g_sPins.ui32Pclk);

        }


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
        row++;
        if (row >= END_ROW)
            bExit = true;
    }
    while(!bExit);

    __asm volatile( "cpsie i" ::: "memory" );

    //
    // Return pixels saved.
    //
    return(ui32Pixels);
}

/***************************************************************************//**
 *
 * EtaDevicesHm01b0OneFrameReadBlocking - TODO
 *
 * @param pui8Frame TODO
 * @param ui32DelayMs TODO
 *
 * @return The number of pixels captured.
 *
 ******************************************************************************/
uint32_t
EtaDevicesHm01b0OneFrameReadBlocking(int8_t *pui8Frame)
{
    uint32_t ui32Pixels;

#ifndef CONFIG_STREAMING_MODE
    //
    // Trigger a frame.
    //
    EtaDevicesHm01b0FramesStream(1);
#endif

    //
    // Read the frame.
    //
    ui32Pixels = _OneFrameRead(pui8Frame);
    //
    // Return the number of pixels captured.
    //
    return(ui32Pixels);
}
