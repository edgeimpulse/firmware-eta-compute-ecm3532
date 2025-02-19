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

#ifndef _EDGE_IMPULSE_RUN_CLASSIFIER_TYPES_H_
#define _EDGE_IMPULSE_RUN_CLASSIFIER_TYPES_H_

#include <stdint.h>
#include "model-parameters/model_metadata.h"

typedef struct {
    const char *label;
    float value;
} ei_impulse_result_classification_t;

typedef struct {
    const char *label;
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
    float value;
} ei_impulse_result_bounding_box_t;

typedef struct {
    int sampling;
    int dsp;
    int classification;
    int anomaly;
    int64_t dsp_us;
    int64_t classification_us;
    int64_t anomaly_us;
} ei_impulse_result_timing_t;

typedef struct {
#if EI_CLASSIFIER_OBJECT_DETECTION == 1
    ei_impulse_result_bounding_box_t bounding_boxes[EI_CLASSIFIER_OBJECT_DETECTION_COUNT];
#else
    ei_impulse_result_classification_t classification[EI_CLASSIFIER_LABEL_COUNT];
#endif
    float anomaly;
    ei_impulse_result_timing_t timing;
} ei_impulse_result_t;

typedef struct {
    uint32_t buf_idx;
    float running_sum;
#if (EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW > 1)
    float maf_buffer[(EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW >> 1)];
#else
    float maf_buffer[1];
#endif
}ei_impulse_maf;

#endif // _EDGE_IMPULSE_RUN_CLASSIFIER_TYPES_H_
