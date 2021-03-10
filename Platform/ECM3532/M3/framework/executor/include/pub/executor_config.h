#ifndef	_EXECUTOR_CFG_H_
#define	_EXECUTOR_CFG_H_

#define CONFIG_EXEC_MAX_SCHEDULED_WORK		4	// Lets be fair 2 in DSP and 2 in M3. assuming they do not have any data dependency
#define CONFIG_EXEC_MAX_INPUTS				5	// 2 inputs and 2 scratches

#define CONFIG_EXEC_M3_TASK_STACK_SIZE          (1024)
#define CONFIG_EXEC_DSP_TASK_STACK_SIZE        (256 /2 )
#define CONFIG_EXEC_M3_QUEUE_NUM_ELEMENTS   4
#define CONFIG_EXEC_DSP_QUEUE_NUM_ELEMENTS 4
//#define  CONFIG_TASK_NOTIFY   1
#define CONFIG_BIN_SEM              1
#endif /*_EXECUTOR_CFG_H_*/
