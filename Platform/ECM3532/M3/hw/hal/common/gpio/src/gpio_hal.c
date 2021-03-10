/*******************************************************************************
*
* @file gpio_hal.c
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
#include "config.h"
#include <stddef.h>
#include "gpio_hal.h"
#include "errno.h"
#ifdef CONFIG_ECM3531
#include "ecm3531.h"
#elif CONFIG_ECM3532
#include "ecm3532.h"
#endif
#include "cm3.h"
#include "eta_csp_gpio.h"
#include "FreeRTOS.h"
#include "task.h"

/* Storage for GPIO callbacks. */
struct tHalGpioInt {
    uint8_t ui32Pin;
    tHalGpioIntHandler fHandler;
    void *vArg;
};

static struct tHalGpioInt sHalGpioIntAArr[CONFIG_GPIO_MAX_IRQ];
static struct tHalGpioInt sHalGpioIntBArr[CONFIG_GPIO_MAX_IRQ];

/**
 * GPIO Pull
 *
 * Configure Pull for the Pin which is configured as Input pin
 *
 * @param Pin for which pull to be initialize
 * @param Pull value
 *
 * @return int  0: no error; -1 otherwise.
 */

int HalGpioPull(uint32_t ui32Gpio, tHalGpioPull iPull)
{
    if (ui32Gpio > CONFIG_GPIO_PIN_CNT)
        return -EINVAL;

    switch (iPull)
    {
        case HalGpioPullUp:
        {
            EtaCspGpioPullBitEnable(ui32Gpio);
            EtaCspGpioPullUpBitSet(ui32Gpio);
            break;
        }
        case HalGpioPullDown:
        {
            EtaCspGpioPullBitEnable(ui32Gpio);
            EtaCspGpioPullDownBitSet(ui32Gpio);
            break;
        }
        case HalGpioPullNone:
        default:
            EtaCspGpioPullBitDisable(ui32Gpio);
            break;
    }
    return (0);

}

/*
 * GPIO In Init
 *
 * Initialize GPIO as input pin, with required pull type
 *
 * @param ui32Pin GPIO number to be initialize
 * @param iPull pull type to be configure for input pin
 *
 * @return int  0: no error; -1 otherwise.
 */
int32_t HalGpioInInit(uint32_t ui32Gpio, tHalGpioPull iPull)
{
    if (ui32Gpio > CONFIG_GPIO_PIN_CNT)
        return -EINVAL;

    EtaCspGpioInputEnableBitSet(ui32Gpio);
    HalGpioPull(ui32Gpio, iPull);
    return 0;
}


/*
 * GPIO Out Init
 *
 * Initialize GPIO as Output pin, with initial value
 *
 * @param gpio pin
 * @param gpio pullup state
 *
 * @return int  0: no error; -1 otherwise.
 */
int32_t HalGpioOutInit(uint32_t ui32Gpio, bool bVal)
{
    if (ui32Gpio > CONFIG_GPIO_PIN_CNT)
        return -EINVAL;

    HalGpioWrite(ui32Gpio, bVal);
    EtaCspGpioOutputEnableBitSet(ui32Gpio);
    return (0);
}
/*
 * GPIO write
 *
 * Set GPIO pin state
 *
 * @param gpio pin
 *
 * @param gpio value
 *
 */
void HalGpioWrite(uint32_t ui32Gpio, bool bVal)
{
    if (ui32Gpio > CONFIG_GPIO_PIN_CNT)
        return;
    if(bVal)
    {
#ifdef CONFIG_ECM3532
        EtaCspGpioOutputBitSet(ui32Gpio);
#else
        EtaCspGpioBitSet(ui32Gpio);
#endif
    }
    else
    {
#ifdef CONFIG_ECM3532
        EtaCspGpioOutputBitClear(ui32Gpio);
#else
        EtaCspGpioBitClear(ui32Gpio);
#endif
    }
}

/**
 * GPIO read
 *
 * Read pad value of input pin
 *
 * @param gpio pin
 *
 * @return 1 or 0 based on pin status
 */
int32_t HalGpioRead(uint32_t ui32Gpio)
{
    if (ui32Gpio > CONFIG_GPIO_PIN_CNT)
        return -EINVAL;
#if CONFIG_ECM3532
    return EtaCspGpioInputBitGet(ui32Gpio);
#else
    return EtaCspGpioBitGet(ui32Gpio);
#endif
}

/**
 *
 * Toggle a GPIO pin.
 *
 * @param gpio pin
 *
 * @return status sucess or error
 */
