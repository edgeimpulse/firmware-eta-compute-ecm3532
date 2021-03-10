/*File::   cfx_sqrt.h                                                             */
/*$Revision:: 56157                                                                    $*/
/*$Date:: 2018-09-18 20:49:34 +0300 (Tue, 18 Sep 2018)                                 $*/
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

/*Author      : Matei Arghirescu                                    */


#ifndef CFX_SQRT_H
#define CFX_SQRT_H

#include "cf6_chess.h"
#include "mathlib.h"
//#include "CoolFlux_defs.h"

#define CFX_SQRT_PRECISION 10
#define CFX_SQRT_UNSIGNED_SQRT1O2 fix(-0.58578634262084961)//to_fix( 0xB504F4 )
#define CFX_SQRT_UNSIGNED_SQRT1O8 fix(0.7071068286895752)//to_fix( 0x5A827A )

#define CFX_SQRT_FAST_NR_INTERVALS 32
#define CFX_SQRT_FAST_NR_USED_INTERVALS ((CFX_SQRT_FAST_NR_INTERVALS*3)>>2)
#define CFX_SQRT_FAST_NR_UNUSED_INTERVALS (CFX_SQRT_FAST_NR_INTERVALS>>2)
#define CFX_SQRT_FAST_LOG2_NR_INTERVALS 5
#define CFX_SQRT_FAST_POLY_DEGREE 3
#define CFX_SQRT_FAST_LOG2_POLY_NR_COEFFS 2
#define CFX_SQRT_FAST_AND_MASK 0x7C /* selects the least LOG2_NR_INTERVALS significant bits */

#define CFX_SQRT_PREC_NR_INTERVALS 32
#define CFX_SQRT_PREC_NR_USED_INTERVALS (CFX_SQRT_PREC_NR_INTERVALS>>1)
#define CFX_SQRT_PREC_NR_UNUSED_INTERVALS (CFX_SQRT_PREC_NR_INTERVALS>>1)
#define CFX_SQRT_PREC_LOG2_NR_INTERVALS 5
#define CFX_SQRT_PREC_POLY_DEGREE 3
#define CFX_SQRT_PREC_LOG2_POLY_NR_COEFFS 2
#define CFX_SQRT_PREC_AND_MASK to_fix( 0xF ) /* selects the least LOG2_NR_INTERVALS-1 significant bits */

extern const fix _YMEM sqrt_poly_fast[100];

int_fix cfx_sqrt_interpolate(int_fix alfa) property(loop_free);
fix cfx_sqrt_interpolate(fix alfa) property(loop_free);

fix cfx_sqrt_polyfit( fix x );
uint_fix cfx_sqrt_polyfit( uint_fix alpha );

#if (defined COOLFLUX_DSP || defined COOLFLUX_DSPL || defined COOLFLUX_BSP)
ulong48 cfx_sqrt_polyfit_43bits( ulong48 alpha_ulong );
ulong48 cfx_sqrt_polyfit_46bits( ulong48 alpha_ulong );
ulong48 cfx_sqrt_polyfit_48bits( ulong48 alpha_ulong );
#endif

#endif
