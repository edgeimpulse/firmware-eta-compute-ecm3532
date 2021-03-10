#ifndef	_EXECUTOR_PRIV_H_
#define	_EXECUTOR_PRIV_H_

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#define BUF_STATE_NO_MEM			0
#define BUF_STATE_MEM_ACTIVE		1
#define BUF_STATE_MEM_IDLE			2

#define BUF_TYPE_NONE				0
#define  BUF_TYPE_IN				1
#define BUF_TYPE_OUT				2
#define BUF_TYPE_INOUT				3

#define WORK_ID_VALID				1
#define WORK_ID_INVALID				0

#define NORMAL_CONV_2D          0
#define DEPTHWISE_CONV_2D   1

struct privateInfo
{
	uint8_t bufState:2;
	uint8_t bufType:2;
	uint8_t usageCount:2;
	uint8_t tobeFreed:1;
	uint8_t reserved:1;
	void *bufAddr;
};

// Represents already scheduled work on various Hw executors.
typedef struct ExecWork
{
	uint8_t execHwId;
	uint8_t numInputs;
    uint8_t opID;
	uint8_t valid:1;
	uint8_t wait4Completion:1;
    uint8_t variant :2;
    uint8_t reserved:4;
	ExecOperand_t * inbufs[CONFIG_EXEC_MAX_INPUTS] ;
	ExecOperand_t * outBuf;	//
#ifdef CONFIG_BIN_SEM
	SemaphoreHandle_t waitSem;
#endif
	void *params;
} ExecWork_t ;

typedef struct
{
    const q7_t * wt;
    const q7_t * bias;
    conv2d_opt opt;
}exec_conv2d_q7_t;

typedef struct
{
      pool2d_opt opt;
}exec_avepool2d_q7_t;

typedef struct
{
      pool2d_opt opt;
}exec_maxpool2d_q7_t;

typedef struct
{
     const q7_t * wt;
    const q7_t * bias;
    conv2d_relu_avgpool_opt opt;
}exec_conv2d_relu_avgpool_q7_t;

typedef struct
{
    const q7_t * wt_pw;
    const q7_t * wt_ds;
    const q7_t * bias_pw;
    const q7_t * bias_ds;
    conv2d_opt opt_pw;
    conv2d_opt opt_ds;
}exec_pw_ds_conv2d_q7_t;

typedef struct
{
    concat_opt opt;
} exec_concat_q7_t;

typedef struct
{
    int size;
    int width;
} exec_sigmoid_q7_t;

typedef struct
{
    const q7_t * wt;
    const q7_t * bias;
    fc_opt opt;
}exec_fc_q7_t;

typedef struct
{
    add_opt opt;
}exec_add_q7_t;


uint8_t CheckAndScheduleWork (ExecWork_t * work, uint8_t inoutExist) ;
void SubmitM3Work (uint8_t execWorkID) ;
void SubmitDSPWork (uint8_t execWorkID) ;




#endif /*_EXECUTOR_PRIV_H_*/
