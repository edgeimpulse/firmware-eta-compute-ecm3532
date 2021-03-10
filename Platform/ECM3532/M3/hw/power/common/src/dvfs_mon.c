/******************************************************************************
 *
 * @file dvfs_mon
 *
 * @brief will implement function to send and receive event from dvfs module
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
 *****************************************************************************/
#include "config.h"
#ifdef CONFIG_PM_MON
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "eta_csp_socctrl.h"
#include "pm.h"
#include "print_util.h"
#include "FreeRTOS_CLI.h"
#include <stdint.h>
#include <string.h>

//#define MON_VERBOSE

#define MONQ_LENGTH 10

static QueueHandle_t mon_eventq = NULL;
struct mon_evd  mev;
int ev_data;
static uint8_t load = 0;
static uint32_t M3Freq;

struct mon_evd {
    uint32_t event;
    uint32_t param;
};

extern volatile uint8_t lpm_en;
#ifdef CONFIG_CLI
// Implement your command handler
static portBASE_TYPE mon_cmd_cb(char *pcWriteBuffer,
                                    size_t xWriteBufferLen,
                                    const char *pcCommandString)
{
    uint8_t pr_cnt = 0;
    const char *pcParameter;
    portBASE_TYPE xParameterStringLength;
	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	pr_cnt += sprintf(pcWriteBuffer + pr_cnt, "load :%d\r\n",
                            (int)(load));
	pr_cnt += sprintf(pcWriteBuffer + pr_cnt, "M3Freq :%d MHz\r\n",
                            (int)(M3Freq));
#ifdef CONFIG_ENABLE_FRONT_PANEL
    pr_cnt += sprintf(pcWriteBuffer + pr_cnt, "M3 Freq counter:%d\r\n",
#ifdef CONFIG_ECM3531
                        (int)(REG_SOCCTRL_M3_FREQ_COUNT.V * 2)
#else
                        (int)( REG_SOCCTRL_M3_FREQUENCY.V * 8)

#endif
                        );
#endif

    /* Obtain the parameter string. */
    pcParameter = FreeRTOS_CLIGetParameter(pcCommandString, 1,
                                            &xParameterStringLength);
    if (pcParameter) {
        if ( strncmp( pcParameter, "lpmdis", strlen( "lpmdis" ) ) == 0 )
            lpm_en = 0;
        else if ( strncmp( pcParameter, "lpmen", strlen( "lpmen" ) ) == 0 )
            lpm_en = 1;
    }


	return pdFALSE;
}
static const CLI_Command_Definition_t dvfs_mon =
{
	"mon",
	"\r\nmon:\r\n command to show current cpu load and current DVFS policy\r\n",
	mon_cmd_cb, /* The function to run. */
	-1 /* No parameters are expected*/
};
#endif

/* Task 1 handler function */
void mt_handler(void *arg)
{
    struct mon_evd recv_me;
    uint32_t evg;

    while (1)
    {
        xQueueReceive(mon_eventq, &recv_me, portMAX_DELAY);
        switch (recv_me.event)
        {
            case DVFS_OPP:
                {
                    evg = recv_me.param;

                    if (evg != M3Freq)
                    {
#ifdef CONFIG_LOAD_TEST
#ifdef CONFIG_ENABLE_FRONT_PANEL
                        ecm35xx_printf("\r\nM3Freq %d MHz FCnt %d\r\n",
#ifdef CONFIG_ECM3531
                        evg, (int)(REG_SOCCTRL_M3_FREQ_COUNT.V * 2));
#else
                        evg, (int)( REG_SOCCTRL_M3_FREQUENCY.V * 8));
#endif

#else
                        ecm35xx_printf("M3Freq %d MHz \r\n", evg);
#endif
#endif
                    }
                    M3Freq = evg;

                break;
                }
            case CPU_LOAD:
                {
                    evg = recv_me.param;
#ifdef MON_VERBOSE
                    if (load > evg) {
                        if ((load - evg) > 10)
                        {
#ifdef CONFIG_ENABLE_FRONT_PANEL
                            ecm35xx_printf("\r\nld %d M3Freq %d MHz FCnt %d\r\n",
#ifdef CONFIG_ECM3531
                        (int)(REG_SOCCTRL_M3_FREQ_COUNT.V * 2)
#else
                        EtaCspSocCtrlM3FrequencyGet()
#endif

#else
                            ecm35xx_printf("\r\nld %d M3Freq %d MHz\r\n",evg, M3Freq);
#endif
                    }
                    else {
                        if ((evg - load) > 10)
                        {
#ifdef CONFIG_ENABLE_FRONT_PANEL
                            ecm35xx_printf("\r\nld %d M3Freq %d MHz Fcnt %d\r\n",
                                                evg, M3Freq, (REG_SOCCTRL_M3_FREQ_COUNT.V * 2));
#else
                            ecm35xx_printf("\r\nld %d M3Freq %d MHz \r\n",evg, M3Freq);
#endif

                    }
#endif

                    load = evg;

                    break;
                }
            default:
                break;

        }

    }
}

void send_mon_event(uint32_t ev, uint32_t param)
{
    BaseType_t xHigherPriorityTaskWoken;
    mev.event = ev;
    mev.param = param;
	xQueueSendToFrontFromISR(mon_eventq, &mev, &xHigherPriorityTaskWoken );

	if( xHigherPriorityTaskWoken )
	{
		taskYIELD();
	}

}

/**
 *
 * This function performs initializations that are required before tasks run.
 *
 * @return int 0 success; error otherwise.
 */
__attribute__((section(".initSection")))
int init_mon_tasks(void)
{
    /*
     * initializing the tasks with the OS.
     */
    mon_eventq = xQueueCreate( MONQ_LENGTH, sizeof(mev));
	xTaskCreate(mt_handler, "mon", configMINIMAL_STACK_SIZE,
                        NULL, tskIDLE_PRIORITY + 2, NULL);
#ifdef CONFIG_CLI
	FreeRTOS_CLIRegisterCommand( &dvfs_mon);
#endif
    return 0;
}
#endif
