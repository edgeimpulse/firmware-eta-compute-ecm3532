#define W_IN 3 
#define H_IN 3 
#define CH_IN 2 
#define NUM_KER 2 
#define DIM_OUT 2

int chess_storage(XMEM) x[18] = {
8, -10, 11, -4, -4, -8, 3, 11, -1, -8, 7, -4, 2, 2, -5, 2, -10, 6
};

int chess_storage(YMEM) w[16] = {
1, 7, -5, -2, 8, -10, 9, -1, 11, -11, -7, 1, 8, 0, -2, 4
};

int chess_storage(YMEM) bias[2] = {
4, -3
};

int bias_shift = 0;
int out_shift = 0;
int output_activation_min = 0;
int output_activation_max = 32767;

int expected_output[DIM_OUT] = {
56, 46
};

