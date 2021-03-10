/*******************************************************************************
*
* @file eta_dsp_nn.h
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

#ifndef _ETA_DSP_NN_
#define _ETA_DSP_NN_

#include "eta_types.h"

/**
 * @brief This function implements point wise convolution CHW Data format
 * @param[in]       in_array     pointer to input tensor
 * @param[in]       wt           pointer to kernel weights
 * @param[in]       bias         pointer to bias
 * @param[in]       pw_opt       pointer to point wise conv paramter structure.
 * @param[out]      out_array    pointer to output tensor
 * @return     The function returns either
 * <code>ETA_GEN_ERR</code> or <code>ETA_STATUS_OK</code> based on the success or failure.
 *
 * This function implement convolution with 1x1 kernel size.
 *
 */

//Input/Weight in CHW format; Output also in CHW format.

eta_rc eta_pw_conv2d_q7_chw(int chess_storage(XMEM)* restrict in_array,
    int chess_storage(YMEM)* restrict wt,
    int chess_storage(YMEM)* restrict bias,
    conv_pw_opt* pw_opt,
    int chess_storage(XMEM)* restrict out_array);



/**
 * @brief This function implements point wise convolution in HWC data format
 * @param[in]       in_array     pointer to input tensor
 * @param[in]       wt           pointer to kernel weights
 * @param[in]       bias         pointer to bias
 * @param[in]       pw_opt       pointer to point wise conv paramter structure.
 * @param[out]      out_array    pointer to output tensor
 * @return     The function returns either
 * <code>ETA_GEN_ERR</code> or <code>ETA_STATUS_OK</code> based on the success or failure.
 *
 * This function implement convolution with 1x1 kernel size.
 *
 */
 //Input/Weight in HWC format; Output also in HWC format.

eta_rc eta_pw_conv2d_q7_hwc(int chess_storage(XMEM)* restrict in_array,
    int chess_storage(YMEM)* restrict wt,
    int chess_storage(YMEM)* restrict bias,
    conv_pw_opt* restrict pw_opt,
    int chess_storage(XMEM)* restrict out_array);


/**
 * @brief Optimized pointwise convolution implementation in hwc format
 * @param[in]       in_array     pointer to input tensor [Input width must be even]
 * @param[in]       wt           pointer to kernel weights [No. of Kernels must be even]
 * @param[in]       bias         pointer to bias
 * @param[in]       pw_opt       pointer to point wise conv paramter structure.
 * @param[out]      out_array    pointer to output tensor
 * @return     The function returns either
 * <code>ETA_GEN_ERR</code> or <code>ETA_STATUS_OK</code> based on the success or failure.
 *
 * This function implement convolution with 1x1 kernel size.
 * [CAUTION]: This fast algorithm operates at two input points and two weight points
 * Hence input width must be multiple of two and no. of kernels must be multiple of 2.
 */
 //Input/Weight in HWC format; Output also in HWC format.

eta_rc eta_pw_conv2d_q7_hwc_2mac_schedule(int chess_storage(XMEM)* restrict in_array,
    int chess_storage(YMEM)* restrict wt,
    int chess_storage(YMEM)* restrict bias,
    conv_pw_opt* restrict pw_opt,
    int chess_storage(XMEM)* restrict out_array);

/**
 * @brief Fast point wise convolution to handle large inputs with configured partial input points at a time.
 * @param[in]       in_array     pointer to input tensor [Input width must be even]
 * @param[in]       wt           pointer to kernel weights [No. of Kernels must be even]
 * @param[in]       bias         pointer to bias
 * @param[in]       pw_opt       pointer to point wise conv paramter structure.
 * @param[in]       num_partial_input  number of partial inputs to process.
 * @param[out]      out_array    pointer to output tensor
 * @return     The function returns either
 * <code>ETA_GEN_ERR</code> or <code>ETA_STATUS_OK</code> based on the success or failure.
 *
 * This function implement convolution with 1x1 kernel size.
 * convolution.
 * [CAUTION]: This fast algorithm operates at two input points and two weight points
 * Hence input width must be multiple of two and no. of kernels must be multiple of 2.
 */
 //Input/Weight in HWC format; Output also in HWC format.
