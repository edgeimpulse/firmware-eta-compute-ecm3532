/*******************************************************************************
*
* @file timer_hal.h
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
#ifndef H_TIMER_HAL_
#define H_TIMER_HAL_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Hal Timer Type*/
typedef enum {
    /** Requires restart after timer expiry */
    HalTmrOneShot = 0,
    /** Automatically restarted after expiry */
    HalTmrPeriodic,
}tHalTmrType;

/** Hal Timer State*/
typedef enum {
    /** Timer is created */
    HalTmrCreated = 0,
    /** Timer is running */
    HalTmrRunning,
    /** Timer is already expired */
    HalTmrExpired,
    /** Timer is stopped */
    HalTmrStopped,
    /** Timer is Deleted */
    HalTmrDeleted,
}tHalTmrState;

/** Hal Timer Channel */
typedef enum {
    /** Timer Channel 0 */
    HalTmrCh0 = 0,
    /** Timer Channel 1 */
    HalTmrCh1,
    /** Timer Channel 2 */
    HalTmrCh2,
}tHalTmrCh;

/** Opaque hal timer structure*/
typedef struct sHalTmr tHalTmr;

/**
 * HAL timer callback func proto type,
 * callback will be called from interrupt context,
 * it should do minimum and should not make any blocking calls*/
typedef void (*tHalTmrCb)(void *vArg);

/**
 * Create Timer Instance
 *
 * @param iTmrCh HW timer channel
 * @param iTmrType HW timer type oneshort/periodic timer
 * @param ui32TmrPeriod Expiry time of Timer, once timer started with HalTmrStart
 * @param fCbFun Callback function associated with timer instance
 * @param vArg argument to provide the timer callback
 *
 * @return Timer Handle on success, NULL on error
 */
tHalTmr * HalTmrCreate(tHalTmrCh iTmrCh, tHalTmrType iTmrType,
                            uint32_t ui32TmrPeriod, tHalTmrCb fCbFun,
                            void *vArg);

/**
 * Start a timer, timer should have beed created earlier with HalTmrCreate
 * timer will expire after ui32TmrPeriod used in HalTmrCreate timer
 *
 * @param sHalTmr Timer to be started, create using HalTmrCreate
 *
 * @return 0 on success, non-zero error code on failure.
 */
int32_t HalTmrStart(tHalTmr *sHalTmr);

/**
 * Stop a currently running timer; associated callback will NOT be called
 *
 * @param sHalTmr Timer to be stopped, create using HalTmrCreate
 *
 * @return 0 on success, non-zero error code on failure.
 */
int32_t HalTmrStop(tHalTmr *sHalTmr);

/**
 * Change the Expiry time of timer, new expiry time after 'ui32Ticks' ticks from
 * now, Timer must be in Stopped state before changing period of timer
 *
 * @param sHalTmr Timer to be started, create using HalTmrCreate
 * @param ui32Ticks The number of ticks from now will be new be new expiry time
 *
 * @return 0 on success, non-zero error code on failure.
 */
int32_t HalTmrChangePeriod(tHalTmr *sHalTmr, uint32_t ui32Ticks);

/**
 * Delete Timer Instance
 *
 * @param sHalTmr Timer instance which was created earlier
 *
 * @return 0n sucess return non zero on error
 */
int32_t HalTmrDelete(tHalTmr *sHalTmr);

/**
 * Get current state of Timer
 *
 * @param sHalTmr Timer instance which was created earlier
 *
 * @return timer currentstate.
 */
tHalTmrState HalTmrGetState(tHalTmr *sHalTmr);

/**
 * Returns the HW timer current tick value
 *
 * @param iTmrCh HW timer channel
 *
 * @return The current tick value
 */
uint32_t HalTmrRead(tHalTmrCh iTmrCh);

/**
 * Perform a blocking delay for a number of ticks.
 *
 * @param iTmrCh timer channel number
 * @param ui32Ticks The number of ticks to delay for
 *
 * @return 0 on success, non-zero error code on failure
 */
int32_t HalTmrDelay(tHalTmrCh iTmrCh, uint32_t ui32Ticks);

#ifdef __cplusplus
}
#endif

#endif /* H_TIMER_HAL_ */
