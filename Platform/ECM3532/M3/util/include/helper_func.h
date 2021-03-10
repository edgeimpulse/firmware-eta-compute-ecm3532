/*******************************************************************************
 *
 * Copyright (C) 2019 Eta Compute, Inc
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
 *
 ******************************************************************************/
#ifndef H_HELPER_FUNC
#define H_HELPER_FUNC

#include <stddef.h>

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x)[0])

int strcmp( const char *pcString1, const char *pcString2);

void *memcpy( void *pvDest, const void *pvSource, size_t xBytes);

void *memset( void *pvDest, int iValue, size_t xBytes);

void* AllocBuf(uint32_t Len, uint32_t Flag);

void FreeBuf(void *Ptr, uint32_t Flag);

#endif
