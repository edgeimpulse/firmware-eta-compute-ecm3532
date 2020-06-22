/***************************************************************************//**
 *
 * @file reg.h
 *
 * @brief Register include.
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
 *
 * This is part of revision 1.0.0a6 of the Tensai Software Development Kit.
 *
 ******************************************************************************/

#ifndef __REG_H__
#define __REG_H__

//------------------------------------------------------------------------------
//  Definitions
//------------------------------------------------------------------------------

#include <stdint.h>
#define reg64_t uint64_t
#define reg32_t uint32_t
#define reg16_t uint16_t
#define reg8_t  uint16_t

typedef enum bit_e
{
  LOW   = 0U,
  ZERO  = 0U,
  FALSE = 0U,
  HIGH  = 1U,
  ONE   = 1U,
  TRUE  = 1U,
} bit_t;

//------------------------------------------------------------------------------
//  bit field value macros - sets only specified fields, other fields cleared
//------------------------------------------------------------------------------
#define FV1(r,f,v)                                                                (((uint32_t)v) << BP_##r##_##f)
#define FV2(r,f1,v1,f2,v2)                                                        (FV1(r,f1,v1) | FV1(r,f2,v2))
#define FV3(r,f1,v1,f2,v2,f3,v3)                                                  (FV2(r,f1,v1,f2,v2) | FV1(r,f3,v3))
#define FV4(r,f1,v1,f2,v2,f3,v3,f4,v4)                                            (FV3(r,f1,v1,f2,v2,f3,v3) | FV1(r,f4,v4))
#define FV5(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5)                                      (FV4(r,f1,v1,f2,v2,f3,v3,f4,v4) | FV1(r,f5,v5))
#define FV6(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6)                                (FV5(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5) | FV1(r,f6,v6))
#define FV7(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7)                          (FV6(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6) | FV1(r,f7,v7))
#define FV8(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8)                    (FV7(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7) | FV1(r,f8,v8))
#define FV9(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9)              (FV8(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8) | FV1(r,f9,v9))
#define FV10(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9,f10,v10)     (FV9(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9) | FV1(r,f10,v10))

//------------------------------------------------------------------------------
//  mask value macros - bit-wise or of the specified bit field masks,
//                       unspecified fields are cleared (zero)
//------------------------------------------------------------------------------
#define MV1(r,f)                                (BM_##r##_##f)
#define MV2(r,f1,f2)                            (MV1(r,f1) | MV1(r,f2))
#define MV3(r,f1,f2,f3)                         (MV2(r,f1,f2) | MV1(r,f3))
#define MV4(r,f1,f2,f3,f4)                      (MV3(r,f1,f2,f3) | MV1(r,f4))
#define MV5(r,f1,f2,f3,f4,f5)                   (MV4(r,f1,f2,f3,f4) | MV1(r,f5))
#define MV6(r,f1,f2,f3,f4,f5,f6)                (MV5(r,f1,f2,f3,f4,f5) | MV1(r,f6))
#define MV7(r,f1,f2,f3,f4,f5,f6,f7)             (MV6(r,f1,f2,f3,f4,f5,f6) | MV1(r,f7))
#define MV8(r,f1,f2,f3,f4,f5,f6,f7,f8)          (MV7(r,f1,f2,f3,f4,f5,f6,f7) | MV1(r,f8))
#define MV9(r,f1,f2,f3,f4,f5,f6,f7,f8,f9)       (MV8(r,f1,f2,f3,f4,f5,f6,f7,f8) | MV1(r,f9))
#define MV10(r,f1,f2,f3,f4,f5,f6,f7,f8,f9,f10)  (MV9(r,f1,f2,f3,f4,f5,f6,f7,f8,f9) | MV1(r,f10))

//------------------------------------------------------------------------------
//  field value modify - sets only specified fields, other fields unchanged
//------------------------------------------------------------------------------
#if 0
#define FVAM1(a,r,f,v)                                                               ((REG_##r##_A(a).V & (~(MV1(r,f)))) | (FV1(r,f,(v))))
#define FVAM2(a,r,f1,v1,f2,v2)                                                       ((REG_##r##_A(a).V & (~(MV2(r,f1,f2)))) | (FV2(r,f1,v1,f2,v2)))
#define FVAM3(a,r,f1,v1,f2,v2,f3,v3)                                                 ((REG_##r##_A(a).V & (~(MV3(r,f1,f2,f3)))) | (FV3(r,f1,v1,f2,v2,f3,v3)))
#define FVAM4(a,r,f1,v1,f2,v2,f3,v3,f4,v4)                                           ((REG_##r##_A(a).V & (~(MV4(r,f1,f2,f3,f4)))) | (FV4(r,f1,v1,f2,v2,f3,v3,f4,v4)))
#define FVAM5(a,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5)                                     ((REG_##r##_A(a).V & (~(MV5(r,f1,f2,f3,f4,f5)))) | (FV5(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5)))
#define FVAM6(a,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6)                               ((REG_##r##_A(a).V & (~(MV6(r,f1,f2,f3,f4,f5,f6)))) | (FV6(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6)))
#define FVAM7(a,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7)                         ((REG_##r##_A(a).V & (~(MV7(r,f1,f2,f3,f4,f5,f6,f7)))) | (FV7(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7)))
#define FVAM8(a,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8)                   ((REG_##r##_A(a).V & (~(MV8(r,f1,f2,f3,f4,f5,f6,f7,f8)))) | (FV8(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8)))
#define FVAM9(a,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9)             ((REG_##r##_A(a).V & (~(MV9(r,f1,f2,f3,f4,f5,f6,f7,f8,f9)))) | (FV9(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9)))
#define FVAM10(a,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9,f10,v10)    ((REG_##r##_A(a).V & (~(MV10(r,f1,f2,f3,f4,f5,f6,f7,f8,f9,f10)))) | (FV10(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9,f10,v10)))

#define FVNCM1(n,c,r,f,v)                                                            ((REG_##r(n,c).V & (~(MV1(r,f)))) | (FV1(r,f,(v))))
#define FVNCM2(n,c,r,f1,v1,f2,v2)                                                    ((REG_##r(n,c).V & (~(MV2(r,f1,f2)))) | (FV2(r,f1,v1,f2,v2)))
#define FVNCM3(n,c,r,f1,v1,f2,v2,f3,v3)                                              ((REG_##r(n,c).V & (~(MV3(r,f1,f2,f3)))) | (FV3(r,f1,v1,f2,v2,f3,v3)))
#define FVNCM4(n,c,r,f1,v1,f2,v2,f3,v3,f4,v4)                                        ((REG_##r(n,c).V & (~(MV4(r,f1,f2,f3,f4)))) | (FV4(r,f1,v1,f2,v2,f3,v3,f4,v4)))
#define FVNCM5(n,c,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5)                                  ((REG_##r(n,c).V & (~(MV5(r,f1,f2,f3,f4,f5)))) | (FV5(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5)))
#define FVNCM6(n,c,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6)                            ((REG_##r(n,c).V & (~(MV6(r,f1,f2,f3,f4,f5,f6)))) | (FV6(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6)))
#define FVNCM7(n,c,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7)                      ((REG_##r(n,c).V & (~(MV7(r,f1,f2,f3,f4,f5,f6,f7)))) | (FV7(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7)))
#define FVNCM8(n,c,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8)                ((REG_##r(n,c).V & (~(MV8(r,f1,f2,f3,f4,f5,f6,f7,f8)))) | (FV8(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8)))
#define FVNCM9(n,c,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9)          ((REG_##r(n,c).V & (~(MV9(r,f1,f2,f3,f4,f5,f6,f7,f8,f9)))) | (FV9(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9)))
#define FVNCM10(n,c,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9,f10,v10) ((REG_##r(n,c).V & (~(MV10(r,f1,f2,f3,f4,f5,f6,f7,f8,f9,f10)))) | (FV10(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9,f10,v10)))
#endif

