/*$Workfile::   cfx_matrixoperation.h                                  $*/
/*$Revision: 1.2                                                       $*/
/* -------------------------------------------------------------------- */
/* Copyright (c) 2012 NXP B.V.                                          */
/* All rights reserved.                                                 */
/* This source code and any compilation or derivative thereof is the    */
/* proprietary information of NXP B.V. and                              */
/* is confidential in nature.                                           */
/*                                                                      */
/* Under no circumstances is this software to be combined with any Open */
/* Source Software in any way or placed under an Open Source License of */
/* any type without the express written permission of                   */
/* NXP B.V.                                                             */
/* -------------------------------------------------------------------- */

/* Author    : Adrian Ungureanu                                          */
/* Last Modify  : September , 2017                                       */


#ifndef MATRIXOPERATION_H
#define MATRIXOPERATION_H

#include "CoolFlux_defs.h"
#include "cfx_aux.h"

// Matrix operations
void cfx_matrix_mult_xxx(int width1, int height1, int width2, fix chess_storage(XMEM)*a, fix chess_storage(XMEM)*b, fix chess_storage(XMEM)*c);
void cfx_matrix_add_xxx(int width, int height, const fix chess_storage(XMEM)* restrict a, const fix chess_storage(XMEM)* restrict b, fix chess_storage(XMEM)* restrict c);
void cfx_matrix_sub_xxx(int width, int height, const fix chess_storage(XMEM)* restrict a, const fix chess_storage(XMEM)* restrict b, fix chess_storage(XMEM)* restrict c);
void cfx_matrix_copy_xx(int width, int height, fix chess_storage(XMEM)*a, fix chess_storage(XMEM)*b);
void cfx_matrix_transpose_xx(int width, int height, fix chess_storage(XMEM)*a, fix chess_storage(XMEM)*b);
void cfx_matrix_clear_x(int width, int height, fix chess_storage(XMEM)*a);

#ifdef CoolFlux

void cfx_matrix_mult_xxy(int width1, int height1, int width2, fix chess_storage(XMEM)*a, fix chess_storage(XMEM)*b, fix chess_storage(YMEM)*c);
void cfx_matrix_mult_xyx(int width1, int height1, int width2, fix chess_storage(XMEM)*a, fix chess_storage(YMEM)*b, fix chess_storage(XMEM)*c);
void cfx_matrix_mult_xyy(int width1, int height1, int width2, fix chess_storage(XMEM)*a, fix chess_storage(YMEM)*b, fix chess_storage(YMEM)*c);
void cfx_matrix_mult_yxx(int width1, int height1, int width2, fix chess_storage(YMEM)*a, fix chess_storage(XMEM)*b, fix chess_storage(XMEM)*c);
void cfx_matrix_mult_yxy(int width1, int height1, int width2, fix chess_storage(YMEM)*a, fix chess_storage(XMEM)*b, fix chess_storage(YMEM)*c);
void cfx_matrix_mult_yyx(int width1, int height1, int width2, fix chess_storage(YMEM)*a, fix chess_storage(YMEM)*b, fix chess_storage(XMEM)*c);
void cfx_matrix_mult_yyy(int width1, int height1, int width2, fix chess_storage(YMEM)*a, fix chess_storage(YMEM)*b, fix chess_storage(YMEM)*c);

void cfx_matrix_add_xxy(int width, int height, const fix chess_storage(XMEM)* restrict a, const fix chess_storage(XMEM)* restrict b, fix chess_storage(YMEM)* restrict c);
void cfx_matrix_add_xyx(int width, int height, const fix chess_storage(XMEM)* restrict a, const fix chess_storage(YMEM)* restrict b, fix chess_storage(XMEM)* restrict c);
void cfx_matrix_add_xyy(int width, int height, const fix chess_storage(XMEM)* restrict a, const fix chess_storage(YMEM)* restrict b, fix chess_storage(YMEM)* restrict c);
void cfx_matrix_add_yxx(int width, int height, const fix chess_storage(YMEM)* restrict a, const fix chess_storage(XMEM)* restrict b, fix chess_storage(XMEM)* restrict c);
void cfx_matrix_add_yxy(int width, int height, const fix chess_storage(YMEM)* restrict a, const fix chess_storage(XMEM)* restrict b, fix chess_storage(YMEM)* restrict c);
void cfx_matrix_add_yyx(int width, int height, const fix chess_storage(YMEM)* restrict a, const fix chess_storage(YMEM)* restrict b, fix chess_storage(XMEM)* restrict c);
void cfx_matrix_add_yyy(int width, int height, const fix chess_storage(YMEM)* restrict a, const fix chess_storage(YMEM)* restrict b, fix chess_storage(YMEM)* restrict c);

