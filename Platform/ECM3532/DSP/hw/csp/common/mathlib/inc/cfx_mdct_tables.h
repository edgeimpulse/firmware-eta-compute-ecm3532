/*File::   cfx_mdct_tables.h                                                             */
/*$Revision:: 56034                                                                    $*/
/*$Date:: 2017-07-14 22:33:33 +0300 (Fri, 14 Jul 2017)                                 $*/
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

#ifndef CFX_MDCT_TABLES_H
#define CFX_MDCT_TABLES_H

#include "cf6_chess.h"
#include "mathlib.h"

//#include "CoolFlux_defs.h"
//#include "cfx_ffttables.h"

const int min_mdct_size = 16;
#if MEM_SIZE_FIX >= MEM_SIZE_FIX_THRESHOLD
    const int max_mdct_size = 2048;
#else
    const int max_mdct_size = 1024;
#endif //#if MEM_SIZE_FIX >= MEM_SIZE_FIX_THRESHOLD

// Combined tables
typedef struct mdct_table
{
    int table_size;
    const fix chess_storage(YMEM) * cos;
    const fix chess_storage(YMEM) * sin;
    const CFX_FFT_TABLE* fft_table;
#if (defined COOLFLUX_HAS_wide_fix)
    const CFX_FFT_TABLE_WIDE* fft_table_wide; // in an actual implementation, one should decide between simple precision / double precision version
#endif
} CFX_MDCT_TABLE;

extern const CFX_MDCT_TABLE cfx_imdct_table_16;
extern const CFX_MDCT_TABLE cfx_imdct_table_32;
extern const CFX_MDCT_TABLE cfx_imdct_table_64;
extern const CFX_MDCT_TABLE cfx_imdct_table_128;
extern const CFX_MDCT_TABLE cfx_imdct_table_256;
extern const CFX_MDCT_TABLE cfx_imdct_table_512;
extern const CFX_MDCT_TABLE cfx_imdct_table_1024;
extern const CFX_MDCT_TABLE cfx_imdct_table_2048;

extern const CFX_MDCT_TABLE *const cfx_imdct_table[];

#endif // #ifdef CFX_MDCT_TABLES_H
