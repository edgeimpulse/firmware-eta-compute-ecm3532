/******************************************************************************
 *
 * @file eta_pwr_gov
 *
 * @brief pwr gov is a sw module , which will implement load based DVFS on
 * ecm3531 SOC
 *
 * Copyright (C) 2018 Eta Compute, Inc
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
 *
 *****************************************************************************/
#include "config.h"
#ifdef CONFIG_PM_ENABLE
#include <stddef.h>
#include "timer_hal.h"
#include "uart_hal.h"
#include "gpio_hal.h"
#include "errno.h"
#include "ecm3532.h"
#include "cm3.h"
#include "reg_eta_ecm3532_m3.h"
#include "eta_csp_socctrl.h"
#include "eta_csp.h"
#include "eta_csp_buck.h"
#include "eta_csp_hfo.h"
#include "pm.h"
#include "print_util.h"


/*****************************************************************************
 *
 *  Defines
 *
 ****************************************************************************/
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif
/* cnt in msec */
#define DEF_TMR_PERIOD 1000

/* threshold in cpu % */
uint8_t load_high_threshold = CONFIG_LOAD_HIGH_THRESH;
uint8_t load_low_threshold = CONFIG_LOAD_LOW_THRESH;
uint8_t load_very_low_threshold = (CONFIG_LOAD_LOW_THRESH / 2);
uint8_t load_crit_high_threshold = CONFIG_LOAD_CRIT_HIGH_THRESH;
/* debounce count */
uint8_t high_threshold_deb = CONFIG_STEP_UP_DEBOUNCE;
uint8_t low_threshold_deb = CONFIG_STEP_DOWN_DEBOUNCE;
int32_t HalTmrChInit(tHalTmrCh iTmrChan);

/* ecm3531 pwr gv timer states */
enum ecm3531_gov_tmr_state {
    TIMER_NCFG,
    TIMER_RUN,
    TIMER_STOP,
};

static uint8_t ecm3531_max_freq;
static uint8_t ecm3531_mini_freq;

struct ecm3531_pwr_gov {
    tHalTmr *sPmTmr;
    /* Index of hal hw timer */
    uint32_t ti;
    /* timer state */
    enum ecm3531_gov_tmr_state tmr_state;
    /* timer prog period */
    uint32_t tmr_period;
    /* lp period is actual active + sleep tim*/
    uint32_t total_period;
    /* timer start cnt */
    uint32_t start_cnt;
    /* timer end cnt */
    uint32_t stop_cnt;
    /* active time in curr period */
    uint32_t active_cnt;
    /* idle time in curr period */
    uint32_t sleep_cnt;
    /* elaspe time in current timer period */
    uint32_t elapsed_cnt;
    /* current freq in active mode*/
    uint32_t curr_freq;
    /* cpu load in % lp is most recent*/
    /*|cpu_load3|cpu_load2|cpu_load1|cpu_load0*/
    uint32_t cpu_load;
};

struct ecm3531_pwr_gov ecm3531_pg = {
    .tmr_state = TIMER_NCFG,
    .tmr_period =  DEF_TMR_PERIOD,
    .start_cnt = 0,
    .stop_cnt = 0,
    .curr_freq = 0,
};

/**
 *  start timer fun
 *
 *  start HAL timer for governor period load measurement
 *
 *  @param timer period
 *
 */
static void
ecm3531_start_gov_timer()
{
    /* start time to curr time */
    unsigned int tout;
    HalTmrChangePeriod(ecm3531_pg.sPmTmr,
                        ecm3531_pg.tmr_period);
    tout = HalTmrRead(ecm3531_pg.ti);
    ecm3531_pg.start_cnt = tout;
    /* period of load measurement */
    tout += ecm3531_pg.tmr_period;
    ecm3531_pg.stop_cnt = tout;
    while(ecm3531_pg.tmr_state == TIMER_RUN);
    HalTmrStart(ecm3531_pg.sPmTmr);
    ecm3531_pg.tmr_state = TIMER_RUN;
}
/**
 *  compute load
 *
 *  computes load if current ticks are greater than
 *
 *  timeout, else update sleep time in current cycle
 *
 */
