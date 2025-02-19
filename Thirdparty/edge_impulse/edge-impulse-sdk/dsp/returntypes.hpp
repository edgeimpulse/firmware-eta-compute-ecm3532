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

#ifndef _EIDSP_RETURN_TYPES_H_
#define _EIDSP_RETURN_TYPES_H_

#include <stdint.h>

namespace ei {

typedef enum {
    EIDSP_OK = 0,
    EIDSP_OUT_OF_MEM = -1002,
    EIDSP_SIGNAL_SIZE_MISMATCH = -1003,
    EIDSP_MATRIX_SIZE_MISMATCH = -1004,
    EIDSP_DCT_ERROR = -1005,
    EIDSP_INPUT_MATRIX_EMPTY = -1006,
    EIDSP_BUFFER_SIZE_MISMATCH = -1007,
    EIDSP_PARAMETER_INVALID = -1008,
    EIDSP_UNEXPECTED_NEXT_OFFSET = -1009,
    EIDSP_OUT_OF_BOUNDS = -1010,
    EIDSP_UNSUPPORTED_FILTER_CONFIG = -1011,
    EIDSP_NARROWING = -1012,
    EIDSP_BLOCK_VERSION_INCORRECT = -1013,
    EIDSP_NOT_SUPPORTED = -1014,
    EIDSP_REQUIRES_CMSIS_DSP = -1015,
    EIDSP_FFT_TABLE_NOT_LOADED = -1016
} EIDSP_RETURN_T;

} // namespace ei

#endif // _EIDSP_RETURN_TYPES_H_
