/*File::   cfx_vectoroperation.h                                                             */
/*$Revision:: 56090                                                                    $*/
/*$Date:: 2018-01-26 12:41:05 +0200 (Fri, 26 Jan 2018)                                 $*/
/* -----------------------------------------------------------------------------------	*/
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
/* -----------------------------------------------------------------------------------	*/
#ifndef __CFX_VECTOROPERATION_H__
#define __CFX_VECTOROPERATION_H__
#include "cf6_chess.h"
#include "mathlib.h"
//#include "coolFlux_defs.h"

int cfx_zerovector_Process(fix *in_buff, int B) property(loop_levels_1);

int cfx_copyvector_Process(fix *in_buff, fix *out_buff, int B) property(loop_levels_1);

int cfx_addvector_Process(fix *in_buff1, fix	chess_storage(YMEM) *in_buff2, fix * restrict out_buff, int B) property(loop_levels_1);

int cfx_subvector_Process(fix *in_buff1, fix chess_storage(YMEM) *in_buff2, fix * restrict pDst, int B) property(loop_levels_1);

int cfx_multvector_Process(fix *in_buff1, fix chess_storage(YMEM) *inbuff2, fix * restrict pDst, int B) property(loop_levels_1);

int cfx_dotprod_Process(fix *pSrc1, fix chess_storage(YMEM)*pSrc2, fix * restrict pDst, int B) property(loop_levels_1);

int cfx_crosscorr_Process(fix *pSrc1, fix chess_storage(YMEM)*pSrc2, fix * restrict pDst, int B);

int cfx_addconstvector_Process(fix *in_buff, fix * restrict out_buff,  fix Val, int B) property(loop_levels_1);

int cfx_multconstvector_Process(fix *in_buff, fix * restrict out_buff, fix Val, int B) property(loop_levels_1);

int cfx_shiftvector_Process(fix *in_buff, fix * restrict out_buff, int shift, int B) property(loop_levels_1);


#endif //__CFX_VECTOROPERATION_H__