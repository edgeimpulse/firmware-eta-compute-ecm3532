#define W_IN 4
#define H_IN 4
#define CH_IN 4 

#define NUM_KER 2
#define CH_OUT NUM_KER

#define DIM_OUT (CH_OUT * H_IN * W_IN)

#include "eta_types.h"

int bias_shift = 3;
int out_shift = 0;
int output_activation_min = -128;
int output_activation_max = 127;

#define CHW_FORMAT  0  //enable chw format else whc format.

#if CHW_FORMAT
//input in [channel][rows][columns] format
int chess_storage(XMEM) xmemInput[CH_IN * H_IN * W_IN] = { 1,2,3,4,5,6,7,8,9,8,7,6,5,4,3,2,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,6,7,8,1,2,3,4,10,11,12,13,9,8,7,6,21,22,23,24,25,26,27,28,33,34,35,36,29,30,31,32 };

// weight for each input channel scale factor.
int chess_storage(YMEM) ymemWeight[NUM_KER * CH_IN] = { 1,1,1,1,  -1,-1,-1,-1 };

// bias for each input channel bias/offset
int chess_storage(YMEM) ymemBias[NUM_KER] = {1,1};

//Allocate space to collect output.
int chess_storage(XMEM) xmemActualOutput[CH_OUT * H_IN * W_IN] = {};

/* expected  output  */
int xmemExpectedOutput_NoOutputShift[CH_OUT * H_IN * W_IN] = { 36,39,42,45,41,44,47,50,63,64,65,66,55,54,53,52,    -20,-23,-26,-29, -25,-28,-31,-34,-47,-48,-49,-50,-39,-38,-37,-36 };

// expected output with rshit=3 and rounding
int xmemExpectedOutput[CH_OUT * H_IN * W_IN] = { 36,39,42,45,41,44,47,50,63,64,65,66,55,54,53,52,    -20,-23,-26,-29, -25,-28,-31,-34,-47,-48,-49,-50,-39,-38,-37,-36 };

#else
//WHC format
int chess_storage(XMEM) xmemInput[CH_IN * H_IN * W_IN] = { 1,1,5,21,  2,1,6,22,  3,1,7,23,  4,1,8,24,    5,2,1,25,  6,2,2,26,  7,2,3,27,  8,2,4,28,   9,3,10,33,  8,3,11,34,  7,3,12,35,  6,3,13,36,  5,4,9,29,  4,4,8,30,  3,4,7,31,  2,4,6,32 };

int chess_storage(YMEM) ymemWeight[NUM_KER * CH_IN] = { 1,1,1,1,   -1,-1,-1,-1 };


// bias for each input channel bias/offset
int chess_storage(YMEM) ymemBias[NUM_KER] = { 1,1 };

//Allocate space to collect output.
int chess_storage(XMEM) xmemActualOutput[CH_OUT * H_IN * W_IN] = {};

int xmemExpectedOutput_NoOutputShift[CH_OUT * H_IN * W_IN] = { 36,-20,39,-23,42,-26,45,-29,41,-25,44,-28,47,-31,50,-34,63,-47,64,-48,65,-49,66,-50,55,-39,54,-38,53,-37,52,-36 };

int xmemExpectedOutput[CH_OUT * H_IN * W_IN] = { 36,-20,39,-23,42,-26,45,-29,41,-25,44,-28,47,-31,50,-34,63,-47,64,-48,65,-49,66,-50,55,-39,54,-38,53,-37,52,-36 };
#endif
