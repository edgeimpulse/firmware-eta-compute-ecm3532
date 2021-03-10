/*File::   cfx_ffttables.h                                                              */
/*$Revision:: 56046                                                                    $*/
/*$Date:: 2017-09-01 14:17:43 +0300 (Fri, 01 Sep 2017)                                 $*/
/* -----------------------------------------------------------------------------------	*/
/* Copyright (c) 2006-2017 NXP B.V.                                                     */
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


/* Author      : Marleen Ade, Radu Vanca, Bogdan Visan                                  */


#ifndef __FFTTABLES_H__
#define __FFTTABLES_H__
#include "cf6_chess.h"
#include "mathlib.h"

#define NO_FFT_TABLES 9

#if MEM_SIZE_FIX >= MEM_SIZE_FIX_THRESHOLD
    #define NO_FFT_MULTIRADIX_TABLES 7
#else
    #define NO_FFT_MULTIRADIX_TABLES 6
#endif //#if MEM_SIZE_FIX >= MEM_SIZE_FIX_THRESHOLD

typedef struct {
    fix r;
    fix i;
}cfx_fft_complex;
typedef struct cfx_fft_table
{
    int table_size;
    const fix chess_storage(YMEM) * Wreal;
    const fix chess_storage(YMEM) * Wimag;
} CFX_FFT_TABLE;
typedef struct cfx_mradixfft_table
{
    int table_size;
    int base_powers[4];
    const fix chess_storage(YMEM) * W;
} CFX_MRADIXFFT_TABLE;

extern const CFX_FFT_TABLE cfx_fft_table_8;
extern const CFX_FFT_TABLE cfx_ifft_table_8;
extern const CFX_FFT_TABLE cfx_fft_table_16;
extern const CFX_FFT_TABLE cfx_ifft_table_16;
extern const CFX_FFT_TABLE cfx_fft_table_32;
extern const CFX_FFT_TABLE cfx_ifft_table_32;
extern const CFX_FFT_TABLE cfx_fft_table_64;
extern const CFX_FFT_TABLE cfx_ifft_table_64;
extern const CFX_FFT_TABLE cfx_fft_table_128;
extern const CFX_FFT_TABLE cfx_ifft_table_128;
extern const CFX_FFT_TABLE cfx_fft_table_256;
extern const CFX_FFT_TABLE cfx_ifft_table_256;
extern const CFX_FFT_TABLE cfx_fft_table_512;
extern const CFX_FFT_TABLE cfx_ifft_table_512;
extern const CFX_FFT_TABLE cfx_fft_table_1024;
extern const CFX_FFT_TABLE cfx_ifft_table_1024;
extern const CFX_FFT_TABLE cfx_fft_table_2048;
extern const CFX_FFT_TABLE cfx_ifft_table_2048;

extern const CFX_MRADIXFFT_TABLE cfx_fft_table_648;
extern const CFX_MRADIXFFT_TABLE cfx_fft_table_900;
extern const CFX_MRADIXFFT_TABLE cfx_fft_table_960;
extern const CFX_MRADIXFFT_TABLE cfx_fft_table_1800;
extern const CFX_MRADIXFFT_TABLE cfx_fft_table_2500;
extern const CFX_MRADIXFFT_TABLE cfx_fft_table_2916;
extern const CFX_MRADIXFFT_TABLE cfx_fft_table_4500;

extern const CFX_FFT_TABLE *const cfx_fft_tables[NO_FFT_TABLES];
extern const CFX_FFT_TABLE *const cfx_ifft_tables[NO_FFT_TABLES];

extern const CFX_MRADIXFFT_TABLE *const cfx_fft_multiradix_tables[NO_FFT_MULTIRADIX_TABLES];

#if (defined COOLFLUX_HAS_wide_fix)
typedef struct cfx_fft_table_wide
{
    int table_size;
    const wide_fix chess_storage(YMEM) * Wreal;
    const wide_fix chess_storage(YMEM) * Wimag;
} CFX_FFT_TABLE_WIDE;

extern const CFX_FFT_TABLE_WIDE cfx_fft_table_8_wide;
extern const CFX_FFT_TABLE_WIDE cfx_ifft_table_8_wide;
extern const CFX_FFT_TABLE_WIDE cfx_fft_table_16_wide;
extern const CFX_FFT_TABLE_WIDE cfx_ifft_table_16_wide;
extern const CFX_FFT_TABLE_WIDE cfx_fft_table_32_wide;
extern const CFX_FFT_TABLE_WIDE cfx_ifft_table_32_wide;
extern const CFX_FFT_TABLE_WIDE cfx_fft_table_64_wide;
extern const CFX_FFT_TABLE_WIDE cfx_ifft_table_64_wide;
extern const CFX_FFT_TABLE_WIDE cfx_fft_table_128_wide;
extern const CFX_FFT_TABLE_WIDE cfx_ifft_table_128_wide;
extern const CFX_FFT_TABLE_WIDE cfx_fft_table_256_wide;
extern const CFX_FFT_TABLE_WIDE cfx_ifft_table_256_wide;
extern const CFX_FFT_TABLE_WIDE cfx_fft_table_512_wide;
extern const CFX_FFT_TABLE_WIDE cfx_ifft_table_512_wide;
extern const CFX_FFT_TABLE_WIDE cfx_fft_table_1024_wide;
extern const CFX_FFT_TABLE_WIDE cfx_ifft_table_1024_wide;
extern const CFX_FFT_TABLE_WIDE cfx_fft_table_2048_wide;
extern const CFX_FFT_TABLE_WIDE cfx_ifft_table_2048_wide;
extern const CFX_FFT_TABLE_WIDE *const cfx_fft_tables_wide[9];
extern const CFX_FFT_TABLE_WIDE *const cfx_ifft_tables_wide[9];
#endif //#if (defined COOLFLUX_HAS_wide_fix)

#endif //__FFTTABLES_H__
