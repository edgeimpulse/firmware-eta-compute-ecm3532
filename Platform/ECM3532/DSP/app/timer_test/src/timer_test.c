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
// spi_timer_test
//
// Example on how to change SPI module into timer
//

#include "eta_csp_mailbox.h"

#include "reg.h"
#include "timer.h"

#define TRACE_UNIT_TEST_TIMER
#ifdef TRACE_UNIT_TEST_TIMER
#define DBG_TIMER  etaPrintf
#else
#define DBG_TIMER(...)
#endif

extern void setupSpiForTimer(void);

////////////////////////UNIT TESTING OF TIMER APIs
#define TIMER_UNIT_TEST  1 

#if (TIMER_UNIT_TEST)
static void timerCb0(void* v)
{
    DBG_TIMER("In timerCb0, arg=%d\r\n", *(uint16_t*)v);
}
static void timerCb1(void* v)
{
    DBG_TIMER("In timerCb1, arg=%d\r\n", *(uint16_t*)v);
}

static void singleTimerTest(void)
{
    tTimerHandle handle0;
    volatile uint32_t counter;
    uint16_t val0 = 99;
    handle0 = createTimer(timerCb0, (void*)&val0);
    DBG_TIMER("st0:\r\n");
    startTimer(handle0, 10, 0);
    //Add delay
    counter = 5000000; while (counter--);
    DBG_TIMER("delay0:\r\n");

    deleteTimer(handle0);
    DBG_TIMER("final delay singleTimerTest:\r\n");
}

static void simpleTimerTest(void)
{
    tTimerHandle handle0, handle1, handle2, handle3, handle4, handle5, handle6;
    tTimerHandle handle7, handle8, handle9, handle10;
    volatile uint32_t counter;
    uint16_t val0, val1, val2, val3, val4, val5, val6, val7, val8, val9, val10;
    val0 = 10, val1 = 11, val2 = 12, val3 = 13, val4 = 14, val5 = 15, val6 = 16, val7 = 17;
    val8 = 18, val9 = 19, val10 = 20;

    handle0 = createTimer(timerCb0, (void*)&val0);
    handle1 = createTimer(timerCb1, (void*)&val1);
    handle2 = createTimer(timerCb1, (void*)&val2);
    handle3 = createTimer(timerCb0, (void*)&val3);
    handle4 = createTimer(timerCb1, (void*)&val4);
    handle5 = createTimer(timerCb1, (void*)&val5);
    handle6 = createTimer(timerCb0, (void*)&val6);
    handle7 = createTimer(timerCb1, (void*)&val7);
    handle8 = createTimer(timerCb1, (void*)&val8);
    handle9 = createTimer(timerCb1, (void*)&val9);

    DBG_TIMER("st0:\r\n");
    startTimer(handle0, 100, 0);
    DBG_TIMER("st1:\r\n");
    startTimer(handle1, 200, 0);
    DBG_TIMER("st2:\r\n");
    startTimer(handle2, 300, 0);
    DBG_TIMER("st3:\r\n");
    startTimer(handle3, 400, 0);
    DBG_TIMER("st4:\r\n");
    startTimer(handle4, 500, 0);
    DBG_TIMER("st5:\r\n");

    //Add delay
    counter = 5000000; while (counter--);
    DBG_TIMER("delay0:\r\n");

    startTimer(handle5, 1000, 0);
    DBG_TIMER("st6:\r\n");
    startTimer(handle6, 900, 0);
    DBG_TIMER("st7:\r\n");
    startTimer(handle7, 800, 0);
    DBG_TIMER("st8:\r\n");
    startTimer(handle8, 700, 0);
    DBG_TIMER("st9:\r\n");
    startTimer(handle9, 600, 0);

    //Add delay
    counter = 5000000; while (counter--);
    DBG_TIMER("delay1:\r\n");

    startTimer(handle0, 1, 0);
    DBG_TIMER("st6:\r\n");
    startTimer(handle1, 2, 0);
    DBG_TIMER("st7:\r\n");
    startTimer(handle2, 3, 0);
    DBG_TIMER("st8:\r\n");
    startTimer(handle3, 4, 0);
    DBG_TIMER("st9:\r\n");
    startTimer(handle4, 5, 0);

    //Add delay
    counter = 5000000; while (counter--);
    DBG_TIMER("delay2:\r\n");

    deleteTimer(handle0);
    deleteTimer(handle1);
    deleteTimer(handle2);
    deleteTimer(handle3);
    deleteTimer(handle4);
    deleteTimer(handle5);
    deleteTimer(handle6);
    deleteTimer(handle7);
    deleteTimer(handle8);
    deleteTimer(handle9);

    DBG_TIMER("final delay simpleTimerTest:\r\n");
}

