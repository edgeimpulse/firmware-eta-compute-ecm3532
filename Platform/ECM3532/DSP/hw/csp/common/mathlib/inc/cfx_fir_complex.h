/*File::   cfx_fir_complex.h                                                             */
/*$Revision:: 55766                                                                    $*/
/*$Date:: 2015-06-19 18:45:02 +0300 (Fri, 19 Jun 2015)                                 $*/
/* -----------------------------------------------------------------------------------  */
/* Copyright (c) 2006, 2007, 2008 NXP B.V.                                              */
/* All rights reserved.                                                                 */
/* This source code and any compilation or derivative thereof is the                    */
/* proprietary information of NXP B.V. and                                              */
/* is confidential in nature.                                                           */
/*                                                                                      */
/* Under no circumstances is this software to be combined with any Open                 */
/* Source Software in any way or placed under an Open Source License of                 */
/* any type without the express written permission of                                   */
/* NXP B.V.                                                                             */
/* -----------------------------------------------------------------------------------  */

#ifndef CFX_FIR_COMPLEX_H
#define CFX_FIR_COMPLEX_H

#include "cf6_chess.h"
#include "mathlib.h"

int cfx_fir_complex_1ch_sb_Process(fix *in_sample,
                                   fix *out_sample,
                                   const fix chess_storage(YMEM) *pFilterCoeff,
                                   int FilterLength,
                                   fix **ppDelay,
                                   fix *pDelayStart);

int cfx_fir_complex_1ch_bb_Process(fix *in_buff,
                                  fix *out_buff,
                                  int B,
                                  const fix chess_storage(YMEM) *pFilterCoeff,
                                  int FilterLength,
                                  fix **ppDelay,
                                  fix *pDelayStart);

int cfx_fir_complex_1ch_bb_v2_Process(fix *in_buff,
                                      fix *out_buff,
                                      int B,
                                      const fix chess_storage(YMEM) *pFilterCoeff,
                                      int FilterLength,
                                      fix **ppDelay,
                                      fix *pDelayStart);

int cfx_fir_complex_Init(int FilterLength,
                         fix *pDelay);

#endif //CFX_FIR_COMPLEX_H