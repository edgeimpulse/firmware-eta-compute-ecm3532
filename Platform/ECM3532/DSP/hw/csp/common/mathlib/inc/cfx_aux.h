/*$Workfile::   cfx_aux.h                                                              $*/
/*$Revision::   1.0                                                                    $*/
/* -----------------------------------------------------------------------------------  */
/* Copyright (c) 2015 NXP Semiconductors                                                */
/* All rights reserved.                                                                 */
/* This source code and any compilation or derivative thereof is the                    */
/* proprietary information of NXP Semiconductors and                                    */
/* is confidential in nature.                                                           */
/*                                                                                      */
/* Under no circumstances is this software to be combined with any Open                 */
/* Source Software in any way or placed under an Open Source License of                 */
/* any type without the express written permission of                                   */
/* NXP B.V.                                                                             */
/* -----------------------------------------------------------------------------------  */

/*Author      : Bogdan Visan                                                            */


#ifndef CFX_AUX_H
#define CFX_AUX_H


#include <stdio.h>
#include "cf6_chess.h"
#include "mathlib.h"

#ifndef CoolFlux
  /* assert is suppressed if NDEBUG is defined (which is true when building the "Release" exe)
  => undefined NDEBUG, then include assert.h to get the definition of assert, then redefine
  NDEBUG if needed (i.e., if not building the Debug exe) */
  #undef  NDEBUG
  #include <assert.h>
  #if !defined (_DEBUG)
    #define  NDEBUG
  #endif
  
  #define ASSERT(_cond) if(!(_cond)) { printf("Assertion failed!\n"); exit(EXIT_FAILURE); }// assert(_cond)
  
#else /*CoolFlux*/

  #define ASSERT(_cond) //chess_assert(_cond) //no asserts on CoolFlux

#endif /*CoolFlux*/

// functions for XMEM
int cfx_io_read_fix_from_int_x (FILE *f, int length, int data_width, fix chess_storage(XMEM) *samples);
int cfx_io_read_lfix_from_int_x (FILE *f, int length, int data_width, lfix chess_storage(XMEM) *samples);
int cfx_io_read_lfix_from_2int_x (FILE *f, int length, int data_width, lfix chess_storage(XMEM) *samples);
int cfx_io_read_int_x (FILE *f, int length, int chess_storage(XMEM) *samples);
int cfx_io_read_uint_fix_x (FILE *f, int length, uint_fix chess_storage(XMEM) *samples);
int cfx_io_read_ulong_x (FILE *f, int length, uint_lfix chess_storage(XMEM) *samples);
int cfx_io_write_fix_to_int_x (FILE *f, int length, int data_width, fix chess_storage(XMEM) *samples);
int cfx_io_write_lfix_to_2int_x (FILE *f, int length, int data_width, lfix chess_storage(XMEM) *samples);
int cfx_io_write_uint_fix_x (FILE *f, int length, uint_fix chess_storage(XMEM) *samples);

#if defined COOLFLUX_HAS_complex_fix || defined COOLFLUX_HAS_COMPLEX12
int cfx_io_read_complex_fix_from_int_x (FILE *f, int length, int data_width, complex_fix chess_storage(XMEM) *samples);
int cfx_io_read_complex_fix_from_int_partitioned_x (FILE *f, int length, int skip_values, int data_width, complex_fix chess_storage(XMEM) *samples);
int cfx_io_write_complex_fix_to_int_x (FILE *f, int length, int data_width, complex_fix chess_storage(XMEM) *samples);
int cfx_io_write_complex_fix_to_int_partitioned_x (FILE *f, int length, int data_width, complex_fix chess_storage(XMEM) *samples);
#endif

#if defined COOLFLUX_HAS_simd_fix || defined COOLFLUX_HAS_SIMD12
int cfx_io_read_simd_fix_from_int_x (FILE *f, int length, int data_width, int high_low, simd_fix chess_storage(XMEM) *samples);
int cfx_io_read_simd_fix_symmetric_from_int_x (FILE *f, int length, int data_width, simd_fix chess_storage(XMEM) *samples);
int cfx_io_read_isimd_fix_x (FILE *f, int length, int high_low, isimd_fix chess_storage(XMEM) *samples);
int cfx_io_read_isimd_fix_symmetric_x (FILE *f, int length, isimd_fix chess_storage(XMEM) *samples);
int cfx_io_write_simd_fix_to_int_x (FILE *f, int length, int data_width, simd_fix chess_storage(XMEM) *samples);
#endif

#ifdef COOLFLUX_HAS_wide_fix
int cfx_io_read_wide_fix_from_int_x (FILE *f, int length, int data_width, wide_fix chess_storage(XMEM) *samples);
int cfx_io_write_wide_fix_to_int_x (FILE *f, int length, int data_width, wide_fix chess_storage(XMEM) *samples);
#endif

// functions for YMEM
#if defined CoolFlux && defined COOLFLUX_HAS_YMEM

int cfx_io_read_fix_from_int_y (FILE *f, int length, int data_width, fix chess_storage(YMEM) *samples);
int cfx_io_read_lfix_from_int_y (FILE *f, int length, int data_width, lfix chess_storage(YMEM) *samples);
int cfx_io_read_lfix_from_2int_y (FILE *f, int length, int data_width, lfix chess_storage(YMEM) *samples);
int cfx_io_read_int_y (FILE *f, int length, int chess_storage(YMEM) *samples);
int cfx_io_read_uint_fix_y (FILE *f, int length, uint_fix chess_storage(YMEM) *samples);
int cfx_io_read_ulong_y (FILE *f, int length, uint_lfix chess_storage(YMEM) *samples);
int cfx_io_write_fix_to_int_y (FILE *f, int length, int data_width, fix chess_storage(YMEM) *samples);
int cfx_io_write_lfix_to_2int_y (FILE *f, int length, int data_width, lfix chess_storage(YMEM) *samples);
int cfx_io_write_uint_fix_y (FILE *f, int length, uint_fix chess_storage(YMEM) *samples);