static void timerTest1(void)
{
    tTimerHandle handle0, handle1, handle2;
    volatile uint32_t counter;
    uint16_t val0, val1, val2;
    val0 = 10, val1 = 11, val2 = 12;

    handle0 = createTimer(timerCb0, (void*)&val0);
    handle1 = createTimer(timerCb1, (void*)&val1);
    handle2 = createTimer(timerCb1, (void*)&val2);

    DBG_TIMER("st0:\r\n");
    startTimer(handle0, 200, 0);
    DBG_TIMER("st1:\r\n");
    startTimer(handle1, 300, 0);
    DBG_TIMER("st2:\r\n");
    startTimer(handle2, 100, 0);

        //Add delay
        counter = 5000000; while (counter--);
    DBG_TIMER("After first config:\r\n");

    DBG_TIMER("st0:\r\n");
    startTimer(handle0, 200, 0);
    DBG_TIMER("st1:\r\n");
    startTimer(handle1, 400, 0);
    DBG_TIMER("st2:\r\n");
    startTimer(handle2, 300, 0);

    //Add delay
    counter = 5000000; while (counter--);
    DBG_TIMER("After first delay:\r\n");


    DBG_TIMER("st0:\r\n");
    startTimer(handle0, 500, 0);
    DBG_TIMER("st1:\r\n");
    startTimer(handle1, 400, 0);
    DBG_TIMER("st2:\r\n");
    startTimer(handle2, 600, 0);

    //Add delay
    counter = 5000000; while (counter--);
    DBG_TIMER("After second delay:\r\n");


    deleteTimer(handle0);

    DBG_TIMER("st1:\r\n");
    startTimer(handle1, 200, 0);
    DBG_TIMER("st2:\r\n");
    startTimer(handle2, 300, 0);

    deleteTimer(handle1);   //Negative case, timer is running and we are trying to stop it.
    deleteTimer(handle2);   //Negative case, timer is running and we are trying to stop it.

    //Add delay
    counter = 5000000; while (counter--);
    DBG_TIMER("After third delay:\r\n");

    deleteTimer(handle1);
    deleteTimer(handle2);

    DBG_TIMER("After final delay test case1:\r\n");
    //Add delay
    counter = 5000000; while (counter--);
}



