#define W_IN 3 
#define H_IN 3 
#define CH_IN 1 
#define NUM_KER 2 
#define DIM_OUT 2

int chess_storage(XMEM) x[9] = {
8, -8, -10, 7, 11, -4, -4, 2, -4
};

int chess_storage(YMEM) w[8] = {
1, 8, 7, -10, 11, 8, -11, 0
};

int chess_storage(YMEM) bias[2] = {
4, -3
};

int bias_shift = 0;
int out_shift = 0;
int output_activation_min = 0;
int output_activation_max = 32767;

int expected_output[DIM_OUT] = {
30, 68
};

