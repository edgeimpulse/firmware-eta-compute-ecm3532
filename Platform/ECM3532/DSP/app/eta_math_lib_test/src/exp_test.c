#include "eta_csp_io.h"
#include "eta_math.h"

#define NUM_TEST_CASES 11

static fix input[NUM_TEST_CASES] = { 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0/32768, 0.9999};

//3.13 output stored as int.
static int output[NUM_TEST_CASES] = {};

//q3.13 output in int array.
static int ref_output[NUM_TEST_CASES] = { 9054,10006,11058,12221,13506,14927,16497,18232,20149,8192,22266 };


static void compare_outputs_int(int *actual, int *expected, int size);

void test_exp(void)
{
    int i;
    etaPrintf("exp test in progress\r\n");

    for (i = 0; i < NUM_TEST_CASES; i++)
    {
        output[i] = to_int(eta_exp(input[i]));
    }

#if 1
    for (i = 0; i < NUM_TEST_CASES; i++)
    {
        etaPrintf("[%d]: in = %d, exp(in)=%d:  ref_output = %d \r\n", 
            i, to_int(input[i]), output[i], ref_output[i] );
    }
#endif    
    compare_outputs_int(ref_output, output, NUM_TEST_CASES);
    etaPrintf("exp test finished \r\n");
}

static void compare_outputs_int(int *actual, int *expected, int size)
{
    int k1;
    int diff_0, diff_1, diff_2, diff_3, diff_gt_3, max_err;
    diff_0 = 0;
    diff_1 = 0;
    diff_2 = 0;
    diff_3 = 0;
    diff_gt_3 = 0;
    max_err = 0;
    etaPrintf("out:\r\n");
    for (k1 = 0; k1 < size; k1++)
    {
        int diff = actual[k1] - expected[k1];
        diff = diff > 0 ? diff : -diff;
        if (diff == 0) diff_0++;
        if (diff == 1) diff_1++;
        if (diff == 2) diff_2++;
        if (diff == 3) diff_3++;
        if (diff > 3) diff_gt_3++;
        if (diff > max_err) max_err = diff;
#if 0
        if (actual[k1] != expected[k1])
        {
            etaPrintf("index: %d exp: %d out: %d\r\n", k1, expected[k1], actual[k1]);
        }
#endif
    }
    etaPrintf("\r\n");
    etaPrintf("Max Error: %d\r\n", max_err);
    etaPrintf("Number of outputs with difference of 0: %d\r\n", diff_0);
    etaPrintf("Number of outputs with difference of 1: %d\r\n", diff_1);
    etaPrintf("Number of outputs with difference of 2: %d\r\n", diff_2);
    etaPrintf("Number of outputs with difference of 3: %d\r\n", diff_3);
    etaPrintf("Number of outputs with difference of more than 3: %d\r\n", diff_gt_3);
}