#define FVNM1(n,r,f,v)                                                               ((REG_##r(n).V & (~(MV1(r,f)))) | (FV1(r,f,(v))))
#define FVNM2(n,r,f1,v1,f2,v2)                                                       ((REG_##r(n).V & (~(MV2(r,f1,f2)))) | (FV2(r,f1,v1,f2,v2)))
#define FVNM3(n,r,f1,v1,f2,v2,f3,v3)                                                 ((REG_##r(n).V & (~(MV3(r,f1,f2,f3)))) | (FV3(r,f1,v1,f2,v2,f3,v3)))
#define FVNM4(n,r,f1,v1,f2,v2,f3,v3,f4,v4)                                           ((REG_##r(n).V & (~(MV4(r,f1,f2,f3,f4)))) | (FV4(r,f1,v1,f2,v2,f3,v3,f4,v4)))
#define FVNM5(n,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5)                                     ((REG_##r(n).V & (~(MV5(r,f1,f2,f3,f4,f5)))) | (FV5(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5)))
#define FVNM6(n,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6)                               ((REG_##r(n).V & (~(MV6(r,f1,f2,f3,f4,f5,f6)))) | (FV6(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6)))
#define FVNM7(n,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7)                         ((REG_##r(n).V & (~(MV7(r,f1,f2,f3,f4,f5,f6,f7)))) | (FV7(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7)))
#define FVNM8(n,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8)                   ((REG_##r(n).V & (~(MV8(r,f1,f2,f3,f4,f5,f6,f7,f8)))) | (FV8(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8)))
#define FVNM9(n,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9)             ((REG_##r(n).V & (~(MV9(r,f1,f2,f3,f4,f5,f6,f7,f8,f9)))) | (FV9(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9)))
#define FVNM10(n,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9,f10,v10)    ((REG_##r(n).V & (~(MV10(r,f1,f2,f3,f4,f5,f6,f7,f8,f9,f10)))) | (FV10(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9,f10,v10)))


#define FVM1(r,f,v)                                                                  ((REG_##r.V & (~(MV1(r,f)))) | (FV1(r,f,(v))))
#define FVM2(r,f1,v1,f2,v2)                                                          ((REG_##r.V & (~(MV2(r,f1,f2)))) | (FV2(r,f1,v1,f2,v2)))
#define FVM3(r,f1,v1,f2,v2,f3,v3)                                                    ((REG_##r.V & (~(MV3(r,f1,f2,f3)))) | (FV3(r,f1,v1,f2,v2,f3,v3)))
#define FVM4(r,f1,v1,f2,v2,f3,v3,f4,v4)                                              ((REG_##r.V & (~(MV4(r,f1,f2,f3,f4)))) | (FV4(r,f1,v1,f2,v2,f3,v3,f4,v4)))
#define FVM5(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5)                                        ((REG_##r.V & (~(MV5(r,f1,f2,f3,f4,f5)))) | (FV5(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5)))
#define FVM6(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6)                                  ((REG_##r.V & (~(MV6(r,f1,f2,f3,f4,f5,f6)))) | (FV6(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6)))
#define FVM7(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7)                            ((REG_##r.V & (~(MV7(r,f1,f2,f3,f4,f5,f6,f7)))) | (FV7(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7)))
#define FVM8(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8)                      ((REG_##r.V & (~(MV8(r,f1,f2,f3,f4,f5,f6,f7,f8)))) | (FV8(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8)))
#define FVM9(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9)                ((REG_##r.V & (~(MV9(r,f1,f2,f3,f4,f5,f6,f7,f8,f9)))) | (FV9(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9)))
#define FVM10(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9,f10,v10)       ((REG_##r.V & (~(MV10(r,f1,f2,f3,f4,f5,f6,f7,f8,f9,f10)))) | (FV10(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9,f10,v10)))

//------------------------------------------------------------------------------
//  field value toggle - toggles only specified fields, other fields unchanged
//------------------------------------------------------------------------------
#if 0
#define FVNCT1(n,c,r,f)                                 ((REG_##r(n,c).V ^ (MV1(r,f))))
#define FVNCT2(n,c,r,f1,f2)                             ((REG_##r(n,c).V ^ (MV2(r,f1,f2))))
#define FVNCT3(n,c,r,f1f2,f3)                           ((REG_##r(n,c).V ^ (MV3(r,f1,f2,f3))))
#define FVNCT4(n,c,r,f1,f2,f3,f4)                       ((REG_##r(n,c).V ^ (MV4(r,f1,f2,f3,f4))))
#define FVNCT5(n,c,r,f1,f2,f3,f4,f5)                    ((REG_##r(n,c).V ^ (MV5(r,f1,f2,f3,f4,f5))))
#define FVNCT6(n,c,r,f1,f2,f3,f4,f5,f6)                 ((REG_##r(n,c).V ^ (MV6(r,f1,f2,f3,f4,f5,f6))))
#define FVNCT7(n,c,r,f1,f2,f3,f4,f5,f6,f7)              ((REG_##r(n,c).V ^ (MV7(r,f1,f2,f3,f4,f5,f6,f7))))
#define FVNCT8(n,c,r,f1,f2,f3,f4,f5,f6,f7,f8)           ((REG_##r(n,c).V ^ (MV8(r,f1,f2,f3,f4,f5,f6,f7,f8))))
#define FVNCT9(n,c,r,f1,f2,f3,f4,f5,f6,f7,f8,f9)        ((REG_##r(n,c).V ^ (MV9(r,f1,f2,f3,f4,f5,f6,f7,f8,f9))))
#define FVNCT10(n,c,r,f1,f2,f3,f4,f5,f6,f7,f8,f9,f10)   ((REG_##r(n,c).V ^ (MV10(r,f1,f2,f3,f4,f5,f6,f7,f8,f9,f10))))
#endif

