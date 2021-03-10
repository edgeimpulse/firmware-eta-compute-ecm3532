#include "dsp_config.h"

#ifdef CONFIG_CONV_RELU_2X2_STRIDE2
#include "eta_csp_io.h"

#include "eta_dsp_nn.h"

#include "test_eta_convolve_CHW_q7_ker2x2_stride2_pad0_relu_dsp.h"

#define ENABLE_OUTPUT_COMPARE 1

#define ROUNDOFF_BEFORE_SHIFT
#define KER_DIM 2
#define KER_SIZE KER_DIM*KER_DIM

static int chess_storage(YMEM) g_wt [2048];

static void HWCq7_to_CHWq15(char *in, int *out, int H, int W, int C)
{
  int kh, kw, kc;
  int HW = H*W;
  int WC = W*C;
  int hin, win, cin, hout, wout, cout, hwin, hwout;
  for (kh = 0; kh < H; kh++){
    hin = kh * WC;
    hout = kh * W;
    for (kw = 0; kw < W; kw++){
      win = kw * C;
      //wout = kw;
      hwin = hin + win;
      hwout = hout + kw; //wout;
      for (kc = 0; kc < C; kc++){
        //cin = kc;
        cout = kc * HW;
        *(out + cout + hwout) = (int)(*(in + kc + hwin));
      }
    }
  }
}

static void reorder_conv2d_kernel(char _YMEM *in, int _YMEM *out, int H, int W, int C_in, int C_out)
/* in = array of input kernel values in m3 format */
/* out = reordered array of kernel values in dsp format */
/* H = height of kernel */
/* W = width of kernel */
/* C_in = # of channels for a kernel */
/* C_out = # of kernels */
{
  int kh, kw, kc_in, kc_out;
  int dsp_linear_address, m3_linear_address;
  for(kc_out = 0; kc_out < C_out; kc_out++){
    for (kh = 0; kh < H; kh++){
      for (kw = 0; kw < W; kw++){
        for (kc_in = 0; kc_in < C_in; kc_in++){
          //dsp[kc_out, kc_in, kh, kw] = m3[kc_out, kh, kw, kc_in]
          //dsp_linear_address = kw + kh*W + kc_in*H*W + kc_out*C_in*H*W; 
          dsp_linear_address = kw + W*(kh + H*(kc_in + kc_out*C_in));
          //m3_linear_address = kc_in + kw*C_in + kh*C_in*W + k_cout*W*H*C_in;
          m3_linear_address = kc_in + C_in*(kw + W * (kh + kc_out * H));
          *(out + dsp_linear_address) = (int)(*(in + m3_linear_address));
        }
      }
    }
  }
}

static void CHWq15_to_HWCq7(int *in, char *out, int H, int W, int C)
{
  int kh, kw, kc;
  int HW = H*W;
  int WC = W*C;
  int hin, win, cin, hout, wout, cout, hwin, hwout;
  for (kh = 0; kh < H; kh++){
    hout = kh * WC;
    hin = kh * W;
    for (kw = 0; kw < W; kw++){
      wout = kw * C;
      //win = kw;
      hwout = hout + wout;
      hwin = hin + kw; //win;
      for (kc = 0; kc < C; kc++){
        //cout = kc;
        cin = kc * HW;
        *(out + kc + hwout) = (char)(*(in + cin + hwin));
      }
    }
  }
}

void test_eta_convolve_CHW_q7_ker2x2_stride2_pad0_relu_dsp(void)
{
  fix a, b , c;
  acc d;
  int k1, z, z1;
  double y;

  //set_rounding_mode(1);
  //set_saturation_mode(1);

  int Im_in [64];


  HWCq7_to_CHWq15(Im_in_HWC, Im_in, W_IN, H_IN, CH_IN);

  reorder_conv2d_kernel(g_wt_HWC, g_wt, KER_DIM, KER_DIM, CH_IN, DIM_OUT);

  int out [DIM_OUT * W_OUT * H_OUT];

  eta_convolve_CHW_q7_ker2x2_stride2_pad0_relu_dsp(&Im_in[0],
                                              W_IN, // input dim x
                                              H_IN, // input dim y
                                              CH_IN, // ch_in
                                              &g_wt[0],
                                              DIM_OUT, // ch_out
                                              &g_bias[0],
                                              bias_shift,
                                              out_shift,
                                              output_activation_min,
                                              output_activation_max,
                                              &out[0],
                                              W_OUT, // output dim x
                                              H_OUT  // output dim y
                                             );

#if ENABLE_OUTPUT_COMPARE
  char out_HWC [DIM_OUT * W_OUT * H_OUT];
  CHWq15_to_HWCq7(out, out_HWC, W_OUT, H_OUT, DIM_OUT);


  int diff_0, diff_1, diff_2, diff_3, diff_gt_3, max_err;
  diff_0 = 0;
  diff_1 = 0;
  diff_2 = 0;
  diff_3 = 0;
  diff_gt_3 = 0;
  max_err = 0;
  etaPrintf("out:\r\n");
  for(k1 = 0; k1 < (DIM_OUT * W_OUT * H_OUT); k1++)
  {
    int diff = out_HWC[k1] - exp_HWC[k1];
    diff = diff > 0 ? diff : -diff;
    if(diff == 0) diff_0++;
    if(diff == 1) diff_1++;
    if(diff == 2) diff_2++;
    if(diff == 3) diff_3++;
    if(diff > 3) diff_gt_3++;
    if(diff > max_err) max_err = diff;
    if(out_HWC[k1] != exp_HWC[k1])
    {
      etaPrintf("index: %d exp: %d out: %d\r\n", k1, exp_HWC[k1], out_HWC[k1]);
    }
  }
  etaPrintf("out:\r\n");
  for(k1 = 0; k1 < (DIM_OUT * W_OUT * H_OUT); k1++)
  {
    etaPrintf("%d, ", out_HWC[k1]);
  }
  etaPrintf("\r\n");
  etaPrintf("Max Error: %d\r\n", max_err);
  etaPrintf("Number of outputs with difference of 0: %d\r\n", diff_0 );
  etaPrintf("Number of outputs with difference of 1: %d\r\n", diff_1 );
  etaPrintf("Number of outputs with difference of 2: %d\r\n", diff_2);
  etaPrintf("Number of outputs with difference of 3: %d\r\n", diff_3);
  etaPrintf("Number of outputs with difference of more than 3: %d\r\n", diff_gt_3);
#endif

}

#endif
