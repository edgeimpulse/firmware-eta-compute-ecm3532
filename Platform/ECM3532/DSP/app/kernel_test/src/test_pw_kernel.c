#include "dsp_config.h"

#ifdef CONFIG_PW_CONV_FAST
#include "eta_csp_io.h"

#include "eta_dsp_nn.h"

//#include "pw_test1.h"
//#include "pw_test2.h"
//#include "pw_test3.h"

#include "test_row36_1x1_pw_conv2d_q7.h"

#define ENABLE_PRINT  1
#define ENABLE_OUTPUT_COMPARE 1 

#if ENABLE_OUTPUT_COMPARE

static void compare_outputs(int *actual, int *expected, int size)
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
#endif //ENABLE_OUTPUT_COMPARE

static void fillconvolutionOptions(conv_pw_opt* opt)
{
    opt->in_rows = H_IN;
    opt->in_cols = W_IN;
    opt->in_depth = CH_IN;
    opt->num_filt = NUM_KER;
    opt->bias_lshift = bias_shift;
    opt->out_rshift = out_shift;
    opt->act_min = -128;
    opt->act_max = 127;
}

void test_pw_conv2d_q7_old(void)
{
    int error_count = 0;
    int maxDeviation = 0;
    int deviation;
    conv_pw_opt pw_opt;
    int k1;

    fillconvolutionOptions(&pw_opt);
#if CHW_FORMAT
    eta_pw_conv2d_q7_chw(xmemInput, ymemWeight, ymemBias, &pw_opt, xmemActualOutput);

#if ENABLE_PRINT
    etaPrintf("first few output of eta_pw_conv2d_q7_chw() output[ ]\n");
    for (k1 = 0; k1 < DIM_OUT; k1++) 
    {
        etaPrintf("%d,", xmemActualOutput[k1]);
        if (k1 == 9) break;  //print max 10 outputs only.
    }
#endif
#if ENABLE_OUTPUT_COMPARE
    compare_outputs(xmemActualOutput, xmemExpectedOutput, DIM_OUT);
#endif //ENABLE_OUTPUT_COMPARE

#else //CHW_FORMAT
    eta_pw_conv2d_q7_hwc(xmemInput, ymemWeight, ymemBias, &pw_opt, xmemActualOutput);
#if ENABLE_PRINT
    etaPrintf("first few output of eta_pw_conv2d_q7_hwc() output[ ]\n");
    for (k1 = 0; k1 < DIM_OUT; k1++)
    {
        etaPrintf("%d,", xmemActualOutput[k1]);
        if (k1 == 9) break;  //print max 10 outputs only.
    }
#endif
#if ENABLE_OUTPUT_COMPARE
    compare_outputs(xmemActualOutput, xmemExpectedOutput, DIM_OUT);
#endif //ENABLE_OUTPUT_COMPARE

    //check if input image width is even and number of kernels is even
    //etaPrintf("\n\rWidth=%d, Num_Ker=%d \n\r",W_IN, NUM_KER);
    if (((W_IN & 1) == 0) && ((NUM_KER & 1) == 0))
    {
#if 1	   
	int numInputPoints = (pw_opt.in_rows * pw_opt.in_cols)/2;

        for (k1 = 0; k1 < DIM_OUT; k1++)
        {
            xmemActualOutput[k1] = 0; //reset output array
        }

        etaPrintf("\n\r calling partial pw with first half set of input");	
        eta_partial_input_pw_conv2d_q7_hwc_2mac(xmemInput, ymemWeight, ymemBias, &pw_opt,
        numInputPoints, xmemActualOutput);

        etaPrintf("\n\r calling partial pw with second half set of input\n\r");	
        eta_partial_input_pw_conv2d_q7_hwc_2mac(xmemInput+(numInputPoints * pw_opt.in_depth), ymemWeight, ymemBias, &pw_opt,
        numInputPoints, xmemActualOutput+(numInputPoints*pw_opt.num_filt));
#else	
        //eta_pw_conv2d_q7_hwc_2mac_schedule(xmemInput, ymemWeight, ymemBias, &pw_opt, xmemActualOutput);
#endif    
#if ENABLE_PRINT
        etaPrintf("first few output of partial eta_pw_conv2d_q7_hwc_2mac_schedule() output[ ]\n");
        for (k1 = 0; k1 < DIM_OUT; k1++)
        {
            etaPrintf("%d,", xmemActualOutput[k1]);
            if (k1 == 9) break;  //print max 10 outputs only.
        }
#endif
#if ENABLE_OUTPUT_COMPARE
        compare_outputs(xmemActualOutput, xmemExpectedOutput, DIM_OUT);
#endif //ENABLE_OUTPUT_COMPARE
    }
#endif ////CHW_FORMAT
}

void test_pw_conv2d_q7(void)
{
    int error_count = 0;
    int maxDeviation = 0;
    int deviation;
    conv_pw_opt pw_opt;
    int k1;
    int numInputPoints;
    int numFilters;

    fillconvolutionOptions(&pw_opt);
    numInputPoints = (pw_opt.in_rows * pw_opt.in_cols) / 2;
    for (k1 = 0; k1 < DIM_OUT; k1++)
    {
        xmemActualOutput[k1] = 0; //reset output array
    }

#if PARTIAL_KERNEL_PW_KERNEL_TEST
    etaPrintf("\n\r calling partial pw with first half set of kernels");
    eta_partial_kernels_pw_conv2d_q7_hwc_2mac(xmemInput, ymemWeight, ymemBias, &pw_opt, pw_opt.num_filt / 2, xmemActualOutput);


    etaPrintf("\n\r calling partial pw with second half set of kernels");
    eta_partial_kernels_pw_conv2d_q7_hwc_2mac(xmemInput,
        ymemWeight + ((pw_opt.num_filt / 2) * (pw_opt.in_depth)), ymemBias + pw_opt.num_filt / 2,
        &pw_opt, pw_opt.num_filt / 2,
        xmemActualOutput + ((pw_opt.in_rows * pw_opt.in_cols) * (pw_opt.num_filt / 2)));

    etaPrintf("\n\r first few output of partial eta_partial_kernels_pw_conv2d_q7_hwc_2mac() output[ ]\n\r");
#else
    etaPrintf("\n\r calling partial pw with first half set of input");
    eta_partial_input_pw_conv2d_q7_hwc_2mac(xmemInput, ymemWeight, ymemBias, &pw_opt,
        numInputPoints, xmemActualOutput);

    etaPrintf("\n\r calling partial pw with second half set of input\n\r");
    eta_partial_input_pw_conv2d_q7_hwc_2mac(xmemInput + (numInputPoints * pw_opt.in_depth), ymemWeight, ymemBias, &pw_opt,
        numInputPoints, xmemActualOutput + (numInputPoints * pw_opt.num_filt));

    etaPrintf("\n\r first few output of partial eta_partial_input_pw_conv2d_q7_hwc_2mac() output[ ]\n\r");
#endif

    for (k1 = 0; k1 < DIM_OUT; k1++)
    {
        etaPrintf("%d,", xmemActualOutput[k1]);
        if (k1 == 9) break;  //print max 10 outputs only.
    }

#if ENABLE_OUTPUT_COMPARE
    compare_outputs(xmemActualOutput, xmemExpectedOutput, DIM_OUT);
#endif //ENABLE_OUTPUT_COMPARE
}


#endif  //CONFIG_PW_CONV_FAST

