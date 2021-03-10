/*File::   CoolFlux_defs.h                                                             */
/*$Revision:: 56046                                                                    $*/
/*$Date:: 2017-09-01 14:17:43 +0300 (Fri, 01 Sep 2017)                                 $*/
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

#ifndef CoolFlux_defs_H
#define CoolFlux_defs_H

#include "cf6_chess.h"

/*common keywords*/
#define ROM const

#define MEM_SIZE_FIX_THRESHOLD   131072UL


#ifndef COOLFLUX_HAS_XYMEM
    #define XYMEM    XMEM
    #define _XYMEM   _XMEM
    #define XY       XMEM
    #define _XYALIGN _XALIGN
#endif


#if (defined COOLFLUX_DSP)


    #define SHORT_INT_SIZE    24
    #define INT_SIZE          24
    #define LONG_INT_SIZE     48
    #define FIX_SIZE          24
    #define LONG_FIX_SIZE     48
    #define ACC_SIZE          56

    #define XMEM_SIZE_FIX     65536UL
    #define YMEM_SIZE_FIX     65536UL
    #define MEM_SIZE_FIX      (XMEM_SIZE_FIX+YMEM_SIZE_FIX)

    #define FIX_HALF_SIZE     (FIX_SIZE>>1)

    #define int_fix           int24
    #define uint_fix          uint24
    #define int_lfix          long48
    #define uint_lfix         ulong48

    #define HALF_RANGE_FIX      (1 << (FIX_SIZE - 2))
    #define QUARTER_RANGE_FIX   (1 << (FIX_SIZE - 3))
    #define MAX_FIX             (((HALF_RANGE_FIX-1)<<1)+1)
    #define SMALLEST_FIX        to_fix(0x000001)

    #define FIX_MASK            ((uint_fix)-1)
    #define MAX_UFIX            (0xFFFFFF)

    #define HALF_RANGE_LFIX     (((int_lfix)1) << (LONG_FIX_SIZE - 2))
    #define MAX_LFIX            (((HALF_RANGE_LFIX-1)<<1) + 1)
    #define LFIX_MASK           ((uint_lfix)-1)

    #define ALMOST_ONE          to_fix(MAX_FIX)

    #if (defined COOLFLUX_DSP_1_1)
        #define MAX_LOOP_RANGE     4096
    #elif (defined COOLFLUX_DSP_1_2)
        #define MAX_LOOP_RANGE     8388608
    #endif

    /* get the number of useful bits in an int value */
    #define LOG2FLOOR(_n) (FIX_SIZE-1-exp(to_fix(_n))-1)


#elif (defined COOLFLUX_DSPL)


    #define YMEM             XMEM
    #define _YMEM            _XMEM
    #define _YALIGN          _XALIGN

    #define SHORT_INT_SIZE   24
    #define INT_SIZE         24
    #define LONG_INT_SIZE    48
    #define FIX_SIZE         24
    #define LONG_FIX_SIZE    48
    #define ACC_SIZE         56

    #define XMEM_SIZE_FIX    32768UL
    #define YMEM_SIZE_FIX    0UL
    #define MEM_SIZE_FIX     (XMEM_SIZE_FIX+YMEM_SIZE_FIX)

    #define FIX_HALF_SIZE    (FIX_SIZE>>1)

    #define int_fix          int24
    #define uint_fix         uint24
    #define int_lfix         long48
    #define uint_lfix        ulong48

    #define HALF_RANGE_FIX      (1 << (FIX_SIZE - 2))
    #define QUARTER_RANGE_FIX   (1 << (FIX_SIZE - 3))
    #define MAX_FIX             (((HALF_RANGE_FIX-1)<<1)+1)
    #define SMALLEST_FIX        to_fix(0x000001)

    #define FIX_MASK            ((uint_fix)-1)
    #define MAX_UFIX            (0xFFFFFF)

    #define HALF_RANGE_LFIX     (((int_lfix)1) << (LONG_FIX_SIZE - 2))
    #define MAX_LFIX            (((HALF_RANGE_LFIX-1)<<1) + 1)
    #define LFIX_MASK           ((uint_lfix)-1)

    #define ALMOST_ONE          to_fix(MAX_FIX)

    #define MAX_LOOP_RANGE      4096

    /* get the number of useful bits in an int value */
    #define LOG2FLOOR(_n)       (FIX_SIZE-1-exp(to_fix(_n))-1)


