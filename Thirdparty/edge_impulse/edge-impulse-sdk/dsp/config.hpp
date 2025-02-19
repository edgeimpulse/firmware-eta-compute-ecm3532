/* The Clear BSD License
 *
 * Copyright (c) 2025 EdgeImpulse Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted (subject to the limitations in the disclaimer
 * below) provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 *   * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 *   * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY
 * THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _EIDSP_CPP_CONFIG_H_
#define _EIDSP_CPP_CONFIG_H_

// clang-format off
#ifndef EIDSP_USE_CMSIS_DSP // __ARM_ARCH_PROFILE is a predefine of arm-gcc.  __TARGET_* is armcc
#if defined(__MBED__) || __ARM_ARCH_PROFILE == 'M' || defined(__TARGET_CPU_CORTEX_M0) || defined(__TARGET_CPU_CORTEX_M0PLUS) || defined(__TARGET_CPU_CORTEX_M3) || defined(__TARGET_CPU_CORTEX_M4) || defined(__TARGET_CPU_CORTEX_M7) || defined(USE_HAL_DRIVER) || defined(ARDUINO_NRF52_ADAFRUIT)
    // Mbed OS versions before 5.7 are not based on CMSIS5, disable CMSIS-DSP and CMSIS-NN instructions
    #if defined(__MBED__)
        #include "mbed_version.h"
        #if (MBED_VERSION < MBED_ENCODE_VERSION((5), (7), (0)))
            #define EIDSP_USE_CMSIS_DSP      0
        #else
            #define EIDSP_USE_CMSIS_DSP      1
        #endif // Mbed OS 5.7 version check

        // Arduino on Mbed targets prior to Mbed OS 6.0.0 ship their own CMSIS-DSP sources
        #if defined(ARDUINO) && (MBED_VERSION < MBED_ENCODE_VERSION((6), (0), (0)))
            #define EIDSP_LOAD_CMSIS_DSP_SOURCES      0
        #else
            #define EIDSP_LOAD_CMSIS_DSP_SOURCES      1
        #endif // Mbed OS 6.0 version check
    #else
        #define EIDSP_USE_CMSIS_DSP		        1
        #define EIDSP_LOAD_CMSIS_DSP_SOURCES    1
    #endif
#else
    #define EIDSP_USE_CMSIS_DSP     0
#endif // Mbed / ARM Core check
#endif // ifndef EIDSP_USE_CMSIS_DSP

#if EIDSP_USE_CMSIS_DSP == 1
#define EIDSP_i32                int32_t
#define EIDSP_i16                int16_t
#define EIDSP_i8                 q7_t
#define ARM_MATH_ROUNDING        1
#else
#define EIDSP_i32                int32_t
#define EIDSP_i16                int16_t
#define EIDSP_i8                 int8_t
#endif // EIDSP_USE_CMSIS_DSP

#ifndef EIDSP_USE_ASSERTS
#define EIDSP_USE_ASSERTS        0
#endif // EIDSP_USE_ASSERTS

#if EIDSP_USE_ASSERTS == 1
#include <assert.h>
#define EIDSP_ERR(err_code) ei_printf("ERR: %d (%s)\n", err_code, #err_code); assert(false)
#else // EIDSP_USE_ASSERTS == 0
#define EIDSP_ERR(err_code) return(err_code)
#endif

// To save memory you can quantize the filterbanks,
// this has an effect on runtime speed as CMSIS-DSP does not have optimized instructions
// for q7 matrix multiplication and matrix transformation...
#ifndef EIDSP_QUANTIZE_FILTERBANK
#define EIDSP_QUANTIZE_FILTERBANK    1
#endif // EIDSP_QUANTIZE_FILTERBANK

// prints buffer allocations to stdout, useful when debugging
#ifndef EIDSP_TRACK_ALLOCATIONS
#define EIDSP_TRACK_ALLOCATIONS      0
#endif // EIDSP_TRACK_ALLOCATIONS

// set EIDSP_TRACK_ALLOCATIONS=1 and EIDSP_PRINT_ALLOCATIONS=0
// to track but not print allocations
#ifndef EIDSP_PRINT_ALLOCATIONS
#define EIDSP_PRINT_ALLOCATIONS      1
#endif

#ifndef EIDSP_SIGNAL_C_FN_POINTER
#define EIDSP_SIGNAL_C_FN_POINTER    0
#endif // EIDSP_SIGNAL_C_FN_POINTER

// clang-format on
#endif // _EIDSP_CPP_CONFIG_H_
