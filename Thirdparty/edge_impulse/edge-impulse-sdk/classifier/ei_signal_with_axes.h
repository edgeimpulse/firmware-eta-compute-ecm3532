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

#ifndef _EI_CLASSIFIER_SIGNAL_WITH_AXES_H_
#define _EI_CLASSIFIER_SIGNAL_WITH_AXES_H_

#include "edge-impulse-sdk/dsp/numpy_types.h"
#include "edge-impulse-sdk/dsp/returntypes.hpp"

#if !EIDSP_SIGNAL_C_FN_POINTER

using namespace ei;

class SignalWithAxes {
public:
    SignalWithAxes(signal_t *original_signal, uint8_t *axes, size_t axes_count):
        _original_signal(original_signal), _axes(axes), _axes_count(axes_count)
    {

    }

    signal_t * get_signal() {
        if (this->_axes_count == EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME) {
            return this->_original_signal;
        }

        wrapped_signal.total_length = _original_signal->total_length / EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME * _axes_count;
#ifdef __MBED__
        wrapped_signal.get_data = mbed::callback(this, &SignalWithAxes::get_data);
#else
        wrapped_signal.get_data = [this](size_t offset, size_t length, float *out_ptr) {
            return this->get_data(offset, length, out_ptr);
        };
#endif
        return &wrapped_signal;
    }

    int get_data(size_t offset, size_t length, float *out_ptr) {
        size_t offset_on_original_signal = offset / _axes_count * EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME;
        size_t length_on_original_signal = length / _axes_count * EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME;

        size_t out_ptr_ix = 0;

        for (size_t ix = offset_on_original_signal; ix < offset_on_original_signal + length_on_original_signal; ix += EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME) {
            for (size_t axis_ix = 0; axis_ix < this->_axes_count; axis_ix++) {
                int r = _original_signal->get_data(ix + _axes[axis_ix], 1, &out_ptr[out_ptr_ix++]);
                if (r != 0) {
                    return r;
                }
            }
        }

        return 0;
    }

private:
    signal_t *_original_signal;
    uint8_t *_axes;
    size_t _axes_count;
    signal_t wrapped_signal;
};

class SignalWithAxesI16 {
public:
    SignalWithAxesI16(signal_i16_t *original_signal, uint8_t *axes, size_t axes_count):
        _original_signal(original_signal), _axes(axes), _axes_count(axes_count)
    {

    }

    signal_i16_t * get_signal() {
        if (this->_axes_count == EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME) {
            return this->_original_signal;
        }

        wrapped_signal.total_length = _original_signal->total_length / EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME * _axes_count;
#ifdef __MBED__
        wrapped_signal.get_data = mbed::callback(this, &SignalWithAxesI16::get_data);
#else
        wrapped_signal.get_data = [this](size_t offset, size_t length, int16_t *out_ptr) {
            return this->get_data(offset, length, out_ptr);
        };
#endif
        return &wrapped_signal;
    }

    int get_data(size_t offset, size_t length, int16_t *out_ptr) {
        size_t offset_on_original_signal = offset / _axes_count * EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME;
        size_t length_on_original_signal = length / _axes_count * EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME;

        size_t out_ptr_ix = 0;

        for (size_t ix = offset_on_original_signal; ix < offset_on_original_signal + length_on_original_signal; ix += EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME) {
            for (size_t axis_ix = 0; axis_ix < this->_axes_count; axis_ix++) {
                int r = _original_signal->get_data(ix + _axes[axis_ix], 1, &out_ptr[out_ptr_ix++]);
                if (r != 0) {
                    return r;
                }
            }
        }

        return 0;
    }

private:
    signal_i16_t *_original_signal;
    uint8_t *_axes;
    size_t _axes_count;
    signal_i16_t wrapped_signal;
};

#endif // #if !EIDSP_SIGNAL_C_FN_POINTER

#endif // _EI_CLASSIFIER_SIGNAL_WITH_AXES_H_