#elif (defined COOLFLUX_BSP)


    #define SHORT_INT_SIZE   24
    #define INT_SIZE         24
    #define LONG_INT_SIZE    48
    #define SHORT_FIX_SIZE   12 /* component of packed types */
    #define FIX_SIZE         24
    #define LONG_FIX_SIZE    48
    #define ACC_SIZE         56

    #define XMEM_SIZE_FIX    16777216UL
    #define YMEM_SIZE_FIX    16777216UL
    #define MEM_SIZE_FIX     (XMEM_SIZE_FIX+YMEM_SIZE_FIX)

    #define FIX_HALF_SIZE     (FIX_SIZE>>1)

    #define int_fix           int24
    #define uint_fix          uint24
    #define int_lfix          long48
    #define uint_lfix         ulong48

    #define complex_fix       complex12
    #define complex_lfix      complex24
    #define complex_acc       complex28
    #define to_complex_fix    to_complex12
    #define to_complex_lfix   to_complex24
    #define to_complex_acc    to_complex28
    #define join_complex_fix  join_complex12
    #define join_complex_lfix join_complex24
    #define join_complex_acc  join_complex28
    #define join_simd_fix     join_simd12
    #define join_isimd_fix    join_isimd12

    #define simd_fix          simd12
    #define simd_lfix         simd24
    #define simd_acc          simd28
    #define to_simd_fix       to_simd12
    #define to_simd_lfix      to_simd24
    #define to_simd_acc       to_simd28
    #define join_simd_fix     join_simd12
    #define join_simd_lfix    join_simd24
    #define join_simd_acc     join_simd28
    #define isimd_fix         isimd12
    #define isimd_lfix        isimd24
    #define isimd_acc         isimd28

    #define HALF_RANGE_FIX      (1 << (FIX_SIZE - 2))
    #define QUARTER_RANGE_FIX   (1 << (FIX_SIZE - 3))
    #define HALF_RANGE_INT      (1 << (INT_SIZE - 2))
    #define QUARTER_RANGE_INT   (1 << (INT_SIZE - 3))
    #define SHORT_FIX_MASK      ((1 << SHORT_FIX_SIZE) - 1)
    #define SMALLEST_FIX        to_fix(0x000001)
    #define SMALLEST_SHORT_FIX  (SMALLEST_FIX << (FIX_SIZE - SHORT_FIX_SIZE))

    #define MAX_FIX             (((HALF_RANGE_FIX-1)<<1)+1)
    #define FIX_MASK            ((uint_fix)-1)
    #define MAX_UFIX            (0xFFFFFF)

    #define HALF_RANGE_LFIX     (((int_lfix)1) << (LONG_FIX_SIZE - 2))
    #define MAX_LFIX            (((HALF_RANGE_LFIX-1)<<1) + 1)
    #define LFIX_MASK           ((uint_lfix)-1)

    #define ALMOST_ONE          to_fix(MAX_FIX)

    #define MAX_LOOP_RANGE      16777216

    /* Complex constructors */
    #define complex_fix_const(_r, _c) complex12(_r, _c)

    /* get the number of useful bits in an int value */
    #define LOG2FLOOR(_n)           (FIX_SIZE-1-exp(to_fix(_n))-1)

    /* get the reciprocal into real part of a complex */
    #define COMPLEXRECIPROCAL(_n)   ( join_complex12( to_fix(0x1 << (exp(to_fix(_n)) + 1)), 0 ) )// to_complex12(0x1 << (exp(to_fix(_n)) - (SHORT_FIX_SIZE - 1))))

    /* move the lower part into higher part */
    #define LOW2HIGH(_n)            (((_n) & SHORT_FIX_MASK) << SHORT_FIX_SIZE)

    /* define missing conversions, that are not type reinterpretation */
    //inline complex_fix join_imag_real(complex_fix x, complex_fix y) {return join_complex_fix(extract_imag(x), extract_real(y));}


