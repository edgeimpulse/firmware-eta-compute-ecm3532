#ifndef _EXECUTOR_PUBLIC_H_
#define _EXECUTOR_PUBLIC_H_
#include <stdint.h>
#include "executor_types.h"

#define EXEC_MAP_ROM(X)             ExecAllocMem(&X,&incbin_ ## X ## _start,0) // do this for all weights and biases'
#define EXEC_UNMAP_ROM(X)           ExecFreeMem(&X)
#define EXEC_ALLOC_MEM(X)                       ExecAllocMem(&X, 0, 0 )
#define EXEC_MAP_TO_IOMEM(X,IOMEM)          do { X.origin= OPD_ORIG_IO;  ExecAllocMem(&X, IOMEM, 0 ); } while(0);
#define EXEC_REMAP_TO_IOMEM(X,IOMEM)        do { X.origin= OPD_ORIG_IO;  ExecReMapMem(&X, IOMEM, 0 ); } while(0);
#define EXEC_FREE_MEM(X)                                             ExecFreeMem(&X)
#define ARRAY_PTR(X)                                                     (int8_t*)&incbin_ ## X ## _start
#define GET_BUFF_ADDR(X)                                            ExecGetBufAddr(&X)
#define NUM_ELEMENTS(X)                                             (X.numElements)
#define NUM_BYTES(X)                                                   ( (X.numElements) * (X.basetypeSize +1))

#define DECL_BUF_M3_SCRATCH( X,N)                            ExecOperand_t X = {.origin= OPD_ORIG_CPU, .accessType = OPD_ACCESS_TYPE_RW,.memType=OPD_MEM_TYPE_M3_LOCAL,.basetypeSize=OPD_BASE_SIZE_BYTES_1,.memScope=OPD_MEM_SCOPE_OPERATION,.numElements=N};
#define DECL_BUF_M3_PERSISTENT( X,N)                     ExecOperand_t X = {.origin= OPD_ORIG_CPU, .accessType = OPD_ACCESS_TYPE_RW,.memType=OPD_MEM_TYPE_M3_LOCAL,.basetypeSize=OPD_BASE_SIZE_BYTES_1,.memScope=OPD_MEM_SCOPE_GLOBAL,.numElements=N};
#define DECL_CONST_INT_ROM( X,N)                             ExecOperand_t X = {.origin= OPD_ORIG_INT_PERSISTENT_MEM, .accessType = OPD_ACCESS_TYPE_RO,.memType=OPD_MEM_TYPE_M3_LOCAL,.basetypeSize=OPD_BASE_SIZE_BYTES_1,.memScope=OPD_MEM_SCOPE_GLOBAL,.numElements=N};\
                                                                                         INCBIN_ETA(X);


#define EXEC_HW_ID_M3                                           0
#define EXEC_HW_ID_DSP                                         1
#define NO_INPUT_ALLOC
#define NO_OUTPUT_ALLOC
#define WAIT_4_COMPLETION()                                 ExecWaitForCompletion()

ExecStatus ExecInit (void);


// Kernel functions on M3 : Limited now  only for CIfar 10
ExecStatus Exec_conv2d_q7 ( uint8_t execHwId, ExecOperand_t * inArray, ExecOperand_t * wt, ExecOperand_t * bias, ExecOperand_t * outArray, ExecOperand_t * buffIn, const conv2d_opt *opt);
ExecStatus Exec_ds_conv2d_q7 ( uint8_t execHwId, ExecOperand_t * inArray, ExecOperand_t * wt, ExecOperand_t * bias, ExecOperand_t * outArray, ExecOperand_t * buffIn, const conv2d_opt * opt);
ExecStatus Exec_ds_conv2d_q7_inplace ( uint8_t execHwId, ExecOperand_t * inOutArray, ExecOperand_t * wt, ExecOperand_t * bias, ExecOperand_t * buffIn, const conv2d_opt * opt);

ExecStatus Exec_avepool2d_q7( uint8_t execHwId, ExecOperand_t * inArray, ExecOperand_t  * outArray, pool2d_opt *opt);

void  Exec_relu_q7( uint8_t execHwId, ExecOperand_t * data, uint16_t size);

ExecStatus Exec_sigmoid_q7 ( uint8_t execHwId, ExecOperand_t * inArray, int size, int width);

ExecStatus Exec_pw_ds_conv2d_layer4and5_q7 ( uint8_t execHwId, ExecOperand_t * inArray,
                                                ExecOperand_t * wt_pw, ExecOperand_t * wt_ds,
                                                ExecOperand_t * bias_pw, ExecOperand_t * bias_ds,
                                                ExecOperand_t * outArray,
                                                ExecOperand_t * buffpsIn,
                                                ExecOperand_t * buffdsIn,
                                                ExecOperand_t * buffpw2ds,
                                                ExecOperand_t * buffoutArray,
                                                const conv2d_opt * opt_pw,
                                                const conv2d_opt * opt_ds);

ExecStatus Exec_ds_conv2d_sb_layer3_q7 ( uint8_t execHwId, ExecOperand_t * inOutArray,
                                        ExecOperand_t * wt, ExecOperand_t * bias,
                                        ExecOperand_t * buffIn, const conv2d_opt * opt);
ExecStatus Exec_concat_height_q7 ( uint8_t execHwId,
                            ExecOperand_t * inArray1, ExecOperand_t * inArray2,
                            ExecOperand_t *outArray, concat_opt *opt);

