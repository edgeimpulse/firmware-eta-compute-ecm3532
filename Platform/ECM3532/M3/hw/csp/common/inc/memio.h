/*
 * @file    memio.h
 * @author  DJW
 * @date    05/09/2018
 *
 * @brief This file contains memio module definitions
 *
 * Copyright (C) 2018 Eta Compute, Inc
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

#ifndef __MEMIO_H__
#define __MEMIO_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------------------------------------------------------

#define MEMIO64(addr) (*(volatile uint64_t*)(addr))
#define MEMIO32(addr) (*(volatile uint32_t*)(addr))
#define MEMIO16(addr) (*(volatile uint16_t*)(addr))
#define MEMIO8(addr)  (*(volatile uint8_t*)(addr))

//-----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif //__MEMIO_H__