#define FVNT1(n,r,f)                                    ((REG_##r(n).V ^ (MV1(r,f))))
#define FVNT2(n,r,f1,f2)                                ((REG_##r(n).V ^ (MV2(r,f1,f2))))
#define FVNT3(n,r,f1f2,f3)                              ((REG_##r(n).V ^ (MV3(r,f1,f2,f3))))
#define FVNT4(n,r,f1,f2,f3,f4)                          ((REG_##r(n).V ^ (MV4(r,f1,f2,f3,f4))))
#define FVNT5(n,r,f1,f2,f3,f4,f5)                       ((REG_##r(n).V ^ (MV5(r,f1,f2,f3,f4,f5))))
#define FVNT6(n,r,f1,f2,f3,f4,f5,f6)                    ((REG_##r(n).V ^ (MV6(r,f1,f2,f3,f4,f5,f6))))
#define FVNT7(n,r,f1,f2,f3,f4,f5,f6,f7)                 ((REG_##r(n).V ^ (MV7(r,f1,f2,f3,f4,f5,f6,f7))))
#define FVNT8(n,r,f1,f2,f3,f4,f5,f6,f7,f8)              ((REG_##r(n).V ^ (MV8(r,f1,f2,f3,f4,f5,f6,f7,f8))))
#define FVNT9(n,r,f1,f2,f3,f4,f5,f6,f7,f8,f9)           ((REG_##r(n).V ^ (MV9(r,f1,f2,f3,f4,f5,f6,f7,f8,f9))))
#define FVNT10(n,r,f1,f2,f3,f4,f5,f6,f7,f8,f9,f10)      ((REG_##r(n).V ^ (MV10(r,f1,f2,f3,f4,f5,f6,f7,f8,f9,f10))))

#if 0
#define FVAT1(a,r,f)                                    ((REG_##r##_A(a).V ^ (MV1(r,f))))
#define FVAT2(a,r,f1,f2)                                ((REG_##r##_A(a).V ^ (MV2(r,f1,f2))))
#define FVAT3(a,r,f1f2,f3)                              ((REG_##r##_A(a).V ^ (MV3(r,f1,f2,f3))))
#define FVAT4(a,r,f1,f2,f3,f4)                          ((REG_##r##_A(a).V ^ (MV4(r,f1,f2,f3,f4))))
#define FVAT5(a,r,f1,f2,f3,f4,f5)                       ((REG_##r##_A(a).V ^ (MV5(r,f1,f2,f3,f4,f5))))
#define FVAT6(a,r,f1,f2,f3,f4,f5,f6)                    ((REG_##r##_A(a).V ^ (MV6(r,f1,f2,f3,f4,f5,f6))))
#define FVAT7(a,r,f1,f2,f3,f4,f5,f6,f7)                 ((REG_##r##_A(a).V ^ (MV7(r,f1,f2,f3,f4,f5,f6,f7))))
#define FVAT8(a,r,f1,f2,f3,f4,f5,f6,f7,f8)              ((REG_##r##_A(a).V ^ (MV8(r,f1,f2,f3,f4,f5,f6,f7,f8))))
#define FVAT9(a,r,f1,f2,f3,f4,f5,f6,f7,f8,f9)           ((REG_##r##_A(a).V ^ (MV9(r,f1,f2,f3,f4,f5,f6,f7,f8,f9))))
#define FVAT10(a,r,f1,f2,f3,f4,f5,f6,f7,f8,f9,f10)      ((REG_##r##_A(a).V ^ (MV10(r,f1,f2,f3,f4,f5,f6,f7,f8,f9,f10))))
#endif

#define FVT1(r,f)                                       ((REG_##r.V ^ (MV1(r,f))))
#define FVT2(r,f1,f2)                                   ((REG_##r.V ^ (MV2(r,f1,f2))))
#define FVT3(r,f1f2,f3)                                 ((REG_##r.V ^ (MV3(r,f1,f2,f3))))
#define FVT4(r,f1,f2,f3,f4)                             ((REG_##r.V ^ (MV4(r,f1,f2,f3,f4))))
#define FVT5(r,f1,f2,f3,f4,f5)                          ((REG_##r.V ^ (MV5(r,f1,f2,f3,f4,f5))))
#define FVT6(r,f1,f2,f3,f4,f5,f6)                       ((REG_##r.V ^ (MV6(r,f1,f2,f3,f4,f5,f6))))
#define FVT7(r,f1,f2,f3,f4,f5,f6,f7)                    ((REG_##r.V ^ (MV7(r,f1,f2,f3,f4,f5,f6,f7))))
#define FVT8(r,f1,f2,f3,f4,f5,f6,f7,f8)                 ((REG_##r.V ^ (MV8(r,f1,f2,f3,f4,f5,f6,f7,f8))))
#define FVT9(r,f1,f2,f3,f4,f5,f6,f7,f8,f9)              ((REG_##r.V ^ (MV9(r,f1,f2,f3,f4,f5,f6,f7,f8,f9))))
#define FVT10(r,f1,f2,f3,f4,f5,f6,f7,f8,f9,f10)         ((REG_##r.V ^ (MV10(r,f1,f2,f3,f4,f5,f6,f7,f8,f9,f10))))

//------------------------------------------------------------------------------
//  bit value - bit-wise OR of specified bit values,
//              unspecified bits are cleared (zero)
//------------------------------------------------------------------------------
#define BV1(r,b)                                        (1U << BP_##r##_##b)
#define BV2(r,b1,b2)                                    (BV1(r,b1) | BV1(r,b2))
#define BV3(r,b1,b2,b3)                                 (BV2(r,b1,b2) | BV1(r,b3))
#define BV4(r,b1,b2,b3,b4)                              (BV3(r,b1,b2,b3) | BV1(r,b4))
#define BV5(r,b1,b2,b3,b4,b5)                           (BV4(r,b1,b2,b3,b4) | BV1(r,b5))
#define BV6(r,b1,b2,b3,b4,b5,b6)                        (BV5(r,b1,b2,b3,b4,b5) | BV1(r,b6))
#define BV7(r,b1,b2,b3,b4,b5,b6,b7)                     (BV6(r,b1,b2,b3,b4,b5,b6) | BV1(r,b7))
#define BV8(r,b1,b2,b3,b4,b5,b6,b7,b8)                  (BV7(r,b1,b2,b3,b4,b5,b6,b7) | BV1(r,b8))
#define BV9(r,b1,b2,b3,b4,b5,b6,b7,b8,b9)               (BV8(r,b1,b2,b3,b4,b5,b6,b7,b8) | BV1(r,b9))
#define BV10(r,b1,b2,b3,b4,b5,b6,b7,b8,b9,b10)          (BV9(r,b1,b2,b3,b4,b5,b6,b7,b8,b9) | BV1(r,b10))

//------------------------------------------------------------------------------
//  bit field modify - sets only specified fields, other fields unchanged,
//                     actually writes to register
//------------------------------------------------------------------------------
#if 0
#define REGA_M1(a,r,f,v)                                                                (REG_##r##_A(a).V = FVAM1(a,r,f,v))
#define REGA_M2(a,r,f1,v1,f2,v2)                                                        (REG_##r##_A(a).V = FVAM2(a,r,f1,v1,f2,v2))
#define REGA_M3(a,r,f1,v1,f2,v2,f3,v3)                                                  (REG_##r##_A(a).V = FVAM3(a,r,f1,v1,f2,v2,f3,v3))
#define REGA_M4(a,r,f1,v1,f2,v2,f3,v3,f4,v4)                                            (REG_##r##_A(a).V = FVAM4(a,r,f1,v1,f2,v2,f3,v3,f4,v4))
#define REGA_M5(a,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5)                                      (REG_##r##_A(a).V = FVAM5(a,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5))
#define REGA_M6(a,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6)                                (REG_##r##_A(a).V = FVAM6(a,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6))
#define REGA_M7(a,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7)                          (REG_##r##_A(a).V = FVAM7(a,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7))
#define REGA_M8(a,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8)                    (REG_##r##_A(a).V = FVAM8(a,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8))
#define REGA_M9(a,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9)              (REG_##r##_A(a).V = FVAM9(a,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9))
#define REGA_M10(a,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9,f10,v10)     (REG_##r##(a).V = FVAM10(a,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9,f10,v10))