void
HalGpioToggle(uint32_t ui32Gpio)
{
    if (ui32Gpio > CONFIG_GPIO_PIN_CNT)
        return;
#if CONFIG_ECM3532
    EtaCspGpioOutputBitToggle(ui32Gpio);
#else
    EtaCspGpioBitToggle(ui32Gpio);
#endif
}

/*
 * GPIO int enable
 *
 * Enable GPIO A interrupt
 *
 * @param iGpioIrq GPIO IP hase two IRQ, GPIO IRQ0 and IRQ1, map to any 1
 * @param ui32GPIO0 gpio pin
 *
 * @return status success or error
 */
void
HalGpioIntEnable(tHalGpioIRQ iGpioIrq, uint32_t ui32Gpio)
{
    if (ui32Gpio > CONFIG_GPIO_PIN_CNT)
        return;
    if (!iGpioIrq)
        EtaCspGpioAIntEnable(ui32Gpio);
    else
        EtaCspGpioBIntEnable(ui32Gpio);
}

/*
 * GPIO int disable
 *
 * Disable GPIO A interrupt
 *
 * @param iGpioIrq GPIO IP hase two IRQ, GPIO IRQ0 and IRQ1, map to any 1
 * @param ui32Gpio gpio pin
 *
 */
void
HalGpioIntDisable(tHalGpioIRQ iGpioIrq, uint32_t ui32Gpio)
{
    if (ui32Gpio > CONFIG_GPIO_PIN_CNT)
        return;
    if (!iGpioIrq)
        EtaCspGpioAIntDisable(ui32Gpio);
    else
        EtaCspGpioBIntDisable(ui32Gpio);
}

/*
 * GPIO int status
 *
 * Read interrupt A register
 *
 * @param iGpioIrq GPIO IP hase two IRQ, GPIO IRQ0 and IRQ1, map to any 1
 * @return interrupt A status register value
 *
 */
uint32_t
HalGpioIntStatusGet(tHalGpioIRQ iGpioIrq)
{
    if (!iGpioIrq)
        return EtaCspGpioAIntStatusGet();
    else
        return EtaCspGpioBIntStatusGet();
}

/*
 * GPIO int clear
 *
 * Clear gpio interrupt
 *
 * @param iGpioIrq GPIO IP hase two IRQ, GPIO IRQ0 and IRQ1, map to any 1
 * @param ui32Gpio interrupt status
 */
void
HalGpioIntClear(tHalGpioIRQ iGpioIrq, uint32_t ui32Gpio)
{
    if (!iGpioIrq)
        EtaCspGpioAIntClear(ui32Gpio);
    else
        EtaCspGpioBIntClear(ui32Gpio);
}

/*
 * GPIO interrupt polarity
 *
 * Set interrupt polarity as Active High or Low.
 *
 * @param iGpioIrq GPIO IP hase two IRQ, GPIO IRQ0 and IRQ1, map to any 1
 * @param gpio pin
 *
 * @param interrupt polarity
 */
static int
HalGpioSetPolarity(tHalGpioIRQ iGpioIrq, uint32_t ui32Gpio, tHalGpioIntTrig Polarity)
{
    int ret = 0;

    switch (Polarity) {
        case HalGpioTrigLow:
        if (!iGpioIrq)
            EtaCspGpioAIntPolarityActiveLow(ui32Gpio);
        else
            EtaCspGpioBIntPolarityActiveLow(ui32Gpio);
        break;
    case HalGpioTrigHigh:
        if (!iGpioIrq)
            EtaCspGpioAIntPolarityActiveHigh(ui32Gpio);
        else
            EtaCspGpioBIntPolarityActiveHigh(ui32Gpio);
        break;
    default:
        break;
    }
    return ret;
}

/*
 * GPIO A irq handler
 *
 * Handles the gpio interrupt attached to a gpio pin.
 *
 */
void GPIO0_ISR(void)
{
    const struct tHalGpioInt *sIrq;
    uint32_t ui32St, ui32I;

    /* Read and clear the GPIO interrupt status. */
    ui32St = HalGpioIntStatusGet(0);
    for (ui32I = 0; ui32I < CONFIG_GPIO_MAX_IRQ; ui32I++) {
        sIrq = sHalGpioIntAArr + ui32I;
        if (sIrq->fHandler != NULL) {
            if (ui32St & HalGpioBIT(sIrq->ui32Pin)) {
                HalGpioIntClear(0, sIrq->ui32Pin);
                sIrq->fHandler(sIrq->vArg);
            }
        }
    }
}

