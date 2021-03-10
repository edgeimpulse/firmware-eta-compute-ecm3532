/*
 * FreeRTOS Kernel V10.2.1
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the ARM CM3 port.
 *----------------------------------------------------------*/

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "ecm3532.h"
#include "cm3.h"
#include "gpio_hal.h"
#include "pm.h"
#include "portmacro.h"
#include "reg_eta_ecm3532_m3.h"
#include "eta_csp_socctrl.h"
#include "eta_csp_rtc.h"
/* 24 bit RTC timer */
#define MAX_SLEEP_TICKS 0xFFFFFF
#define STALL_OFFSET (12)

#define configSYSTICK_CLOCK_HZ configCPU_CLOCK_HZ
/* Ensure the SysTick is clocked at the same frequency as the core. */
#define portNVIC_SYSTICK_CLK_BIT	( 1UL << 2UL )

/* Constants required to manipulate the core.  Registers first... */
#define portNVIC_SYSTICK_CTRL_REG			( * ( ( volatile uint32_t * ) 0xe000e010 ) )
#define portNVIC_SYSTICK_LOAD_REG			( * ( ( volatile uint32_t * ) 0xe000e014 ) )
#define portNVIC_SYSTICK_CURRENT_VALUE_REG	( * ( ( volatile uint32_t * ) 0xe000e018 ) )
/* ...then bits in the registers. */
#define portNVIC_SYSTICK_INT_BIT			( 1UL << 1UL )
#define portNVIC_SYSTICK_ENABLE_BIT			( 1UL << 0UL )
#define portNVIC_SYSTICK_COUNT_FLAG_BIT		( 1UL << 16UL )
#define portNVIC_PENDSVCLEAR_BIT 			( 1UL << 27UL )
#define portNVIC_PEND_SYSTICK_CLEAR_BIT		( 1UL << 25UL )

/*
 * RTM timer interrupt
 * for Sleep timer
 */
#define ECM3532_OS_TICK_IRQ   SysTick_IRQn
#define ECM3532_SLEEP_TICK_IRQ RTC_IRQn
/* Default systick clock is CONFIG_SYSTICK_REF_CLOCK */
volatile uint8_t lpm_en = 1;

/*
 * Setup the timer to generate the tick interrupts.  The implementation in this
 * file is weak to allow application writers to change the timer used to
 * generate the tick interrupt.
 */
void vPortSetupTimerInterrupt( void );

/*
 * Exception handlers.
 */
void SysTick_Handler(void);

/*
 * The number of SysTick increments that make up one tick period.
 */
static uint32_t ulTimerCountsForOneTick;

/*
 * RTM interrupt handler
 *
 * sleep timer isr
 *
 */
void RTC_TMR_ISR(void)
{
    NVIC_ClearPendingIRQ(RTC_IRQn);
}

void SysTick_Handler(void)
{

	/* The SysTick runs at the lowest interrupt priority, so when this interrupt
	executes all interrupts must be unmasked.  There is therefore no need to
	save and then restore the interrupt mask value as its value is already
	known. */
	portDISABLE_INTERRUPTS();
	{
        /* set the count value back to zero */
        portNVIC_SYSTICK_CURRENT_VALUE_REG = 0UL;
#ifdef CONFIG_SYSTICK_ON_CPU_CLK
        ulTimerCountsForOneTick = (EtaCspSocCtrlM3FrequencyGet() / configTICK_RATE_HZ) - 1;
#endif
        /* Set the new reload value. */
        portNVIC_SYSTICK_LOAD_REG = ulTimerCountsForOneTick;

        /* Restart SysTick. */
        portNVIC_SYSTICK_CTRL_REG |= portNVIC_SYSTICK_ENABLE_BIT;
		/* Increment the RTOS tick. */
		if( xTaskIncrementTick() != pdFALSE )
		{
			/* A context switch is required.  Context switching is performed in
			the PendSV interrupt.  Pend the PendSV interrupt. */
			portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT;
		}
	}
	portENABLE_INTERRUPTS();
}

