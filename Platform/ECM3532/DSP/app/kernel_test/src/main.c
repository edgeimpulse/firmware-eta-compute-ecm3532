#include "cf16_chess.h"
#include <stdio.h>
#include "eta_dsp_nn.h"
#include "mathlib.h"
#include "reg.h"
#include "eta_csp_mailbox.h"
#include "eta_csp_io.h"
#include "dsp_config.h"
#include "kernel_tests.h"


int main()
{
  EtaCspIoPrintfViaDspUart(true); //enable prints
  etaPrintf("####### In kernel unit test #######\r\n");
  set_saturation_mode(1);
  #ifdef CONFIG_CONV_RELU_AVGPOOL_2X2
  	test_conv_relu_avgpool_2X2();
  #endif
  #ifdef CONFIG_CONV_RELU_AVGPOOL_3X3
  	test_conv_relu_avgpool_3X3();
  #endif

  #ifdef CONFIG_PW_CONV_FAST
    test_pw_conv2d_q7();
  #endif
    
  #ifdef CONFIG_CONV_RELU_3X3_STRIDE2
    //test_eta_convolve_CHW_q7_ker3x3_stride2_pad0_relu_dsp();
    test_eta_conv2d_q7_chw_ker3x3_stride2_pad0_relu();
  #endif
    
  #ifdef CONFIG_CONV_RELU_2X2_STRIDE2
    //test_eta_convolve_CHW_q7_ker2x2_stride2_pad0_relu_dsp();
    test_eta_conv2d_q7_chw_ker2x2_stride2_pad0_relu();
  #endif
    
  #ifdef CONFIG_CONV_RELU_3X3_STRIDE1
    test_eta_conv2d_q7_chw_ker3x3_stride1_pad0_relu();
  #endif
    
  while(1){};
  
  return 0;
}
