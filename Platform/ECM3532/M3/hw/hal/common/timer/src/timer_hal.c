/*******************************************************************************
*
* @file timer_hal.c
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
#include <string.h>
#include "eta_csp_timer.h"
#include "timer_hal.h"
#include "queue_helper.h"
#include "errno.h"
#ifdef CONFIG_ECM3531
#include "ecm3531.h"
#elif CONFIG_ECM3532
#include "ecm3532.h"
#endif
#include "cm3.h"
#include "FreeRTOS.h"
#include "semphr.h"



#define HAL_TIMER_FREQ_HZ 1000
/*
 * we have two timer irq, channel can map to same irq
 */
static uint8_t ui8TmrIrqInit[2] = {0, 0};

/**
 * * Internal platform timer structure
 */
struct sHalTmr {
    tHalTmrType iTmrTyp;
    uint32_t ui32TmrPrd;
    uint32_t ui32StartTime;
    uint32_t ui32Expiry;
    tHalTmrState iTmrSt;
    tHalTmrCb fTmrCb;
    void *vCbArg;
    struct sDevTmr *sDev;
    /* Queue sLinked list structure */
    TAILQ_ENTRY(sHalTmr) sLink;
};

/**
 * * Internal platform timer structure
 */
struct sDevTmr {
    /* Index of timer */
    tTimerInterrupts iChanIdx;
    /* Timer IRQ idx */
    tTimerIrqNumber iIrqIdx;
    uint32_t ui32AtExpiry;
    uint32_t ui32LastExpiry;
    uint32_t ui32NextExpiry;
    /* Timer Q */
    TAILQ_HEAD(HalTimerQHead, sHalTmr) sHalTmrQ;
};

#ifdef CONFIG_TMR_CH_0
static struct sDevTmr sDevTmr_0 = {
    .sHalTmrQ = TAILQ_HEAD_INITIALIZER(sDevTmr_0.sHalTmrQ),
    /* use CMPA */
    .iChanIdx = eTimerIntCompareA,
    /* timer 0 TIMER0_IRQn*/
    .iIrqIdx = eTimer0,
};
#endif
#ifdef CONFIG_TMR_CH_1
static struct sDevTmr sDevTmr_1 = {
    .sHalTmrQ = TAILQ_HEAD_INITIALIZER(sDevTmr_1.sHalTmrQ),
    /* use CMPB */
    .iChanIdx = eTimerIntCompareB,
    /* timer 1 TIMER0_IRQn*/
    .iIrqIdx = eTimer0,
};
#endif
#ifdef CONFIG_TMR_CH_2
static struct sDevTmr sDevTmr_2 = {
    .sHalTmrQ = TAILQ_HEAD_INITIALIZER(sDevTmr_2.sHalTmrQ),
    /* use CMPC */
    .iChanIdx = eTimerIntCompareC,
    /* timer 2 TIMER0_IRQn*/
    .iIrqIdx = eTimer0,
};
#endif
/* Reserved for Power Management */
#ifdef CONFIG_PM_ENABLE
static struct sDevTmr sDevTmr_3 = {
    .sHalTmrQ = TAILQ_HEAD_INITIALIZER(sDevTmr_3.sHalTmrQ),
    /* use CMPD */
    .iChanIdx = eTimerIntCompareD,
    /* timer 3 TIMER1_IRQn*/
    .iIrqIdx = eTimer1,
};
#endif

static struct sDevTmr *
TmrGetDev(uint32_t ui32TimerNum)
{
    switch (ui32TimerNum)
    {
#ifdef CONFIG_TMR_CH_0
        case 0:
        return &sDevTmr_0;
#endif
#ifdef CONFIG_TMR_CH_1
        case 1:
        return &sDevTmr_1;
#endif
#ifdef CONFIG_TMR_CH_2
        case 2:
        return &sDevTmr_2;
#endif

#ifdef CONFIG_PM_ENABLE
        case 3:
        return &sDevTmr_3;
#endif
        default:
        return NULL;
    }
}

/**
 * Retrieves the current time from the  timer.
 */
static uint32_t ECM3531TimerCurrTicks(const struct sDevTmr *sDev)
{
    return ((uint32_t)EtaCspTimerCountGetMs());
}

/**
 * Configures a BSP timer to generate an interrupt at the speficied absolute
 * timer ticks.
 */