#elif (defined COOLFLUX_DSP16)

    #define INT_SIZE         16
    #define LONG_INT_SIZE    32
    #define FIX_SIZE         16
    #define LONG_FIX_SIZE    32
    #define ACC_SIZE         40

    #define XMEM_SIZE_FIX    65536UL
    #define YMEM_SIZE_FIX    65536UL
    #define MEM_SIZE_FIX     (XMEM_SIZE_FIX+YMEM_SIZE_FIX)

    #define FIX_HALF_SIZE    (FIX_SIZE>>1)

    #define int_fix          int16
    #define uint_fix         uint16
    #define int_lfix         long32
    #define uint_lfix        ulong32

    #define HALF_RANGE_FIX      (1 << (FIX_SIZE - 2))
    #define QUARTER_RANGE_FIX   (1 << (FIX_SIZE - 3))
    #define MAX_FIX             (((HALF_RANGE_FIX-1)<<1)+1)
    #define ALMOST_ONE          to_fix(MAX_FIX)
    #define MAX_UFIX            (0xFFFF)
    #define SMALLEST_FIX        to_fix(0x0001)

    #define SHORT_FIX_MASK      ((1 << SHORT_FIX_SIZE) - 1)

    #define HALF_RANGE_LFIX     (((int_lfix)1) << (LONG_FIX_SIZE - 2))
    #define QUARTER_RANGE_LFIX  (((int_lfix)1) << (LONG_FIX_SIZE - 3))
    #define MAX_LFIX            (((HALF_RANGE_LFIX-1)<<1)+1)

    #define MAX_LOOP_RANGE      65536

    /* get the number of useful bits in an int value */
    #define LOG2FLOOR(_n)       (FIX_SIZE-1-exp(to_fix(_n))-1)


#elif (defined COOLFLUX_DSP16L)


    #define YMEM             XMEM
    #define _YMEM            _XMEM
    #define _YALIGN          _XALIGN

    #define INT_SIZE         16
    #define LONG_INT_SIZE    32
    #define FIX_SIZE         16
    #define LONG_FIX_SIZE    32
    #define ACC_SIZE         40

    #define XMEM_SIZE_FIX    65536UL
    #define YMEM_SIZE_FIX    0UL
    #define MEM_SIZE_FIX     (XMEM_SIZE_FIX+YMEM_SIZE_FIX)

    #define FIX_HALF_SIZE    (FIX_SIZE>>1)

    #define int_fix          int16
    #define uint_fix         uint16
    #define int_lfix         long32
    #define uint_lfix        ulong32

    #define HALF_RANGE_FIX      (1 << (FIX_SIZE - 2))
    #define QUARTER_RANGE_FIX   (1 << (FIX_SIZE - 3))
    #define MAX_FIX             (((HALF_RANGE_FIX-1)<<1)+1)
    #define ALMOST_ONE          to_fix(MAX_FIX)
    #define MAX_UFIX            (0xFFFF)
    #define SMALLEST_FIX     to_fix(0x0001)

    #define SHORT_FIX_MASK      ((1 << SHORT_FIX_SIZE) - 1)

    #define HALF_RANGE_LFIX     (((int_lfix)1) << (LONG_FIX_SIZE - 2))
    #define QUARTER_RANGE_LFIX  (((int_lfix)1) << (LONG_FIX_SIZE - 3))
    #define MAX_LFIX            (((HALF_RANGE_LFIX-1)<<1)+1)

    #define MAX_LOOP_RANGE      4096

    /* get the number of useful bits in an int value */
    #define LOG2FLOOR(_n)       (FIX_SIZE-1 - exp(to_fix(_n)) - 1)


