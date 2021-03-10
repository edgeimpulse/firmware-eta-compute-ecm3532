/*File::   cfx_fftcomplex.h                                                             */
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

/*Author      : Marleen Ade - Jonathan Hodgson */


/* Synopsis:     Prototypes of the complex fft and ifft, structures,
and defines to select x or y memory. */

/**********************************************************************/

#ifndef __fftcomplex
#define __fftcomplex
#include "cf6_chess.h"
#include "mathlib.h"
#include "cfx_ffttables.h"

int cfx_fft_c_blockscaling_Process (fix *Xinr, 
                                    fix *Xini, 
                                    fix * Xoutr, 
                                    fix * Xouti, 
                                    int Points, const CFX_FFT_TABLE * table, 
                                    int scale) property(loop_levels_3);

int cfx_fft_c_stagescaling_Process (fix *Xinr, 
                                    fix *Xini, 
                                    fix * Xoutr, 
                                    fix * Xouti, 
                                    int Points, 
                                    const CFX_FFT_TABLE * table) property(loop_levels_3);
                                    
int cfx_fft_c_stagescaling_inplace_Process (fix *Xinr,
                                            fix *Xini,
                                            int Points,
                                            const CFX_FFT_TABLE * table) property(loop_levels_3);

int cfx_fft_c_bfp_Process (fix *Xinr, 
                           fix *Xini, 
                           fix * Xoutr, 
                           fix * Xouti, 
                           int Points, 
                           const CFX_FFT_TABLE * table, 
                           int& scale) property(loop_levels_3);

int cfx_fft_c_noscaling_Process (fix *Xinr, 
                                 fix *Xini, 
                                 fix *restrict Xoutr, 
                                 fix *restrict Xouti, 
                                 int Points, 
                                 const CFX_FFT_TABLE * table) property(loop_levels_3);

int cfx_ifft_c_stagescaling_Process (fix *Xinr, 
                                     fix *Xini, 
                                     fix *Xoutr, 
                                     fix *Xouti, 
                                     int Points, 
                                     const CFX_FFT_TABLE * itable) property(loop_levels_3);

int cfx_ifft_c_noscaling_Process (fix *Xinr, 
                                  fix *Xini, 
                                  fix *Xoutr, 
                                  fix *Xouti, 
                                  int Points, 
                                  const CFX_FFT_TABLE * table) property(loop_levels_3);

int cfx_ifft_c_bfp_Process (fix *Xinr, 
                            fix *Xini, 
                            fix *Xoutr, 
                            fix *Xouti, 
                            int Points, 
                            const CFX_FFT_TABLE * table, 
                            int& scale) property(loop_levels_3);

#endif