void cfx_matrix_sub_xxy(int width, int height, const fix chess_storage(XMEM)* restrict a, const fix chess_storage(XMEM)* restrict b, fix chess_storage(YMEM)* restrict c);
void cfx_matrix_sub_xyx(int width, int height, const fix chess_storage(XMEM)* restrict a, const fix chess_storage(YMEM)* restrict b, fix chess_storage(XMEM)* restrict c);
void cfx_matrix_sub_xyy(int width, int height, const fix chess_storage(XMEM)* restrict a, const fix chess_storage(YMEM)* restrict b, fix chess_storage(YMEM)* restrict c);
void cfx_matrix_sub_yxx(int width, int height, const fix chess_storage(YMEM)* restrict a, const fix chess_storage(XMEM)* restrict b, fix chess_storage(XMEM)* restrict c);
void cfx_matrix_sub_yxy(int width, int height, const fix chess_storage(YMEM)* restrict a, const fix chess_storage(XMEM)* restrict b, fix chess_storage(YMEM)* restrict c);
void cfx_matrix_sub_yyx(int width, int height, const fix chess_storage(YMEM)* restrict a, const fix chess_storage(YMEM)* restrict b, fix chess_storage(XMEM)* restrict c);
void cfx_matrix_sub_yyy(int width, int height, const fix chess_storage(YMEM)* restrict a, const fix chess_storage(YMEM)* restrict b, fix chess_storage(YMEM)* restrict c);

void cfx_matrix_copy_xy(int width, int height, fix chess_storage(XMEM)*a, fix chess_storage(YMEM)*b);
void cfx_matrix_copy_yx(int width, int height, fix chess_storage(YMEM)*a, fix chess_storage(XMEM)*b);
void cfx_matrix_copy_yy(int width, int height, fix chess_storage(YMEM)*a, fix chess_storage(YMEM)*b);

void cfx_matrix_transpose_xy(int width, int height, fix chess_storage(XMEM)*a, fix chess_storage(YMEM)*b);
void cfx_matrix_transpose_yx(int width, int height, fix chess_storage(YMEM)*a, fix chess_storage(XMEM)*b);
void cfx_matrix_transpose_yy(int width, int height, fix chess_storage(YMEM)*a, fix chess_storage(YMEM)*b);

void cfx_matrix_clear_y(int width, int height, fix chess_storage(YMEM)*a);

#else /*CoolFlux*/

#define cfx_matrix_mult_xxy cfx_matrix_mult_xxx
#define cfx_matrix_mult_xyx cfx_matrix_mult_xxx
#define cfx_matrix_mult_xyy cfx_matrix_mult_xxx
#define cfx_matrix_mult_yxx cfx_matrix_mult_xxx
#define cfx_matrix_mult_yxy cfx_matrix_mult_xxx
#define cfx_matrix_mult_yyx cfx_matrix_mult_xxx
#define cfx_matrix_mult_yyy cfx_matrix_mult_xxx

#define cfx_matrix_add_xxy cfx_matrix_add_xxx
#define cfx_matrix_add_xyx cfx_matrix_add_xxx
#define cfx_matrix_add_xyy cfx_matrix_add_xxx
#define cfx_matrix_add_yxx cfx_matrix_add_xxx
#define cfx_matrix_add_yxy cfx_matrix_add_xxx
#define cfx_matrix_add_yyx cfx_matrix_add_xxx
#define cfx_matrix_add_yyy cfx_matrix_add_xxx

#define cfx_matrix_sub_xxy cfx_matrix_sub_xxx
#define cfx_matrix_sub_xyx cfx_matrix_sub_xxx
#define cfx_matrix_sub_xyy cfx_matrix_sub_xxx
#define cfx_matrix_sub_yxx cfx_matrix_sub_xxx
#define cfx_matrix_sub_yxy cfx_matrix_sub_xxx
#define cfx_matrix_sub_yyx cfx_matrix_sub_xxx
#define cfx_matrix_sub_yyy cfx_matrix_sub_xxx

#define cfx_matrix_copy_xy cfx_matrix_copy_xx
#define cfx_matrix_copy_yx cfx_matrix_copy_xx
#define cfx_matrix_copy_yy cfx_matrix_copy_xx

#define cfx_matrix_transpose_xy cfx_matrix_transpose_xx
#define cfx_matrix_transpose_yx cfx_matrix_transpose_xx
#define cfx_matrix_transpose_yy cfx_matrix_transpose_xx

#define cfx_matrix_clear_y cfx_matrix_clear_x

#endif /*CoolFlux*/


#endif
