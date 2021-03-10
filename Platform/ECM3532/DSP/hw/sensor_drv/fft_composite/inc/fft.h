/*
 * Copyright (C) 2018 Eta Compute, Inc.
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
 */

#ifndef H_FFT_MAIN_
#define H_FFT_MAIN_

#include <stdint.h>
#include "config.h"

#define N_FFT 512
#define FFT_OUTPUT_BUF_SIZE (N_FFT * 3 * 2)  //size in terms of bytes.
//#define FFT_WITH_WINDOWING

//offset contains pointer to non-interleave x,y,z sensor components.
void doFFTOnSensorData(uint16_t offset, uint16_t length);
void deInterleaveSensorData(uint16_t* data);

#endif  //H_FFT_MAIN_

