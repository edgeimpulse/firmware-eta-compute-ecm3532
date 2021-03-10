
/*******************************************************************************
 *
 * @file eta_nnfunctions.h
 *
 * Copyright (C) 2019 Eta Compute, Inc
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************/

#ifndef _ETA_NNFUNCTIONS_H
#define _ETA_NNFUNCTIONS_H

#include "eta_nnsupportfunctions.h"
#include "eta_nn_tables.h"
#include "eta_types.h"

/**
 * @file eta_nnfunctions.h
 * Group of NN kernel functions available for M3 only.
 */

#ifdef __cplusplus
extern    "C"
{
#endif

/**
 * @brief Basic Q7 convolution function, input format HWI
 * @param[in]       in_array     pointer to input tensor. Format is HWI.
 * @param[in]       wt           pointer to kernel weights. Format is OHWI.
 * @param[in]       bias         pointer to bias
 * @param[in,out]   out_array    pointer to output tensor. Format is HWO.
 * @param[in,out]   buff_in      pointer to buffer space for input
 * @param[in]       opt          struct for convolution options
 * @return     The function returns <code>ETA_STATUS_OK</code>
 */

  eta_rc eta_conv2d_q7(const q7_t * in_array, 
                       const q7_t * wt,
                       const q7_t * bias,
                       q7_t * out_array, 
                       q7_t * buff_in, 
                       const conv2d_opt opt);

/**
 * @cond
 */

/**
 * @brief Basic Q15 convolution function, input format HWI
 * @param[in]       in_array     pointer to input tensor. Format is HWI.
 * @param[in]       wt           pointer to kernel weights. Format is OHWI.
 * @param[in]       bias         pointer to bias
 * @param[in,out]   out_array    pointer to output tensor. Format is HWO.
 * @param[in,out]   buff_in      pointer to buffer space for input
 * @param[in]       opt          struct for convolution options
 * @return     The function returns <code>ETA_STATUS_OK</code>
 */

  eta_rc eta_conv2d_q15(q15_t * in_array,
                        const q15_t * wt,
                        const q15_t * bias,
                        q15_t * out_array, 
                        q15_t * buff_in, 
                        conv2d_opt opt);

/**
 * @endcond
 */

/**
 * @brief Fast Q7 version of 1x1 convolution (non-sqaure shape)
 * @param[in]       in_array        pointer to input tensor
 * @param[in]       wt           pointer to kernel weights
 * @param[in]       bias         pointer to bias
 * @param[in,out]   out_array       pointer to output tensor
 * @param[in,out]   buff_in      pointer to buffer space for input 
 * @param[in]       opt          struct for convolution options
 * @return     The function returns either
 * <code>ETA_GEN_ERR</code> or <code>ETA_STATUS_OK</code> based on the outcome of size checking.
 *
 * This function implement convolution with 1x1 kernel size.
 * It can be used for second half of MobileNets after depthwise 
 * convolution.
 *
 */
  eta_rc eta_pw_conv2d_q7(const q7_t * in_array,
                          const q7_t * wt,
                          const q7_t * bias,
                          q7_t * out_array,
                          q7_t * buff_in,
                          conv2d_opt opt);

/**
 * @cond
 */

/**
 * @brief Fast Q7 version of 1x1 convolution (non-sqaure shape). Srictly for layer5 of Mobilenet
 * @param[in]       in_array     pointer to input tensor
 * @param[in]       wt           pointer to kernel weights
 * @param[in]       bias         pointer to bias
 * @param[in,out]   out_array    pointer to output tensor
 * @param[in,out]   buff_in      pointer to buffer space for input 
 * @param[in]       opt          struct opt for the conv2d params
 * @return     The function returns either
 * <code>ETA_GEN_ERR</code> or <code>ETA_STATUS_OK</code> based on the outcome of size checking.
 *
 * This function implement convolution with 1x1 kernel size.
 * It can be used for second half of MobileNets after depthwise 
 * convolution.
 *
 */
  eta_rc eta_pw_conv2d_layer4_q7(const q7_t * in_array,
                                  const q7_t * wt,
                                  const q7_t * bias,
                                  q7_t * out_array,
                                  q7_t * buff_in,
                                  conv2d_opt opt);

/**
 * @brief Q7 depthwise separable convolution function with shared input/output buffer. Num_filt must equal 1. Input and output dimensions must match.
 * @param[in]       in_array      pointer to input tensor. Format is HWI.
 * @param[in]       wt_pw         pointer to pointwise kernel weights. Format is OHWI.
 * @param[in]       wt_ds         pointer to depthwise kernel weights. NOTE: Special format is OIHW.
 * @param[in]       bias_pw       pointer to pointwise bias
 * @param[in]       bias_ds       pointer to depthwise bias
 * @param[in,out]   out_array     pointer to output tensor. Format is HWOI.
 * @param[in,out]   buff_in_pw    pointer to buffer space for pw_conv2d input.
 * @param[in,out]   buff_in_ds    pointer to buffer space for ds_conv2d input.
 * @param[in,out]   buff_pw2ds    pointer to buffer space for intermediate channel between PW and DS kernels. Size is ds_in_rows * ds_in_cols.
 * @param[in,out]   buff_chan_out pointer to buffer space for temporary output channel. Size is ds_out_rows * ds_out_cols.
 * @param[in]       opt_pw        struct for pointwise convolution options
 * @param[in]       opt_ds        struct for depthwise convolution options
 * @return     The function returns <code>ETA_STATUS_OK</code>
 */

eta_rc eta_pw_ds_conv2d_q7(const q7_t * in_array,
                           const q7_t * wt_pw,
                           const q7_t * wt_ds,
                           const q7_t * bias_pw,
                           const q7_t * bias_ds,
                           q7_t * out_array,
                           q7_t * buff_in_pw,
                           q7_t * buff_in_ds,
                           q7_t * buff_pw2ds,
                           q7_t * buff_chan_out,
                           const conv2d_opt opt_pw,
                           const conv2d_opt opt_ds);

/**
 * @brief Q7 depthwise separable convolution function with shared input/output buffer. Num_filt must equal 1. Input and output dimensions must match. Srictly for layer4 and layer5 of Mobilenet
 * @param[in]       in_array      pointer to input tensor. Format is HWI.
 * @param[in]       wt_pw         pointer to pointwise kernel weights. Format is OHWI.
 * @param[in]       wt_ds         pointer to depthwise kernel weights. NOTE: Special format is OIHW.
 * @param[in]       bias_pw       pointer to pointwise bias
 * @param[in]       bias_ds       pointer to depthwise bias
 * @param[in,out]   out_array     pointer to output tensor. Format is HWOI.
 * @param[in,out]   buff_in_pw    pointer to buffer space for pw_conv2d input.
 * @param[in,out]   buff_in_ds    pointer to buffer space for ds_conv2d input.
 * @param[in,out]   buff_pw2ds    pointer to buffer space for intermediate channel between PW and DS kernels. Size is ds_in_rows * ds_in_cols.
 * @param[in,out]   buff_chan_out pointer to buffer space for temporary output channel. Size is ds_out_rows * ds_out_cols.
 * @param[in]       opt_pw        struct for pointwise convolution options
 * @param[in]       opt_ds        struct for depthwise convolution options
 * @return     The function returns <code>ETA_STATUS_OK</code>
 */

eta_rc eta_pw_ds_conv2d_layer4and5_q7(const q7_t * in_array,
                                      const q7_t * wt_pw,
                                      const q7_t * wt_ds,
                                      const q7_t * bias_pw,
                                      const q7_t * bias_ds,
                                      q7_t * out_array,
                                      q7_t * buff_in_pw,
                                      q7_t * buff_in_ds,
                                      q7_t * buff_pw2ds,
                                      q7_t * buff_chan_out,
                                      const conv2d_opt opt_pw,
                                      const conv2d_opt opt_ds);

/**
 * @endcond
 */

/**
 * @brief Q7 depthwise separable convolution function
 * @param[in]       in_array       pointer to input tensor
 * @param[in]       wt          pointer to kernel weights
 * @param[in]       bias        pointer to bias
 * @param[in,out]   out_array      pointer to output tensor
 * @param[in,out]   buff_in     pointer to buffer space for input 
 * @param[in]       opt         struct for convolution options
 * @return     The function returns <code>ETA_STATUS_OK</code>
 */

  eta_rc eta_ds_conv2d_q7(const q7_t * in_array,
                          const q7_t * wt,
                          const q7_t * bias,
                          q7_t * out_array,
                          q7_t * buff_in, 
                          const conv2d_opt opt);

/**
 * @brief Q7 depthwise separable convolution function for in_depth == 1
 * @param[in]       in_array    pointer to input tensor. Format is HW.
 * @param[in]       wt          pointer to kernel weights. Format is OHW
 * @param[in]       bias        pointer to bias. 
 * @param[in,out]   out_array   pointer to output tensor. Format is HWO.
 * @param[in,out]   buff_in     pointer to buffer space for input.
 * @param[in]       opt         struct for convolution options
 * @return     The function returns <code>ETA_STATUS_OK</code>
 */

  eta_rc eta_ds_conv2d_depth1_q7(const q7_t * in_array,
                                  const q7_t * wt,
                                  const q7_t * bias,
                                  q7_t * out_array,
                                  q7_t * buff_in, 
                                  const conv2d_opt opt);

/**
 * @brief Q7 depthwise separable convolution function with shared input/output buffer. Num_filt must equal 1. Input and output dimensions must match.
 * @param[in]       in_out_array  pointer to shared input/output tensor. Format is HWI.
 * @param[in]       wt            pointer to kernel weights. NOTE: Special format is IHW.
 * @param[in]       bias          pointer to bias
 * @param[in,out]   buff_in       pointer to buffer space for ds_conv2d algorithm.
 * @param[in,out]   buff_chan_in  pointer to buffer space for temporary input channel. Size is in_rows * in_cols.
 * @param[in,out]   buff_chan_out pointer to buffer space for temporary output channel. Size is out_rows * out_cols.
 * @param[in]       opt           struct for convolution options
 * @return     The function returns <code>ETA_STATUS_OK</code>
 */

  eta_rc eta_ds_conv2d_sb_q7(q7_t * in_out_array,
                              const q7_t * wt,
                              const q7_t * bias,
                              q7_t * buff_in,
                              q7_t * buff_chan_in,
                              q7_t * buff_chan_out,
                              const conv2d_opt opt);

/**
 * @cond
 */

/**
 * @brief Q7 depthwise separable convolution function. Srictly for layer2 of Mobilenet
 * @param[in]       in_array       pointer to input tensor
 * @param[in]       wt          pointer to kernel weights
 * @param[in]       bias        pointer to bias
 * @param[in,out]   out_array      pointer to output tensor
 * @param[in,out]   buff_in     pointer to buffer space for input 
 * @param[in]       opt         struct for convolution options
 * @return     The function returns <code>ETA_STATUS_OK</code>
 */

  eta_rc eta_ds_conv2d_layer2_q7(const q7_t * in_array,
                                  const q7_t * wt,
                                  const q7_t * bias,
                                  q7_t * out_array,
                                  q7_t * buff_in, 
                                  const conv2d_opt opt);

/**
 * @brief Basic Q7 convolution function (non-sqaure shape), input format HWI. Srictly for layer3 of Mobilenet
 * @param[in]       in_out_array pointer to shared input/output tensor. Format is HWI.
 * @param[in]       wt           pointer to kernel weights. Format is OHWI.
 * @param[in]       bias         pointer to bias
 * @param[in,out]   buff_in      pointer to buffer space for input
 * @param[in]       opt          struct for convolution options
 * @return     The function returns <code>ETA_STATUS_OK</code>
 */

  eta_rc eta_ds_conv2d_sb_layer3_q7(q7_t * in_out_array,
                                    const q7_t * wt,
                                    const q7_t * bias,
                                    q7_t * buff_in, 
                                    const conv2d_opt opt);

/**
 * @brief Basic Q7 convolution function (non-sqaure shape), input format HWI. Srictly for layer5 of Mobilenet
 * @param[in]       in_array    pointer to input tensor. Format is HW.
 * @param[in]       wt          pointer to kernel weights. Format is OHW
 * @param[in]       bias        pointer to bias. 
 * @param[in,out]   out_array   pointer to output tensor. Format is HWO.
 * @param[in,out]   buff_in     pointer to buffer space for input.
 * @param[in]       opt         struct for convolution options
 * @return     The function returns <code>ETA_STATUS_OK</code>
 */
  eta_rc eta_ds_conv2d_layer5_q7(const q7_t * in_array,
                                  const q7_t * wt,
                                  const q7_t * bias,
                                  q7_t * out_array,
                                  q7_t * buff_in, 
                                  const conv2d_opt opt);

/**
 * @endcond
 */


/**
 * @brief Q7 fully-connected layer function
 * @param[in]       pV          pointer to input vector
 * @param[in]       pM          pointer to matrix weights
 * @param[in]       bias        pointer to bias
 * @param[in,out]   pOut        pointer to output vector
 * @param[in]       opt         structure for fully connected parameters
 * @return     The function returns ETA_STATUS_OK
 *
 */

  eta_rc eta_fully_connected_q7(const q7_t * pV,
                                const q7_t * pM,
                                const q7_t * bias, 
                                q7_t * pOut, 
                                fc_opt opt);
  /**
   * @cond
   */
  /**
   * @brief Matrix-multiplication function for convolution
   * @param[in]       pA          pointer to operand A
   * @param[in]       pInBuffer   pointer to operand B, always consists of 2 vectors
   * @param[in]       bias        the bias
   * @param[in,out]   pOut        pointer to output
   * @param[in]       opt         structure for fully connected parameters
   * @return     The function returns the incremented output pointer
   */

    q7_t *eta_nn_mat_mult_kernel_q7(const q7_t * pA,
                                    const q7_t * pInBuffer,
                                    const q7_t * bias, 
                                    q7_t * pOut,
                                    fc_opt opt);
  /**
   * @endcond
   */


#ifdef __cplusplus
}
#endif

