#define W_IN 4
#define H_IN 1
#define CH_IN 3 

#define NUM_KER 2
#define CH_OUT NUM_KER

#define DIM_OUT (CH_OUT * H_IN * W_IN)

#include "eta_types.h"

int bias_shift = 5;
int out_shift = 2;
int output_activation_min = -128;
int output_activation_max = 127;

#define CHW_FORMAT  0  //enable chw format else whc format.

#if CHW_FORMAT
//input in [channel][rows][columns] format
int chess_storage(XMEM) xmemInput[CH_IN * H_IN * W_IN] = { 20,30,40,50,7,7,7,7,-2,-100,20,30 };

// weight for each input channel scale factor.
int chess_storage(YMEM) ymemWeight[NUM_KER * CH_IN] = { 1,3,3,   -4,-20,-2 };

// bias for each input channel bias/offset
int chess_storage(YMEM) ymemBias[NUM_KER] = { 20,-10 };

//Allocate space to collect output.
int chess_storage(XMEM) xmemActualOutput[CH_OUT * H_IN * W_IN] = {};

/* expected  output  */
int xmemExpectedOutput[CH_OUT * H_IN * W_IN] = { 127,98,127,127,-128,-95,-128,-128 };

#else
//WHC format
int chess_storage(XMEM) xmemInput[CH_IN * H_IN * W_IN] = { 20,7,-2,  30,7,-100,  40,7,20, 50,7,30 };

int chess_storage(YMEM) ymemWeight[NUM_KER * CH_IN] = { 1,3,3,   -4,-20,-2 };


// bias for each input channel bias/offset
int chess_storage(YMEM) ymemBias[NUM_KER] = { 20,-10 };

//Allocate space to collect output.
int chess_storage(XMEM) xmemActualOutput[CH_OUT * H_IN * W_IN] = {};

int xmemExpectedOutput[CH_OUT * H_IN * W_IN] = { 127,-128,98,-95,127,-128,127,-128 };
#endif
