#ifndef	_EXECUTOR_TRACE_H_
#define	_EXECUTOR_TRACE_H_
#include "eta_nnfunctions.h"
#include "print_util.h"
#include  "executor_err.h"

#define PRINT(fmt, args...) ecm35xx_printf("%s:%d " fmt "\r\n" ,__FILE__,__LINE__, ## args)

// Error messages
#ifdef 	DEBUG_ERR
#define EX_ERR(fmt, args...)  PRINT(fmt,## args)
#else
#define EX_ERR(fmt, args...)
#endif

// Normal debug trace messages
#ifdef 	DEBUG_TRACE
#define EX_DBG(fmt, args...)  PRINT(fmt,## args)
#else
#define EX_DBG(fmt, args...)
#endif
// Info messages like execution time, pending transactions etc.
#ifdef 	DEBUG_INFO
#define EX_INFO(fmt, args...)  PRINT(fmt,## args)
#else
#define EX_INFO(fmt, args...)
#endif

#define BUFFER_NAME(X)      #X
#define BUFFER_DUMP(X)   { WAIT_4_COMPLETION() ; dumpToStdout( BUFFER_NAME(X) , &(X));}
void dumpToStdout ( char * arrayname, ExecOperand_t * array );




#endif /*_EXECUTOR_TRACE_H_*/