static int
ecm3531_compute_load(void)
{
    int load = -1;

    if (ecm3531_pg.stop_cnt <= ecm3531_pg.elapsed_cnt) {
	/*
     * total period can be program period or it can be
     * more then program period based on sleep time
     * elapsed_cnt get increament with system tick
     *
     */
	ecm3531_pg.total_period = ecm3531_pg.elapsed_cnt -
				ecm3531_pg.start_cnt;
    if (ecm3531_pg.total_period > ecm3531_pg.sleep_cnt) {
	    ecm3531_pg.active_cnt = ecm3531_pg.total_period -
				                    ecm3531_pg.sleep_cnt;
    }
    else {
        ecm3531_pg.active_cnt = 0;
    }
	load = (ecm3531_pg.active_cnt * 100) / ecm3531_pg.total_period;
	ecm3531_pg.cpu_load = (ecm3531_pg.cpu_load << 8) | (load & 0xFF);
	ecm3531_pg.sleep_cnt = 0;
	ecm3531_pg.elapsed_cnt = 0;

    }

    return load;
}

/**
 *  set policy
 *
 *  set m3 policy
 *
 *  policy index
 *
 *
 */
static void
ecm3531_set_policy(uint32_t uiFreq)
{
    int ret = 0;
    static tBuckMemTarget iMemTarget =
        (CONFIG_M3_INIT_FREQ > 60 ? eBuckMem1100Mv : eBuckMem900Mv);
    static tBuckAoTarget iAoTarget =
        (CONFIG_M3_INIT_FREQ >= 40 ? eBuckAo700Mv : eBuckAo600Mv);

    if (uiFreq < 40)
    {
        if (iAoTarget != eBuckAo600Mv)
        {
            iAoTarget = eBuckAo600Mv;
            ret|= EtaCspBuckAoVoltageSet(iAoTarget, 1);
            while(ret);
            EtaCspHfoTuneDefaultUpdate();
        }
        ret |= EtaCspBuckM3FrequencySet((uiFreq * 1000), 0);
            while(ret);

    }
    if (uiFreq <= 60)
    {
        if (iMemTarget != eBuckMem900Mv)
        {
            ret |= EtaCspBuckM3FrequencySet((uiFreq * 1000), 0);
            iMemTarget = eBuckMem900Mv;
            ret |= EtaCspBuckMemVoltageSet(iMemTarget, 1);
            if (iAoTarget != eBuckAo700Mv)
            {
                iAoTarget = eBuckAo700Mv;
                ret|= EtaCspBuckAoVoltageSet(iAoTarget, 1);
                EtaCspHfoTuneDefaultUpdate();
            }
        }
        else
        {
            ret |= EtaCspBuckM3FrequencySet((uiFreq * 1000), 0);
        }

    }

    else
    {
        if (iMemTarget != eBuckMem1100Mv)
        {
            iMemTarget = eBuckMem1100Mv;
            if (iAoTarget != eBuckAo700Mv)
            {
                iAoTarget = eBuckAo700Mv;
                ret |= EtaCspBuckAoVoltageSet(iAoTarget, 1);
                EtaCspHfoTuneDefaultUpdate();
            }
            ret |= EtaCspBuckMemVoltageSet(iMemTarget, 1);

        }

        ret |= EtaCspBuckM3FrequencySet((uiFreq * 1000), 0);
    }

    if (ret) ecm35xx_printf("Err in setting buck %s\r\n", __func__);
}


/**
 *  step up policy
 *
 *  if current policy is not the max, step up policy
 *
 *  index and set the same
 *
 *  @param void
 *
 */