// n = module number, c = channel number
#define REGNC_M1(n,c,r,f,v)                                                             (REG_##r(n,c).V = FVNCM1(n,c,r,f,v))
#define REGNC_M2(n,c,r,f1,v1,f2,v2)                                                     (REG_##r(n,c).V = FVNCM2(n,c,r,f1,v1,f2,v2))
#define REGNC_M3(n,c,r,f1,v1,f2,v2,f3,v3)                                               (REG_##r(n,c).V = FVNCM3(n,c,r,f1,v1,f2,v2,f3,v3))
#define REGNC_M4(n,c,r,f1,v1,f2,v2,f3,v3,f4,v4)                                         (REG_##r(n,c).V = FVNCM4(n,c,r,f1,v1,f2,v2,f3,v3,f4,v4))
#define REGNC_M5(n,c,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5)                                   (REG_##r(n,c).V = FVNCM5(n,c,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5))
#define REGNC_M6(n,c,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6)                             (REG_##r(n,c).V = FVNCM6(n,c,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6))
#define REGNC_M7(n,c,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7)                       (REG_##r(n,c).V = FVNCM7(n,c,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7))
#define REGNC_M8(n,c,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8)                 (REG_##r(n,c).V = FVNCM8(n,c,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8))
#define REGNC_M9(n,c,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9)           (REG_##r(n,c).V = FVNCM9(n,c,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9))
#define REGNC_M10(n,c,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9,f10,v10)  (REG_##r(n,c).V = FVNCM10(n,c,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9,f10,v10))
#endif

#define REGN_M1(n,r,f,v)                                                                (REG_##r(n).V = FVNM1(n, r,f,v))
#define REGN_M2(n,r,f1,v1,f2,v2)                                                        (REG_##r(n).V = FVNM2(n,r,f1,v1,f2,v2))
#define REGN_M3(n,r,f1,v1,f2,v2,f3,v3)                                                  (REG_##r(n).V = FVNM3(n,r,f1,v1,f2,v2,f3,v3))
#define REGN_M4(n,r,f1,v1,f2,v2,f3,v3,f4,v4)                                            (REG_##r(n).V = FVNM4(n,r,f1,v1,f2,v2,f3,v3,f4,v4))
#define REGN_M5(n,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5)                                      (REG_##r(n).V = FVNM5(n,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5))
#define REGN_M6(n,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6)                                (REG_##r(n).V = FVNM6(n,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6))
#define REGN_M7(n,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7)                          (REG_##r(n).V = FVNM7(n,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7))
#define REGN_M8(n,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8)                    (REG_##r(n).V = FVNM8(n,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8))
#define REGN_M9(n,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9)              (REG_##r(n).V = FVNM9(n,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9))
#define REGN_M10(n,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9,f10,v10)     (REG_##r(n).V = FVNM10(n,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9,f10,v10))

#define REG_M1(r,f,v)                                                                   (REG_##r.V = FVM1(r,f,v))
#define REG_M2(r,f1,v1,f2,v2)                                                           (REG_##r.V = FVM2(r,f1,v1,f2,v2))
#define REG_M3(r,f1,v1,f2,v2,f3,v3)                                                     (REG_##r.V = FVM3(r,f1,v1,f2,v2,f3,v3))
#define REG_M4(r,f1,v1,f2,v2,f3,v3,f4,v4)                                               (REG_##r.V = FVM4(r,f1,v1,f2,v2,f3,v3,f4,v4))
#define REG_M5(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5)                                         (REG_##r.V = FVM5(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5))
#define REG_M6(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6)                                   (REG_##r.V = FVM6(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6))
#define REG_M7(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7)                             (REG_##r.V = FVM7(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7))
#define REG_M8(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8)                       (REG_##r.V = FVM8(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8))
#define REG_M9(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9)                 (REG_##r.V = FVM9(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9))
#define REG_M10(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9,f10,v10)        (REG_##r.V = FVM10(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9,f10,v10))

//------------------------------------------------------------------------------
//  bit field toggle - toggles only specified fields, other fields unchanged,
//                     actually writes to register
//------------------------------------------------------------------------------
#if 0
#define REGNC_T1(n,c,r,f)                                 (REG_##r(n,c).V = FVNCT1(n,c,r,f))
#define REGNC_T2(n,c,r,f1,f2)                             (REG_##r(n,c).V = FVNCT2(n,c,r,f1,f2))
#define REGNC_T3(n,c,r,f1,f2,f3)                          (REG_##r(n,c).V = FVNCT3(n,c,r,f1,f2,f3))
#define REGNC_T4(n,c,r,f1,f2,f3,f4)                       (REG_##r(n,c).V = FVNCT4(n,c,r,f1,f2,f3,f4))
#define REGNC_T5(n,c,r,f1,f2,f3,f4,f5)                    (REG_##r(n,c).V = FVNCT5(n,c,r,f1,f2,f3,f4,f5))
#define REGNC_T6(n,c,r,f1,f2,f3,f4,f5,f6)                 (REG_##r(n,c).V = FVNCT6(n,c,r,f1,f2,f3,f4,f5,f6))
#define REGNC_T7(n,c,r,f1,f2,f3,f4,f5,f6,f7)              (REG_##r(n,c).V = FVNCT7(n,c,r,f1,f2,f3,f4,f5,f6,f7))
#define REGNC_T8(n,c,r,f1,f2,f3,f4,f5,f6,f7,f8)           (REG_##r(n,c).V = FVNCT8(n,c,r,f1,f2,f3,f4,f5,f6,f7,f8))
#define REGNC_T9(n,c,r,f1,f2,f3,f4,f5,f6,f7,f8,f9)        (REG_##r(n,c).V = FVNCT9(n,c,r,f1,f2,f3,f4,f5,f6,f7,f8,f9))
#define REGNC_T10(n,c,r,f1,f2,f3,f4,f5,f6,f7,f8,f9,f10)   (REG_##r(n,c).V = FVNCT10(n,c,r,f1,f2,f3,f4,f5,f6,f7,f8,f9,f10))
#endif