#if( configUSE_TICKLESS_IDLE == 1 )
/*
 * get elapse ticks
 *
 * @param programmed number of ticks
 *
 * @return ticks elapsed since ticks programmed
 */
static TickType_t
ecm3531_get_rtc_elapse_ticks(TickType_t ticks)
{
    TickType_t cnt;

    cnt  = REG_RTC_TMR.BF.COUNT;
    if (!cnt) {
        cnt = ticks;
    } else {
        cnt = ticks - cnt;
    }
    return cnt;
}

void vPortSuppressTicksAndSleep(TickType_t xExpectedIdleTime)
{
    TickType_t cnt;
    int ret = 0, abort = 0;

    /* convert systick to msec */
    xExpectedIdleTime = (xExpectedIdleTime * 1000) / configTICK_RATE_HZ;

    /* Stop the SysTick momentarily.  The time the SysTick is stopped for
       is accounted for as best it can be, but using the tickless mode will
       inevitably result in some tiny drift of the time maintained by the
       kernel with respect to calendar time. */
    portNVIC_SYSTICK_CTRL_REG &= ~portNVIC_SYSTICK_ENABLE_BIT;

    /* Enter a critical section but don't use the taskENTER_CRITICAL()
       method as that will mask interrupts that should exit sleep mode. */
    __asm volatile( "cpsid i" ::: "memory" );
    __asm volatile( "dsb" );
    __asm volatile( "isb" );

    /* Make sure the SysTick reload value does not overflow the counter. */
    if( xExpectedIdleTime > MAX_SLEEP_TICKS)
    {
        xExpectedIdleTime = MAX_SLEEP_TICKS;
    }

    ret = ecm35xx_pre_lpm(xExpectedIdleTime);
    if (ret < 0)
        abort = 1;
    else if( eTaskConfirmSleepModeStatus() == eAbortSleep )
        abort = 1;

      /* If a context switch is pending or a task is waiting for the scheduler
       to be unsuspended then abandon the low power entry. */
    if(abort)
    {
        /* Restart from whatever is left in the count register to complete
           this tick period. */
        portNVIC_SYSTICK_CURRENT_VALUE_REG = 0UL;
#ifdef CONFIG_SYSTICK_ON_CPU_CLK
        ulTimerCountsForOneTick = (EtaCspSocCtrlM3FrequencyGet() / configTICK_RATE_HZ) - 1;
#endif
        portNVIC_SYSTICK_LOAD_REG = ulTimerCountsForOneTick;
        /* Restart SysTick. */
        portNVIC_SYSTICK_CTRL_REG |= portNVIC_SYSTICK_ENABLE_BIT;

        /* Re-enable interrupts - see comments above the cpsid instruction()
           above. */
        __asm volatile( "cpsie i" ::: "memory" );
    }
#ifdef CONFIG_PM_WFI_ONLY
    else
    {
#else
    else if (xExpectedIdleTime < STALL_OFFSET)
    {
#endif

    /* if Idle time is less then overhead in proceding to stall
     * we will not save any thing, so just do wfi in that case
     */
        portNVIC_SYSTICK_CURRENT_VALUE_REG = 0UL;
#ifdef CONFIG_SYSTICK_ON_CPU_CLK
        ulTimerCountsForOneTick = (EtaCspSocCtrlM3FrequencyGet() / configTICK_RATE_HZ) - 1;
        portNVIC_SYSTICK_LOAD_REG = (xExpectedIdleTime * ulTimerCountsForOneTick) - 1;
#else
        portNVIC_SYSTICK_LOAD_REG = (xExpectedIdleTime - 1);
#endif
        /* Restart SysTick. */
        portNVIC_SYSTICK_CTRL_REG |= portNVIC_SYSTICK_ENABLE_BIT;
#ifdef CONFIG_LPM_LED
        HalGpioWrite(CONFIG_LPM_LED_GPIO, 0);
#endif
        __asm("wfi");
#ifdef CONFIG_LPM_LED
        HalGpioWrite(CONFIG_LPM_LED_GPIO, 1);
#endif
        portNVIC_SYSTICK_CTRL_REG &= ~portNVIC_SYSTICK_ENABLE_BIT;
        cnt = portNVIC_SYSTICK_CURRENT_VALUE_REG;
        if (!cnt)
            cnt = xExpectedIdleTime - 1;
        else
            cnt = xExpectedIdleTime - cnt;

        ecm35xx_post_lpm(cnt);
        /* convert timer ticks to systick count */
        cnt  = ((cnt * configTICK_RATE_HZ) / 1000);

#ifdef CONFIG_SYSTICK_ON_CPU_CLK
        ulTimerCountsForOneTick = (EtaCspSocCtrlM3FrequencyGet() / configTICK_RATE_HZ) - 1;
#endif
        /* reenable periodic systick */
        portNVIC_SYSTICK_LOAD_REG = ulTimerCountsForOneTick;
        portNVIC_SYSTICK_CTRL_REG |= portNVIC_SYSTICK_ENABLE_BIT;

        vTaskStepTick(cnt);
        __asm volatile( "cpsie i" ::: "memory" );

    }

#ifndef CONFIG_PM_WFI_ONLY
    else
    {
        /* Idle time is long enough to get saving from stall */
        EtaCspRtcTmrTimedRun(xExpectedIdleTime);
        ecm35xx_enter_lpm(xExpectedIdleTime);
        EtaCspRtcTmrIntDisable();
        cnt = ecm3531_get_rtc_elapse_ticks(xExpectedIdleTime);
        ecm35xx_post_lpm(cnt);
        /* convert timer ticks to systick count */
        cnt = (cnt * configTICK_RATE_HZ) / 1000;
        if (cnt)
        {
            vTaskStepTick(cnt);
        }

        /* Restart SysTick so it runs from portNVIC_SYSTICK_LOAD_REG
           again, then set portNVIC_SYSTICK_LOAD_REG back to its standard
           value. */
        portNVIC_SYSTICK_CURRENT_VALUE_REG = 0UL;
#ifdef CONFIG_SYSTICK_ON_CPU_CLK
        ulTimerCountsForOneTick = (EtaCspSocCtrlM3FrequencyGet() / configTICK_RATE_HZ) - 1;
#endif
        portNVIC_SYSTICK_LOAD_REG = ulTimerCountsForOneTick;
        portNVIC_SYSTICK_CTRL_REG |= portNVIC_SYSTICK_ENABLE_BIT;

        /* Exit with interrpts enabled. */
        __asm volatile( "cpsie i" ::: "memory" );
    }
#endif

}

#endif /* configUSE_TICKLESS_IDLE */

/*
 * Setup the systick timer to generate the tick interrupts at the required
 * frequency.
 */
void vPortSetupTimerInterrupt( void )
{
    int prio = OS_TICK_PRIO;
    /* Calculate the constants required to configure the tick interrupt. */
#ifdef CONFIG_SYSTICK_ON_CPU_CLK
    ulTimerCountsForOneTick = (EtaCspSocCtrlM3FrequencyGet() / configTICK_RATE_HZ) - 1;
#else
    ulTimerCountsForOneTick = (CONFIG_SYSTICK_REF_CLOCK / configTICK_RATE_HZ) - 1;
#endif
    /* Stop and clear the SysTick. */
    portNVIC_SYSTICK_CTRL_REG = 0UL;
    portNVIC_SYSTICK_CURRENT_VALUE_REG = 0UL;
    NVIC_SetPriority(ECM3532_OS_TICK_IRQ, prio);
    /* Configure SysTick to interrupt at the requested rate. */
    portNVIC_SYSTICK_LOAD_REG = ulTimerCountsForOneTick;
    portNVIC_SYSTICK_CTRL_REG =
#ifdef CONFIG_SYSTICK_ON_CPU_CLK
                                portNVIC_SYSTICK_CLK_BIT |
#endif
                                portNVIC_SYSTICK_INT_BIT |
                                portNVIC_SYSTICK_ENABLE_BIT;
}
