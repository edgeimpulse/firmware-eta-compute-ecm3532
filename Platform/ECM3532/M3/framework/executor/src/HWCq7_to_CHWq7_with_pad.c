#include <stdint.h>
#include <stdio.h>

/* This function converts a tensor of format [HWC] to [CHW] and also adds zeros to pad the output data */
/* With this padding, dsp conv functions can be called with pad = 0 to process the data */
/* The conversion is done only on the specified channels starting from start_channel to 
 * (start_channel+num_channels-1) */
void HWCq7_to_CHWq7_with_pad_partial_channels(int8_t *in, int8_t *out, uint16_t H, uint16_t W, uint16_t C, 
		 uint16_t padLeft, uint16_t padRight, uint16_t padUp, uint16_t padDown, 
		 uint16_t start_channel, uint16_t num_channels)
/* (*in) is the input of size H*W*C and has [HWC] format
 * (*out) is the output of size (padUp+H+padDown)*(padLeft+W+padRight)*num_channels and has [CHW] format 
 * H, W and C are the height, width and channels of (*in). 
 * start_channel is the first output channel.
 * */
{
  int16_t kh, kw, kc, kh1, kw1;
  uint32_t H1 = H + padUp + padDown; //height with pad
  uint32_t W1 = W + padLeft + padRight; //width with pad
  int32_t inIndex0, outIndex0, inIndex1, outIndex1, inIndex2, outIndex2;

  //outIndex0 = 0;
  for (kh1 = 0; kh1 < padUp; kh1++)
  {
    outIndex0 = kh1*W1;
    for (kw1 = 0; kw1 < W1; kw1++)
    {
      outIndex1 = outIndex0 + kw1;
      for (kc = start_channel; kc < (start_channel+num_channels); kc++)
      {
	outIndex2 = outIndex1 + (kc-start_channel)*H1*W1;
        out[outIndex2] = 0;
      }
    }
    //outIndex0 += W1;
  }

  for (kh1 = 0+padUp; kh1 < H1-padDown; kh1++)
  {
    outIndex0 = kh1*W1;
    for (kw1 = 0; kw1 < padLeft; kw1++)
    {
      outIndex1 = outIndex0 + kw1;
      for (kc = start_channel; kc < (start_channel+num_channels); kc++)
      {
	outIndex2 = outIndex1 + (kc-start_channel)*H1*W1;
        out[outIndex2] = 0;
      }
    }
  }

  for (kh1 = H1-padDown; kh1 < H1; kh1++)
  {
    outIndex0 = kh1*W1;
    for (kw1 = 0; kw1 < W1; kw1++)
    {
      outIndex1 = outIndex0 + kw1;
      for (kc = start_channel; kc < (start_channel+num_channels); kc++)
      {
	outIndex2 = outIndex1 + (kc-start_channel)*H1*W1;
        out[outIndex2] = 0;
      }
    }
  }

  for (kh1 = 0+padUp; kh1 < H1-padDown; kh1++)
  {
    outIndex0 = kh1*W1;
    for (kw1 = W1-padRight; kw1 < W1; kw1++)
    {
      outIndex1 = outIndex0 + kw1;
      for (kc = start_channel; kc < (start_channel+num_channels); kc++)
      {
	outIndex2 = outIndex1 + (kc-start_channel)*H1*W1;
        out[outIndex2] = 0;
      }
    }
  }

  if(num_channels == 1)
  {
    uint32_t vStep = padLeft + W + padRight;
    uint32_t outHight = padUp + H + padDown;
                    //e.g. 0 + (1*66) + 1 = 67
    uint32_t outLowBound = 0 + (padUp * vStep) + padLeft;
                                                 // e.g.
    uint32_t outUppBound = (vStep * outHight) -  // (66*66)-
                           (vStep * padDown)  -  //  (66*1)-
                            padRight;            //      1 = 4289
    uint32_t linearIndex;
    uint32_t vOffset;
    uint32_t inOffset = start_channel;
    uint32_t inStride = C;
  
    //
    // Approach is to traverse the output array and fill.
    // We need to hop by the total number of channels in
    // the input array to get the mapped element once
    // we figure out where to start from (offset).
    //
    for (vOffset=outLowBound; vOffset<outUppBound; vOffset+=vStep)
    {
      for(linearIndex=vOffset; linearIndex<(vOffset+W); linearIndex++)
      {
        out[linearIndex] = in[inOffset];
        inOffset += inStride;
      }
    }
  }
  else
  {
    for (kh1 = 0+padUp; kh1 < H1-padDown; kh1++){
      kh = kh1 - padUp;
      outIndex0 = kh1*W1;
      inIndex0  = kh*W*C;
      for (kw1 = 0+padLeft; kw1 < W1-padRight; kw1++){
        kw = kw1 - padLeft;
        outIndex1 = outIndex0 + kw1;
        inIndex1  = inIndex0  + kw*C; 
        for (kc = start_channel; kc < (start_channel+num_channels); kc++){
  	outIndex2 = outIndex1 + (kc-start_channel)*H1*W1;
  	//if ((kh >= 0) && (kw >= 0) && (kh < H) && (kw < W)){
  	  inIndex2 = inIndex1 + kc;
            out[outIndex2] = in[inIndex2];
  	//} 
  	//else {
          //  out[outIndex2] = 0;
  	//}
        }
      }
    }
  }
}
