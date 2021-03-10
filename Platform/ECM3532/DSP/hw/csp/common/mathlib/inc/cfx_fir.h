/*File::   cfx_fir.h                                                             */
/*$Revision:: 56183                                                                    $*/
/*$Date:: 2018-11-07 19:27:39 +0200 (Wed, 07 Nov 2018)                                 $*/
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
#ifndef CFX_FIR_H 
#define CFX_FIR_H 

#include "cf6_chess.h"
#include "mathlib.h"
//#include "coolFlux_defs.h"

int cfx_fir_Init(int FilterLength, 
                 fix *pDelay) property(loop_levels_1);

fix cfx_fir_1ch_sb_Process(fix in_sample, 
                           const fix chess_storage(YMEM) *pFilterCoeff, 
                           int FilterLength, 
                           fix **ppDelay, 
                           fix *pDelayStart) property(loop_levels_1);

int cfx_fir_1ch_bb_Process(fix *in_buff, 
                           int in_step, 
                           fix *out_buff, 
                           int out_step, 
                           int B, 
                           const fix chess_storage(YMEM) *pFilterCoeff, 
                           int FilterLength, 
                           fix **ppDelay, 
                           fix *pDelayStart) property(loop_levels_2);
                           
int cfx_fir_1ch_bb_sym_Process(fix *in_buff, 
                               int in_step, 
                               fix *out_buff, 
                               int out_step, 
                               int B, 
                               const fix chess_storage(YMEM) *pFilterCoeff, 
                               int FilterLength, 
                               fix **ppDelay, 
                               fix *pDelayStart) property(loop_levels_2);

#if (defined COOLFLUX_HAS_wide_fix)
wide_fix cfx_fir_1ch_sb_Process_wide(wide_fix in_sample,
                                     const wide_fix chess_storage(YMEM) *pFilterCoeff,
                                     int FilterLength,
                                     wide_fix **ppDelay,
                                     wide_fix *pDelayStart) property(loop_levels_1);
int cfx_fir_1ch_bb_Process_wide(wide_fix *in_buff,
                                int in_step,
                                wide_fix *out_buff,
                                int out_step,
                                int B,
                                const wide_fix chess_storage(YMEM) *pFilterCoeff,
                                int FilterLength,
                                wide_fix **ppDelay,
                                wide_fix *pDelayStart) property(loop_levels_2);
int cfx_fir_Init_wide(int FilterLength,
                      wide_fix *pDelay) property(loop_levels_1);
#endif //#if (defined COOLFLUX_HAS_wide_fix)

#endif // CFX_FIR_H 