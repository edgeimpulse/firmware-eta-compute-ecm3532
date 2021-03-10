#include"reg.h"
#include "module_common.h"
#include "utility.h"
#include "sensor_common.h"
#include "eta_csp_mailbox.h"
#include "m3Interface.h"
#include "dsp_task.h"
#include "dsp_msg.h"
#include "sensor.h"

uint8_t sensorTaskId;
static tStatus processRemoteSensorCmd(uint16_t callParamShmOffset, uint16_t operation);
static void sensorTaskHandler(tdspLocalMsg* msg, void* pArg);
static void sendResponseFromSensor(uint8_t moduleId, uint8_t evtRsp, uint8_t hwSensorId);
remoteCmdHandlerCb remoteCmdHandlerList [TOTAL_SENSOR_COUNT]={0};
localMsgHandlerCb  localMsgHanlderList  [TOTAL_SENSOR_COUNT]={0};

tStatus registerRemoteCmdHandlerCb ( remoteCmdHandlerCb cbFn ,int sensorID )
{
	tStatus status = FAIL;	
    if (!(remoteCmdHandlerList[sensorID]))
    {
        remoteCmdHandlerList[sensorID] = cbFn;
		status = SUCCESS;          
    }  
    return status;
}

tStatus registerLocalMsgHandlerCb ( localMsgHandlerCb cbFn, int sensorID)
{
	tStatus status = FAIL;	
    if (!(localMsgHanlderList[sensorID]))
    {
        localMsgHanlderList[sensorID] = cbFn;
		status = SUCCESS;          
    }
    else
    {
        TRACE("local msg handler registration failed for sensorid=%d\n\r", sensorID);
    }      
    return status;
}

static tStatus processRemoteSensorCmd(uint16_t callParamShmOffset, uint16_t operation)
{
    tStatus status = SUCCESS;
    tSensorCallParams callParam;
    volatile void chess_storage(IOMEM)* address = (void chess_storage(IOMEM)*)getSharedMemoryAddress(callParamShmOffset);
    memcpy_io_mem_to_xmem((void*)&callParam, (void chess_storage(IOMEM)*)address, (uint16_t)sizeof(callParam));	
    TRACE("sensorHwId=%d\n\r", callParam.sensorHwId);
	status = remoteCmdHandlerList[callParam.sensorHwId](operation, (tUnionSensorArg*)&callParam.args);
	memcpy_xmem_to_iomem((void chess_storage(IOMEM)*)address, &callParam, (uint16_t)sizeof(callParam));
    sendResponseFromSensor(RPC_MODULE_ID_SENSOR, RPC_RESPONSE, (uint8_t)callParam.sensorHwId);
	return status;
}


static void sensorTaskHandler(tdspLocalMsg* msg, void* pArg)
{
    chess_dont_warn_dead(pArg);
    if(ISLOCAL_MSG(msg->msgId))
	localMsgHanlderList[msg->data[0]](msg);
    else
     processRemoteSensorCmd(msg->data[0], msg->msgId);
}


void createSensorTask(void)
{
    int8_t status;

    status = DspTaskCreate(sensorTaskHandler, 0, &sensorTaskId);
    if (status == -1)
    {
        TRACE("Issue in creating sensor Task:\n\r");
        return;// status;
    }
    bindTasktoRPCModule(RPC_MODULE_ID_SENSOR, sensorTaskId);
    return;// status;
}
static void sendResponseFromSensor(uint8_t moduleId, uint8_t evtRsp, uint8_t hwSensorId)
{
    uint32_t mboxHeader = 0;
    SET_MODULEID(mboxHeader, moduleId);
    SET_EVT_RSP(mboxHeader, evtRsp);
    SET_SENSOR_ID(mboxHeader, hwSensorId);

    //sendMailBox
    TRACE("mboxHeader=0x%x\n\r",mboxHeader);
    EtaCspMboxDsp2M3((uint16_t)mboxHeader, 0x00);
}
/*
D[31 - 24]     D[23 - 16]       D[15 - 8]    D[7 - 5]    D4 D3         D2         D1 D0
---------- - ------------ ------------------ - -------------- - ---------- - -----
| RSVD | RSVD | SENSOR TYPE | EVENT TYPE | HW SENSOR ID | EVNT / RSP | MODULE ID |
------------------------ - ---------------------------------------- - ------------
-------- - USER DEFINED------------------------------------ - | ----COMMON---------- -
*/
void sendSensorEventData(uint8_t hwSensorId, uint8_t sensorType, uint16_t evtType, uint16_t index, uint16_t length)
{
    uint32_t mboxHeader = 0;
    uint32_t mboxData = 0;

    SET_MODULEID(mboxHeader, RPC_MODULE_ID_SENSOR);

    SET_EVT_RSP(mboxHeader, RPC_EVENT);

    SET_SENSOR_ID(mboxHeader, hwSensorId);

    SET_SENSOR_TYPE(mboxHeader, sensorType);

    SET_EVENT_TYPE(mboxHeader, evtType);

    mboxData = (((uint32_t)length << 16) | ((uint32_t)index & 0xffff));
    TRACE("evt sent:\n\r");
    //eta_csp_dsp2m3_64bit_mbox(mboxHeader, mboxData);
    EtaCspMboxDsp2M3((uint16_t)mboxHeader, mboxData);
    return;
}

