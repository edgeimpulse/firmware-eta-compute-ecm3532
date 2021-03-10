//#include "reg.h"
#include "workQ_common.h"
//#include "executor_common.h"
#include "module_common.h"
#include "utility.h"
#include "eta_csp_mailbox.h"
#include "m3Interface.h"
#include "dsp_task.h"
#include "reg_eta_ecm3532_dsp.h"
#include "dsp_malloc.h"
#include "executor_public.h"

extern tMsgQueue     dspMsgQ;
int main(void)
{
    set_rounding_mode(3);
    set_saturation_mode(1);
    EtaCspPrintViaMboxInit();
    EtaCspIoPrintfViaDspUart(true); //enable prints.

    Dsp_Xmem_heap_Init();
    Dsp_Ymem_heap_Init();
    // TBD: stays here till CSP fixes this
    REG_DSPCTRL_DSP_CONFIG_STATUS |= BM_DSPCTRL_DSP_CONFIG_STATUS_DONT_WAIT;
    //REG_DSPCTRL_DSP_CONFIG_STATUS |= BM_DSPCTRL_DSP_CONFIG_STATUS_CACHE_DISABLE; 
    initializeSharedMemory();    
    MsgQueueInit(&dspMsgQ);
    createExecutorTask();
    etaPrintf("started  DSP executor app\n\r");
    DspTaskSchedule();
}

