#ifndef _EXECUTOR_COMMON_H_
#define _EXECUTOR_COMMON_H_

#ifndef __GNUC__

  typedef struct conv2d_relu_avgpool_opt_st
{
  uint16_t in_rows;     // input tensor dimension height
  uint16_t in_cols;     // input tensor dimension width
  uint16_t in_depth;    // number of input tensor channels
  uint16_t num_filt;    // number of filters
  uint16_t bias_lshift; // amount of left-shift for bias
  uint16_t out_rshift;  // amount of right-shift for output
  uint16_t act_min;     // the minimum value of the output range
  uint16_t act_max;     // the maximum value of the output range
} conv2d_relu_avgpool_opt;



#endif
#ifdef  __GNUC__
typedef struct conv2d_pw_st
{
    uint16_t in_rows;     // input tensor dimension height
    uint16_t in_cols;     // input tensor dimension width
    uint16_t in_depth;    // number of input tensor channels
    uint16_t num_filt;    // number of filters
    uint16_t bias_lshift; // amount of left-shift for bias
    uint16_t out_rshift;  // amount of right-shift for output
    int16_t act_min;     // the minimum value of the output range
    int16_t act_max;     // the maximum value of the output range
} conv_pw_opt;
#endif


typedef  struct
{ 
    uint16_t mapped;
    uint16_t offset;
    uint16_t size;
    uint16_t ahbAddrHi;
    uint16_t ahbAddrLo;
}dspShmemBuf;

typedef struct dsp_math_fft_opt_st
{
    uint16_t fft_length;  // input fft length
} tDsp_math_fft_opt;

typedef struct dsp_math_func_opt_st
{
    uint16_t len;  // input length
} tDsp_math_func_opt;

typedef union
{
    conv2d_relu_avgpool_opt convReluPoolParams;
    conv_pw_opt             convPwParams;
    conv2d_opt              conv2dParams;
    tDsp_math_fft_opt       fft_opt;
}tExecutorRpcParams;


typedef struct {
    uint16_t status;
    dspShmemBuf inbuf;
    dspShmemBuf  outbuf;
    dspShmemBuf weight;
    dspShmemBuf bias;
    uint16_t index;
    tExecutorRpcParams params;    
} tExecutorRpcWork;


#endif /*_EXECUTOR_COMMON_H_*/
