
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

/**
 * @file eta_types.h
 * This header lists structures used for kernel options.
 */

/**
 * @brief The return code used in the kernel functions.
 */
typedef enum
{
  ETA_STATUS_OK     =  0,        /**< No error */
  ETA_GEN_ERR       = -1,        /**< Error in the compute of the function */
} eta_rc;


/**
 * @brief The struct for parameters for Fully Connected layers
 */
typedef struct fc_opt_st
{
	uint16_t filt_cols;     /**< num of cols in weight matrix */
	uint16_t filt_rows;     /**< num of rows in weight matrix */
	uint16_t bias_shift;    /**< amount of left shift for bias */
	uint16_t out_shift;     /**< amount of right shift for output */
	uint16_t input_length;  /**< length of input vector for FC layer */
  int16_t act_min;        /**< the minimum value of the output range */
  int16_t act_max;        /**< the maximum value of the output range */
} fc_opt;


/**
 * @brief The struct for parameters for 2D Pooling layers
 */
typedef struct pool2d_opt_st
{
  uint16_t in_rows;     /**< input tensor dimension height */
  uint16_t in_cols;     /**< input tensor dimension width */
  uint16_t in_depth;    /**< number of input tensor channels */
  uint16_t filt_rows;   /**< filter kernel size height */
  uint16_t filt_cols;   /**< filter kernel size width */
  uint16_t row_pad;     /**< padding size height */
  uint16_t col_pad;     /**< padding size width */
  uint16_t row_stride;  /**< convolution stride height */
  uint16_t col_stride;  /**< convolution stride width */
  uint16_t out_rows;    /**< output tensor dimension height */
  uint16_t out_cols;    /**< output tensor dimension width */
  uint16_t out_lshift;  /**< amount of left-shift for output */
  int16_t act_min;      /**< the minimum value of the output range */
  int16_t act_max;      /**< the maximum value of the output range */
} pool2d_opt;


/**
 * @brief The struct for parameters for 2D Convolution layers
 */
typedef struct conv2d_opt_st
{
  uint16_t in_rows;     /**< input tensor dimension height */
  uint16_t in_cols;     /**< input tensor dimension width */
  uint16_t in_depth;    /**< number of input tensor channels */
  uint16_t num_filt;    /**< number of filters */
  uint16_t filt_rows;   /**< filter kernel size height */
  uint16_t filt_cols;   /**< filter kernel size width */
  uint16_t row_pad;     /**< padding size height */
  uint16_t col_pad;     /**< padding size width */
  uint16_t row_stride;  /**< convolution stride height */
  uint16_t col_stride;  /**< convolution stride width */
  uint16_t bias_lshift; /**< amount of left-shift for bias */
  uint16_t out_rshift;  /**< amount of right-shift for output */
  uint16_t out_rows;    /**< output tensor dimension height */
  uint16_t out_cols;    /**< output tensor dimension width */
  int16_t act_min;      /**< the minimum value of the output range */
  int16_t act_max;      /**< the maximum value of the output range */
} conv2d_opt;


/// @cond
typedef struct fast_grnn_opt_st
{
  uint16_t in_cols;        /**< input tensor dimension width */
  uint16_t num_filt;       /**< number of filters */
  uint16_t bias_lshift;    /**< amount of left-shift for bias */
  uint16_t mid_rshift;     /**< amount of right-shift for sigmoid/tanh operation */
  uint16_t mid_int_width;  /**< integer size for sigmoid/tanh operation */
  uint16_t out_rshift;     /**< amount of right-shift for output */
} grnn_opt;


typedef struct rnnpool_opt_st
{
  uint16_t in_rows;     /**< input tensor dimension height */
  uint16_t in_cols;     /**< input tensor dimension width */
  uint16_t in_depth;    /**< number of input tensor channels */
  uint16_t num_filt1;   /**< number of filters for RNN1 */
  uint16_t num_filt2;   /**< number of filters for RNN2 */
  uint16_t bias_lshift; /**< amount of left-shift for bias */
  uint16_t out_rshift;  /**< amount of right-shift for output */
} rnnpool_opt;
/// @endcond


/**
 * @brief The struct for parameters for Add layers
 */
typedef struct add_opt_st
{
  uint8_t int_bits1;    /**< integer bits of the input1 */
  uint8_t int_bits2;    /**< integer bits of the input2 */
  uint8_t out_rshift;   /**< outshift for the desired output range */
  uint16_t num_elem;    /**< size of the input arrays */
} add_opt;


/**
 * @brief The struct for parameters for Concatenation layers
 */
typedef struct concat_opt_st
{
  uint8_t num_inputs;        /**< total number of input arrays */
  uint16_t axis;             /**< axis to be concatenated */
  uint16_t out_dims;         /**< dimension of the output */
  uint16_t *out_shape;       /**< output shape */
  uint16_t *in_dims_axis;    /**< array with the dimensions across the concat 
                             axis per input */
  uint16_t *in_bitshift;     /**< array with bit shift needed per input */
} concat_opt;

#endif