/*
 *  Other functions
 *  These layers are typically not timing critical
 *  Basic implementation is supported here
 */

#ifdef __cplusplus
extern    "C"
{
#endif


  /**
   * @brief Q7 RELU function
   * @param[in,out]   data        pointer to input
   * @param[in]       size        number of elements
   */

    void eta_relu_q7(q7_t * data, uint16_t size);

/**
 * @cond
 */
  /**
   * @brief Q15 RELU function
   * @param[in,out]   data        pointer to input
   * @param[in]       size        number of elements
   */

    void eta_relu_q15(q15_t * data, uint16_t size);
/**
 * @endcond
 */

  /**
   * @brief Q7 RELU6 function
   * @param[in,out]   data        pointer to input
   * @param[in]       size        number of elements
   * @param[in]       frac_bits   fractional bits of the input range
   * @details
   *
   * Optimized relu6 with QSUB instructions.
   *
   */

    void eta_relu6_q7(q7_t *data, uint16_t size, uint16_t frac_bits);

  /**
   * @brief Q7 sigmoid activation function using direct table look-up
   * @param[in,out]   data        pointer to input
   * @param[in]       size        number of elements
   * @param[in]       int_width   bit-width of the integer part, assume to be smaller than 3
   *
   * @details
   * 
   * This is the direct table look-up approach.
   *
   * Assumption: the integer part of the fixed-point is <= 3.
   * More than 3 doesn't make much sense, as there is no difference with
   * saturation followed by any of these activation functions. 
   */

  void eta_sigmoid_q7(q7_t * data, uint16_t size, uint16_t int_width);

  /**
   * @brief Q7 tanh activation function using direct table look-up
   * @param[in,out]   data        pointer to input
   * @param[in]       size        number of elements
   * @param[in]       int_width   bit-width of the integer part, assume to be smaller than 3
   *
   * @details
   * 
   * This is the direct table look-up approach.
   *
   * Assumption: the integer part of the fixed-point is <= 3.
   * More than 3 doesn't make much sense, as there is no difference with
   * saturation followed by any of these activation functions. 
   */

  void eta_tanh_q7(q7_t * data, uint16_t size, uint16_t int_width);
  
  /**
   * @brief Q15 sigmoid activation function using direct table look-up
   * @param[in,out]   data        pointer to input
   * @param[in]       size        number of elements
   * @param[in]       int_width   bit-width of the integer part, assume to be smaller than 3
   *
   * @details
   * 
   * This is the direct table look-up approach.
   *
   * Assumption: the integer part of the fixed-point is <= 3.
   * More than 3 doesn't make much sense, as there is no difference with
   * saturation followed by any of these activation functions. 
   */

  void eta_sigmoid_q15(q15_t * data, uint16_t size, uint16_t int_width);

  /**
   * @brief Q15 tanh activation function using direct table look-up
   * @param[in,out]   data        pointer to input
   * @param[in]       size        number of elements
   * @param[in]       int_width   bit-width of the integer part, assume to be smaller than 3
   *
   * @details
   * 
   * This is the direct table look-up approach.
   *
   * Assumption: the integer part of the fixed-point is <= 3.
   * More than 3 doesn't make much sense, as there is no difference with
   * saturation followed by any of these activation functions. 
   */

  void eta_tanh_q15(q15_t * data, uint16_t size, uint16_t int_width);


  /**
   * @brief Q7 max pooling 2d function
   * @param[in]       in_array        pointer to input tensor
   * @param[in,out]   out_array       pointer to output tensor
   * @param[in]       opt             structure with pool2d options
   */

    void eta_maxpool_q7(q7_t * in_array, 
                        q7_t * out_array, 
                        pool2d_opt opt);

  /**
   * @brief Q7 average pooling 2d function
   * @param[in]       in_array        pointer to input tensor
   * @param[in,out]   out_array       pointer to output tensor
   * @param[in]       opt             structure with pool2d options
   * @return The function returns <code>ETA_STATUS_OK</code>.
   */

    eta_rc eta_avepool2d_q7(q7_t * in_array,
                            q7_t * out_array,
                            pool2d_opt opt);


/**
 * @cond
 */

  /**
   * @brief Basic Q7 RNNPool function (non-square), input format HWI
   * @param[in]       in_array     pointer to input tensor
   * @param[in]       wt1          pointer to RNN1 W kernel weights
   * @param[in]       wt2          pointer to RNN2 W kernel weights
   * @param[in]       ut1          pointer to RNN1 U kernel weights
   * @param[in]       ut2          pointer to RNN2 U kernel weights
   * @param[in]       bias_gate1   pointer to RNN1 gate bias
   * @param[in]       bias_gate2   pointer to RNN2 gate bias
   * @param[in]       bias_update1 pointer to RNN1 update bias
   * @param[in]       bias_update2 pointer to RNN2 update bias
   * @param[in,out]   out_array    pointer to output tensor
   * @param[in,out]   buff_a       pointer to buffer space for GRNN kernel.
   * @param[in,out]   buff_b       pointer to buffer space for GRNN kernel.
   * @param[in,out]   buff_c       pointer to buffer space for RNN1 -> RNN2.
   * @param[in]       opt          struct for convolution options
   * @return     The function returns <code>ETA_STATUS_OK</code>
   */

    eta_rc eta_rnnpool_q7(const q7_t * in_array,
                          const q7_t * wt1,
                          const q7_t * wt2,
                          const q7_t * ut1,
                          const q7_t * ut2,
                          const q7_t * bias_gate1,
                          const q7_t * bias_gate2,
                          const q7_t * bias_update1,
                          const q7_t * bias_update2,
                          q7_t * out_array,
                          q7_t * buff_a,
                          q7_t * buff_b,
                          q7_t * buff_c,
                          const rnnpool_opt opt);


  /**
   * @brief Basic Q7 fast GRNN function
   * @param[in]       in_array     pointer to input vector
   * @param[in]       in_array     pointer to hidden layer
   * @param[in]       wt           pointer to W kernel weights
   * @param[in]       ut           pointer to U kernel weights
   * @param[in]       bias_gate    pointer to gate bias
   * @param[in]       bias_update  pointer to update bias
   * @param[in,out]   buff_a       pointer to buffer space
   * @param[in,out]   buff_b       pointer to buffer space
   * @param[in]       opt          struct for convolution options
   * @return     The function returns <code>ETA_STATUS_OK</code>
   */


    eta_rc eta_fast_grnn_q7(const q7_t * in_array,
                            q7_t * hidden,
                            const q7_t * wt,
                            const q7_t * ut,
                            const q7_t * bias_gate,
                            const q7_t * bias_update,
                            q7_t * buff_a,
                            q7_t * buff_b,
                            const grnn_opt opt);
/**
 * @endcond
 */

  /**
   * @brief Q7 softmax function
   * @param[in]       vec_in      pointer to input vector
   * @param[in]       dim_vec     input vector dimention
   * @param[out]      p_out       pointer to output vector
   *
   */

    void eta_softmax_q7(const q7_t * vec_in, const uint16_t dim_vec, q7_t * p_out);

  /**
   * @brief Q15 softmax function
   * @param[in]       vec_in      pointer to input vector
   * @param[in]       dim_vec     input vector dimention
   * @param[out]      p_out       pointer to output vector
   *
   */

    void eta_softmax_q15(const q15_t * vec_in, const uint16_t dim_vec, q15_t * p_out);

/**
 * @cond
 */
  /**
   * @brief           Q7 vector multiplication with variable output shifts
   * @param[in]       *pSrcA        pointer to the first input vector
   * @param[in]       *pSrcB        pointer to the second input vector
   * @param[out]      *pDst         pointer to the output vector
   * @param[in]       out_shift     amount of right-shift for output
   * @param[in]       blockSize     number of samples in each vector
   *
   * <b>Scaling and Overflow Behavior:</b>
   * \par
   * The function uses saturating arithmetic.
   * Results outside of the allowable Q15 range [0x8000 0x7FFF] will be 
   * saturated.
   */

  void eta_nn_mult_q15(
    q15_t * pSrcA,
    q15_t * pSrcB,
    q15_t * pDst,
    const uint16_t out_shift,
    uint32_t blockSize);
    
  /**
   * @brief           Q7 vector multiplication with variable output shifts
   * @param[in]       *pSrcA        pointer to the first input vector
   * @param[in]       *pSrcB        pointer to the second input vector
   * @param[out]      *pDst         pointer to the output vector
   * @param[in]       out_shift     amount of right-shift for output
   * @param[in]       blockSize     number of samples in each vector
   *
   * <b>Scaling and Overflow Behavior:</b>
   * \par
   * The function uses saturating arithmetic.
   * Results outside of the allowable Q7 range [0x80 0x7F] will be saturated.
   */

  void eta_nn_mult_q7(
    q7_t * pSrcA,
    q7_t * pSrcB,
    q7_t * pDst,
    const uint16_t out_shift,
    uint32_t blockSize);
/**
 * @endcond
 */

  /**
   * @brief     Q7 vector addition of two arrays with different Qm.n format
   * @param[in]     *in_array1    Pointer to the first input vector
   * @param[in]     *in_array2    Pointer to the second input vector
   * @param[out]    *out_array    Pointer to the output vector
   * @param[in]     opt           Atructure containing additional parameters
   * 
   * <b>Scaling and Overflow Behavior:</b>
   * \par
   * The function uses saturating arithmetic.
   * Results outside of the allowable Q7 range [0x80 0x7F] will be saturated.
   */

  void eta_add_q7(const q7_t *in_array1,
                  const q7_t *in_array2,
                  q7_t *out_array,
                  add_opt opt);


  /**
   * @brief     Q7 concatenation of input arrays
   * @param[in]   *in_array1    Pointer to input pointer array1, not null
   * @param[in]   *in_array2    Pointer to input pointer array2, not null
   * @param[in]   *in_array3    Pointer to input pointer array3, can be null
   * @param[in]   *in_array4    Pointer to input pointer array4, can be null
   * @param[in]   *in_array5    Pointer to input pointer array5, can be null
   * @param[in]   *in_array6    Pointer to input pointer array6, can be null
   * @param[in]   *in_array7    Pointer to input pointer array7, can be null
   * @param[in]   *in_array8    Pointer to input pointer array8, can be null
   * @param[out]  *out_array    Pointer to flattened output array
   * @param[in]   opt           Structure containing concatenation parameters
   * 
   * This function assumes:
   * - All the input arrays have same dimensions except the concatenation axis.
   * - All the arrays will be right shifted to preserve the integer part of 
   *   the input with largest range. 
   * 
   */
  void eta_concat_q7(const q7_t *in_array1, const q7_t *in_array2,  
                     const q7_t *in_array3, const q7_t *in_array4, 
                     const q7_t *in_array5, const q7_t *in_array6, 
                     const q7_t *in_array7, const q7_t *in_array8, 
                     q7_t *out_array, concat_opt opt);

  /**
   * @brief     Q7 concatenation of input arrays for Height axis
   * @param[in]   *in_array1    Pointer to input pointer array1, not null
   * @param[in]   *in_array2    Pointer to input pointer array2, not null
   * @param[out]  *out_array    Pointer to flattened output array
   * @param[in]   opt           Structure containing concatenation parameters
   * 
   * This function assumes:
   * - All the input arrays have same dimensions except the concatenation axis.
   * - All the arrays will be right shifted to preserve the integer part of 
   *   the input with largest range. 
   * 
   */
  void eta_concat_height_q7(const q7_t *in_array1, const q7_t *in_array2,  
                            q7_t *out_array, concat_opt opt);

#ifdef __cplusplus
}
#endif

#endif
