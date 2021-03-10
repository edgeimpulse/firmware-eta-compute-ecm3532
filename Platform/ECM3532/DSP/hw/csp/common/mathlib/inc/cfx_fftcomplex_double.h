/*File::   cfx_fftcomplex_double.h                                                             */
/*$Revision:: 56034                                                                    $*/
/*$Date:: 2017-07-14 22:33:33 +0300 (Fri, 14 Jul 2017)                                 $*/
/*-----------------------------------------------------------------------------------	*/
/*Copyright (c) 2006, 2007, 2008 NXP B.V.                                              */
/*All rights reserved.                                                                 */
/*This source code and any compilation or derivative thereof is the                    */
/*proprietary information of NXP B.V. and                                              */
/*is confidential in nature.                                                           */
/*                                                                                     */
/*Under no circumstances is this software to be combined with any Open                 */
/*Source Software in any way or placed under an Open Source License of                 */
/*any type without the express written permission of                                   */
/*NXP B.V.                                                                             */
/*-----------------------------------------------------------------------------------	*/


/*
Author      : Marleen Adé

Synopsis:     Prototypes of the complex fft and ifft, structures,
and defines to select x or y memory.

**********************************************************************/

#ifndef __fftcomplex_double
#define __fftcomplex_double

#include "cf6_chess.h"
#include "mathlib.h"


#if (defined COOLFLUX_HAS_wide_fix)


int cfx_fft_c_double_blockscaling_Process (wide_fix chess_storage(XYMEM) *Xinr,
                                           wide_fix chess_storage(XYMEM) *Xini,
                                           wide_fix chess_storage(XYMEM) *Xoutr,
                                           wide_fix chess_storage(XYMEM) *Xouti,
                                           int Points,
                                           const CFX_FFT_TABLE_WIDE *table,
                                           int scale) property(loop_levels_3);

int cfx_fft_c_double_stagescaling_Process (wide_fix chess_storage(XYMEM) *Xinr,
                                           wide_fix chess_storage(XYMEM) *Xini,
                                           wide_fix chess_storage(XYMEM) *Xoutr,
                                           wide_fix chess_storage(XYMEM) *Xouti,
                                           int Points,
                                           const CFX_FFT_TABLE_WIDE *table) property(loop_levels_3);

int cfx_fft_c_double_noscaling_Process (wide_fix chess_storage(XYMEM) *Xinr,
                                        wide_fix chess_storage(XYMEM) *Xini,
                                        wide_fix chess_storage(XYMEM) *Xoutr,
                                        wide_fix chess_storage(XYMEM) *Xouti,
                                        int Points,
                                        const CFX_FFT_TABLE_WIDE *table) property(loop_levels_3);

int cfx_ifft_c_double_stagescaling_Process (wide_fix chess_storage(XYMEM) *Xinr,
                                            wide_fix chess_storage(XYMEM) *Xini,
                                            wide_fix chess_storage(XYMEM) *Xoutr,
                                            wide_fix chess_storage(XYMEM) *Xouti,
                                            int Points,
                                            const CFX_FFT_TABLE_WIDE *itable) property(loop_levels_3);

int cfx_ifft_c_double_noscaling_Process (wide_fix chess_storage(XYMEM) *Xinr,
                                         wide_fix chess_storage(XYMEM) *Xini,
                                         wide_fix chess_storage(XYMEM) *Xoutr,
                                         wide_fix chess_storage(XYMEM) *Xouti,
                                         int Points,
                                         const CFX_FFT_TABLE_WIDE *table) property(loop_levels_3);


#else //!(defined COOLFLUX_HAS_wide_fix)


int cfx_fft_c_double_blockscaling_Process (lfix chess_storage(XYMEM) *Xinr,
                                           lfix chess_storage(XYMEM) *Xini,
                                           lfix chess_storage(XYMEM) *Xoutr,
                                           lfix chess_storage(XYMEM) *Xouti,
                                           int Points,
                                           const CFX_FFT_TABLE *table,
                                           int scale) property(loop_levels_3);

int cfx_fft_c_double_stagescaling_Process (lfix chess_storage(XYMEM) *Xinr,
                                           lfix chess_storage(XYMEM) *Xini,
                                           lfix chess_storage(XYMEM) *Xoutr,
                                           lfix chess_storage(XYMEM) *Xouti,
                                           int Points,
                                           const CFX_FFT_TABLE *table) property(loop_levels_3);

int cfx_fft_c_double_noscaling_Process (lfix chess_storage(XYMEM) *Xinr,
                                        lfix chess_storage(XYMEM) *Xini,
                                        lfix chess_storage(XYMEM) *Xoutr,
                                        lfix chess_storage(XYMEM) *Xouti,
                                        int Points,
                                        const CFX_FFT_TABLE *table) property(loop_levels_3);

int cfx_ifft_c_double_stagescaling_Process (lfix chess_storage(XYMEM) *Xinr,
                                            lfix chess_storage(XYMEM) *Xini,
                                            lfix chess_storage(XYMEM) *Xoutr,
                                            lfix chess_storage(XYMEM) *Xouti,
                                            int Points,
                                            const CFX_FFT_TABLE *itable) property(loop_levels_3);

int cfx_ifft_c_double_noscaling_Process (lfix chess_storage(XYMEM) *Xinr,
                                         lfix chess_storage(XYMEM) *Xini,
                                         lfix chess_storage(XYMEM) *Xoutr,
                                         lfix chess_storage(XYMEM) *Xouti,
                                         int Points,
                                         const CFX_FFT_TABLE *table) property(loop_levels_3);


#endif //!(defined COOLFLUX_HAS_wide_fix)


#endif  // __fftcomplex_double
