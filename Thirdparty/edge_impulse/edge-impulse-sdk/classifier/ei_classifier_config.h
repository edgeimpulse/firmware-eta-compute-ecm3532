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

#ifndef _EI_CLASSIFIER_CONFIG_H_
#define _EI_CLASSIFIER_CONFIG_H_

// clang-format off
#ifndef EI_CLASSIFIER_TFLITE_ENABLE_CMSIS_NN
#if defined(__MBED__)
    #include "mbed.h"
    #if (MBED_VERSION < MBED_ENCODE_VERSION(5, 7, 0))
        #define EI_CLASSIFIER_TFLITE_ENABLE_CMSIS_NN      0
    #else
        #define EI_CLASSIFIER_TFLITE_ENABLE_CMSIS_NN      1
    #endif // Mbed OS 5.7 version check

// __ARM_ARCH_PROFILE is a predefine of arm-gcc.  __TARGET_* is armcc
#elif __ARM_ARCH_PROFILE == 'M' || defined(__TARGET_CPU_CORTEX_M0) || defined(__TARGET_CPU_CORTEX_M0PLUS) || defined(__TARGET_CPU_CORTEX_M3) || defined(__TARGET_CPU_CORTEX_M4) || defined(__TARGET_CPU_CORTEX_M7) || defined(ARDUINO_NRF52_ADAFRUIT)
    #define EI_CLASSIFIER_TFLITE_ENABLE_CMSIS_NN      1
#else
    #define EI_CLASSIFIER_TFLITE_ENABLE_CMSIS_NN      0
#endif
#endif // EI_CLASSIFIER_TFLITE_ENABLE_CMSIS_NN

// CMSIS-NN falls back to reference kernels when __ARM_FEATURE_DSP and __ARM_FEATURE_MVE are not defined
// we should never use those... So disable CMSIS-NN in that case and throw a warning
#if EI_CLASSIFIER_TFLITE_ENABLE_CMSIS_NN == 1
    #if !defined(__ARM_FEATURE_DSP) && !defined(__ARM_FEATURE_MVE)
        #pragma message( \
            "CMSIS-NN enabled, but neither __ARM_FEATURE_DSP nor __ARM_FEATURE_MVE defined. Falling back.")
        #undef EI_CLASSIFIER_TFLITE_ENABLE_CMSIS_NN
        #define EI_CLASSIFIER_TFLITE_ENABLE_CMSIS_NN 0
    #endif
#endif // EI_CLASSIFIER_TFLITE_ENABLE_CMSIS_NN == 1

#if EI_CLASSIFIER_TFLITE_ENABLE_CMSIS_NN == 1
#define CMSIS_NN                    1
#endif

#ifndef EI_CLASSIFIER_TFLITE_ENABLE_ARC
#ifdef CPU_ARC
#define EI_CLASSIFIER_TFLITE_ENABLE_ARC             1
#else
#define EI_CLASSIFIER_TFLITE_ENABLE_ARC             0
#endif // CPU_ARC
#endif // EI_CLASSIFIER_TFLITE_ENABLE_ARC

// clang-format on
#endif // _EI_CLASSIFIER_CONFIG_H_