static void timerTest2(void)
{
    tTimerHandle handle0, handle1, handle2, handle3, handle4, handle5, handle6;
    tTimerHandle handle7, handle8, handle9;
    volatile uint32_t counter;
    uint16_t val0, val1, val2, val3, val4, val5, val6, val7, val8, val9, val10;
    val0 = 10, val1 = 11, val2 = 12, val3 = 13, val4 = 14, val5 = 15, val6 = 16, val7 = 17;
    val8 = 18, val9 = 19;

    handle0 = createTimer(timerCb0, (void*)&val0);
    handle1 = createTimer(timerCb1, (void*)&val1);
    handle2 = createTimer(timerCb1, (void*)&val2);
    handle3 = createTimer(timerCb0, (void*)&val3);
    handle4 = createTimer(timerCb1, (void*)&val4);
    handle5 = createTimer(timerCb1, (void*)&val5);
    handle6 = createTimer(timerCb0, (void*)&val6);
    handle7 = createTimer(timerCb1, (void*)&val7);
    handle8 = createTimer(timerCb1, (void*)&val8);
    handle9 = createTimer(timerCb1, (void*)&val9);


    DBG_TIMER("st0:\r\n");
    startTimer(handle0, 100, 0);
    DBG_TIMER("st1:\r\n");
    startTimer(handle1, 200, 0);
    DBG_TIMER("st2:\r\n");
    startTimer(handle2, 300, 0);
    DBG_TIMER("st3:\r\n");
    startTimer(handle3, 400, 0);
    DBG_TIMER("st4:\r\n");
    startTimer(handle4, 500, 0);
    DBG_TIMER("st5:\r\n");

    stopTimer(handle0);
    stopTimer(handle2);
    stopTimer(handle6);  //This timer not yet started. 

    //Add delay
    counter = 5000000; while (counter--);
    DBG_TIMER("delay0:\r\n");

    startTimer(handle5, 1000, 0);
    DBG_TIMER("st6:\r\n");
    startTimer(handle6, 900, 0);
    DBG_TIMER("st7:\r\n");
    startTimer(handle7, 800, 0);
    DBG_TIMER("st8:\r\n");
    startTimer(handle8, 700, 0);
    DBG_TIMER("st9:\r\n");
    startTimer(handle9, 600, 0);

    stopTimer(handle5);
    stopTimer(handle8);
    stopTimer(handle9);
    stopTimer(handle5);  //Already stopped timer.


    //Add delay
    counter = 5000000; while (counter--);
    DBG_TIMER("delay1:\r\n");

    startTimer(handle0, 1, 0);
    DBG_TIMER("st6:\r\n");
    startTimer(handle1, 2, 0);
    DBG_TIMER("st7:\r\n");
    startTimer(handle2, 3, 0);
    DBG_TIMER("st8:\r\n");
    startTimer(handle3, 4, 0);
    DBG_TIMER("st9:\r\n");
    startTimer(handle4, 5, 0);

    //Add delay
    counter = 5000000; while (counter--);
    DBG_TIMER("delay2:\r\n");

    deleteTimer(handle0);
    deleteTimer(handle1);
    deleteTimer(handle2);
    deleteTimer(handle3);
    deleteTimer(handle4);
    deleteTimer(handle5);
    deleteTimer(handle6);
    deleteTimer(handle7);
    deleteTimer(handle8);
    deleteTimer(handle9);

    DBG_TIMER("final delay test case 2:\r\n");
}

static void periodicTimerTest(void)
{
    tTimerHandle handle0, handle1;
    volatile uint32_t counter;
    uint16_t val0 = 40, val1 = 80;
    handle0 = createTimer(timerCb0, (void*)&val0);
    handle1 = createTimer(timerCb1, (void*)&val1);
    DBG_TIMER("st0:\r\n");
    startTimer(handle0, val0, 1);
    //Add delay
    counter = 5000; while (counter--);
    DBG_TIMER("delay00:\r\n");

    DBG_TIMER("st1:\r\n");
    startTimer(handle1, val1, 1);

    //Add delay
    counter = 5000000; while (counter--);
    DBG_TIMER("delay0:\r\n");

    stopTimer(handle0);
    stopTimer(handle1);
    deleteTimer(handle0);
    deleteTimer(handle1);
    DBG_TIMER("final delay periodicTimerTest:\r\n");
}


static void testSpiTimer(void)
{
    setupSpiForTimer();
    //singleTimerTest();
    //simpleTimerTest();
    //timerTest1();
    //timerTest2();
    periodicTimerTest();
}

int main()
{
    EtaCspIoPrintfViaDspUart(true); //enable prints.
    etaPrintf("Starting DSP timer test\n\r");    
    testSpiTimer();
    while (1);
}
#endif  //#if  (TIMER_UNIT_TEST)
