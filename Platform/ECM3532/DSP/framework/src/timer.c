//////////////////////////////////////////////////////////////////////////
// The confidential and proprietary information contained in this file may
// only be used by a person authorized under and to the extent permitted
// by a subsisting licensing agreement from Eta Compute Inc.
//
//        (C) Copyright 2016 Eta Compute Inc
//                All Rights Reserved
//
// This entire notice must be reproduced on all copies of this file
// and copies of this file may only be made by a person if such person is
// permitted to do so under the terms of a subsisting license agreement
// from Eta Compute Inc.
//////////////////////////////////////////////////////////////////////////

//
// spi_timer
//
// Timer implementation based on SPI module.
//
#include "dsp_config.h"
#include "dsp_fw_cfg.h"
#include "reg.h"
#include "eta_csp_mailbox.h"
#include "eta_csp_io.h"
#include "eta_csp_spi.h"
#include "timer.h"

#define TIMER_TRACE
#ifdef TIMER_TRACE
#define DBG_TIMER  etaPrintf
#else
#define DBG_TIMER(...)
#endif

#define VERY_LONG_TIME (0xFFFFFFFF)

typedef uint32_t  TIME;  //1 tick of time is 16us

struct timer {
    tpfTimerHandler  timerCb; //timer callback()
    void* context;
    TIME  time;         //relative time to wait in terms of tick. (1 tick = 16us)
    uint8_t periodic;
    TIME  periodTime;
    uint8_t   inUse;        //When timer is started, this variable will be true.
} timers[MAX_TIMERS] ={0};

volatile TIME time_now =0;      /* current time */
struct timer* timer_next = 0;/* timer we expect to run down next */

TIME time_timer_set =0;         /* time when physical timer was set */
uint16_t physical_timer_tick_set =0 ;  /* future ticks for which the physical HW timer was set to give an interrupt */

void setupSpiForTimer(void);

static void timers_update(TIME time);
static void start_physical_timer(uint16_t timer_ticks);
static inline TIME get_time_now(void);

static void timer_interrupt_handler(tIrqNum iIrqNum, intline_T int_line);
static inline void enable_spi_interrupt(void);
static inline void disable_spi_interrupt(void);


//configure timer with specified callback

tTimerHandle createTimer(tpfTimerHandler  timerCb, void *context)
{
    int i;
	tTimerHandle handle = INVALID_TIMER_HANDLE;
	if ( timerCb){			
		//Get free timer slot.
		for (i = 0; i < MAX_TIMERS; i++) {
			if (0 == timers[i].timerCb)
			{
				timers[i].timerCb = timerCb;
				timers[i].context = context;
				handle = i;
				break;
			}
			   
		}
	}
    return handle;
}

//start periodic/one short timer.
void startTimer(tTimerHandle timerHandle, uint16_t msVal,uint8_t periodic)
{
    timer *t;
    uint16_t timeInTick;
    TIME temp_time_now;
    if (msVal > MAX_TIMER_MS_COUNT) {
        DBG_TIMER("Issue:timer set value exceeds MAX_TIMER_MS_COUNT:\n\r");
        return;
    }
    if ((timerHandle >= MAX_TIMERS) || (0 == timers[timerHandle].timerCb)) {
        DBG_TIMER("Issue in starting timer\n\r");
        return;
    }
	 //To avoid saturation, multipication by (1000/16) is achieved in two stages.
    timeInTick =  (1000 / 32) * msVal;
    timeInTick += timeInTick + (uint32_t)(msVal >> 1);
	
    disable_spi_interrupt();
    t = &timers[timerHandle];
    t->time = timeInTick;
    // Store periodic timer details
    t->periodic = periodic;
    t->periodTime = timeInTick;
           
    //DBG_TIMER("time=%d:\n\r", (uint16_t)t->time);

    temp_time_now = get_time_now();
    //DBG_TIMER("timeNow=%d:\n\r", (uint16_t)temp_time_now);

    if (!timer_next) {
        /* no timers set at all, so this is shortest */
        time_now = temp_time_now;
        time_timer_set = time_now;
        start_physical_timer((timer_next = t)->time);
    } else if ((t->time + time_now) < (timer_next->time + time_timer_set)) {
        /* new timer is shorter than current one, so */
        time_now = temp_time_now;
        timers_update(time_now - time_timer_set);
        time_timer_set = time_now;
        start_physical_timer((timer_next = t)->time);
    } else {
        /* new timer is longer, than current one */
        t->time += (temp_time_now- time_timer_set);
    }

    t->inUse = TRUE;
    enable_spi_interrupt();
    return;
}

//Stops timer.
void stopTimer(tTimerHandle timerHandle)
{
    struct timer* t;
    //DBG_TIMER("stoping timer with handle=%d:\n\r", timerHandle);
    if (timerHandle >= MAX_TIMERS) {
        DBG_TIMER("Issue in stoping timer\n\r");
        return;
    }
    disable_spi_interrupt();

    t = &timers[timerHandle];

    if (!t->inUse) {
        enable_spi_interrupt();
        return;
    }

    t->inUse = FALSE;

    /* check if we were waiting on this one */
    if (t == timer_next) {
        time_now = get_time_now();
        timers_update(time_now - time_timer_set);
        if (timer_next) {
            start_physical_timer(timer_next->time);
            time_timer_set = time_now;
        }
    }
    enable_spi_interrupt();
}


//delete Timer.
void deleteTimer(tTimerHandle timerHandle)
{
    DBG_TIMER("deleting timer with handle=%d:\n\r", timerHandle);
    disable_spi_interrupt();
    if ((timerHandle >= MAX_TIMERS) || (0 == timers[timerHandle].timerCb) 
        || (timers[timerHandle].inUse)) {
        DBG_TIMER("Issue in deleting timer\n\r");
        enable_spi_interrupt();
        return;
    }
    timers[timerHandle].timerCb = 0;
    enable_spi_interrupt();
}


