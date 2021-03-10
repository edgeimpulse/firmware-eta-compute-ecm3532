/*File::   cfx_polynomial.h                                                             */
/*$Revision:: 53764                                                                    $*/
/*$Date:: 2013-01-03 18:01:33 +0300 (Thu, 3 Jan 2013)                                  $*/
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

/*Author      : Bogdan Visan                                    */


#ifndef CFX_POLYNOMIAL_H
#define CFX_POLYNOMIAL_H

#include "cf6_chess.h"
#include "mathlib.h"
//#include "coolFlux_defs.h"


/* C[0] +
   C[1] * xStart +
   C[2] * xStart * XStep +
   C[3] * xStart * XStep * XStep +
   ...
*/

#if (defined COOLFLUX_DSP16L || defined COOLFLUX_DSPL)
fix cfx_polynomial(int n,
                   const fix *pCoeff,
                   fix xStart,
                   fix xStep,
                   int scale
                   ) property(loop_levels_1);
#else
fix cfx_polynomial(int n,
                   const fix *pCoeff,
                   fix chess_storage(x0) xCurrPower1,
                   fix chess_storage(y0) xStep,
                   int scale
                   ) property(loop_levels_1);
#endif
                   
extern const fix cfx_sin_coeff[9];
extern const fix cfx_cos_coeff[9];
extern const fix cfx_exp_coeff[11];
#if FIX_SIZE == 24
#define cfx_polynomial_sin(_X) \
    cfx_polynomial(9, \
                   cfx_sin_coeff, \
                   (_X), \
                   (fix)-((_X) * (_X)), /* to avoid storing 1 = -1 * -1 */ \
                   3)

#define cfx_polynomial_cos(_X) \
    cfx_polynomial(9, \
                   cfx_cos_coeff, \
                   (fix)-((_X) * (_X)), /* to avoid storing 1 = -1 * -1 */ \
                   (fix)-((_X) * (_X)), /* to avoid storing 1 = -1 * -1 */ \
                   3)

#define cfx_polynomial_exp(_X) \
    cfx_polynomial(11, \
                   cfx_exp_coeff, \
                   (_X), \
                   (_X), \
                   0)
#elif FIX_SIZE == 16
#define cfx_polynomial_sin(_X) \
    cfx_polynomial(9, \
                   cfx_sin_coeff, \
                   (_X), \
                   (fix)-((_X) * (_X)), /* to avoid storing 1 = -1 * -1 */ \
                   3)

#define cfx_polynomial_cos(_X) \
    cfx_polynomial(7, \
                   cfx_cos_coeff, \
                   (fix)-((_X) * (_X)), /* to avoid storing 1 = -1 * -1 */ \
                   (fix)-((_X) * (_X)), /* to avoid storing 1 = -1 * -1 */ \
                   3)

#define cfx_polynomial_exp(_X) \
    cfx_polynomial(9, \
                   cfx_exp_coeff, \
                   (_X), \
                   (_X), \
                   0)
#endif

fix cfx_arcsin(fix x);


#if (defined COOLFLUX_HAS_wide_fix)

wide_fix cfx_polynomial_wide ( int n,
                               const wide_fix *pCoeff,
                               wide_fix /*chess_storage(xw0)*/ xStart,
                               wide_fix /*chess_storage(yw0)*/ xStep,
                               int scale
                               ) property(loop_levels_1);

extern const wide_fix cfx_sin_coeff_wide[11];
#define cfx_polynomial_sin_wide(_X) \
        cfx_polynomial_wide( 11, \
                             cfx_sin_coeff_wide, \
                             (_X), \
                             (wide_fix)(-((_X) * (_X))), /* to avoid storing 1 = -1 * -1 */ \
                             3)

extern const wide_fix cfx_cos_coeff_wide[11];
#define cfx_polynomial_cos_wide(_X) \
        cfx_polynomial_wide( 11, \
                             cfx_cos_coeff_wide, \
                             (wide_fix)(-((_X) * (_X))), /* to avoid storing 1 = -1 * -1 */ \
                             (wide_fix)(-((_X) * (_X))), /* to avoid storing 1 = -1 * -1 */ \
                             3)


extern const wide_fix cfx_exp_coeff_wide[13];
#define cfx_polynomial_exp_wide(_X) \
        cfx_polynomial_wide( 13, \
                             cfx_exp_coeff_wide, \
                             (_X), \
                             (_X), \
                             0)
#endif //#if (defined COOLFLUX_HAS_wide_fix)

#endif //CFX_POLYNOMIAL_H
