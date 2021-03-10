/*File::   cfx_imdct_double.h                                                             */
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

#ifndef IMDCT_DOUBLE_H
#define IMDCT_DOUBLE_H
#include "cf6_chess.h"
#include "mathlib.h"
//#include "coolFlux_defs.h"
//#include "cfx_fftcomplex_double.h"
//#include "cfx_mdct_tables.h"

//#define IMDCT_INPUT_SINGLE
//#define IMDCT_OUTPUT_SINGLE

#ifdef IMDCT_INPUT_SINGLE
#define IMDCT_INPUT_SHIFT(x) ((x) >> 0)
#else
#define IMDCT_INPUT_SHIFT(x) ((x) << 0/*8*/)
#endif

#ifdef IMDCT_OUTPUT_SINGLE
#define IMDCT_OUTPUT_SHIFT(x) ((x) << 7)
#else
#define IMDCT_OUTPUT_SHIFT(x) ((x) >> 2/*7*/)
#endif

extern int cfx_imdct_double_PreTwiddle(lfix chess_storage(XYMEM)* coeffs, 
                                       lfix chess_storage(XYMEM)* output, 
                                       const CFX_MDCT_TABLE *table) property(loop_levels_1);

extern int cfx_imdct_double_PostTwiddle(lfix chess_storage(XYMEM)* buffer, 
                                        const CFX_MDCT_TABLE *table) property(loop_levels_1);

extern lfix chess_storage(XYMEM)* cfx_imdct_double_DeInterleave(lfix chess_storage(XYMEM)* input, 
                                                                lfix chess_storage(XYMEM)* restrict output, 
                                                                const CFX_MDCT_TABLE* table, 
                                                                lfix chess_storage(XYMEM)* buffer_start, 
                                                                int buffer_limit) property(loop_levels_1);

extern lfix chess_storage(XYMEM)* cfx_imdct_double_Process(lfix chess_storage(XYMEM)* input_coeffs, 
                                                           lfix chess_storage(XYMEM)* workspace, 
                                                           const CFX_MDCT_TABLE *const imdct_table[], 
                                                           lfix chess_storage(XYMEM)* output, 
                                                           int outputLimit, 
                                                           lfix chess_storage(XYMEM)* outputBase) property(loop_levels_3);

#endif // #ifdef IMDCT_DOUBLE_H