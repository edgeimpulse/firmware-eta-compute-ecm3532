/*******************************************************************************
*
* @file eta_math.h
*
* Copyright (C) 2020 Eta Compute, Inc
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

#ifndef _ETA_MATH_
#define _ETA_MATH_

#include "cf6_chess.h"
#include "mathlib.h"

/**
 * @brief Error status returned by some functions in the library.
 */

typedef enum
{
    ETA_MATH_SUCCESS = 0,                /**< No error */
    ETA_MATH_ARGUMENT_ERROR = -1,        /**< One or more arguments are incorrect */
    ETA_MATH_LENGTH_ERROR = -2,          /**< Length of data buffer is incorrect */
    ETA_MATH_NANINF = -4,                /**< Not-a-number (NaN) or infinity is generated */
} eta_status;

/**
 * @brief Instance structure for the Q15 RFFT function.
 */
typedef struct
{
    int fftLenReal;      /**< length of the real FFT. */
    const CFX_FFT_TABLE* table;    /**< pointer to twiddle factors table. */
} eta_rfft_instance_q15;



/**
  @brief         This function computes natural log of input. y=ln(x)
  @param[in]     x -> input value in Q1.15 format.
  @return        returns natural log of input in Q5.11 format.

  [Caution]      Assumes input is always positive value as log is not defined for -ve values.
                 Behaviour is not defined for -ve input.
 */
fix eta_log(fix x);

/**
  @brief         This function computes exponential of input. y = exponential(x)
  @param[in]     x -> input value in Q1.15 format.
  @return        returns exponential of input in Q3.13 format.
 */
fix eta_exp(fix x);

/**
  @brief         This function computes square root of input. y = sqrt(x)
  @param[in]     x -> input value in Q1.15 format.
  @return        returns sqrt of input in Q1.15 format.
 */
fix eta_sqrt(fix x);

/**
  @brief         Initialization function for the Q15 RFFT.
  @param[in,out] S              points to an instance of the Q15 RFFT/RIFFT structure
  @param[in]     fftLenReal     length of the FFT
  @return        execution status
                   - \ref ETA_MATH_SUCCESS        : Operation successful
                   - \ref ETA_MATH_ARGUMENT_ERROR : <code>fftLenReal</code> is not a supported length

  @par           Details
                   The parameter <code>fftLenReal</code> specifies length of RFFT/RIFFT Process.
                   Supported FFT Lengths are 8, 16, 32, 64, 128, 256, 512, 1024.
                   This function also initializes Twiddle factor table.
 */
eta_status eta_rfft_init_q15(eta_rfft_instance_q15* S, int fftLenReal);

/**
  @brief         Processing function for the Q15 RFFT/RIFFT.
  @param[in]     S     points to an instance of the Q15 RFFT structure
  @param[in]     pSrc  points to input buffer
  @param[out]    pDst  points to output buffer
  @param[out]    pTemp  temporary scratch buffer internally used by this function. its size is fft_length/2

  @return        none

  @par           Internally input is downscaled by 2 for every stage to avoid saturations inside rFFT process.
                 input output data format is compatible with CMSIS fft library functions.
  @par
 */

void eta_rfft_q15(const eta_rfft_instance_q15* S, fix* pSrc, fix* pDst, fix* pTemp);


#endif //_ETA_MATH_
