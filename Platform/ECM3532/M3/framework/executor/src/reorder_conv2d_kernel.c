#include <stdint.h>
#include <stdio.h>

//[Cout H W Cin]
/* This function reorders a kernel in the m3 [Cout H W Cin] to the dsp [Cout Cin H W] format.  
 * This will be done by the compiler where by it will produce different kernel formats 
 * for the M3 and the dsp.  But since this is not yet in place, this function has been created.
 * As this fuunction will be removed eventually, no attempt is made to optimize its speed.
 */
void reorder_conv2d_kernel(int8_t *in, int8_t *out, uint16_t H, uint16_t W, uint16_t C_in, uint16_t C_out)
/* in = array of input kernel values in m3 format */
/* out = reordered array of kernel values in dsp format */
/* H = height of kernel */
/* W = width of kernel */
/* C_in = # of channels for a kernel */
/* C_out = # of kernels */
{
  uint16_t kh, kw, kc_in, kc_out;
  uint32_t dsp_linear_address, m3_linear_address;
  for(kc_out = 0; kc_out < C_out; kc_out++){
    for (kh = 0; kh < H; kh++){
      for (kw = 0; kw < W; kw++){
        for (kc_in = 0; kc_in < C_in; kc_in++){
          //dsp[kc_out, kc_in, kh, kw] = m3[kc_out, kh, kw, kc_in]
	  //dsp_linear_address = kw + kh*W + kc_in*H*W + kc_out*C_in*H*W; 
	  dsp_linear_address = kw + W*(kh + H*(kc_in + kc_out*C_in)); 
          //m3_linear_address = kc_in + kw*C_in + kh*C_in*W + k_cout*W*H*C_in;
          m3_linear_address = kc_in + C_in*(kw + W * (kh + kc_out * H)); 
          *(out + dsp_linear_address) = (int8_t)(*(in + m3_linear_address));
	}
      }
    }
  }
}

/*
void main()
{
  int8_t m3_data[18] = {-25, 61, -44, 37, -23, -53, 38, -15, -16, -56, 2, 52, -1, -43, 61, -11, 47, 3};
  int16_t dsp_data[18];
  reorder_conv2d_kernel(m3_data, dsp_data, 3, 3, 1, 2);
  int k1;
  for(k1=0; k1< 18; k1++){
    printf("%d, ", m3_data[k1]);
  }
  printf("\r\n");
  for(k1=0; k1< 18; k1++){
    printf("%d, ", dsp_data[k1]);
  }
  printf("\r\n");
}
*/
