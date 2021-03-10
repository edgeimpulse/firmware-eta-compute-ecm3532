/*File::   cfx_fftcomplex.h                                                             */
/*$Revision:: 53764                                                                    $*/
/*$Date:: 2011-06-30 18:01:33 +0300 (Thu, 30 Jun 2011)                                 $*/
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

#ifndef __fftcomplex_xy
#define __fftcomplex_xy
#include "cf6_chess.h"
#include "mathlib.h"
//#include "cfx_ffttables.h"


int cfx_fft_c_blockscaling_xy_Process (fix *Xinr, 
                                    fix *Xini, 
                                    fix *Xoutr, 
                                    fix _YMEM * Xouti, 
                                    int Points, const CFX_FFT_TABLE * table, 
                                    int scale) property(loop_levels_3);

int cfx_fft_c_stagescaling_xy_Process (fix *Xinr, 
                                    fix *Xini, 
                                    fix * Xoutr, 
                                    fix _YMEM *Xouti, 
                                    int Points, 
                                    const CFX_FFT_TABLE * table) property(loop_levels_3);

int cfx_fft_c_bfp_xy_Process (fix *Xinr, 
                           fix *Xini, 
                           fix *Xoutr, 
                           fix _YMEM *Xouti, 
                           int Points, 
                           const CFX_FFT_TABLE * table, 
                           int& scale) property(loop_levels_3);


int cfx_fft_c_noscaling_xy_Process (fix *Xinr, 
                                 fix *Xini, 
                                 fix *Xoutr, 
                                 fix _YMEM *Xouti, 
                                 int Points, 
                                 const CFX_FFT_TABLE * table) property(loop_levels_3);

int cfx_ifft_c_stagescaling_xy_Process (fix *Xinr, 
                                     fix *Xini, 
                                     fix *Xoutr, 
                                     fix _YMEM *Xouti, 
                                     int Points, 
                                     const CFX_FFT_TABLE * itable) property(loop_levels_3);

int cfx_ifft_c_noscaling_xy_Process (fix *Xinr, 
                                  fix *Xini, 
                                  fix *Xoutr, 
                                  fix _YMEM *Xouti, 
                                  int Points, 
                                  const CFX_FFT_TABLE * table) property(loop_levels_3);

int cfx_ifft_c_bfp_xy_Process (fix *Xinr, 
                            fix *Xini, 
                            fix *Xoutr, 
                            fix _YMEM *Xouti, 
                            int Points, 
                            const CFX_FFT_TABLE * table, 
                            int& scale) property(loop_levels_3);
#endif

