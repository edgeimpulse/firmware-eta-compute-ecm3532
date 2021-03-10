#ifndef	_EXECUTOR_TYPES_H_
#define	_EXECUTOR_TYPES_H_
#include <stdint.h>
#include "incbin.h"
//#include "eta_utils_stdio.h"
#include "eta_types.h"
#include "eta_nnfunctions.h"
#include "print_util.h"
#include  "executor_err.h"

//Origin  of Operand
#define OPD_ORIG_IO 												0
#define OPD_ORIG_CPU											1
#define OPD_ORIG_INT_PERSISTENT_MEM			2
#define OPD_ORIG_EXT_PERSISTENT_MEM			3

// Acess type of Operand
#define OPD_ACCESS_TYPE_RO								0
#define OPD_ACCESS_TYPE_RW 							1

//Memory Type ( M3 only or DSP)
#define OPD_MEM_TYPE_M3_LOCAL 						0
#define OPD_MEM_TYPE_SHARED							2
#define OPD_MEM_TYPE_DSP_LOCAL 					1

// base type size 1,2,4 bytes
#define OPD_BASE_SIZE_BYTES_1							0
#define OPD_BASE_SIZE_BYTES_2							1
#define OPD_BASE_SIZE_BYTES_3							2
#define OPD_BASE_SIZE_BYTES_4							3
//scope

#define OPD_MEM_SCOPE_OPERATION					0
#define OPD_MEM_SCOPE_GLOBAL						1
#define INCBIN_ETA(X)  INCBIN(X,#X".bin")





 struct   privateInfo; // opaque structure private to implementataion

// Information about an operand

typedef struct ExecOperand
{
uint32_t origin :2; // IO/CPU/PERSISTENT_INT/PERSISTENT_EXT
uint32_t accessType :1; // RO/ RW
uint32_t memType :2; //DSP/M3/Both
uint32_t basetypeSize:2; // base size -1  1 /2/3/4 bytes
uint32_t memScope :1; // Per operation, Global
uint32_t numElements:24; // Number of elements of basetype
struct   privateInfo * privInfo;
} ExecOperand_t;


// DIrect Memory Functions
ExecStatus ExecAllocMem(ExecOperand_t  *p, void * baseAddr, uint32_t offset );
void ExecFreeMem (ExecOperand_t  *p);
void * ExecGetBufAddr (ExecOperand_t  *p);
void  ExecWaitForCompletion ( void );
#endif /* _EXECUTOR_TYPES_H_*/
