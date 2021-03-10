#include "dsp_config.h"

#ifdef CONFIG_CONV_RELU_3X3_STRIDE1
#include "eta_csp_io.h"

#include "eta_dsp_nn.h"

//#include "layer27.h"
#include "test_3x3_dw_stride1.h"

//#define ENABLE_PRINT  1
#define ENABLE_OUTPUT_COMPARE 1 

#if DW
int chess_storage(YMEM) g_wt [3 * 3 * CH_IN];
#else
int chess_storage(YMEM) g_wt [3 * 3 * CH_IN * DIM_OUT];
#endif

#define LINEAR_INDEX_CHW(kc, kh1, kw1)  ((kc*H1*W1)+(kh1*W1)+(kw1))
#define LINEAR_INDEX_HWC(kh, kw, kc)  ((kh*W*C)+(kw*C)+(kc))
void HWCq7_to_CHWq15_with_pad(char *in, int *out, int H, int W, int C, 
		 int padLeft, int padRight, int padUp, int padDown)
/* (*in) is the input of size H*W*C and has [HWC] format
 * (*out) is the output of size (padUp+H+padDown)*(padLeft+W+padRight) and has [CHW] format 
 * H, W and C are the height, width and channels of (*in). */
{
  int kh, kw, kc, kh1, kw1;
  int hin, win, cin, hout, wout, cout, hwin, hwout;
  int H1 = H + padUp + padDown;
  int W1 = W + padLeft + padRight;
  int foo, foo1;

  for (kh1 = 0; kh1 < H1; kh1++){
    kh = kh1 - padUp;
    for (kw1 = 0; kw1 < W1; kw1++){
      kw = kw1 - padLeft;
      for (kc = 0; kc < C; kc++){
	foo = LINEAR_INDEX_HWC(kh, kw, kc);
	foo1 = LINEAR_INDEX_CHW(kc, kh1, kw1);
	if ((kh >= 0) && (kw >= 0) && (kh < H) && (kw < W)){
          //out[LINEAR_INDEX_CHW(kc, kh1, kw1)] = in[LINEAR_INDEX_HWC(kh, kw, kc)];
	  //etaPrintf("kh1=%d, kw1=%d, kh=%d, kw=%d, foo1=%d, foo=%d\n", kh1, kw1, kh, kw, foo1, foo);
          out[foo1] = (int)in[foo];
	} 
	else {
          //out[LINEAR_INDEX_CHW(kc, kh1, kw1)] = 0;
          out[foo1] = 0;
	}
      }
    }
  }
}

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

void test_eta_conv2d_q7_chw_ker3x3_stride1_pad0_relu(void)
{

  etaPrintf("####### testing eta_conv2d_q7_chw_ker3x3_stride1_pad0_relu #######\r\n");

  int k1;
  int w_in = W_IN;
  int h_in = H_IN;
  int ch_in = CH_IN;
  int dim_out = ch_in;
  int w_out = W_OUT;
  int h_out = H_OUT;

  conv2d_opt_st opt;

  //set_rounding_mode(1);
  //set_saturation_mode(1);

  opt.in_rows = H_IN + 2;
  opt.in_cols = W_IN + 2;
  opt.in_depth = 1;
  opt.num_filt = 1;
  opt.bias_lshift = bias_shift;
  opt.out_rshift = out_shift;
  opt.act_min = act_min;
  opt.act_max = act_max;
  opt.out_rows = H_OUT;
  opt.out_cols = W_OUT;

  int Im_in [(W_IN+2) * (H_IN+2) * CH_IN];

  //HWCq7_to_CHWq15(Im_in_HWC, Im_in, w_in, h_in, ch_in);
  HWCq7_to_CHWq15_with_pad(Im_in_HWC, Im_in, w_in, h_in, ch_in, 1, 1, 1, 1);

  reorder_conv2d_kernel(g_wt_HWC, g_wt, 3, 3, ch_in, 1);

  int out [W_OUT * H_OUT * DIM_OUT + 1];

  char out_HWC [W_OUT * H_OUT * DIM_OUT + 1];

  for(k1 = 0; k1 < (W_OUT * H_OUT * DIM_OUT + 1); k1++)
  {
    out[k1] = 0;
    out_HWC[k1] = 0;
  }

  for(k1 = 0; k1 < dim_out; k1++)
  {
    eta_conv2d_q7_chw_ker3x3_stride1_pad0_relu
    (
      &Im_in[(w_in+2)*(h_in+2)*k1],
      &g_wt[9*k1],
      &g_bias[k1],
      &opt,
      &out[w_out*h_out*k1]
    );
  }

  CHWq15_to_HWCq7(out, out_HWC, w_out, h_out, dim_out);

  int diff_0, diff_1, diff_2, diff_3, diff_gt_3, max_err;
  diff_0 = 0;
  diff_1 = 0;
  diff_2 = 0;
  diff_3 = 0;
  diff_gt_3 = 0;
  max_err = 0;
  etaPrintf("out:\r\n");
  for(k1 = 0; k1 < (w_out*h_out*dim_out); k1++)
  {
    int diff = out_HWC[k1] - pExpect[k1];
    diff = diff > 0 ? diff : -diff;
    if(diff == 0) diff_0++;
    if(diff == 1) diff_1++;
    if(diff == 2) diff_2++;
    if(diff == 3) diff_3++;
    if(diff > 3) diff_gt_3++;
    if(diff > max_err) max_err = diff;
    if(out_HWC[k1] != pExpect[k1])
    {
      //etaPrintf("ERROR! exp: %d out: %d\r\n", pExpect[k1], out_HWC[k1]);
    }
  }
  /*etaPrintf("out:\r\n");
  for(k1 = 0; k1 < (w_out*h_out*dim_out + 1); k1++)
  {
    etaPrintf("%d, ", out_HWC[k1]);
  }
  etaPrintf("out exp\r\n");
  for(k1 = 0; k1 < (w_out*h_out*10 + 1); k1++)
  {
    etaPrintf("%d, %d\r\n", out_HWC[k1], pExpect[k1]);
  }*/
  //etaPrintf("\r\n");
  etaPrintf("Max Error: %d\r\n", max_err);
  etaPrintf("Number of outputs with difference of 0: %d\r\n", diff_0 );
  etaPrintf("Number of outputs with difference of 1: %d\r\n", diff_1 );
  etaPrintf("Number of outputs with difference of 2: %d\r\n", diff_2);
  etaPrintf("Number of outputs with difference of 3: %d\r\n", diff_3);
  etaPrintf("Number of outputs with difference of more than 3: %d\r\n", diff_gt_3);
  etaPrintf("Total outputs: %d\r\n", w_out*h_out*dim_out);

}

#endif  //CONFIG_CONV_RELU_3X3_STRIDE1