static void
ecm3531_stepup_policy(uint32_t step_size)
{
    if (ecm3531_pg.curr_freq  < ecm3531_max_freq)
    {
        ecm3531_pg.curr_freq += step_size;
        if (ecm3531_pg.curr_freq > ecm3531_max_freq)
            ecm3531_pg.curr_freq = ecm3531_max_freq;
        ecm3531_set_policy(ecm3531_pg.curr_freq);
#ifdef CONFIG_PM_MON
        send_mon_event(DVFS_OPP, ecm3531_pg.curr_freq);
#endif
    }
}

/**
 *  step down policy
 *
 *  if current policy is not the minimum, reduce policy
 *
 *  index and set the same
 *
 *  @param void
 *
 */

static void
ecm3531_stepdown_policy(uint32_t step_size)
{

        if (ecm3531_pg.curr_freq != ecm3531_mini_freq)
        {
            if (ecm3531_pg.curr_freq >=
                    (step_size + ecm3531_mini_freq))
                ecm3531_pg.curr_freq -= step_size;
            else
                ecm3531_pg.curr_freq = ecm3531_mini_freq;

            ecm3531_set_policy(ecm3531_pg.curr_freq);
#ifdef CONFIG_PM_MON
            send_mon_event(DVFS_OPP, ecm3531_pg.curr_freq);
#endif
        }
}

/**
 *  set next policy
 *
 *  if load is computed decide next policy based on load
 *
 *  else restart timer and continue on current policy
 *
 *  @return int   actual sleep ticks programmed
 */
static void
ecm3531_set_next_policy(void)
{
    uint32_t tout;
    int lc;
    static uint8_t high_deb, low_deb;

    /*
     * this will return -ve value when time elaspe is
     * less then  DEF_TMR_PERIOD, minimum time for
     * load meaurement
     */
    lc = ecm3531_compute_load();
    if (lc >= 0) {
        /*
         * measure load is greater then crit high threshold
         * bump up freq
         */
       if (lc >= load_crit_high_threshold)
        {
            ecm3531_stepup_policy(CONFIG_DELTA_STEP_UP * 2);
        }
        /*
         * measure load is greater then threshold
         * after debounce count, we need to bump up freq
         */
        else if (lc > load_high_threshold)
        {
            high_deb++;
            if (high_deb >= high_threshold_deb)
            {
                ecm3531_stepup_policy(CONFIG_DELTA_STEP_UP);
                high_deb = 0;
            }
        }
        /*
         * load is very less step down with debounce
         */
        else if (lc < load_very_low_threshold)
        {
            ecm3531_stepdown_policy((CONFIG_DELTA_STEP_DOWN * 2));
            low_deb = 0;
        }
        else if (lc < load_low_threshold)
        {
            low_deb++;
            /*
             * measure load is less then threshold
             * after debounce count, we need to reduce freq
             */
            if (low_deb >= low_threshold_deb)
            {
                ecm3531_stepdown_policy(CONFIG_DELTA_STEP_DOWN);
                low_deb = 0;
            }
        }
         else {
             low_deb = 0;
             high_deb = 0;
        }
        /* re start counter */
        ecm3531_start_gov_timer(ecm3531_pg.tmr_period);
#ifdef CONFIG_PM_MON
	    send_mon_event(CPU_LOAD, lc);
#endif
    }
    /*
     * time elaspe is  less then  DEF_TMR_PERIOD
     * we will reprogram timer with DEF_TMR_PERIOD - elapse
     * time and continue on last policy
     */
    else {
        tout = HalTmrRead(ecm3531_pg.ti);
        tout = (ecm3531_pg.stop_cnt - tout);
        ecm3531_pg.tmr_state = TIMER_RUN;
        /* continue last cycle timer */
        HalTmrChangePeriod(ecm3531_pg.sPmTmr, tout);
        HalTmrStart(ecm3531_pg.sPmTmr);
        //ecm3531_set_policy(ecm3531_pg.curr_freq);
    }
}