eta_rc eta_partial_input_pw_conv2d_q7_hwc_2mac(int chess_storage(XMEM)* restrict in_array,
    int chess_storage(YMEM)* restrict wt,
    int chess_storage(YMEM)* restrict bias,
    conv_pw_opt* restrict pw_opt,
    int num_partial_input,
    int chess_storage(XMEM)* restrict out_array);

/**
 * @brief point wise kernel implementation for handling arbitrary long weight sizes.
 * @param[in]       in_array     pointer to input tensor [Input width must be even]
 * @param[in]       wt           pointer to kernel weights [No. of Kernels must be even]
 * @param[in]       bias         pointer to bias
 * @param[in]       pw_opt       pointer to point wise conv paramter structure.
 * @param[in]       num_partial_kernels  number of partial kernels to process.
 * @param[out]      out_array    pointer to output tensor
 * @return     The function returns either
 * <code>ETA_GEN_ERR</code> or <code>ETA_STATUS_OK</code> based on the success or failure.
 *
 * This function implement convolution with 1x1 kernel size.
 * [CAUTION]: This fast algorithm operates at two input points and two weight points
 * Hence input width must be multiple of two and no. of kernels must be multiple of 2.
 */
 //Input/Weight in HWC format; Output in CHW format.
eta_rc eta_partial_kernels_pw_conv2d_q7_hwc_2mac(int chess_storage(XMEM)* restrict in_array,
    int chess_storage(YMEM)* restrict wt,
    int chess_storage(YMEM)* restrict bias,
    conv_pw_opt* restrict pw_opt,
    int num_partial_kernels,
    int chess_storage(XMEM)* restrict out_array);


/**
 * @brief 2x2 convolution followed by relu and 2x2 averge pooling.
 * @param[in]       Im_in           pointer to input tensor
 * @param[in]       dim_im_in_x     Input width = No of columns in input
 * @param[in]       dim_im_in_y     Input height = No of rows in input
 * @param[in]       dim_ch_in       No of channels in input.
 * @param[in]       wt,             pointer to kernel weights
 * @param[in]       dim_ch_out      no. of kernels = No. of output channels.
 * @param[in]       bias            pointer to bias
 * @param[in]       bias_shift      input shift
 * @param[in]       out_shift       output shift
 * @param[in]       output_activation_min   min value for relu
 * @param[in]       output_activation_max   max value for relu
 * @param[in]       tmp_buffer      //temporary buffer one output channel of conv2d is stored here and sent to pool
 * @param[out]      out             pointer to output tensor
 * @return          void
 *
 */
void eta_convolve_CHW_q7_ker2x2_stride1_pad0_relu_avgPool_2x2_stride2_pad0_dsp(
    int _XMEM *Im_in,
    int dim_im_in_x, //x = width = columns
    int dim_im_in_y, //y = height = rows
    int dim_ch_in,
    int chess_storage(YMEM) *wt,
    int dim_ch_out, // = # kernels
    //int dim_x_ker, = 2
    //int dim_y_ker, = 2
    //int dim_ch_ker = dim_ch_in
    //padding = 0
    //stride = 1
    int _YMEM *bias,  //bias vector size = # kernels = dim_ch_out
    int bias_shift,
    int out_shift,
    int output_activation_min,  //min value for relu
    int output_activation_max,  //max value for relu
    int *tmp_buffer, //one output channel of conv2d is stored here and sent to pool
    int *out);  //output of pool is stored here

/**
 * @brief 3x3 convolution followed by relu and 2x2 averge pooling.
 * @param[in]       Im_in           pointer to input tensor
 * @param[in]       dim_im_in_x     Input width = No of columns in input
 * @param[in]       dim_im_in_y     Input height = No of rows in input
 * @param[in]       dim_ch_in       No of channels in input.
 * @param[in]       wt,             pointer to kernel weights
 * @param[in]       dim_ch_out      no. of kernels = No. of output channels.
 * @param[in]       bias            pointer to bias
 * @param[in]       bias_shift      input shift
 * @param[in]       out_shift       output shift
 * @param[in]       output_activation_min   min value for relu
 * @param[in]       output_activation_max   max value for relu
 * @param[in]       tmp_buffer      //temporary buffer one output channel of conv2d is stored here and sent to pool
 * @param[out]      out             pointer to output tensor
 * @return          void
 *
 */