#if defined COOLFLUX_HAS_complex_fix || defined COOLFLUX_HAS_COMPLEX12
int cfx_io_read_complex_fix_from_int_y (FILE *f, int length, int data_width, complex_fix chess_storage(YMEM) *samples);
int cfx_io_read_complex_fix_from_int_partitioned_y (FILE *f, int length, int skip_values, int data_width, complex_fix chess_storage(YMEM) *samples);
int cfx_io_write_complex_fix_to_int_y (FILE *f, int length, int data_width, complex_fix chess_storage(YMEM) *samples);
int cfx_io_write_complex_fix_to_int_partitioned_y (FILE *f, int length, int data_width, complex_fix chess_storage(YMEM) *samples);
#endif

#if defined COOLFLUX_HAS_simd_fix || defined COOLFLUX_HAS_SIMD12
int cfx_io_read_simd_fix_from_int_y (FILE *f, int length, int data_width, int high_low, simd_fix chess_storage(YMEM) *samples);
int cfx_io_read_simd_fix_symmetric_from_int_y (FILE *f, int length, int data_width, simd_fix chess_storage(YMEM) *samples);
int cfx_io_read_isimd_fix_y (FILE *f, int length, int high_low, isimd_fix chess_storage(YMEM) *samples);
int cfx_io_read_isimd_fix_symmetric_y (FILE *f, int length, isimd_fix chess_storage(YMEM) *samples);
int cfx_io_write_simd_fix_to_int_y (FILE *f, int length, int data_width, simd_fix chess_storage(YMEM) *samples);
#endif

#ifdef COOLFLUX_HAS_wide_fix
int cfx_io_read_wide_fix_from_int_y (FILE *f, int length, int data_width, wide_fix chess_storage(YMEM) *samples);
int cfx_io_write_wide_fix_to_int_y (FILE *f, int length, int data_width, wide_fix chess_storage(YMEM) *samples);
#endif

#else /*CoolFlux && COOLFLUX_HAS_YMEM*/

#define cfx_io_read_fix_from_int_y                      cfx_io_read_fix_from_int_x
#define cfx_io_read_lfix_from_int_y                     cfx_io_read_lfix_from_int_x
#define cfx_io_read_lfix_from_2int_y                    cfx_io_read_lfix_from_2int_x
#define cfx_io_read_int_y                               cfx_io_read_int_x
#define cfx_io_read_uint_fix_y                          cfx_io_read_uint_fix_x
#define cfx_io_read_ulong_y                             cfx_io_read_ulong_x
#define cfx_io_write_fix_to_int_y                       cfx_io_write_fix_to_int_x
#define cfx_io_write_lfix_to_2int_y                     cfx_io_write_lfix_to_2int_x
#define cfx_io_write_uint_fix_y                         cfx_io_write_uint_fix_x

#if defined COOLFLUX_HAS_complex_fix || defined COOLFLUX_HAS_COMPLEX12
#define cfx_io_read_complex_fix_from_int_y              cfx_io_read_complex_fix_from_int_x
#define cfx_io_read_complex_fix_from_int_partitioned_y  cfx_io_read_complex_fix_from_int_partitioned_x
#define cfx_io_write_complex_fix_to_int_y               cfx_io_write_complex_fix_to_int_x
#define cfx_io_write_complex_fix_to_int_partitioned_y   cfx_io_write_complex_fix_to_int_partitioned_x
#endif

#if defined COOLFLUX_HAS_simd_fix || defined COOLFLUX_HAS_SIMD12
#define cfx_io_read_simd_fix_from_int_y                 cfx_io_read_simd_fix_from_int_x
#define cfx_io_read_simd_fix_symmetric_from_int_y       cfx_io_read_simd_fix_symmetric_from_int_x
#define cfx_io_read_isimd_fix_y                         cfx_io_read_isimd_fix_x
#define cfx_io_read_isimd_fix_symmetric_y               cfx_io_read_isimd_fix_symmetric_x
#define cfx_io_write_simd_fix_to_int_y                  cfx_io_write_simd_fix_to_int_x
#endif

#ifdef COOLFLUX_HAS_wide_fix
#define cfx_io_read_wide_fix_from_int_y                 cfx_io_read_wide_fix_from_int_x
#define cfx_io_write_wide_fix_to_int_y                  cfx_io_write_wide_fix_to_int_x
#endif

#endif /*CoolFlux && COOLFLUX_HAS_YMEM*/

// functions for XYMEM
#if defined CoolFlux && defined COOLFLUX_HAS_XYMEM

int cfx_io_read_lfix_from_int_xy (FILE *f, int length, int data_width, lfix chess_storage(XYMEM) *samples);
int cfx_io_read_lfix_from_2int_xy (FILE *f, int length, int data_width, lfix chess_storage(XYMEM) *samples);

int cfx_io_write_lfix_to_2int_xy (FILE *f, int length, int data_width, lfix chess_storage(XYMEM) *samples);

#else /*CoolFlux && COOLFLUX_HAS_XYMEM*/

#define cfx_io_read_lfix_from_int_xy        cfx_io_read_lfix_from_int_x
#define cfx_io_read_lfix_from_2int_xy       cfx_io_read_lfix_from_2int_x

#define cfx_io_write_lfix_to_2int_xy        cfx_io_write_lfix_to_2int_x

#endif /*CoolFlux && COOLFLUX_HAS_XYMEM*/

#endif /*CFX_AUX_H*/
