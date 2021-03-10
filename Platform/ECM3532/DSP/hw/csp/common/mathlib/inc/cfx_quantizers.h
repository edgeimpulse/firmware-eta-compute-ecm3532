/*File::   cfx_quantizers.h                                                             */
/*$Revision:: 55766                                                                    $*/
/*$Date:: 2015-06-19 18:45:02 +0300 (Fri, 19 Jun 2015)                                 $*/
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
#ifndef __CFX_QUANTIZERS_H__
#define __CFX_QUANTIZERS_H__

#include "cf6_chess.h"
#include "mathlib.h"
//#include "coolFlux_defs.h"

void cfx_preprocess_apcm( fix *in_buff, int N, int *shift, fix *delta, int delta_bits );
void cfx_quantize_apcm( fix *in_buff, fix *out_buff, int N, int shift, fix delta, int delta_bits, int bits );
void cfx_dequantize_apcm( fix *in_buff, fix *out_buff, int N, int shift, fix delta, int delta_bits, int bits );

void cfx_quantize_linear(fix *in_buff, fix *out_buff, int B, int bits) property(loop_levels_1);
void cfx_dequantize_linear(fix *in_buff, fix *out_buff, int B, int bits) property(loop_levels_1);

#endif //__CFX_QUANTIZERS_H__