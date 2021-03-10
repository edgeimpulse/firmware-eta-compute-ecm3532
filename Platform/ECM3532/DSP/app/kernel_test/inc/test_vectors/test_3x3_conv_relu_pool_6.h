#define W_IN 4 
#define H_IN 4 
#define CH_IN 2 
#define NUM_KER 1 
#define DIM_OUT 1

int chess_storage(XMEM) x[32] = {
8, -10, 11, -4, -4, -8, 3, 11, -1, -9, 9, 2, 11, -11, -2, 1, -8, 7, -4, 2, 2, -5, 2, -10, 6, 4, -3, 8, 5, 3, -5, 4
};

int chess_storage(YMEM) w[18] = {
1, 8, 7, -10, -5, 9, -2, -1, 11, 11, 8, -11, 0, -7, -2, 1, 4, 1
};

int chess_storage(YMEM) bias[1] = {
4
};

int bias_shift = 0;
int out_shift = 0;

int out_min = 0;
int out_max = 32767;

int expected_output[1] = {
127
};

