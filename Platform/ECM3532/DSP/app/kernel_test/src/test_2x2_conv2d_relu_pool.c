#include "dsp_config.h"
#ifdef CONFIG_CONV_RELU_AVGPOOL_2X2
#include "cf16_chess.h"
#include <stdio.h>
#include "eta_dsp_nn.h"
#include "mathlib.h"
#include "reg.h"
#include "eta_csp_mailbox.h"
#include "eta_csp_io.h"
//#include "common.h"
#include "test_2x2_conv_relu_pool_1.h"



int chess_storage(XMEM) z[DIM_OUT];
int tmp_buffer[(H_IN -1)*(W_IN -1)];

void  test_conv_relu_avgpool_2X2()
{
  int H_OUT_CONV = H_IN -1;
  int W_OUT_CONV = W_IN -1;
  int SIZE_PER_CHANNEL_OUT_CONV = H_OUT_CONV * W_OUT_CONV;
  int k1;

  etaPrintf("####### executing test %s #######\r\n","test_conv_relu_avgpool_2X2");
 
  
  for(k1 = 0; k1 < 100; k1++){
    eta_convolve_CHW_q7_ker2x2_stride1_pad0_relu_avgPool_2x2_stride2_pad0_dsp(x, W_IN, H_IN, CH_IN, w, NUM_KER, bias, bias_shift, out_shift,  output_activation_min, output_activation_max, tmp_buffer, z);
   
  }

 
  
  int error_count = 0;
  for(k1 = 0; k1 < DIM_OUT; k1++){
    //etaPrintf("%d, %d\r\n", z[k1], expected_output[k1]);
    if(expected_output[k1] > 127){
	    expected_output[k1] = 127;
    }
    if(abs(z[k1]- expected_output[k1]) > 1){
      //etaPrintf("%d, %d\r\n", z[k1], expected_output[k1]);
      error_count += 1;
    }
  }
  etaPrintf("Num errors = %d\r\n", error_count);
 
  
  return ;
}
#endif
