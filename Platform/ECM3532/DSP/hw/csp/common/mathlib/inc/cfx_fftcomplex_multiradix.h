/*File::   cfx_fftcomplex_multiradix.h                                                  */
/*$Revision:: 53764                                                                    $*/
/*$Date:: 2011-06-30 18:01:33 +0300 (Thu, 30 Jun 2011)                                 $*/
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


/*Author      : Radu Stoicescu, Bogdan Visan                                            */


#ifndef __fftcomplex_multiradix
#define __fftcomplex_multiradix
#include "cf6_chess.h"
#include "mathlib.h"
//#include "coolFlux_defs.h"
#include "cfx_ffttables.h"


#define MAXFFTSIZE     2048
#define MAX_POWERS_NO  4    // 5, 3, 2, 4
#define MAX_FACTORS_NO 8    // should cover all factorizations into powers of 3, 4 (with possibly an extra 2), or 5 up to 4500
#define MAX_POWER      6    // the largest power of a factor that appear in table sizes; it is 3^6 from 2916

typedef struct
{
  fix scale;                             //== to_fix(MAX_FIX) / Points
  int factorization[2+MAX_FACTORS_NO*3]; //Points, 1 | factor[0], Points/factor[0], 1*factor[0] | factor[1], Points/factor[0]/factor[1], 1*factor[0]*factor[1] | ...
  int base_powers[MAX_POWERS_NO+1];      //order is 5, 3, 2, 4, total
  int reshuffle_table[MAXFFTSIZE];       //bit-reversed reordering in {p[1], p[2], ...} quasi-base of decomposition
} cfx_fft_c_multiradix_context_t;


int cfx_factorization_Process (cfx_fft_c_multiradix_context_t *context, int Points);

int cfx_fft_c_scaling_multiradix_Process (cfx_fft_c_multiradix_context_t *context, const fix *fin, fix *fout, int nfft, const CFX_MRADIXFFT_TABLE* table);

int cfx_ifft_c_scaling_multiradix_Process (cfx_fft_c_multiradix_context_t *context, const fix *fin, fix *fout, int nfft, const CFX_MRADIXFFT_TABLE* table);


#endif