/*
 * GPIO B irq handler
 *
 * Handles the gpio interrupt attached to a gpio pin.
 *
 * @param index
 */
void GPIO1_ISR(void)
{
    const struct tHalGpioInt *sIrq;
    uint32_t ui32St, ui32I;

    /* Read and clear the GPIO interrupt status. */
    ui32St = HalGpioIntStatusGet(1);
    for (ui32I = 0; ui32I < CONFIG_GPIO_MAX_IRQ; ui32I++) {
        sIrq = sHalGpioIntBArr + ui32I;
        if (sIrq->fHandler != NULL) {
            if (ui32St & HalGpioBIT(sIrq->ui32Pin)) {
                HalGpioIntClear(1, sIrq->ui32Pin);
                sIrq->fHandler(sIrq->vArg);
            }
        }
    }

}

/*
 * Register IRQ handler for GPIOTE, and enable it.
 * Only executed once, during first registration.
 */
static void
HalGpioIntSetup(tHalGpioIRQ iGpioIrq)
{
    static uint8_t ui8IrqASetup;
    static uint8_t ui8IrqBSetup;
    if (!iGpioIrq & (!ui8IrqASetup))
    {
        NVIC_SetPriority(GPIO0_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
        NVIC_ClearPendingIRQ(GPIO0_IRQn);
        NVIC_EnableIRQ(GPIO0_IRQn);
        ui8IrqASetup = 1;
    }
    if (iGpioIrq & (!ui8IrqBSetup))
    {
        NVIC_SetPriority(GPIO1_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
        NVIC_ClearPendingIRQ(GPIO1_IRQn);
        NVIC_EnableIRQ(GPIO1_IRQn);
        ui8IrqBSetup = 1;
    }

}

/*
 * Find out whether we have an GPIO pin to use.
 */
static int
HalGpioGetEmptySlot(tHalGpioIRQ iGpioIrq)
{
    int i32I;
    struct tHalGpioInt *sIntArr;

    if (!iGpioIrq)
        sIntArr = sHalGpioIntAArr;
    else
        sIntArr = sHalGpioIntBArr;


    for (i32I = 0; i32I < CONFIG_GPIO_MAX_IRQ; i32I++) {
        if (sIntArr[i32I].fHandler == NULL) {
            return i32I;
        }
    }
    return -1;
}

/**
 * gpio irq init
 *
 * Initialize an external interrupt on a gpio pin
 *
 * @param pin       Pin number to enable gpio.
 * @param handler   Interrupt handler
 * @param pArg       Argument to pass to interrupt handler
 * @param trig      Trigger mode of interrupt
 * @param pull      Push/pull mode of input.
 *
 * @return int
 */
int
HalGpioIntInit(uint32_t ui32Gpio, tHalGpioIRQ iGpioIrq, tHalGpioIntHandler fIntHandler, void *vArg,
                      tHalGpioIntTrig iTrig, tHalGpioPull iPull)
{
    int32_t i32Slot;
    struct tHalGpioInt *sIntArr;
    if (ui32Gpio > CONFIG_GPIO_PIN_CNT)
        return -EINVAL;
    i32Slot = HalGpioGetEmptySlot(iGpioIrq);
    if (i32Slot < 0) {
        return -ENOMEM;
    }
    if (!iGpioIrq)
        sIntArr = sHalGpioIntAArr;
    else
        sIntArr = sHalGpioIntBArr;

    HalGpioInInit(ui32Gpio, iPull);

    HalGpioSetPolarity(iGpioIrq, ui32Gpio, iTrig);

    sIntArr[i32Slot].ui32Pin = ui32Gpio;
    sIntArr[i32Slot].fHandler = fIntHandler;
    sIntArr[i32Slot].vArg = vArg;

    HalGpioIntSetup(iGpioIrq);
    HalGpioIntClear(iGpioIrq, ui32Gpio);
    HalGpioIntEnable(iGpioIrq, ui32Gpio);
    return 0;
}

/**
 * gpio irq release
 *
 * No longer interrupt when something occurs on the pin. NOTE: this function
 * does not change the GPIO push/pull setting.
 * It also does not disable the NVIC interrupt enable setting for the irq.
 *
 * @param gpio pin
 */
void
HalGpioIntRelease(uint32_t ui32Gpio)
{
    /* XXX: Unimplemented. */
}
