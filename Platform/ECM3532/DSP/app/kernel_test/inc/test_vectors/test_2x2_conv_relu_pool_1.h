#define W_IN 3 
#define H_IN 3 
#define CH_IN 1 
#define NUM_KER 1 
#define DIM_OUT 1

int chess_storage(XMEM) x[9] = {
8, -8, -10, 7, 11, -4, -4, 2, -4
};

int chess_storage(YMEM) w[4] = {
1, 11, 8, 8
};

int chess_storage(YMEM) bias[1] = {
4
};

int bias_shift = 0;
int out_shift = 0;
int output_activation_min = 0;
int output_activation_max = 127;

int expected_output[DIM_OUT] = {
46
};

