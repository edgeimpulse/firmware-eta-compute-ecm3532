//////////////////////////////////////////////////////////////////////////
// The confidential and proprietary information contained in this file may
// only be used by a person authorized under and to the extent permitted
// by a subsisting licensing agreement from Eta Compute Inc.
//
//        (C) Copyright 2019 Eta Compute Inc
//                All Rights Reserved
//
// This entire notice must be reproduced on all copies of this file
// and copies of this file may only be made by a person if such person is
// permitted to do so under the terms of a subsisting license agreement
// from Eta Compute Inc.
//////////////////////////////////////////////////////////////////////////
#ifndef H_TIMER_
#define H_TIMER_

#define INVALID_TIMER_HANDLE 0xffff

typedef void (*tpfTimerHandler) (void *context);
typedef uint16_t  tTimerHandle;


/* Timer usage sequence: 
 * 1. First create timer and register timer callback.
 * 2. Load timeout period and start timer. When timer expires, it will call callback().
 * 3. To restart timer again, repeat step 2.
 * 4. Once done with timer usage, delete timer.
*/

//configure timer with specified callback, it will return handle. Use this handle in subsequent calls.
tTimerHandle createTimer(tpfTimerHandler  timerCb, void *context);

//start periodic/one short timer
void startTimer(tTimerHandle timerHandle, uint16_t msVal, uint8_t periodic);

//stops timer.
void stopTimer(tTimerHandle timerHandle);


//delete Timer.
void deleteTimer(tTimerHandle timerHandle);


#endif //H_TIMER_