#elif (defined COOLFLUX_BSP32_2_0)


    #define SHORT_INT_SIZE  16
    #define INT_SIZE        32
    #define LONG_INT_SIZE   32
    #define SHORT_FIX_SIZE  16 /* component of packed types */
    #define FIX_SIZE        16
    #define LONG_FIX_SIZE   32
    #define ACC_SIZE        40

    #define WIDE_FIX_SIZE   32
    #define WIDE_ACC_SIZE   72

    #define XMEM_SIZE_FIX   16777216UL
    #define YMEM_SIZE_FIX   16777216UL
    #define MEM_SIZE_FIX    (XMEM_SIZE_FIX+YMEM_SIZE_FIX)

    #define FIX_HALF_SIZE    (FIX_SIZE>>1)

    #define int_fix         short16
    #define uint_fix        ushort16
    #define int_lfix        long32
    #define uint_lfix       ulong32
    #define int_wide_fix    int32
    #define uint_wide_fix   uint32

    #define HALF_RANGE_FIX      (1 << (FIX_SIZE - 2))
    #define QUARTER_RANGE_FIX   (1 << (FIX_SIZE - 3))
    #define HALF_RANGE_INT      (1 << (INT_SIZE - 2))
    #define QUARTER_RANGE_INT   (1 << (INT_SIZE - 3))
    #define SHORT_FIX_MASK      ((1 << SHORT_FIX_SIZE) - 1)
    #define SMALLEST_FIX        to_fix(0x0001)
    #define SMALLEST_SHORT_FIX  (SMALLEST_FIX << (FIX_SIZE - SHORT_FIX_SIZE))

    #define MAX_FIX             ((1 << (FIX_SIZE - 1))-1)
    #define MAX_UFIX            (0xFFFF)

    #define SHORT_FIX_MASK      ((1 << SHORT_FIX_SIZE) - 1)

    #define HALF_RANGE_LFIX     (((int_lfix)1) << (LONG_FIX_SIZE - 2))
    #define QUARTER_RANGE_LFIX  (((int_lfix)1) << (LONG_FIX_SIZE - 3))
    #define MAX_LFIX            (((HALF_RANGE_LFIX-1)<<1)+1)

    #define HALF_RANGE_WIDE_FIX     (((int_wide_fix)1) << (WIDE_FIX_SIZE - 2))
    #define QUARTER_RANGE_WIDE_FIX  (((int_wide_fix)1) << (WIDE_FIX_SIZE - 3))
    #define MAX_WIDE_FIX            (((HALF_RANGE_WIDE_FIX-1)<<1)+1)

    #define ALMOST_ONE          to_fix((int_fix)MAX_FIX)
    #define ALMOST_ONE_WIDE     ((wide_fix)to_lfix(MAX_WIDE_FIX))

    #define MAX_LOOP_RANGE      2147483648

    #define to_long(_n)         to_int(_n)

    /* get the number of useful bits in an int value */
    #define LOG2FLOOR(_n)           (LONG_FIX_SIZE-1-exp(to_lfix(_n))-1)

    /* get the reciprocal into real part of a complex */
    #define COMPLEXRECIPROCAL(_n)   join_complex_fix( to_fix(((int_fix)1) << (exp(to_fix((int_fix)_n)) + 1)), 0 )// to_complex_fix(0x1 << (exp(to_fix((int_fix)_n)) - (SHORT_FIX_SIZE - 1)))

    /* move the lower part into higher part */
    #define LOW2HIGH(_n)            (((_n) & SHORT_FIX_MASK) << SHORT_FIX_SIZE)

    /* define missing conversions, that are not type reinterpretation */
    fix inline to_fix(int x)  {return to_fix((short)x);} /* lower part of int -> fix */
    fix inline to_fix(long x) {return to_fix((short)x);} /* lower part of long -> fix */
    int inline to_int(fix x)  {return to_short(x);}      /* fix -> lower part of int */

#endif


#endif