#define REGN_T1(n,r,f)                                    (REG_##r(n).V = FVNT1(n,r,f))
#define REGN_T2(n,r,f1,f2)                                (REG_##r(n).V = FVNT2(n,r,f1,f2))
#define REGN_T3(n,r,f1,f2,f3)                             (REG_##r(n).V = FVNT3(n,r,f1,f2,f3))
#define REGN_T4(n,r,f1,f2,f3,f4)                          (REG_##r(n).V = FVNT4(n,r,f1,f2,f3,f4))
#define REGN_T5(n,r,f1,f2,f3,f4,f5)                       (REG_##r(n).V = FVNT5(n,r,f1,f2,f3,f4,f5))
#define REGN_T6(n,r,f1,f2,f3,f4,f5,f6)                    (REG_##r(n).V = FVNT6(n,r,f1,f2,f3,f4,f5,f6))
#define REGN_T7(n,r,f1,f2,f3,f4,f5,f6,f7)                 (REG_##r(n).V = FVNT7(n,r,f1,f2,f3,f4,f5,f6,f7))
#define REGN_T8(n,r,f1,f2,f3,f4,f5,f6,f7,f8)              (REG_##r(n).V = FVNT8(n,r,f1,f2,f3,f4,f5,f6,f7,f8))
#define REGN_T9(n,r,f1,f2,f3,f4,f5,f6,f7,f8,f9)           (REG_##r(n).V = FVNT9(n,r,f1,f2,f3,f4,f5,f6,f7,f8,f9))
#define REGN_T10(n,r,f1,f2,f3,f4,f5,f6,f7,f8,f9,f10)      (REG_##r(n).V = FVNT10(n,r,f1,f2,f3,f4,f5,f6,f7,f8,f9,f10))

#if 0
#define REGA_T1(a,r,f)                                    (REG_##r##_A(a).V = FVAT1(a,r,f))
#define REGA_T2(a,r,f1,f2)                                (REG_##r##_A(a).V = FVAT2(a,r,f1,f2))
#define REGA_T3(a,r,f1,f2,f3)                             (REG_##r##_A(a).V = FVAT3(a,r,f1,f2,f3))
#define REGA_T4(a,r,f1,f2,f3,f4)                          (REG_##r##_A(a).V = FVAT4(a,r,f1,f2,f3,f4))
#define REGA_T5(a,r,f1,f2,f3,f4,f5)                       (REG_##r##_A(a).V = FVAT5(a,r,f1,f2,f3,f4,f5))
#define REGA_T6(a,r,f1,f2,f3,f4,f5,f6)                    (REG_##r##_A(a).V = FVAT6(a,r,f1,f2,f3,f4,f5,f6))
#define REGA_T7(a,r,f1,f2,f3,f4,f5,f6,f7)                 (REG_##r##_A(a).V = FVAT7(a,r,f1,f2,f3,f4,f5,f6,f7))
#define REGA_T8(a,r,f1,f2,f3,f4,f5,f6,f7,f8)              (REG_##r##_A(a).V = FVAT8(a,r,f1,f2,f3,f4,f5,f6,f7,f8))
#define REGA_T9(a,r,f1,f2,f3,f4,f5,f6,f7,f8,f9)           (REG_##r##_A(a).V = FVAT9(a,r,f1,f2,f3,f4,f5,f6,f7,f8,f9))
#define REGA_T10(a,r,f1,f2,f3,f4,f5,f6,f7,f8,f9,f10)      (REG_##r##_A(a).V = FVAT10(a,r,f1,f2,f3,f4,f5,f6,f7,f8,f9,f10))
#endif

#define REG_T1(r,f)                                       (REG_##r.V = FVT1(r,f))
#define REG_T2(r,f1,f2)                                   (REG_##r.V = FVT2(r,f1,f2))
#define REG_T3(r,f1,f2,f3)                                (REG_##r.V = FVT3(r,f1,f2,f3))
#define REG_T4(r,f1,f2,f3,f4)                             (REG_##r.V = FVT4(r,f1,f2,f3,f4))
#define REG_T5(r,f1,f2,f3,f4,f5)                          (REG_##r.V = FVT5(r,f1,f2,f3,f4,f5))
#define REG_T6(r,f1,f2,f3,f4,f5,f6)                       (REG_##r.V = FVT6(r,f1,f2,f3,f4,f5,f6))
#define REG_T7(r,f1,f2,f3,f4,f5,f6,f7)                    (REG_##r.V = FVT7(r,f1,f2,f3,f4,f5,f6,f7))
#define REG_T8(r,f1,f2,f3,f4,f5,f6,f7,f8)                 (REG_##r.V = FVT8(r,f1,f2,f3,f4,f5,f6,f7,f8))
#define REG_T9(r,f1,f2,f3,f4,f5,f6,f7,f8,f9)              (REG_##r.V = FVT9(r,f1,f2,f3,f4,f5,f6,f7,f8,f9))
#define REG_T10(r,f1,f2,f3,f4,f5,f6,f7,f8,f9,f10)         (REG_##r.V = FVT10(r,f1,f2,f3,f4,f5,f6,f7,f8,f9,f10))

//------------------------------------------------------------------------------
//  bit field value write - writes specified fields, unspecified fields are
//                          cleared (zero), actually writes to register
//------------------------------------------------------------------------------
#if 0
#define REGNC_W1(n,c,r,f,v)                                                             (REG_##r(n,c).V = FV1(r,f,v))
#define REGNC_W1(n,c,r,f,v)                                                             (REG_##r(n,c).V = FV1(r,f,v))
#define REGNC_W2(n,c,r,f1,v1,f2,v2)                                                     (REG_##r(n,c).V = FV2(r,f1,v1,f2,v2))
#define REGNC_W3(n,c,r,f1,v1,f2,v2,f3,v3)                                               (REG_##r(n,c).V = FV3(r,f1,v1,f2,v2,f3,v3))
#define REGNC_W4(n,c,r,f1,v1,f2,v2,f3,v3,f4,v4)                                         (REG_##r(n,c).V = FV4(r,f1,v1,f2,v2,f3,v3,f4,v4))
#define REGNC_W5(n,c,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5)                                   (REG_##r(n,c).V = FV5(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5))
#define REGNC_W6(n,c,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6)                             (REG_##r(n,c).V = FV6(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6))
#define REGNC_W7(n,c,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7)                       (REG_##r(n,c).V = FV7(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7))
#define REGNC_W8(n,c,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8)                 (REG_##r(n,c).V = FV8(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8))
#define REGNC_W9(n,c,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9)           (REG_##r(n,c).V = FV9(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9))
#define REGNC_W10(n,c,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9,f10,v10)  (REG_##r(n,c).V = FV10(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9,f10,v10))
#endif

#define REGN_W1(n,r,f,v)                                                                (REG_##r(n).V = FV1(r,f,v))
#define REGN_W2(n,r,f1,v1,f2,v2)                                                        (REG_##r(n).V = FV2(r,f1,v1,f2,v2))
#define REGN_W3(n,r,f1,v1,f2,v2,f3,v3)                                                  (REG_##r(n).V = FV3(r,f1,v1,f2,v2,f3,v3))
#define REGN_W4(n,r,f1,v1,f2,v2,f3,v3,f4,v4)                                            (REG_##r(n).V = FV4(r,f1,v1,f2,v2,f3,v3,f4,v4))
#define REGN_W5(n,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5)                                      (REG_##r(n).V = FV5(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5))
#define REGN_W6(n,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6)                                (REG_##r(n).V = FV6(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6))
#define REGN_W7(n,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7)                          (REG_##r(n).V = FV7(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7))
#define REGN_W8(n,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8)                    (REG_##r(n).V = FV8(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8))
#define REGN_W9(n,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9)              (REG_##r(n).V = FV9(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9))
#define REGN_W10(n,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9,f10,v10)     (REG_##r(n).V = FV10(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9,f10,v10))