static int ECM3531TimerSetCmpAt(struct sDevTmr *sDev, uint32_t ui32AtTick)
{
    uint32_t ui32Ticknow;

    ui32Ticknow = ECM3531TimerCurrTicks(sDev);
    if (ui32AtTick <= ui32Ticknow)
    {
        return -1;
    }
    else
    {
        EtaCspTimerCmpSet(ui32AtTick, sDev->iChanIdx);
        EtaCspTimerIntEnableSet(sDev->iIrqIdx, sDev->iChanIdx);
        sDev->ui32LastExpiry = sDev->ui32NextExpiry;
        sDev->ui32NextExpiry = ui32AtTick;
    }

    return 0;
}

/**
 * Unsets a scheduled interrupt for the specified BSP timer.
 */
static void ECM3531TimerClearCmp(const struct sDevTmr *sDev)
{
    EtaCspTimerIntClear(sDev->iChanIdx);
    EtaCspTimerIntEnableClear(sDev->iIrqIdx, sDev->iChanIdx);
}

/**
 * Executes callbacks for all expired timers in a BSP timer's queue.  This
 * function is called when a timer interrupt is handled.
 */
static void ECM3531TimerChkQ(struct sDevTmr *sDev)
{
    tHalTmr *sHalTmr;
    uint32_t ui32Ticks;
    UBaseType_t uxSavedInterruptStatus;
    uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();

    /*
     * Remove and process each expired timer in the sorted queue
     */
    while ((sHalTmr = TAILQ_FIRST(&sDev->sHalTmrQ)) != NULL)
    {
        ui32Ticks = ECM3531TimerCurrTicks(sDev);
        sDev->ui32AtExpiry = ui32Ticks;
        if ((int32_t)(ui32Ticks - sHalTmr->ui32Expiry) >= 0)
        {
            TAILQ_REMOVE(&sDev->sHalTmrQ, sHalTmr, sLink);
            sHalTmr->sLink.tqe_prev = NULL;
            sHalTmr->fTmrCb(sHalTmr->vCbArg);
            sHalTmr->iTmrSt = HalTmrExpired;
            if(sHalTmr->iTmrTyp == HalTmrPeriodic)
                HalTmrStart(sHalTmr);
        }
        else
        {
            break;
        }
    }

    /*
     * If any timers remain, schedule an interrupt
     * for the timer that expires next.
     */
    sHalTmr = TAILQ_FIRST(&sDev->sHalTmrQ);
    if (sHalTmr != NULL)
    {
        if (!ECM3531TimerSetCmpAt(sDev, sHalTmr->ui32Expiry))
        {
            sDev->ui32NextExpiry = sHalTmr->ui32Expiry;
        }
        else
        {
            taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStatus);
            ECM3531TimerChkQ(sDev);
            return;
        }
    }

    taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStatus);
}

/**
 * Handles a ctimer interrupt.
 */
void ECM3531TimerIsr(void)
{
    /* Service the appropriate timers. */
#ifdef CONFIG_TMR_CH_0
    if(REG_TIMER_INT_STATUS.BF.CMPA)
    {
        ECM3531TimerClearCmp(&sDevTmr_0);
        ECM3531TimerChkQ(&sDevTmr_0);
    }
#endif
#ifdef CONFIG_TMR_CH_1
    if(REG_TIMER_INT_STATUS.BF.CMPB)
    {
        ECM3531TimerClearCmp(&sDevTmr_1);
        ECM3531TimerChkQ(&sDevTmr_1);
    }
#endif
#ifdef CONFIG_TMR_CH_2
    if(REG_TIMER_INT_STATUS.BF.CMPC)
    {
        ECM3531TimerClearCmp(&sDevTmr_2);
        ECM3531TimerChkQ(&sDevTmr_2);
    }

#endif
#ifdef CONFIG_PM_ENABLE
    if(REG_TIMER_INT_STATUS.BF.CMPD)
    {
        ECM3531TimerClearCmp(&sDevTmr_3);
        ECM3531TimerChkQ(&sDevTmr_3);
    }

#endif

}

void TIMER0_ISR(void)
{
    ECM3531TimerIsr();
}
void TIMER1_ISR(void)
{
    ECM3531TimerIsr();
}
/**
 * hal timer resolution
 *
 * Get the resolution of the timer
 *
 * @param iTmrCh timer channel
 *
 * @return uint32_t
 */
uint32_t HalTimerResolution(tHalTmrCh iTmrCh)
{
    const struct sDevTmr *sDev;
    sDev = TmrGetDev(iTmrCh);
    if (sDev == NULL) {
        return 0;
    }
   return 1000000000 / HAL_TIMER_FREQ_HZ;
}

