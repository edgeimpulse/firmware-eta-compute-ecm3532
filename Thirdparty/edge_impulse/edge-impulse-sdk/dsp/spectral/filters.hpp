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

#ifndef _EIDSP_SPECTRAL_FILTERS_H_
#define _EIDSP_SPECTRAL_FILTERS_H_

#include <math.h>
#include "../numpy.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif // M_PI

namespace ei {
namespace spectral {
namespace filters {
    /**
     * The Butterworth filter has maximally flat frequency response in the passband.
     * @param filter_order Even filter order (between 2..8)
     * @param sampling_freq Sample frequency of the signal
     * @param cutoff_freq Cut-off frequency of the signal
     * @param src Source array
     * @param dest Destination array
     * @param size Size of both source and destination arrays
     */
    static void butterworth_lowpass(
        int filter_order,
        float sampling_freq,
        float cutoff_freq,
        const float *src,
        float *dest,
        size_t size)
    {
        int n_steps = filter_order / 2;
        float a = tan(M_PI * cutoff_freq / sampling_freq);
        float a2 = pow(a, 2);
        float *A = (float*)ei_calloc(n_steps, sizeof(float));
        float *d1 = (float*)ei_calloc(n_steps, sizeof(float));
        float *d2 = (float*)ei_calloc(n_steps, sizeof(float));
        float *w0 = (float*)ei_calloc(n_steps, sizeof(float));
        float *w1 = (float*)ei_calloc(n_steps, sizeof(float));
        float *w2 = (float*)ei_calloc(n_steps, sizeof(float));

        // Calculate the filter parameters
        for(int ix = 0; ix < n_steps; ix++) {
            float r = sin(M_PI * ((2.0 * ix) + 1.0) / (2.0 * filter_order));
            sampling_freq = a2 + (2.0 * a * r) + 1.0;
            A[ix] = a2 / sampling_freq;
            d1[ix] = 2.0 * (1 - a2) / sampling_freq;
            d2[ix] = -(a2 - (2.0 * a * r) + 1.0) / sampling_freq;
        }

        // Apply the filter
        for (size_t sx = 0; sx < size; sx++) {
            dest[sx] = src[sx];

            for (int i = 0; i < n_steps; i++) {
                w0[i] = d1[i] * w1[i] + d2[i] * w2[i] + dest[sx];
                dest[sx] = A[i] * (w0[i] + (2.0 * w1[i]) + w2[i]);
                w2[i] = w1[i];
                w1[i] = w0[i];
            }
        }

        ei_free(A);
        ei_free(d1);
        ei_free(d2);
        ei_free(w0);
        ei_free(w1);
        ei_free(w2);
    }

    /**
     * The Butterworth filter has maximally flat frequency response in the passband.
     * @param filter_order Even filter order (between 2..8)
     * @param sampling_freq Sample frequency of the signal
     * @param cutoff_freq Cut-off frequency of the signal
     * @param src Source array
     * @param dest Destination array
     * @param size Size of both source and destination arrays
     */
    static void butterworth_highpass(
        int filter_order,
        float sampling_freq,
        float cutoff_freq,
        const float *src,
        float *dest,
        size_t size)
    {
        int n_steps = filter_order / 2;
        float a = tan(M_PI * cutoff_freq / sampling_freq);
        float a2 = pow(a, 2);
        float *A = (float*)ei_calloc(n_steps, sizeof(float));
        float *d1 = (float*)ei_calloc(n_steps, sizeof(float));
        float *d2 = (float*)ei_calloc(n_steps, sizeof(float));
        float *w0 = (float*)ei_calloc(n_steps, sizeof(float));
        float *w1 = (float*)ei_calloc(n_steps, sizeof(float));
        float *w2 = (float*)ei_calloc(n_steps, sizeof(float));

        // Calculate the filter parameters
        for (int ix = 0; ix < n_steps; ix++) {
            float r = sin(M_PI * ((2.0 * ix) + 1.0) / (2.0 * filter_order));
            sampling_freq = a2 + (2.0 * a * r) + 1.0;
            A[ix] = 1.0f / sampling_freq;
            d1[ix] = 2.0 * (1 - a2) / sampling_freq;
            d2[ix] = -(a2 - (2.0 * a * r) + 1.0) / sampling_freq;
        }

        // Apply the filter
        for (size_t sx = 0; sx < size; sx++) {
            dest[sx] = src[sx];

            for (int i = 0; i < n_steps; i++) {
                w0[i] = d1[i] * w1[i] + d2[i] * w2[i] + dest[sx];
                dest[sx] = A[i] * (w0[i] - (2.0 * w1[i]) + w2[i]);
                w2[i] = w1[i];
                w1[i] = w0[i];
            }
        }

        ei_free(A);
        ei_free(d1);
        ei_free(d2);
        ei_free(w0);
        ei_free(w1);
        ei_free(w2);
    }

} // namespace filters
} // namespace spectral
} // namespace ei

#endif // _EIDSP_SPECTRAL_FILTERS_H_