#if 0
#define REGA_W1(a,r,f,v)                                                                (REG_##r##_A(a).V = FV1(r,f,v))
#define REGA_W2(a,r,f1,v1,f2,v2)                                                        (REG_##r##_A(a).V = FV2(r,f1,v1,f2,v2))
#define REGA_W3(a,r,f1,v1,f2,v2,f3,v3)                                                  (REG_##r##_A(a).V = FV3(r,f1,v1,f2,v2,f3,v3))
#define REGA_W4(a,r,f1,v1,f2,v2,f3,v3,f4,v4)                                            (REG_##r##_A(a).V = FV4(r,f1,v1,f2,v2,f3,v3,f4,v4))
#define REGA_W5(a,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5)                                      (REG_##r##_A(a).V = FV5(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5))
#define REGA_W6(a,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6)                                (REG_##r##_A(a).V = FV6(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6))
#define REGA_W7(a,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7)                          (REG_##r##_A(a).V = FV7(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7))
#define REGA_W8(a,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8)                    (REG_##r##_A(a).V = FV8(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8))
#define REGA_W9(a,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9)              (REG_##r##_A(a).V = FV9(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9))
#define REGA_W10(a,r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9,f10,v10)     (REG_##r##_A(a).V = FV10(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9,f10,v10))
#endif

#define REG_W1(r,f,v)                                                                   (REG_##r.V = FV1(r,f,v))
#define REG_W2(r,f1,v1,f2,v2)                                                           (REG_##r.V = FV2(r,f1,v1,f2,v2))
#define REG_W3(r,f1,v1,f2,v2,f3,v3)                                                     (REG_##r.V = FV3(r,f1,v1,f2,v2,f3,v3))
#define REG_W4(r,f1,v1,f2,v2,f3,v3,f4,v4)                                               (REG_##r.V = FV4(r,f1,v1,f2,v2,f3,v3,f4,v4))
#define REG_W5(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5)                                         (REG_##r.V = FV5(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5))
#define REG_W6(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6)                                   (REG_##r.V = FV6(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6))
#define REG_W7(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7)                             (REG_##r.V = FV7(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7))
#define REG_W8(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8)                       (REG_##r.V = FV8(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8))
#define REG_W9(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9)                 (REG_##r.V = FV9(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9))
#define REG_W10(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9,f10,v10)        (REG_##r.V = FV10(r,f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6,f7,v7,f8,v8,f9,v9,f10,v10))

//------------------------------------------------------------------------------
//  bit field value set - sets specified fields using read-modify-write,
//                        actually writes to register
//------------------------------------------------------------------------------
#define REGN_WS1(n,r,f)                               (REG_##r(n).V = (MV1(r,f)))
#define REGN_WS2(n,r,f1,f2)                           (REG_##r(n).V = (MV2(r,f1,f2)))
#define REGN_WS3(n,r,f1,f2,f3)                        (REG_##r(n).V = (MV3(r,f1,f2,f3)))
#define REGN_WS4(n,r,f1,f2,f3,f4)                     (REG_##r(n).V = (MV4(r,f1,f2,f3,f4)))
#define REGN_WS5(n,r,f1,f2,f3,f4,f5)                  (REG_##r(n).V = (MV5(r,f1,f2,f3,f4,f5)))
#define REGN_WS6(n,r,f1,f2,f3,f4,f5,f6)               (REG_##r(n).V = (MV6(r,f1,f2,f3,f4,f5,f6)))
#define REGN_WS7(n,r,f1,f2,f3,f4,f5,f6,f7)            (REG_##r(n).V = (MV7(r,f1,f2,f3,f4,f5,f6,f7)))
#define REGN_WS8(n,r,f1,f2,f3,f4,f5,f6,f7,f8)         (REG_##r(n).V = (MV8(r,f1,f2,f3,f4,f5,f6,f7,f8)))
#define REGN_WS9(n,r,f1,f2,f3,f4,f5,f6,f7,f8,f9)      (REG_##r(n).V = (MV9(r,f1,f2,f3,f4,f5,f6,f7,f8,f9)))
#define REGN_WS10(n,r,f1,f2,f3,f4,f5,f6,f7,f8,f9,f10) (REG_##r(n).V = (MV10(r,f1,f2,f3,f4,f5,f6,f7,f8,f9,f10)))

#define REGN_MS1(n,r,f)                                (REG_##r(n).V |= (MV1(r,f)))
#define REGN_MS2(n,r,f1,f2)                            (REG_##r(n).V |= (MV2(r,f1,f2)))
#define REGN_MS3(n,r,f1,f2,f3)                         (REG_##r(n).V |= (MV3(r,f1,f2,f3)))
#define REGN_MS4(n,r,f1,f2,f3,f4)                      (REG_##r(n).V |= (MV4(r,f1,f2,f3,f4)))
#define REGN_MS5(n,r,f1,f2,f3,f4,f5)                   (REG_##r(n).V |= (MV5(r,f1,f2,f3,f4,f5)))
#define REGN_MS6(n,r,f1,f2,f3,f4,f5,f6)                (REG_##r(n).V |= (MV6(r,f1,f2,f3,f4,f5,f6)))
#define REGN_MS7(n,r,f1,f2,f3,f4,f5,f6,f7)             (REG_##r(n).V |= (MV7(r,f1,f2,f3,f4,f5,f6,f7)))
#define REGN_MS8(n,r,f1,f2,f3,f4,f5,f6,f7,f8)          (REG_##r(n).V |= (MV8(r,f1,f2,f3,f4,f5,f6,f7,f8)))
#define REGN_MS9(n,r,f1,f2,f3,f4,f5,f6,f7,f8,f9)       (REG_##r(n).V |= (MV9(r,f1,f2,f3,f4,f5,f6,f7,f8,f9)))
#define REGN_MS10(n,r,f1,f2,f3,f4,f5,f6,f7,f8,f9,f10)  (REG_##r(n).V |= (MV10(r,f1,f2,f3,f4,f5,f6,f7,f8,f9,f10)))

