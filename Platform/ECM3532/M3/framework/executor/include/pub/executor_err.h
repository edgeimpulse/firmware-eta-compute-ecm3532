#ifndef	_EXECUTOR_ERR_H_
#define	_EXECUTOR_ERR_H_
typedef enum
{
  EXEC_STATUS_OK     =  0,      
  EXEC_STATUS_ERR_GEN     = -1, 
  EXEC_STATUS_ERR_MEM= -2
} ExecStatus;
#endif /*_EXECUTOR_ERR_H_ */