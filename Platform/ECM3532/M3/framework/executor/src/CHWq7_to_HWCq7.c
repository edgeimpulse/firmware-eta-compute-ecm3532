#include <stdint.h>

/* This function converts a tensor of format [CHW] to [HWC]  */
/* The converted input is written to the *out array starting from location start_channel */
/* The assumption is that the input holds fewer channels than the output */
/* Params: */
/* in:  input tensor in CHW format, size = W*H*num_channels */
/* out:  output tensor in HWC format, size = W*H*C */
/* H: tensor height */
/* W: tensor width */
/* C: # of channels for entire tensor */
/* start_channel: location in *out where the converted channels should be placed */
/* num_channels: number of channels for *in */
void CHWq7_to_HWCq7_Ex(int8_t *in, int8_t *out, uint16_t H, uint16_t W, uint16_t C,
		uint16_t start_channel, uint16_t num_channels)
{
  if(num_channels == 1)
  {
    uint16_t inLowBound = 0;
    uint16_t inUppBound = H * W * num_channels;
    uint16_t linearIndex;
    uint16_t outOffset = start_channel;
    uint16_t outStride = C;
  
    //
    // Approach is to traverse the input array and fill.
    // We need to hop by the total number of channels in
    // the output array to get the mapped element once
    // we figure out where to start from (offset).
    //
    for (linearIndex=inLowBound; linearIndex<inUppBound; linearIndex++)
    {
      out[outOffset] = in[linearIndex];
      outOffset += outStride;
    }
  }
  else
  {
    uint16_t kh, kw, kc;
    uint32_t HW = H*W;
    uint32_t WC = W*C;
    uint32_t hin, cin, hout, wout, hwin, hwout;
    int8_t *out1;
    hout = 0;
    hin = 0;
    out1 = out + start_channel;
    for (kh = 0; kh < H; kh++){
      //hout = kh * WC;
      //hin = kh * W;
      wout = 0;
      for (kw = 0; kw < W; kw++){
        //wout = kw * C;
        hwout = hout + wout;
        hwin = hin + kw; 
        cin = 0;
        //for (kc = 0; kc < C; kc++){
        for (kc = 0; kc < num_channels; kc++){
          //cin = kc * HW;
  	//out[kh*W*C + kw*C + kc] = in[kc*H*W + kh*W + kw] 
          *(out1 + kc + hwout) = *(in + cin + hwin);
          cin += HW;
        }
        wout += C;
      }
      hout += WC;
      hin += W;
    }
  }
}