#define REGN_S1(n,r,f)                                (REG_##r(n).V |= (MV1(r,f)))
#define REGN_S2(n,r,f1,f2)                            (REG_##r(n).V |= (MV2(r,f1,f2)))
#define REGN_S3(n,r,f1,f2,f3)                         (REG_##r(n).V |= (MV3(r,f1,f2,f3)))
#define REGN_S4(n,r,f1,f2,f3,f4)                      (REG_##r(n).V |= (MV4(r,f1,f2,f3,f4)))
#define REGN_S5(n,r,f1,f2,f3,f4,f5)                   (REG_##r(n).V |= (MV5(r,f1,f2,f3,f4,f5)))
#define REGN_S6(n,r,f1,f2,f3,f4,f5,f6)                (REG_##r(n).V |= (MV6(r,f1,f2,f3,f4,f5,f6)))
#define REGN_S7(n,r,f1,f2,f3,f4,f5,f6,f7)             (REG_##r(n).V |= (MV7(r,f1,f2,f3,f4,f5,f6,f7)))
#define REGN_S8(n,r,f1,f2,f3,f4,f5,f6,f7,f8)          (REG_##r(n).V |= (MV8(r,f1,f2,f3,f4,f5,f6,f7,f8)))
#define REGN_S9(n,r,f1,f2,f3,f4,f5,f6,f7,f8,f9)       (REG_##r(n).V |= (MV9(r,f1,f2,f3,f4,f5,f6,f7,f8,f9)))
#define REGN_S10(n,r,f1,f2,f3,f4,f5,f6,f7,f8,f9,f10)  (REG_##r(n).V |= (MV10(r,f1,f2,f3,f4,f5,f6,f7,f8,f9,f10)))

#if 0
#define REGA_S1(a,r,f)                                (REG_##r##_A(a).V |= (MV1(r,f)))
#define REGA_S2(a,r,f1,f2)                            (REG_##r##_A(a).V |= (MV2(r,f1,f2)))
#define REGA_S3(a,r,f1,f2,f3)                         (REG_##r##_A(a).V |= (MV3(r,f1,f2,f3)))
#define REGA_S4(a,r,f1,f2,f3,f4)                      (REG_##r##_A(a).V |= (MV4(r,f1,f2,f3,f4)))
#define REGA_S5(a,r,f1,f2,f3,f4,f5)                   (REG_##r##_A(a).V |= (MV5(r,f1,f2,f3,f4,f5)))
#define REGA_S6(a,r,f1,f2,f3,f4,f5,f6)                (REG_##r##_A(a).V |= (MV6(r,f1,f2,f3,f4,f5,f6)))
#define REGA_S7(a,r,f1,f2,f3,f4,f5,f6,f7)             (REG_##r##_A(a).V |= (MV7(r,f1,f2,f3,f4,f5,f6,f7)))
#define REGA_S8(a,r,f1,f2,f3,f4,f5,f6,f7,f8)          (REG_##r##_A(a).V |= (MV8(r,f1,f2,f3,f4,f5,f6,f7,f8)))
#define REGA_S9(a,r,f1,f2,f3,f4,f5,f6,f7,f8,f9)       (REG_##r##_A(a).V |= (MV9(r,f1,f2,f3,f4,f5,f6,f7,f8,f9)))
#define REGA_S10(a,r,f1,f2,f3,f4,f5,f6,f7,f8,f9,f10)  (REG_##r##_A(a).V |= (MV10(r,f1,f2,f3,f4,f5,f6,f7,f8,f9,f10)))
#endif

#define REG_WS1(r,f)                                   (REG_##r.V = (MV1(r,f)))
#define REG_WS2(r,f1,f2)                               (REG_##r.V = (MV2(r,f1,f2)))
#define REG_WS3(r,f1,f2,f3)                            (REG_##r.V = (MV3(r,f1,f2,f3)))
#define REG_WS4(r,f1,f2,f3,f4)                         (REG_##r.V = (MV4(r,f1,f2,f3,f4)))
#define REG_WS5(r,f1,f2,f3,f4,f5)                      (REG_##r.V = (MV5(r,f1,f2,f3,f4,f5)))
#define REG_WS6(r,f1,f2,f3,f4,f5,f6)                   (REG_##r.V = (MV6(r,f1,f2,f3,f4,f5,f6)))
#define REG_WS7(r,f1,f2,f3,f4,f5,f6,f7)                (REG_##r.V = (MV7(r,f1,f2,f3,f4,f5,f6,f7)))
#define REG_WS8(r,f1,f2,f3,f4,f5,f6,f7,f8)             (REG_##r.V = (MV8(r,f1,f2,f3,f4,f5,f6,f7,f8)))
#define REG_WS9(r,f1,f2,f3,f4,f5,f6,f7,f8,f9)          (REG_##r.V = (MV9(r,f1,f2,f3,f4,f5,f6,f7,f8,f9)))
#define REG_WS10(r,f1,f2,f3,f4,f5,f6,f7,f8,f9,f10)     (REG_##r.V = (MV10(r,f1,f2,f3,f4,f5,f6,f7,f8,f9,f10)))

#define REG_MS1(r,f)                                   (REG_##r.V |= (MV1(r,f)))
#define REG_MS2(r,f1,f2)                               (REG_##r.V |= (MV2(r,f1,f2)))
#define REG_MS3(r,f1,f2,f3)                            (REG_##r.V |= (MV3(r,f1,f2,f3)))
#define REG_MS4(r,f1,f2,f3,f4)                         (REG_##r.V |= (MV4(r,f1,f2,f3,f4)))
#define REG_MS5(r,f1,f2,f3,f4,f5)                      (REG_##r.V |= (MV5(r,f1,f2,f3,f4,f5)))
#define REG_MS6(r,f1,f2,f3,f4,f5,f6)                   (REG_##r.V |= (MV6(r,f1,f2,f3,f4,f5,f6)))
#define REG_MS7(r,f1,f2,f3,f4,f5,f6,f7)                (REG_##r.V |= (MV7(r,f1,f2,f3,f4,f5,f6,f7)))
#define REG_MS8(r,f1,f2,f3,f4,f5,f6,f7,f8)             (REG_##r.V |= (MV8(r,f1,f2,f3,f4,f5,f6,f7,f8)))
#define REG_MS9(r,f1,f2,f3,f4,f5,f6,f7,f8,f9)          (REG_##r.V |= (MV9(r,f1,f2,f3,f4,f5,f6,f7,f8,f9)))
#define REG_MS10(r,f1,f2,f3,f4,f5,f6,f7,f8,f9,f10)     (REG_##r.V |= (MV10(r,f1,f2,f3,f4,f5,f6,f7,f8,f9,f10)))

#define REG_S1(r,f)                                   (REG_##r.V |= (MV1(r,f)))
#define REG_S2(r,f1,f2)                               (REG_##r.V |= (MV2(r,f1,f2)))
#define REG_S3(r,f1,f2,f3)                            (REG_##r.V |= (MV3(r,f1,f2,f3)))
#define REG_S4(r,f1,f2,f3,f4)                         (REG_##r.V |= (MV4(r,f1,f2,f3,f4)))
#define REG_S5(r,f1,f2,f3,f4,f5)                      (REG_##r.V |= (MV5(r,f1,f2,f3,f4,f5)))
#define REG_S6(r,f1,f2,f3,f4,f5,f6)                   (REG_##r.V |= (MV6(r,f1,f2,f3,f4,f5,f6)))
#define REG_S7(r,f1,f2,f3,f4,f5,f6,f7)                (REG_##r.V |= (MV7(r,f1,f2,f3,f4,f5,f6,f7)))
#define REG_S8(r,f1,f2,f3,f4,f5,f6,f7,f8)             (REG_##r.V |= (MV8(r,f1,f2,f3,f4,f5,f6,f7,f8)))
#define REG_S9(r,f1,f2,f3,f4,f5,f6,f7,f8,f9)          (REG_##r.V |= (MV9(r,f1,f2,f3,f4,f5,f6,f7,f8,f9)))
#define REG_S10(r,f1,f2,f3,f4,f5,f6,f7,f8,f9,f10)     (REG_##r.V |= (MV10(r,f1,f2,f3,f4,f5,f6,f7,f8,f9,f10)))