void eta_convolve_CHW_q7_ker3x3_stride1_pad0_relu_avgPool_2x2_stride2_pad0_dsp(
    int _XMEM *Im_in,
    int dim_im_in_x, //x = width = columns
    int dim_im_in_y, //y = height = rows
    int dim_ch_in,
    int chess_storage(YMEM) *wt,
    int dim_ch_out, // = # kernels
    //int dim_x_ker, = 2
    //int dim_y_ker, = 2
    //int dim_ch_ker = dim_ch_in
    //padding = 0
    //stride = 1
    int _YMEM *bias,  //bias vector size = # kernels = dim_ch_out
    int bias_shift,
    int out_shift,
    int output_activation_min,  //min value for relu
    int output_activation_max,  //max value for relu
    int *tmp_buffer, //one output channel of conv2d is stored here and sent to pool
    int *out);  //output of pool is stored here

/*
 * @brief Q7 convolution function for 3x3 kernel and stride 2, input format CHW
 * @param[in]       Im_in        pointer to input tensor in X memory
 * @param[in]       dim_im_in_x  input tensor dimention x
 * @param[in]       dim_im_in_y  input tensor dimention y
 * @param[in]       dim_ch_in    number of input tensor channels
 * @param[in]       wt           pointer to kernel weights in Y memory
 * @param[in]       dim_ch_out   number of filters, i.e., output tensor channels
 * @param[in]       bias         pointer to bias in Y memory
 * @param[in]       bias_shift   amount of left-shift for bias
 * @param[in]       out_shift    amount of right-shift for output
 * @param[in]       output_activation_min    the minimum value of the output range
 * @param[in]       output_activation_max    the maximum value of the output range
 * @param[in,out]   out          pointer to output tensor
 * @param[in]       dim_im_out_x output tensor dimention x
 * @param[in]       dim_im_out_y output tensor dimention y
 *
 * This function implements convolution with 3x3 kernel size and stride of 2.
 * It is optimized to make use of both macs available in the DSP.
 */

void eta_convolve_CHW_q7_ker3x3_stride2_pad0_relu_dsp(
    int _XMEM *Im_in,
    int dim_im_in_x, //x = width = columns
    int dim_im_in_y, //y = height = rows
    int dim_ch_in,
    int chess_storage(YMEM) *wt,
    int dim_ch_out, // = # kernels
    //int dim_kernel_x,
    //int dim_kernel_y,
    //int padding_x,
    //int padding_y,
    int _YMEM *bias,  //bias vector size = # kernels = dim_ch_out
    int bias_shift,
    int out_shift,
    int output_activation_min,  //min value for relu 
    int output_activation_max,  //max value for relu
    //int *tmp_buffer, //one output channel of conv2d is stored here and sent to pool
                //size = (dim_im_in_x -1)*(dim_im_in_y -1)
    int *out,  //output of pool 
    int dim_im_out_x, //x = width = columns
    int dim_im_out_y); //y = height = rows

/*
 * @brief Q7 convolution function for 2x2 kernel and stride 2, input format CHW
 * @param[in]       Im_in        pointer to input tensor in X memory
 * @param[in]       dim_im_in_x  input tensor dimention x
 * @param[in]       dim_im_in_y  input tensor dimention y
 * @param[in]       dim_ch_in    number of input tensor channels
 * @param[in]       wt           pointer to kernel weights in Y memory
 * @param[in]       dim_ch_out   number of filters, i.e., output tensor channels
 * @param[in]       bias         pointer to bias in Y memory
 * @param[in]       bias_shift   amount of left-shift for bias
 * @param[in]       out_shift    amount of right-shift for output
 * @param[in]       output_activation_min    the minimum value of the output range
 * @param[in]       output_activation_max    the maximum value of the output range
 * @param[in,out]   out          pointer to output tensor
 * @param[in]       dim_im_out_x output tensor dimention x
 * @param[in]       dim_im_out_y output tensor dimention y
 *
 * This function implements convolution with 2x2 kernel size and stride of 2.
 */