/**
 * Reads the absolute time from the specified continuous timer.
 *
 * @param iTmrCh timer channel
 *
 * @return uint32_t The timer counter register.
 */
uint32_t HalTmrRead(tHalTmrCh iTmrCh)
{
    const struct sDevTmr *sDev;
    sDev = TmrGetDev(iTmrCh);
    if (sDev == NULL) {
        return 0;
    }
    return ECM3531TimerCurrTicks(sDev);
}

/**
 * Get State of Timer
 *
 * @param iTmrCh timer channel
 *
 * @return timer state.
 */
tHalTmrState HalTmrGetState(tHalTmr *sHalTmr)
{
    if (!sHalTmr || !sHalTmr->sDev)
        return HalTmrDeleted;
    return sHalTmr->iTmrSt;
}
/**
 * Blocking delay for n ticks
 *
 * @param iTmrCh timer channel
 *
 * @return int 0 on success; error code otherwise.
 */
int32_t HalTmrDelay(tHalTmrCh iTmrCh, uint32_t ui32Tick)
{
    const struct sDevTmr *sDev;
    uint32_t ui32Until;

    sDev = TmrGetDev(iTmrCh);
    if (sDev == NULL) {
        return -EINVAL;
    }

    ui32Until = ECM3531TimerCurrTicks(sDev) + ui32Tick;
    while ((int32_t)(ECM3531TimerCurrTicks(sDev) - ui32Until) <= 0);
    return 0;
}

/**
 * Start a timer. Timer fires after its period
 * configured with create call
 *
 * @param sHalTmr timer handle
 * @param tick
 *
 * @return int
 */
int32_t HalTmrStart(tHalTmr *sHalTmr)
{
    tHalTmr *sCurrTmr;
    struct sDevTmr *sDev;
    uint32_t ui32Ticks;
    int32_t i32Ret = 0;
    UBaseType_t uxSavedInterruptStatus;

    sDev = sHalTmr->sDev;
    if (!sHalTmr || !sDev)
        return -EINVAL;

    uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();
    ui32Ticks = ECM3531TimerCurrTicks(sDev);
    sHalTmr->ui32Expiry =  sHalTmr->ui32TmrPrd + ui32Ticks;
    sHalTmr->ui32StartTime = ui32Ticks;

    if (TAILQ_EMPTY(&sDev->sHalTmrQ))
    {
        TAILQ_INSERT_HEAD(&sDev->sHalTmrQ, sHalTmr, sLink);
    }
    else
    {
        TAILQ_FOREACH(sCurrTmr, &sDev->sHalTmrQ, sLink)
        {
            if ((int32_t)(sHalTmr->ui32Expiry - sCurrTmr->ui32Expiry) < 0)
            {
                TAILQ_INSERT_BEFORE(sCurrTmr, sHalTmr, sLink);
                break;
            }
        }
        if (sCurrTmr == NULL)
        {
            TAILQ_INSERT_TAIL(&sDev->sHalTmrQ, sHalTmr, sLink);
        }
    }

    if (sHalTmr == TAILQ_FIRST(&sDev->sHalTmrQ))
    {
        i32Ret = ECM3531TimerSetCmpAt(sDev, sHalTmr->ui32Expiry);
        if (i32Ret >= 0)
        {
            sHalTmr->iTmrSt = HalTmrRunning;
        }
        else
        {
            ECM3531TimerChkQ(sDev);
        }

    }
    taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStatus);

    return 0;
}

/**
 * Change period of timer
 *
 * @param sHalTmr timer handle
 * @param ui32TmrPrd new timer period
 *
 * @return int
 */

int32_t HalTmrChangePeriod(tHalTmr *sHalTmr, uint32_t ui32TmrPrd)
{
    UBaseType_t uxSavedInterruptStatus;
    if (!sHalTmr || !sHalTmr->sDev)
        return -EINVAL;

    uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();
    sHalTmr->ui32TmrPrd = ui32TmrPrd;
    taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStatus);
    return 0;
}

/**
 * HALTimerStop()
 *
 * Cancels the timer.
 *
 * @param sHalTmr timer handle
 *
 * @return int
 */
