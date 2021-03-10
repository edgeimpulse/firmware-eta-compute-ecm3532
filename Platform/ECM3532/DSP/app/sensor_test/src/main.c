//#include "reg.h"
#include "workQ_common.h"

#include "sensor_common.h"
#include "module_common.h"
#include "sensor.h"
#include "lsm6dsl.h"
#include "fftCompositeSensor.h"

#include "dspI2c.h"
#include "utility.h"
#include "eta_csp_mailbox.h"
#include "m3Interface.h"

#include "dsp_task.h"
#include "reg_eta_ecm3532_dsp.h"

extern void setupSpiForTimer(void);  //temp code
extern tMsgQueue     dspMsgQ;
int main(void)
{
    set_rounding_mode(3);
    set_saturation_mode(1);

    EtaCspPrintViaMboxInit();
    EtaCspIoPrintfViaDspUart(true); //enable prints.

    etaPrintf("Started DSP sensor test app\n\r");     

    i2c_setup();

    setupSpiForTimer();

    REG_DSPCTRL_DSP_CONFIG_STATUS |= BM_DSPCTRL_DSP_CONFIG_STATUS_DONT_WAIT;

    initializeSharedMemory();
#if (CONFIG_LSM6DSL_DSP == 1)
	initLsm6dslSensor();
#endif 	
#if (CONFIG_LSM6DSL_FFT_DSP == 1)
    initFFTCompositeSensor();
#endif
    
    MsgQueueInit(&dspMsgQ);
    createSensorTask();
    DspTaskSchedule();

}

