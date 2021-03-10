/*File::   cfx_biquad_dpfb.h                                                             */
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


/* Synopsis:                               */
/* Cascaded Bi-Qquadratic Filters (BIQUAD) */
/* ======================================= */

/* One BIQUAD filter section is calculated according to the following equations 
*   
*            A0 * z^0 + A1 * z^-1 + A2 * z^-2
*   H(z) = ----------------------------------
*             1       + B1 * z^-1 + B2 * z^-2
* or
*
*   y(n) = A0 * x(n) + A1 * x(n-1) + A2 * x(n-2) - B1 * y(n-1) - B2 * y(n-2)
*
*
* The total filter is calculated as a cascade of N Biquadratic Sections 
*                           
*   H(z) = H1(z) * H2(z) * .. * HN(z)
*
*/


#ifndef CFX_BIQUAD_DPFB_H 
#define CFX_BIQUAD_DPFB_H 

#include "cf6_chess.h"
#include "mathlib.h"

#define BIQUAD_DPFB_COEFFS_PER_SECTION 5
#define BIQUAD_DPFB_DELAYS_PER_SECTION 6


/********************************************************************************
cfx_biquad_nsect_Init

Resets the delay line of an N-section biquad to 0
Assumptions:
- non-circular delay buffer pre-allocated with 
nr_sections*BIQUAD_DPFB_DELAYS_PER_SECTION elements
*********************************************************************************/
int cfx_biquad_dpfb_nsect_Init(fix *pDelay, int nr_sections) property(loop_levels_1);


/********************************************************************************
cfx_biquad_dpfb_1ch_bb_1sect_scale_Process 

Double precision feedback biquad, 1-section, block based, with scaled coefficients
Assumptions:
- coefficients are stored in the coeff buffer in order: A2, A1, A0, -B2, -B1
- non-circular delay buffer contents: [ y(n-2)H y(n-2)L y(n-1)H y(n-1)L x(n-2) x(n-1)]
- the result y(n) are upscaled by left-shifting with the "shift" parameter
*********************************************************************************/
int cfx_biquad_dpfb_1ch_bb_1sect_scale_Process (fix *in_buff,
                                                int step_in,
                                                fix *out_buff,
                                                int step_out,
                                                int nr_samples,
                                                fix *restrict pdelay,
                                                const fix  chess_storage(YMEM) *coeff,
                                                int shift) property(loop_levels_1);

/********************************************************************************
cfx_biquad_dpfb_1ch_bb_1sect_noscale_Process 

Double precision feedback biquad, 1-section, block based, non-scaled coefficients
Assumptions:
- coefficients are stored in the coeff buffer in order: A2, A1, A0, -B2, -B1
- non-circular delay buffer contents: [ y(n-2)H y(n-2)L y(n-1)H y(n-1)L x(n-2) x(n-1)]
*********************************************************************************/
int cfx_biquad_dpfb_1ch_bb_1sect_noscale_Process (fix *in_buff,
                                                  int step_in,
                                                  fix *out_buff,
                                                  int step_out,
                                                  int nr_samples,
                                                  fix *restrict pdelay,
                                                  const fix  chess_storage(YMEM) *coeff) property(loop_levels_1);


/********************************************************************************
cfx_biquad_dpfb_1ch_sb_1sect_scale_Process 

Double precision feedback biquad, sample based, with scaled coefficients:
Assumptions:
- circular delay buffer that holds y(n-2)H y(n-2)L x(n-2) y(n-1)H y(n-1)L x(n-1)
- coefficients stored in YMEM in the order -B2, A2, -B1, A1, A0
- the result y(n) are upscaled by left-shifting with the "shift" parameter
*********************************************************************************/
fix cfx_biquad_dpfb_1ch_sb_1sect_scale_Process (fix sample,
                                                fix **ppDelay,
                                                fix *pDelayStart,
                                                fix chess_storage(YMEM) *pFilterCoeff,
                                                int shift) property(loop_free);




/********************************************************************************
cfx_biquad_dpfb_1ch_bb_nsect_scale_Process

Cascaded N biquad sections, with block based computation and scaled coefficients;
The total filter is calculated as a cascade of N Biquadratic Sections
H(z) = H1(z) * H2(z) * .. * HN(z)
Internally calls biquad_dpfb_1ch_bb_1sect_scale_Process for each biquad section

Assumptions:
- non-cicrular delay buffer that holds the delay lines of the N biquad sections,
in sequential order
- coefficients stored in YMEM for the N biquad sections, in sequential order,
with the format requested by biquad_1ch_bb_1sect_scale_Process
- the result y(n) are upscaled by left-shifting with the "shift" parameter
*********************************************************************************/
int cfx_biquad_dpfb_1ch_bb_nsect_scale_Process (fix *in_buff,
                                                int step_in,
                                                fix *out_buff,
                                                int step_out,
                                                int nr_samples,
                                                fix *delay,
                                                const fix chess_storage(YMEM) *coeffs,
                                                int shift,
                                                int nr_sections) property(loop_levels_2);


#endif // CFX_BIQUAD_DPFB_H 