int32_t HalTmrStop(tHalTmr *sHalTmr)
{
    struct sDevTmr *sDev;
    uint32_t ui32RestCmp = 0;
    tHalTmr *sHalTmrNext;
    UBaseType_t uxSavedInterruptStatus;

    if (!sHalTmr || !sHalTmr->sDev)
        return -EINVAL;

    uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();
    sDev = sHalTmr->sDev;

    /*
     * If timer's prev pointer is null,
     * the timer hasn't been started
     */

    if (sHalTmr->sLink.tqe_prev == NULL)
    {
        taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStatus);
        return 0;
    }

    if (sHalTmr == TAILQ_FIRST(&sDev->sHalTmrQ)) {
        /* If first on queue, we will need to reset OCMP */
        ui32RestCmp = 1;
    }

    TAILQ_REMOVE(&sDev->sHalTmrQ, sHalTmr, sLink);

    sHalTmr->iTmrSt = HalTmrStopped;
    sHalTmr->sLink.tqe_prev = NULL;

    if (ui32RestCmp)
    {
        sHalTmrNext = TAILQ_FIRST(&sDev->sHalTmrQ);
        if (sHalTmrNext != NULL)
        {
            if(ECM3531TimerSetCmpAt(sDev, sHalTmrNext->ui32Expiry))
                ECM3531TimerChkQ(sDev);
        }
        else
        {
            ECM3531TimerClearCmp(sDev);
        }
    }
    taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStatus);
    return 0;
}

/**
 * Create new Timer instance, timer can be oneshort or periodic, base on
 * iTmrType value, period of timer will be ui32TmrPrd.
 * Create API return Handle of newly created timer, that need to be pass
 * to start API
 *
 *
 * @param iTmrCh timer channel
 * @param iTmrType timer type one short or periodic
 * @param ui32TmrPrd Period of new created timer
 * @param fCbFunc timer callback function, will be called after tiemr expiry
 * @param vArg agrument of callback function to be called with
 *
 * @return on sucess tHalTmrHandle handle of newly create timer, NULL on error
 */
tHalTmr * HalTmrCreate(tHalTmrCh iTmrCh, tHalTmrType iTmrType,
                            uint32_t ui32TmrPrd,
                            tHalTmrCb fCbFun, void *vArg)
{
    struct sDevTmr *sDev;
    tHalTmr *sHalNewTmr;

    sDev= TmrGetDev(iTmrCh);
    if(sDev == NULL)
        return NULL;

    sHalNewTmr =
        (tHalTmr *)pvPortMalloc(sizeof(tHalTmr));
    if(sHalNewTmr == NULL)
        return NULL;
    memset(sHalNewTmr,0,sizeof(tHalTmr));
    sHalNewTmr->iTmrTyp = iTmrType;
    sHalNewTmr->ui32TmrPrd = ui32TmrPrd;
    sHalNewTmr->fTmrCb = fCbFun;
    sHalNewTmr->vCbArg = vArg;
    sHalNewTmr->sDev = sDev;
    sHalNewTmr->sLink.tqe_prev = NULL;
    return sHalNewTmr;
}


/**
 * hal timer delete
 *
 * remove the time created earlier
 *
 * @param sHalTmr timer handle
 *
 * @return int
 */
int32_t HalTmrDelete(tHalTmr *sHalTmr)
{
	taskENTER_CRITICAL();
    HalTmrStop(sHalTmr);
    vPortFree(sHalTmr);
	taskEXIT_CRITICAL();
    return 0;
}

/**
 * hal timer init
 *
 * Initialize platform specific timer items
 *
 * @param iTmrChan HW timer comparator channel
 *
 * @return 0 on success; error code otherwise
 */
 __attribute__((section(".initSection")))int32_t HalTmrChInit(tHalTmrCh iTmrChan)
{
    struct sDevTmr *sDev;
    static uint8_t ui8TmrHwInit;

    if (!ui8TmrHwInit)
    {
        EtaCspTimerInitMs();
        ui8TmrHwInit = 1;
    }

    sDev = TmrGetDev(iTmrChan);
    if (sDev == NULL)
    {
        return -EINVAL;
    }
    if (!ui8TmrIrqInit[sDev->iIrqIdx])
    {

        NVIC_DisableIRQ(TIMER0_IRQn + sDev->iIrqIdx);
        NVIC_ClearPendingIRQ(TIMER0_IRQn + sDev->iIrqIdx);
        NVIC_SetPriority(TIMER0_IRQn + sDev->iIrqIdx,
                            (1 << __NVIC_PRIO_BITS) - 1);
        NVIC_EnableIRQ(TIMER0_IRQn + sDev->iIrqIdx);
        ui8TmrIrqInit[sDev->iIrqIdx]++;
    }
    EtaCspTimerIntEnableClear(sDev->iIrqIdx, sDev->iChanIdx);
    return 0;
}
