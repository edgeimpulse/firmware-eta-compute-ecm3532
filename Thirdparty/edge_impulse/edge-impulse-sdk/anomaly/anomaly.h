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

#ifndef _EDGE_IMPULSE_ANOMALY_H_
#define _EDGE_IMPULSE_ANOMALY_H_

#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include "model-parameters/anomaly_types.h"

#ifdef __cplusplus
namespace {
#endif // __cplusplus

/**
 * Standard scaler, scales all values in the input vector
 * Note that this *modifies* the array in place!
 * @param input Array of input values
 * @param scale Array of scale values (obtain from StandardScaler in Python)
 * @param mean Array of mean values (obtain from StandardScaler in Python)
 * @param input_size Size of input, scale and mean arrays
 */
void standard_scaler(float *input, const float *scale, const float *mean, size_t input_size) {
    for (size_t ix = 0; ix < input_size; ix++) {
        input[ix] = (input[ix] - mean[ix]) / scale[ix];
    }
}

/**
 * Calculate the distance between input vector and the cluster
 * @param input Array of input values (already scaled by standard_scaler)
 * @param input_size Size of the input array
 * @param cluster A cluster (number of centroids should match input_size)
 */
float calculate_cluster_distance(float *input, size_t input_size, const ei_classifier_anom_cluster_t *cluster) {
    // todo: check input_size and centroid size?

    float dist = 0.0f;
    for (size_t ix = 0; ix < input_size; ix++) {
        dist += pow(input[ix] - cluster->centroid[ix], 2);
    }
    return sqrt(dist) - cluster->max_error;
}

/**
 * Get minimum distance to a cluster
 * @param input Array of input values (already scaled by standard_scaler)
 * @param input_size Size of the input array
 * @param clusters Array of clusters
 * @param cluster_size Size of cluster array
 */
float get_min_distance_to_cluster(float *input, size_t input_size, const ei_classifier_anom_cluster_t *clusters, size_t cluster_size) {
    float min = 1000.0f;
    for (size_t ix = 0; ix < cluster_size; ix++) {
        float dist = calculate_cluster_distance(input, input_size, &clusters[ix]);
        if (dist < min) {
            min = dist;
        }
    }
    return min;
}

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _EDGE_IMPULSE_ANOMALY_H_
