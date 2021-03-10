/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */
#include "config.h"
#include "gpio_hal.h"
#include "eta_csp_dsp.h"
#include "eta_csp_mailbox_cmd.h"
#ifdef CONFIG_ECM3532
#include "reg_eta_ecm3532_m3.h"
#include "ecm3532.h"
#else
#include "reg_eta_ecm3531_m3.h"
#include "ecm3531.h"
#endif
#include "cm3.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <stdint.h>

#define DSP_MB_QLEN 2

struct dspmb_evarg {
    int event;
    int param;
};

static QueueHandle_t dspmb_eventq = NULL;
volatile int wait_for_event = 0;

void send_dsp_event(int ev, int param)
{
    BaseType_t xHigherPriorityTaskWoken;
    struct dspmb_evarg  dsp_evarg;

    dsp_evarg.event = ev;
    dsp_evarg.param = param;
	xQueueSendToFrontFromISR(dspmb_eventq, &dsp_evarg, &xHigherPriorityTaskWoken);
}

int wait_for_dsp_event(void)
{
    struct dspmb_evarg  dspev;
    int ret = 0;

    wait_for_event = 1;
	xQueueReceive(dspmb_eventq, &dspev, portMAX_DELAY);
    switch (dspev.event)
    {
#ifdef MAILBOX_DSP2M3CMD_BUF_ID
        case MAILBOX_DSP2M3CMD_BUF_ID:
            {
                ret = dspev.param;
                break;
            }
#endif
        default:
            break;

    }

    return ret;
}

void send_dsp_ack(int param)
{
    uint16_t cmd = MAILBOX_DSP2M3CMD_RSVD;
    uint32_t data;

    data = param;
#ifdef MAILBOX_DSP2M3CMD_BUF_ID
    cmd  = MAILBOX_DSP2M3CMD_BUF_ID;
#endif
    EtaCspDspMboxCmdSend(cmd, data);
}

void send2dsp(uint32_t param1, uint32_t param2)
{
    EtaCspDspMboxCmdSend(param1, param2);
}


/***************************************************************************//**
 *
 *  EtaCspDspMboxM3Receive - The routine to processed received mailboxes.
 *
 *  @param iIrqNum is the interrupt number.
 *
 ******************************************************************************/

void dspIpcCb(uint32_t low32, uint32_t high32);

void DSP_ISR(void)
{

    uint32_t low32;
    uint32_t high32;

    //
    // Read DSP mailbox, clear interrupt.
    //
    high32 = REG_MBOX_M3_DSP2M3_MBOX_EXTEND.V;
    low32 = REG_MBOX_M3_DSP2M3_MBOX_LOWER.V;

    //
    // Clear NVIC interrupt
    //
    // REG_NVIC_EICPR0.BF.DSP = 1;

    //
    // ui16RxMboxCmd must be 1 byte command OR we must deal with non-16 bit
    // allignment.
    //

    dspIpcCb(high32, low32);

    NVIC_ClearPendingIRQ(DSP_IRQn);
}

__attribute__((section(".initSection")))
void dsp_irq_setup(void)
{
  NVIC_DisableIRQ(DSP_IRQn);
  NVIC_SetPriority(DSP_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
  NVIC_ClearPendingIRQ(DSP_IRQn);
  //NVIC_SetVector(DSP_IRQn, (uint32_t)ecm3531_dsp_mbox_isr_handler);
  NVIC_EnableIRQ(DSP_IRQn);

  dspmb_eventq = xQueueCreate(DSP_MB_QLEN, sizeof(struct dspmb_evarg));
}
