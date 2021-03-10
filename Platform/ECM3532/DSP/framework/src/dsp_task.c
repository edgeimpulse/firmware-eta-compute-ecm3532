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
#include "reg.h"
#include "dsp_fw_cfg.h"
#include "dsp_msg.h"
#include "dsp_task.h"
#include "workQ_common.h"
#include "module_common.h"
#include "dsp_support.h"
#include "utility.h"
#include "m3Interface.h"
#include "eta_csp_io.h"
#include "eta_csp_isr.h"

typedef struct taskInfo
{
    DspTaskHandler handler;
    void* pArg;
}tTaskInfo;

uint16_t module2TaskMap[MAX_MODULE_ID] = { 0 };
tTaskInfo dspTaskList[DSP_MAX_TASKS] = { 0 };

int8_t DspTaskCreate(DspTaskHandler handler, void* pArg, uint8_t* taskId) // A module can have multiple handlers.
{
    int8_t index  = 0;
    int8_t status = -1;
    for (index = 0; index < DSP_MAX_TASKS; index++)
    {
        if (!(dspTaskList[index].handler))
        {
            dspTaskList[index].handler = handler;
            dspTaskList[index].pArg = pArg;
            status = 0;
            *taskId = index;
            break;
        }
    }
    return status;
}
int8_t DspTaskDelete(uint8_t taskId)
{
    int8_t status = -1;
    if (taskId < DSP_MAX_TASKS)
    {
        dspTaskList[taskId].handler = 0;
        dspTaskList[taskId].pArg = 0;
        status = 0;
    }
    return status;
}

tMsgQueue     dspMsgQ;

void bindTasktoRPCModule(uint16_t  moduleId, uint8_t  taskId)
{
    module2TaskMap[moduleId] = taskId;
}
static  void work2Msg(tWorkDescriptor* work, tdspLocalMsg* msg)
{
    //TBD: one to one or one to many
    msg->taskId = module2TaskMap[work->moduleId];
    msg->msgId = work->operation;
    msg->data[0] = work->argumentPointerOffset;
}

extern volatile int16_t mbIrq;
void DspTaskSchedule(void)
{
    tdspLocalMsg msg;
    tWorkDescriptor work;
    while (1)
    {
        if (IsWorkQueueEmpty(&(PhysicalSharedMemory.workQueue)))
        {

            if (IsMsgQueueEmpty(&dspMsgQ))
            {
                //etaPrintf("DSP going to sleep\n\r");

                disable_interrupt();
                if(!mbIrq)
                {
                    sleep();  //Sleep till it gets interrupt.
                }
                enable_interrupt();

            }
            else
            {
                MsgQueueRemove(&dspMsgQ, &msg);
                //TRACE("local message: taskId=%d, msgId=%d, data[0]=%d\n\r", msg.taskId, msg.msgId, msg.data[0]);
                // call the handler
                dspTaskList[msg.taskId].handler(&msg, dspTaskList[msg.taskId].pArg);
            }
        }
        else
        {
            // receive from Work Qeue
            WorkQueueRemove(&(PhysicalSharedMemory.workQueue), &work);

            //TRACE("Work: offset=%d, operation=%d\n\r", work.argumentPointerOffset, work.operation);
            work2Msg(&work, &msg);
            //  one approach is add to the queue    and come back again

            MsgQueueAdd(&dspMsgQ, &msg);

            // another  way : we do not add rather call handler  here as we have work to do
            // in that way works from M3 has a higher prio than locallly scheduled work.
            // The only drawback with this is if 2 works are there from m3 and one has yilded the processor for
            // DMA to get over then the next work starts and the previous one starves for DMA.

            //dspTaskList[msg.taskId].handler(&msg, dspTaskList[msg.taskId].pArg);
        }
        //temp code
        //pollForSensorEvents();
    }

}