void eta_convolve_CHW_q7_ker2x2_stride2_pad0_relu_dsp(
    int _XMEM *Im_in,
    int dim_im_in_x, //x = width = columns
    int dim_im_in_y, //y = height = rows
    int dim_ch_in,
    int chess_storage(YMEM) *wt,
    int dim_ch_out, // = # kernels
    //int dim_kernel_x,
    //int dim_kernel_y,
    //int padding_x,
    //int padding_y,
    int _YMEM *bias,  //bias vector size = # kernels = dim_ch_out
    int bias_shift,
    int out_shift,
    int output_activation_min,  //min value for relu 
    int output_activation_max,  //max value for relu
    //int *tmp_buffer, //one output channel of conv2d is stored here and sent to pool
                //size = (dim_im_in_x -1)*(dim_im_in_y -1)
    int *out,  //output of pool 
    int dim_im_out_x, //x = width = columns
    int dim_im_out_y); //y = height = rows

/*
 * @brief Q7 convolution function for 2x2 kernel and stride 2, input format CHW
 * @param[in]       Im_in        pointer to input tensor in X memory
 * @param[in]       wt           pointer to kernel weights in Y memory
 * @param[in]       bias         pointer to bias in Y memory
 * @param[in]       opt_st       pointer to conv2d paramter structure.
 * @param[in,out]   out          pointer to output tensor
 * @return     The function returns either
 * <code>ETA_GEN_ERR</code> or <code>ETA_STATUS_OK</code> based on the success or failure.
 *
 * This function implements convolution with 2x2 kernel size and stride of 2.
 */

eta_rc eta_conv2d_q7_chw_ker2x2_stride2_pad0_relu(
    int _XMEM *Im_in,
    int chess_storage(YMEM) *wt,
    int _YMEM *bias,  //bias vector size = # kernels = dim_ch_out
    conv2d_opt_st* opt_st,
    int *out  //output
    );

/*
 * @brief Q7 convolution function for 3x3 kernel and stride 2, input format CHW
 * @param[in]       Im_in        pointer to input tensor in X memory
 * @param[in]       wt           pointer to kernel weights in Y memory
 * @param[in]       bias         pointer to bias in Y memory
 * @param[in]       opt_st       pointer to conv2d paramter structure.
 * @param[in,out]   out          pointer to output tensor in X memory
 * @return     The function returns either
 * <code>ETA_GEN_ERR</code> or <code>ETA_STATUS_OK</code> based on the success or failure.
 *
 * This function implements convolution with 3x3 kernel size and stride of 2.
 * It is optimized to make use of both macs available in the DSP.
 */

eta_rc eta_conv2d_q7_chw_ker3x3_stride2_pad0_relu(
    int _XMEM *Im_in,
    int chess_storage(YMEM) *wt,
    int _YMEM *bias,  //bias vector size = # kernels = dim_ch_out
    conv2d_opt_st* opt_st,
    int *out
    );

eta_rc eta_conv2d_q7_chw_ker3x3_stride2_pad0_relu_hwc(
    int _XMEM *Im_in,
    int chess_storage(YMEM) *wt,
    int _YMEM *bias,  //bias vector size = # kernels = dim_ch_out
    conv2d_opt_st* opt_st,
    int *out
    );

/*
 * @brief Q7 depthwise convolution function for 3x3 kernel and stride 2, input format CHW
 * @param[in]       Im_in        pointer to input tensor in X memory
 * @param[in]       wt           pointer to kernel weights in Y memory
 * @param[in]       bias         pointer to bias in Y memory
 * @param[in]       opt_st       pointer to conv2d paramter structure.
 * @param[in,out]   out          pointer to output tensor in X memory
 * @return     The function returns either
 * <code>ETA_GEN_ERR</code> or <code>ETA_STATUS_OK</code> based on the success or failure.
 *
 * This function implements depthwise convolution with 3x3 kernel size and stride of 2.
 * It is optimized to make use of both macs available in the DSP.
 */

eta_rc eta_ds_conv2d_q7_chw_ker3x3_stride2_pad0_relu(
    int _XMEM *Im_in,
    int chess_storage(YMEM) *wt,
    int _YMEM *bias,  //bias vector size = # kernels = dim_ch_out
    conv2d_opt_st* opt_st,
    int *out
    );

eta_rc eta_ds_conv2d_q7_chw_ker3x3_stride2_pad0_relu_hwc(
    int _XMEM *Im_in,
    int chess_storage(YMEM) *wt,
    int _YMEM *bias,  //bias vector size = # kernels = dim_ch_out
    conv2d_opt_st* opt_st,
    int *out
    );

