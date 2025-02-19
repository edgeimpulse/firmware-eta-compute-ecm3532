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

#ifndef _EIDSP_SPEECHPY_FUNCTIONS_H_
#define _EIDSP_SPEECHPY_FUNCTIONS_H_

#include <math.h>
#include "../numpy.hpp"
#include "../returntypes.hpp"

namespace ei {
namespace speechpy {

class functions {
public:
    /**
     * Converting from frequency to Mel scale
     *
     * @param f The frequency values(or a single frequency) in Hz.
     * @returns The mel scale values(or a single mel).
     */
    static float frequency_to_mel(float f) {
        return 1127.0 * numpy::log(1 + f / 700.0f);
    }

    /**
     * Converting from Mel scale to frequency.
     *
     * @param mel The mel scale values(or a single mel).
     * @returns The frequency values(or a single frequency) in Hz.
     */
    static float mel_to_frequency(float mel) {
        return 700.0f * (exp(mel / 1127.0f) - 1.0f);
    }

    /**
     * This function handle the issue with zero values if the are exposed
     * to become an argument for any log function.
     * @param input Array
     * @param input_size Size of array
     * @returns void
     */
    static void zero_handling(float *input, size_t input_size) {
        for (size_t ix = 0; ix < input_size; ix++) {
            if (input[ix] == 0) {
                input[ix] = 1e-10;
            }
        }
    }

    /**
     * This function handle the issue with zero values if the are exposed
     * to become an argument for any log function.
     * @param input Matrix
     * @returns void
     */
    static void zero_handling(matrix_t *input) {
        zero_handling(input->buffer, input->rows * input->cols);
    }


    /**
     * Triangle, I'm not really sure what this does
     * @param x Linspace output, will be overwritten!
     * @param x_size Size of the linspace output
     * @param left
     * @param middle
     * @param right
     */
    static int triangle(float *x, size_t x_size, int left, int middle, int right) {
        EI_DSP_MATRIX(out, 1, x_size);

        for (size_t ix = 0; ix < x_size; ix++) {
            if (x[ix] > left && x[ix] <= middle) {
                out.buffer[ix] = (x[ix] - left) / (middle - left);
            }

            if (x[ix] < right && middle <= x[ix]) {
                out.buffer[ix] = (right - x[ix]) / (right - middle);
            }
        }

        memcpy(x, out.buffer, x_size * sizeof(float));

        return EIDSP_OK;
    }
};

} // namespace speechpy
} // namespace ei

#endif // _EIDSP_SPEECHPY_FUNCTIONS_H_