/**
 * post lpm work func
 *
 * Start HW timer of not confiure else restart it
 *
 * @param ticks  amounts of ticks in idle
 *
 * @return int   actual sleep ticks programmed
 */

int ecm35xx_post_lpm(uint32_t sticks)
{
    if (ecm3531_pg.tmr_state == TIMER_NCFG) {
        /* start timer */
        ecm3531_start_gov_timer(ecm3531_pg.tmr_period);
#ifdef CONFIG_PM_MON
        send_mon_event(DVFS_OPP, ecm3531_pg.curr_freq);
#endif
    } else {
        ecm3531_pg.sleep_cnt += sticks;
        ecm3531_pg.elapsed_cnt += sticks;
        ecm3531_set_next_policy();
    }
    return sticks;
}

/**
 * pwr gov do lpm
 *
 * @param ticks  amounts of ticks in idle
 *
 * @return int
 */
int ecm35xx_enter_lpm(uint32_t sticks)
{
    int ret = 0;

    if (ecm3531_pg.curr_freq >= 40)
        ret |= EtaCspBuckM3VoltageSet(eBuckM3Voltage700Mv, 0);
    else
        ret |= EtaCspBuckM3VoltageSet(eBuckM3Voltage600Mv, 0);
    if (ret)
        ecm35xx_printf("Err in setting buck %s\r\n", __func__);
#ifdef CONFIG_LPM_LED
    HalGpioWrite( CONFIG_LPM_LED_GPIO, 0);
#endif

    /* do M3 Stall */
    EtaCspSocCtrlStallAnyIrq();

#ifdef CONFIG_LPM_LED
    HalGpioWrite(CONFIG_LPM_LED_GPIO, 1);
#endif
    ecm3531_set_policy(ecm3531_pg.curr_freq);
    return 0;
}

/**
 *  pre lpm work func
 *
 *  stops HW timer if running
 *
 * @param ticks  amounts of ticks in idle
 *
 * @return actual sleep ticks programmed
 */

int ecm35xx_pre_lpm(uint32_t sticks)
{
    if (ecm3531_pg.tmr_state == TIMER_RUN)
    {
        ecm3531_pg.elapsed_cnt = HalTmrRead(ecm3531_pg.ti);
        HalTmrStop(ecm3531_pg.sPmTmr);
        ecm3531_pg.tmr_state = TIMER_STOP;
    }
    return sticks;
}

/**
 *  HW timer callback
 *
 * calculates load and decides on next system policy
 *
 * @param void pointer
 */

static void ecm3531_tcb(void *arg1)
{
	ecm3531_pg.tmr_state = TIMER_STOP;
	ecm3531_pg.elapsed_cnt = HalTmrRead(ecm3531_pg.ti);
    ecm3531_set_next_policy();
}

inline unsigned long ecm35xx_get_curr_freq(void)
{
   return (ecm3531_pg.curr_freq * 1000000);
}
/**
 *  pwr gov init
 *
 * intial pwr gov is sw module, which will
 *
 * @param timer_num     Timer number to initialize
 */

__attribute__((section(".initSection"))) int ecm35xx_pwrg_init(int hw_tmr)
{
    HalTmrChInit(hw_tmr);
    ecm3531_pg.ti = hw_tmr;
    ecm3531_pg.sPmTmr = HalTmrCreate(hw_tmr, HalTmrOneShot,
                                    ecm3531_pg.tmr_period,
                                    ecm3531_tcb, NULL);
#ifdef CONFIG_PM_MON
    init_mon_tasks();
#endif
    ecm3531_mini_freq = CONFIG_MIN_M3_FREQ;
    ecm3531_max_freq = CONFIG_MAX_M3_FREQ;
    ecm3531_pg.curr_freq = CONFIG_M3_INIT_FREQ;

#ifdef CONFIG_LPM_LED
    HalGpioOutInit(CONFIG_LPM_LED_GPIO, 1);
#endif
    return 0;
}
#endif
