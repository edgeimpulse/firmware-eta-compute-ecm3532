#include "workQ_common.h"
#include "rpc.h"
#include "shmem.h"
#include "config.h"
#include "FreeRTOS.h" //Include it for M3 build but not for DSP.
#include "semphr.h"
#include "dsp_ipc.h"
#include "task.h"
#include "print_util.h"

#define MEMORY_PATTERN 0XDEAD
#define NOTIFIER_TASK_PRIORITY  (tskIDLE_PRIORITY + 2)
#define NOTIFIER_QUEUE_LENGTH       2
#define NOTIFY_FN_LIST_SIZE        (1<< NO_OF_HEADER_FILTER_BITS)

//TBD: Following needs to be shared witth the IPC ISR code too
#define IPC_FROM_DSP                1
#define EXIT_MSG_FROM_M3            2
typedef struct {
    uint8_t msgType; // IPC_FROM_DSP/EXIT_MSG_FROM_M3
    uint32_t header;
    uint32_t data;
}tIpcInfo;


//TBD: temp fix to get compilation going in windows

#ifdef __GNUC__
tM3DSPSharedMemory __attribute__((section (".shmSection"))) shmemM3Dsp = {0};
// TBD: use GCC directive to allocate this memory @ the sharedmem area specified in linker file.
#endif
SemaphoreHandle_t xNotifyFnListMutex;
QueueHandle_t xNotifierQueue;
tnotifyEventCb  notifyFnList[NOTIFY_FN_LIST_SIZE] = { 0 }; // Cost is memory ( which can be optimised)  advantage is faster serach
uint8_t notifierTaskActive = 0;
static void vnotifierTask(void* pvParameters)
{
    tIpcInfo ipcInfo;
    uint8_t index = 0;
    // Receive a message from the  IPC ISR
    while (notifierTaskActive) {
        if (xQueueReceive(xNotifierQueue, &(ipcInfo), portMAX_DELAY))
        {
            switch (ipcInfo.msgType) {
            case EXIT_MSG_FROM_M3:   // Graceful shutdown
                notifierTaskActive = 0; //TBD: True / False
                break;
            case  IPC_FROM_DSP:
                xSemaphoreTake(xNotifyFnListMutex, portMAX_DELAY);
                index = (uint8_t)(ipcInfo.header & (NOTIFY_FN_LIST_SIZE -1));
                if (notifyFnList[index]) {
                    notifyFnList[index](ipcInfo.header,ipcInfo.data);   // Actual Cb call
                }
                else {
                    //TBD: add trce or  assert
                }
                xSemaphoreGive(xNotifyFnListMutex);
                break;
            default:
                //TBD: add trce or  assert
                break;
            }
        }
    }
}

__attribute__((section(".initSection")))
void rpcRegisterEventCb(uint8_t eventHeaderMask, tnotifyEventCb cbFn)
{
    xSemaphoreTake(xNotifyFnListMutex, portMAX_DELAY);
    notifyFnList[eventHeaderMask & (NOTIFY_FN_LIST_SIZE - 1)] = cbFn;
    xSemaphoreGive(xNotifyFnListMutex);
}

__attribute__((section(".initSection"))) int rpcInit(void)
{
    //Fill the pattern
    shmemM3Dsp.topMemoryBoundaryPattern = shmemM3Dsp.bottomMemoryBoundaryPattern = MEMORY_PATTERN;
    if (SharedMemInit(shmemM3Dsp.byteArray, SHARED_MEMORY_ALLOCATOR_SIZE)) {
        //TBD: add trce or  assert
        return -1;
    }
    //Init the workqueue
    if (WorkQueueInit(&(shmemM3Dsp.workQueue))) {
        //TBD: add trce or  assert
        return -1;
    }
    //Create the Mutex for the notifier callback table
    xNotifyFnListMutex = xSemaphoreCreateMutex();
    if (xNotifyFnListMutex == NULL) {
        //TBD: Add trace message of error
        return -1;
    }
    //Create the queue for the notifier task
    xNotifierQueue = xQueueCreate(NOTIFIER_QUEUE_LENGTH, sizeof(tIpcInfo));
    if (!(xNotifierQueue)) {
        //TBD: add trce or  assert
        return -1;
    }
    notifierTaskActive = 1; //TBD: use TRUE/FALSE
    //Create the notifier Task
#ifdef CONFIG_SENSOR_MANAGER 
    if (pdPASS != xTaskCreate(vnotifierTask, "notifier", (configMINIMAL_STACK_SIZE), NULL, NOTIFIER_TASK_PRIORITY, NULL)) {
#else
    if (pdPASS != xTaskCreate(vnotifierTask, "notifier", (configMINIMAL_STACK_SIZE / 4 ), NULL, NOTIFIER_TASK_PRIORITY, NULL)) {
#endif
        //TBD: add trce or  assert
        return -1;
    }

    dsp_irq_setup();
    return 0;
}

void  rpcDeinit(void)
{
    notifierTaskActive = 0;
    // wait for the task to get killed
    // delete the queue
    // delete the mutex.
    // delete the work queue
    // delete the shmemem
}
int rpcSubmitWork(uint8_t moduleId, uint8_t operation, void* params) {
    tWorkDescriptor work;
    work.moduleId = moduleId;
    work.operation = operation;
    work.argumentPointerOffset = SharedMemGetOffset(params);
    //Add this task into work queue
    if (WorkQueueAdd(&(shmemM3Dsp.workQueue), &work))
    {
        //TBD: add trce or  assert
        return -1;
    }
    //TBD: Send and IPC message to DSP if it is sleeping
    send2dsp(DSP_WAKEUP_MSG, 0);
    return 0;
}

/* param2 is header (cmd), param1 data) */
void dspIpcCb(uint32_t param1, uint32_t param2)
{
    BaseType_t xTaskWoken;
    tIpcInfo IpcMsg;
    IpcMsg.msgType = IPC_FROM_DSP;
    IpcMsg.header = param2;
    IpcMsg.data = param1;
    xQueueSendFromISR(xNotifierQueue, &IpcMsg, &xTaskWoken);
	if( xTaskWoken == pdTRUE)
	{
        portYIELD_FROM_ISR(xTaskWoken);
	}

}