//------------------------------------------------------------------------------
//  bit field value clear - clears specified fields using read-modify-write,
//                          actually writes to register
//------------------------------------------------------------------------------
#define REGN_C1(n,r,f)                                (REG_##r(n).V &= ~(MV1(r,f)))
#define REGN_C2(n,r,f1,f2)                            (REG_##r(n).V &= ~(MV2(r,f1,f2)))
#define REGN_C3(n,r,f1,f2,f3)                         (REG_##r(n).V &= ~(MV3(r,f1,f2,f3)))
#define REGN_C4(n,r,f1,f2,f3,f4)                      (REG_##r(n).V &= ~(MV4(r,f1,f2,f3,f4)))
#define REGN_C5(n,r,f1,f2,f3,f4,f5)                   (REG_##r(n).V &= ~(MV5(r,f1,f2,f3,f4,f5)))
#define REGN_C6(n,r,f1,f2,f3,f4,f5,f6)                (REG_##r(n).V &= ~(MV6(r,f1,f2,f3,f4,f5,f6)))
#define REGN_C7(n,r,f1,f2,f3,f4,f5,f6,f7)             (REG_##r(n).V &= ~(MV7(r,f1,f2,f3,f4,f5,f6,f7)))
#define REGN_C8(n,r,f1,f2,f3,f4,f5,f6,f7,f8)          (REG_##r(n).V &= ~(MV8(r,f1,f2,f3,f4,f5,f6,f7,f8)))
#define REGN_C9(n,r,f1,f2,f3,f4,f5,f6,f7,f8,f9)       (REG_##r(n).V &= ~(MV9(r,f1,f2,f3,f4,f5,f6,f7,f8,f9)))
#define REGN_C10(n,r,f1,f2,f3,f4,f5,f6,f7,f8,f9,f10)  (REG_##r(n).V &= ~(MV10(r,f1,f2,f3,f4,f5,f6,f7,f8,f9,f10)))

#if 0
#define REGA_C1(a,r,f)                                (REG_##r##_A(a).V &= ~(MV1(r,f)))
#define REGA_C2(a,r,f1,f2)                            (REG_##r##_A(a).V &= ~(MV2(r,f1,f2)))
#define REGA_C3(a,r,f1,f2,f3)                         (REG_##r##_A(a).V &= ~(MV3(r,f1,f2,f3)))
#define REGA_C4(a,r,f1,f2,f3,f4)                      (REG_##r##_A(a).V &= ~(MV4(r,f1,f2,f3,f4)))
#define REGA_C5(a,r,f1,f2,f3,f4,f5)                   (REG_##r##_A(a).V &= ~(MV5(r,f1,f2,f3,f4,f5)))
#define REGA_C6(a,r,f1,f2,f3,f4,f5,f6)                (REG_##r##_A(a).V &= ~(MV6(r,f1,f2,f3,f4,f5,f6)))
#define REGA_C7(a,r,f1,f2,f3,f4,f5,f6,f7)             (REG_##r##_A(a).V &= ~(MV7(r,f1,f2,f3,f4,f5,f6,f7)))
#define REGA_C8(a,r,f1,f2,f3,f4,f5,f6,f7,f8)          (REG_##r##_A(a).V &= ~(MV8(r,f1,f2,f3,f4,f5,f6,f7,f8)))
#define REGA_C9(a,r,f1,f2,f3,f4,f5,f6,f7,f8,f9)       (REG_##r##_A(a).V &= ~(MV9(r,f1,f2,f3,f4,f5,f6,f7,f8,f9)))
#define REGA_C10(a,r,f1,f2,f3,f4,f5,f6,f7,f8,f9,f10)  (REG_##r##_A(a).V &= ~(MV10(r,f1,f2,f3,f4,f5,f6,f7,f8,f9,f10)))
#endif

#define REG_C1(r,f)                                   (REG_##r.V &= ~(MV1(r,f)))
#define REG_C2(r,f1,f2)                               (REG_##r.V &= ~(MV2(r,f1,f2)))
#define REG_C3(r,f1,f2,f3)                            (REG_##r.V &= ~(MV3(r,f1,f2,f3)))
#define REG_C4(r,f1,f2,f3,f4)                         (REG_##r.V &= ~(MV4(r,f1,f2,f3,f4)))
#define REG_C5(r,f1,f2,f3,f4,f5)                      (REG_##r.V &= ~(MV5(r,f1,f2,f3,f4,f5)))
#define REG_C6(r,f1,f2,f3,f4,f5,f6)                   (REG_##r.V &= ~(MV6(r,f1,f2,f3,f4,f5,f6)))
#define REG_C7(r,f1,f2,f3,f4,f5,f6,f7)                (REG_##r.V &= ~(MV7(r,f1,f2,f3,f4,f5,f6,f7)))
#define REG_C8(r,f1,f2,f3,f4,f5,f6,f7,f8)             (REG_##r.V &= ~(MV8(r,f1,f2,f3,f4,f5,f6,f7,f8)))
#define REG_C9(r,f1,f2,f3,f4,f5,f6,f7,f8,f9)          (REG_##r.V &= ~(MV9(r,f1,f2,f3,f4,f5,f6,f7,f8,f9)))
#define REG_C10(r,f1,f2,f3,f4,f5,f6,f7,f8,f9,f10)     (REG_##r.V &= ~(MV10(r,f1,f2,f3,f4,f5,f6,f7,f8,f9,f10)))

//------------------------------------------------------------------------------
// reads the specified field only (shifted to least signicant bit position)
//------------------------------------------------------------------------------
#define REG_R(r,f)        (((REG_##r.V) & (BM_##r##_##f)) >> BP_##r##_##f)
#define REGN_R(n,r,f)     (((REG_##r(n).V) & (BM_##r##_##f)) >> BP_##r##_##f)
#if 0
#define REGNC_R(n,c,r,f)  (((REG_##r(n,c).V) & (BM_##r##_##f)) >> BP_##r##_##f)
#define REGA_R(a,r,f)     (((REG_##r##_A(a).V) & (BM_##r##_##f)) >> BP_##r##_##f)
#endif

//------------------------------------------------------------------------------
// calculate the maximum value that a given register field can hold
//------------------------------------------------------------------------------
#define REG_FMAX(r,f)     (BM_##r##_##f >> BP_##r##_##f)

//------------------------------------------------------------------------------
// mem field read - extracts the value of a field contained in an in-memory
// copy of a register
//------------------------------------------------------------------------------
#define REG_MFR(r,f,memval)   ((memval & (BM_##r##_##f)) >> BP_##r##_##f)

#endif  // __REG_H__