//HWC format
ExecStatus Exec_pw_conv2d_q7 ( uint8_t execHwId, ExecOperand_t * inArray, ExecOperand_t * wt, ExecOperand_t * bias, ExecOperand_t * outArray, ExecOperand_t * buffIn, const conv2d_opt *opt);

//HWC fast only available in DSP
ExecStatus Exec_pw_conv2d_q7_fast ( uint8_t execHwId, ExecOperand_t * inArray, ExecOperand_t * wt, ExecOperand_t * bias, ExecOperand_t * outArray, ExecOperand_t * buffIn, const conv2d_opt *opt);
//CHW format  only in DSP
ExecStatus Exec_CHW_pw_conv2d_q7( uint8_t execHwId, ExecOperand_t * inArray, ExecOperand_t * wt, ExecOperand_t * bias, ExecOperand_t * outArray, ExecOperand_t * buffIn, const conv2d_opt *opt);



//DSP Only  special functionsorsourcw


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


ExecStatus Exec_conv2d_q7_CHW_ker2x2_stride1_pad0_relu_avgPool_2x2_stride2_pad0(
        uint8_t execHwId,
        ExecOperand_t * inArray,
        ExecOperand_t * wt,
        ExecOperand_t * bias,
        ExecOperand_t * outArray,
        ExecOperand_t * buffIn,
        const conv2d_relu_avgpool_opt *opt
);

ExecStatus Exec_conv2d_q7_CHW_ker3x3_stride1_pad0_relu_avgPool_2x2_stride2_pad0(
        uint8_t execHwId,
        ExecOperand_t * inArray,
        ExecOperand_t * wt,
        ExecOperand_t * bias,
        ExecOperand_t * outArray,
        ExecOperand_t * buffIn,
        const conv2d_relu_avgpool_opt *opt
);

ExecStatus Exec_pw_ds_conv2d_q7 ( uint8_t execHwId, ExecOperand_t * inArray,
                ExecOperand_t * wt_pw, ExecOperand_t * wt_ds,
                ExecOperand_t * bias_pw, ExecOperand_t * bias_ds,
                ExecOperand_t * outArray,
                ExecOperand_t * buffpsIn,
                ExecOperand_t * buffdsIn,
                ExecOperand_t * buffpw2ds,
                ExecOperand_t * buffoutArray,
                const conv2d_opt * opt_pw,
                const conv2d_opt * opt_ds
);

ExecStatus Exec_ds_conv2d_sb_q7 ( uint8_t execHwId, ExecOperand_t * inOutArray,
        ExecOperand_t * wt, ExecOperand_t * bias,
        ExecOperand_t * buffIn,
        ExecOperand_t * buffChanIn,
        ExecOperand_t * buffChanOut,
        const conv2d_opt * opt
);

ExecStatus Exec_ds_conv2d_layer2_q7( uint8_t execHwId, ExecOperand_t * inArray,
        ExecOperand_t * wt, ExecOperand_t * bias, ExecOperand_t * outArray,
        ExecOperand_t * buffIn, const conv2d_opt * opt
);

ExecStatus Exec_maxpool_q7 (uint8_t execHwId, ExecOperand_t * inArray,
                ExecOperand_t * outArray,
                const pool2d_opt * opt);

ExecStatus Exec_fully_connected_q7( uint8_t execHwId, ExecOperand_t * inArray,
        ExecOperand_t * wt, ExecOperand_t * bias,
        ExecOperand_t  * outArray, fc_opt *opt);

ExecStatus Exec_add_q7(uint8_t execHwId, ExecOperand_t * inArray0,
        ExecOperand_t * inArray1, ExecOperand_t  * outArray, add_opt *opt);

//TBD: The below two will go to private eventually
void HWCq7_to_CHWq15(int8_t *in, int8_t *out, uint16_t H, uint16_t W, uint16_t C);
void CHWq15_to_HWCq7(int8_t *in, int8_t *out, uint16_t H, uint16_t W, uint16_t C);
void reorder_conv2d_kernel(int8_t *in, int8_t *out, uint16_t H, uint16_t W, uint16_t C_in, uint16_t C_out);
void CHWq7_to_HWCq7_Ex(int8_t *in, int8_t *out, uint16_t H, uint16_t W, uint16_t C,uint16_t start_channel, uint16_t num_channels);
void HWCq7_to_CHWq7_with_pad_partial_channels(int8_t *in, int8_t *out, uint16_t H, uint16_t W, uint16_t C, uint16_t padLeft, uint16_t padRight, uint16_t padUp, uint16_t padDown, uint16_t start_channel, uint16_t num_channels);

#ifdef CONFIG_OP_DSP_FFT_Q15
ExecStatus Exec_fft_q15(uint8_t execHwId, ExecOperand_t * inArray0, ExecOperand_t  * outArray, uint16_t fft_length);
#endif
#ifdef CONFIG_OP_DSP_SQRT_IN_Q15_OUT_Q15
ExecStatus Exec_sqrt_q15(uint8_t execHwId, ExecOperand_t * inArray0, ExecOperand_t  * outArray, uint16_t len);
#endif
#ifdef CONFIG_OP_DSP_EXP_IN_Q15_OUT_Q3_13
ExecStatus Exec_exp_q15(uint8_t execHwId, ExecOperand_t * inArray0, ExecOperand_t  * outArray, uint16_t len);
#endif
#ifdef CONFIG_OP_DSP_LOG_IN_Q15_OUT_Q5_11
ExecStatus Exec_log_q15(uint8_t execHwId, ExecOperand_t * inArray0, ExecOperand_t  * outArray, uint16_t len);
#endif
#endif /*_EXECUTOR_PUBLIC_H_*/




