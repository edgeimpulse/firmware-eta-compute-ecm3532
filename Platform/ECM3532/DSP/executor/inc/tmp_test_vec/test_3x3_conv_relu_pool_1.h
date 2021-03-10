#define W_IN 4 
#define H_IN 4 
#define CH_IN 1 
#define NUM_KER 1 
#define DIM_OUT 1


int chess_storage(XMEM) x[16] = {
1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16
};

int chess_storage(YMEM) w[9] = {
1,2,3,4,5,6,7,8,9
};

int chess_storage(YMEM) bias[1] = {
-5
};

int bias_shift = 0;
int out_shift = 0;
int output_activation_min = 0;
int output_activation_max = 32767;

int expected_output[DIM_OUT] = {
456
};

