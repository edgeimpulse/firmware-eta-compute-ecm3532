/*******************************************************************************
*
* @file gpio_hal.h
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
******************************************************************************/
#ifndef H_GPIO_HAL_
#define H_GPIO_HAL_

#include <stdint.h>
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif

/* \cond DO_NOT_DOCUMENT */
#define HalGpioBIT(n)          (((uint64_t) 0x1) << n)
/* \endcond */

/**
 * GPIO pin pull type.
 */
typedef enum {
    /** Pull not enabled */
    HalGpioPullNone = 0,
    /** Pull-up enabled */
    HalGpioPullUp = 1,
    /** Pull-down enabled */
    HalGpioPullDown = 2
}tHalGpioPull;

/**
 * GPIO IRQ trigger type.
 */
typedef enum {
    /** IRQ occurs when line is low */
    HalGpioTrigLow = 0,
    /** IRQ occurs when line is high */
    HalGpioTrigHigh = 1
}tHalGpioIntTrig;

/**
 * GPIO Pin Direction.
 */
typedef enum {
    /** GPIO Direction Input */
    HalGpioDirIn = 0,
    /** GPIO Direction Output */
    HalGpioDirOut = 1,
}tHalGpioDir;

/**
 * GPIO IRQ Number.
 */
typedef enum {
    /** Use GPIO0 IRQ */
    HalGpioIRQ0 = 0,
    /** Use GPIO1 IRQ */
    HalGpioIRQ1 = 1,
}tHalGpioIRQ;


/**
 *  Function prototype for Gpio irq handler functions
 */
typedef void (*tHalGpioIntHandler)(void *pArg);

/**
 * Initialize the specified Gpio pin as Input
 *
 * @param ui32Gpio Gpio number to be initialize
 * @param iPull pull type to be configure for input pin
 *
 * @return 0 on success, negative error code on failure.
 */
int32_t HalGpioInInit(uint32_t ui32Gpio, tHalGpioPull iPull);

/**
 * Initialize the specified Gpio pin as Output pin
 *
 * @param ui32Gpio Gpio number to be initialize
 * @param bVal Initial value of the Output pin, 0 or 1
 *
 * @return 0 on success, negative error code on failure.
 */
int32_t HalGpioOutInit(uint32_t ui32Gpio, bool bVal);

/**
 * Write a value to the specified Gpio pin.
 *
 * @param ui32Gpio Gpio output pin to be set
 * @param bVal Value to set for pin, 0 or 1
 */
void HalGpioWrite(uint32_t ui32Gpio, bool bVal);

/**
 * Reads the specified pin.
 *
 * @param ui32Gpio Gpio input pin to be read
 *
 * @return 0: low, 1: high on success, negative error code on failure
 */
int32_t HalGpioRead(uint32_t ui32Gpio);

/**
 * Toggles the specified pin
 *
 * @param ui32Gpio Gpio output pin to be change
 *
 */
void HalGpioToggle(uint32_t ui32Gpio);

/**
 * Initialize a given pin as IRQ PIN.
 *
 * @param ui32Gpio      Gpio pin to be configure as interrupt pin
 * @param iGpioIrq      GPIO IP hase two IRQ, GPIO IRQ0 and IRQ1, map to any 1
 * @param fIntHandler   Function handler to be call when IRQ triggered
 * @param vArg          Argument to provide to the IRQ handler
 * @param iTrig         Trigger type (e.g. rising, falling)
 * @param iPull         Pull type of pin (e.g. pullup, pulldown)
 *
 * @return 0 on success, non-zero error code on failure.
 */
int HalGpioIntInit(uint32_t ui32Gpio, tHalGpioIRQ iGpioIrq, tHalGpioIntHandler fIntHandler,
                    void *vArg, tHalGpioIntTrig iTrig, tHalGpioPull iPull);

/**
 * Release a pin from being configured to trigger IRQ on state change.
 *
 * @param ui32Gpio Gpio pin to be released
 */
void HalGpioIntRelease(uint32_t ui32Gpio);

/**
 * Enable IRQs on the passed pin
 *
 * @param iGpioIrq GPIO IP hase two IRQ, GPIO IRQ0 and IRQ1, map to any 1
 * @param ui32Gpio Gpio pin IRQ to be enabled
 */
void HalGpioIntEnable(tHalGpioIRQ iGpioIrq, uint32_t ui32Gpio);

/**
 * Disable IRQs on the passed pin
 *
 * @param iGpioIrq GPIO IP hase two IRQ, GPIO IRQ0 and IRQ1, map to any 1
 * @param ui32Gpio Gpio pin IRQ to be disabled
 */
void HalGpioIntDisable(tHalGpioIRQ iGpioIrq, uint32_t ui32Gpio);

/*
 * GPIO int clear
 *
 * Clear gpio interrupt
 *
 * @param iGpioIrq GPIO IP hase two IRQ, GPIO IRQ0 and IRQ1, map to any 1
 * @param ui32Gpio Gpio pin IRQ to be cleared
 */
void HalGpioIntClear(tHalGpioIRQ iGpioIrq, uint32_t ui32Gpio);
#ifdef __cplusplus
}
#endif

#endif /* H_GPIO_HAL_ */