void setupSpiForTimer(void)
{
    DBG_TIMER("\nInitialization of spi_timer:\r\n");
    // SPI as interrupt Setup
    // 0xF means 500KHz SPI Clock, or 2us per bit. 16us per byte
#ifdef CONFIG_DSP_ECM3532
{
    uint16_t tempdata0 = 20;	
    REG_DSPCTRL_DSP_CONFIG_STATUS |= BM_DSPCTRL_DSP_CONFIG_STATUS_DSP_SPI_CLK_ON;
    while (--tempdata0);
}
#endif
    REG_SPI2_CLKDIV = (0xf << BP_SPI2_CLKDIV_CLKDIV) | (0x0 << BP_SPI2_CLKDIV_IBG);

    // Disable RX FIFO (don't care)
    REG_SPI2_CONFIG = BM_SPI2_CONFIG_DIS_RX_FIFO;

    // Start command
    REG_SPI2_STATUS = BM_SPI2_STATUS_START_BUSY | BM_SPI2_STATUS_HOLD_CS | (0 << BP_SPI2_STATUS_CS_SEL);

#ifdef CONFIG_DSP_ECM3531
    // Set up the interrupts 
    EtaCspIsrHandlerSet(INT_NUM_SPI, CF_INT_LINE_0, timer_interrupt_handler);
    eta_csp_isr_enable_ext_int(INT_NUM_SPI, CF_INT_LINE_0);
#endif 
#ifdef CONFIG_DSP_ECM3532
    EtaCspIsrHandlerSet(eIrqNumSpi, OTHER_ISR_LINE, timer_interrupt_handler);
    EtaCspIsrExtIntEnable(eIrqNumSpi, OTHER_ISR_LINE);
#endif    
    return;
}

//////////static functions/////////////////////////////


/* subtract time from all timers, enabling any that run out along the way */
/* Also identifies timer with least byteCnt, which is going to expire*/
static void timers_update(TIME time)
{
    static struct timer timer_last = {
        0,
        0,
        VERY_LONG_TIME    /* time */,
        FALSE     /* in use */,
    };
    struct timer* t;
    int doReparse = 0;
    timer_next = &timer_last;
    for (t = timers; t < &timers[MAX_TIMERS]; t++) {
        if (t->inUse) {
            if (time < t->time) { /* unexpired */
                t->time -= time;
                if (t->time < timer_next->time)
                    timer_next = t;
            }
            else { /* expired */
           /* tell scheduler */
                t->timerCb(t->context);
                if (t->periodic == 0) {
                    t->inUse = 0; /* remove timer */
                }                    
                else {
                    doReparse = 1;
                    t->time = t->periodTime;
                } 
            }
        }
    }
    //Now reparse the list to find the shortest timer
    if (doReparse) {
        timer_next = &timer_last;
        for (t = timers; t < &timers[MAX_TIMERS]; t++) {
            if (t->inUse) {  
                if (t->time < timer_next->time)
                    timer_next = t;
            }
        }
    }

    /* reset timer_next if no timers found */
    if (!timer_next->inUse) {
        timer_next = 0;
        time_now = 0;
        time_timer_set = 0;
        physical_timer_tick_set = 0;
    }

}

static inline TIME get_time_now(void)
{
    //elapsed time since physical timer set+previous time.
    return ((physical_timer_tick_set - REG_SPI2_TX_DUMMY_BYTE_CNT) + time_now);
}

static void start_physical_timer(uint16_t timer_ticks)
{
    //DBG_TIMER("physical timer with tick=%d started\n\r", timer_ticks);
    physical_timer_tick_set = timer_ticks;
    REG_SPI2_TX_DUMMY_BYTE_CNT = timer_ticks;
    REG_SPI2_INT_EN = BM_SPI2_INT_EN_XMIT_COMPLETE_EN0;

}

/////////////interrupt related functions
static inline void enable_spi_interrupt(void)
{
#ifdef CONFIG_DSP_ECM3531	   
    eta_csp_isr_enable_ext_int(INT_NUM_SPI2, CF_INT_LINE_0);
#endif
#ifdef CONFIG_DSP_ECM3532	
    EtaCspIsrExtIntEnable(eIrqNumSpi, OTHER_ISR_LINE);
#endif
}

static inline void disable_spi_interrupt(void)
{
#ifdef CONFIG_DSP_ECM3531	   
    eta_csp_isr_disable_ext_int(INT_NUM_SPI2, CF_INT_LINE_0);
#endif
#ifdef CONFIG_DSP_ECM3532	
 EtaCspIsrExtIntDisable(eIrqNumSpi, OTHER_ISR_LINE);
#endif
}

static void timer_interrupt_handler(tIrqNum iIrqNum, intline_T int_line)
{
 
   //ACK
    REG_SPI2_INT_EN =0;	
    time_now = get_time_now();
    timers_update(time_now - time_timer_set);

    /* start physical timer for next shortest time if one exists */
    if (timer_next) {
        time_timer_set = time_now;
        //DBG_TIMER("timer start from isr with tick=%d", (uint16_t)timer_next->time);
        start_physical_timer(timer_next->time);
	}
#ifdef CONFIG_DSP_ECM3531	   
    eta_csp_isr_clear_int(iIrqNum, int_line);
#endif
#ifdef CONFIG_DSP_ECM3532	
    EtaCspIsrIntClear(iIrqNum, int_line);
#endif
}
