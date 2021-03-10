#define W_IN 3
#define H_IN 3
#define CH_IN 2
#define NUM_KER 2
#define CH_OUT NUM_KER

#define DIM_OUT (CH_OUT * H_IN * W_IN)

#include "eta_types.h"

int bias_shift = 2;
int out_shift = 3;
int output_activation_min = -128;
int output_activation_max = 127;

#define CHW_FORMAT  1  //enable chw format else whc format.


#if CHW_FORMAT
//input in [channel][rows][columns] format
int chess_storage(XMEM) xmemInput[CH_IN * H_IN * W_IN] = { 1,2,3,4,5,6,7,8,9,
                                                          -1,-1,-1,2,3,4,-10,-11,-12};

// weight for each input channel scale factor.
int chess_storage(YMEM) ymemWeight[NUM_KER * CH_IN] = { 2,2,
                                                        2,-2};

// bias for each input channel bias/offset
int chess_storage(YMEM) ymemBias[NUM_KER] = {1,2};

//Allocate space to collect output.
int chess_storage(XMEM) xmemActualOutput[CH_OUT * H_IN * W_IN] = {};

/* expected  output  */
int xmemExpectedOutput_NoOutputShift[CH_OUT * H_IN * W_IN] = {4,6,8,16,20,24,-2,-2,-2,
                                                12,14,16,12,12,12,42,46,50};

// expected output with rshit=3 and rounding
int xmemExpectedOutput[CH_OUT * H_IN * W_IN] = {1,1,1,2,3,3,0,0,0,
                                                2,2,2,2,2,2,5,6,6};

#else
//WHC format
//input in [channel][rows][columns] format
//int chess_storage(XMEM) xmemInput[CH_IN * H_IN * W_IN] = { 1,2,3,4,5,6,7,8,9,
//                                                          -1,-1,-1,2,3,4,-10,-11,-12 };
int chess_storage(XMEM) xmemInput[CH_IN * H_IN * W_IN] = { 1,-1,2,-1,3,-1,4,2,5,3,6,4,7,-10,8,-11,9,-12 };

// weight for each input channel scale factor.
//int chess_storage(YMEM) ymemWeight[NUM_KER * CH_IN] = { 2,2,
//                                                        2,-2 };
int chess_storage(YMEM) ymemWeight[NUM_KER * CH_IN] = { 2,2,2,-2 };



// bias for each input channel bias/offset
int chess_storage(YMEM) ymemBias[NUM_KER] = { 1,2 };

//Allocate space to collect output.
int chess_storage(XMEM) xmemActualOutput[CH_OUT * H_IN * W_IN] = {};

/* expected  output  */
//int xmemExpectedOutput_NoOutputShift[CH_OUT * H_IN * W_IN] = { 4,6,8,16,20,24,-2,-2,-2,
//                                                12,14,16,12,12,12,42,46,50 };
int xmemExpectedOutput_NoOutputShift[CH_OUT * H_IN * W_IN] = { 4,12 ,6,14 ,8,16 ,16,12 ,20,12 ,24,12 ,-2,42 ,-2,46 ,-2,50 };

// expected output with rshit=3 and rounding
//int xmemExpectedOutput[CH_OUT * H_IN * W_IN] = { 1,1,1,2,3,3,0,0,0,
//                                                2,2,2,2,2,2,5,6,6 };

int xmemExpectedOutput[CH_OUT * H_IN * W_IN] = { 1,2 ,1,2 ,1,2 ,2,2 ,3,2 ,3,2 ,0,5 ,0,6 ,0,6 };


#endif