/*
 * @brief Q7 convolution function for 3x3 kernel and stride 2, input format CHW
 * @param[in]       Im_in        pointer to input tensor in X memory
 * @param[in]       wt           pointer to kernel weights in Y memory
 * @param[in]       bias         pointer to bias in Y memory
 * @param[in]       opt_st       pointer to conv2d paramter structure.
 * @param[in,out]   out          pointer to output tensor in Y memory
 * @return     The function returns either
 * <code>ETA_GEN_ERR</code> or <code>ETA_STATUS_OK</code> based on the success or failure.
 *
 * This function implements convolution with 3x3 kernel size and stride of 2.
 * It is optimized to make use of both macs available in the DSP.
 */

eta_rc eta_conv2d_q7_chw_ker3x3_stride2_pad0_relu_ex(
    int _XMEM *Im_in,
    int chess_storage(YMEM) *wt,
    int _YMEM *bias,  //bias vector size = # kernels = dim_ch_out
    conv2d_opt_st* opt_st,
    int _YMEM *out
    );

/*
 * @brief Q7 convolution function for 3x3 kernel and stride 1, input format CHW
 * @param[in]       Im_in        pointer to input tensor in X memory
 * @param[in]       wt           pointer to kernel weights in Y memory
 * @param[in]       bias         pointer to bias in Y memory
 * @param[in]       opt_st       pointer to conv2d paramter structure.
 * @param[in,out]   out          pointer to output tensor in X memory
 * @return     The function returns either
 * <code>ETA_GEN_ERR</code> or <code>ETA_STATUS_OK</code> based on the success or failure.
 *
 * This function implements convolution with 3x3 kernel size and stride of 1.
 * It is optimized to make use of both macs available in the DSP.
 */

eta_rc eta_conv2d_q7_chw_ker3x3_stride1_pad0_relu(
    int _XMEM *Im_in,
    int chess_storage(YMEM) *wt,
    int _YMEM *bias,  //bias vector size = # kernels = dim_ch_out
    conv2d_opt_st* opt_st,
    int *out
    );

/*
 * @brief Q7 depthwise convolution function for 3x3 kernel and stride 1, input format CHW
 * @param[in]       Im_in        pointer to input tensor in X memory
 * @param[in]       wt           pointer to kernel weights in Y memory
 * @param[in]       bias         pointer to bias in Y memory
 * @param[in]       opt_st       pointer to conv2d paramter structure.
 * @param[in,out]   out          pointer to output tensor in X memory
 * @return     The function returns either
 * <code>ETA_GEN_ERR</code> or <code>ETA_STATUS_OK</code> based on the success or failure.
 *
 * This function implements depthwise convolution with 3x3 kernel size and stride of 1.
 * It is optimized to make use of both macs available in the DSP.
 */

eta_rc eta_ds_conv2d_q7_chw_ker3x3_stride1_pad0_relu(
    int _XMEM *Im_in,
    int chess_storage(YMEM) *wt,
    int _YMEM *bias,  //bias vector size = # kernels = dim_ch_out
    conv2d_opt_st* opt_st,
    int *out
    );

eta_rc eta_ds_conv2d_q7_chw_ker3x3_stride1_pad0_relu_hwc(
    int _XMEM *Im_in,
    int chess_storage(YMEM) *wt,
    int _YMEM *bias,  //bias vector size = # kernels = dim_ch_out
    conv2d_opt_st* opt_st,
    int *out
    );

/*
 * @brief Q7 convolution function for 3x3 kernel and stride 1, input format CHW
 * @param[in]       Im_in        pointer to input tensor in X memory
 * @param[in]       wt           pointer to kernel weights in Y memory
 * @param[in]       bias         pointer to bias in Y memory
 * @param[in]       opt_st       pointer to conv2d paramter structure.
 * @param[in,out]   out          pointer to output tensor in Y memory
 * @return     The function returns either
 * <code>ETA_GEN_ERR</code> or <code>ETA_STATUS_OK</code> based on the success or failure.
 *
 * This function implements convolution with 3x3 kernel size and stride of 1.
 * It is optimized to make use of both macs available in the DSP.
 */

eta_rc eta_conv2d_q7_chw_ker3x3_stride1_pad0_relu_ex(
    int _XMEM *Im_in,
    int chess_storage(YMEM) *wt,
    int _YMEM *bias,  //bias vector size = # kernels = dim_ch_out
    conv2d_opt_st* opt_st,
    int _YMEM *out
    );
#endif
