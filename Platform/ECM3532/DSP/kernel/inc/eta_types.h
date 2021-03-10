/*******************************************************************************
*
* @file eta_types.h
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

#ifndef _ETA_TYPES_H
#define _ETA_TYPES_H

/** kernel return type*/
typedef enum
{
    ETA_STATUS_OK     =  0,        /**< No error */
    ETA_GEN_ERR       = -1,        /**< Error in the compute of the function */
} eta_rc;

/** kernel configuration options*/
typedef struct fc_opt_st
{
    /** num of cols in weight matrix */
    int filt_cols;
    /** num of rows in weight matrix */
    int filt_rows;
    /** amount of left shift for bias */
    int bias_shift;
    /** amount of right shift for output */
    int out_shift;
    /** length of input vector for FC layer */
    int input_length;
    /** the minimum value of the output range */
    int act_min;
    /** the maximum value of the output range */
    int act_max;
} fc_opt;

/** pooling layer configuration options*/
typedef struct pool2d_opt_st
{
    /** input tensor dimension height */
    int in_rows;
    /** input tensor dimension width */
    int in_cols;
    /** number of input tensor channels */
    int in_depth;
    /** filter kernel size height */
    int filt_rows;
    /** filter kernel size width */
    int filt_cols;
    /** padding size height */
    int row_pad;
    /** padding size width */
    int col_pad;
    /** convolution stride height */
    int row_stride;
    /** convolution stride width */
    int col_stride;
    /** output tensor dimension height */
    int out_rows;
    /** output tensor dimension width */
    int out_cols;
    /** amount of left-shift for output */
    int out_lshift;
    /** the minimum value of the output range */
    int act_min;
    /** the maximum value of the output range */
    int act_max;
} pool2d_opt;

/** conv2d kernel configuration options*/
typedef struct conv2d_opt_st
{
    /** input tensor dimension height */
    int in_rows;
    /** input tensor dimension width */
    int in_cols;
    /** number of input tensor channels */
    int in_depth;
    /** number of filters */
    int num_filt;
    /** filter kernel size height */
    int filt_rows;
    /** filter kernel size width */
    int filt_cols;
    /** padding size height */
    int row_pad;
    /** padding size width */
    int col_pad;
    /** convolution stride height */
    int row_stride;
    /** convolution stride width */
    int col_stride;
    /** amount of left-shift for bias */
    int bias_lshift;
    /** amount of right-shift for output */
    int out_rshift;
    /** output tensor dimension height */
    int out_rows;
    /** output tensor dimension width */
    int out_cols;
    /** the minimum value of the output range */
    int act_min;
    /** the maximum value of the output range */
    int act_max;
} conv2d_opt;

typedef struct concat_opt_st
{
    /** total number of input arrays */
    int num_inputs;
    /** axis to be concatenated */
    int  axis;
    /** dimension of the output */
    int out_dims;
    /** output shape, used to determine internal loops */
    int *out_shape;
    /** array with the dimensions across the concat */
    int *in_dims_axis;
    /** axis per input, array with bit shift needed per input */
    int *in_bitshift;
}concat_opt;

/** point-wise kernel configuration options*/
typedef struct conv2d_pw_st
{
    /** input tensor dimension height */
    unsigned short int in_rows;
    /** input tensor dimension width */
    unsigned short int in_cols;
    /** number of input tensor channels */
    unsigned short int in_depth;
    /** number of filters */
    unsigned short int num_filt;
    /** amount of left-shift for bias */
    unsigned short int bias_lshift;
    /** amount of right-shift for output */
    unsigned short int out_rshift;
    /** the minimum value of the output range */
    signed short int act_min;
    /** the maximum value of the output range */
    signed short int act_max;
} conv_pw_opt;



#endif
