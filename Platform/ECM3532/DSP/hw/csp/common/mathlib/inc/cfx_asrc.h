/*File::   cfx_prng.h                                                             */
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

/*Author      : Bulancea Melutu-Iulian                                    */

#ifndef CFX_ASRC_H
#define CFX_ASRC_H

#include "cf6_chess.h"
#include "mathlib.h"

#define CFX_ASRC_MAX_D_LOG2     8

typedef struct
{
    int I, D;

    int r;
    int r_inc;
    int q_inc;

    fix delta;
    fix delta_inc;
    fix D_recip;

    const fix chess_storage(YMEM) *pFilterCoeff;
    int phaseFilterLength;

    fix *pDelayStart;
    fix *pDelay;
    int flag;
    fix delta_d;
} asrc_Instance;

void cfx_asrc_Reset(asrc_Instance *pInstance);
void cfx_asrc_Init(asrc_Instance *pInstance, int I, int D, fix D_frac, fix D_recip, const fix chess_storage(YMEM) *pFilterCoeff, int phaseFilterLength, fix *pDelayStart);
int cfx_asrc_interpolate_Process(asrc_Instance *pInstance,fix *inputBuffer, fix *outputBuffer, int blockLength);
int cfx_asrc_decimate_Process(asrc_Instance *pInstance,fix *in_buff, fix *out_buff, int B);
int cfx_asrc_rational_Process(asrc_Instance *pInstance, fix *in_buff, fix *out_buff, int B);
int cfx_asrc_arbitrary_Process(asrc_Instance *pInstance, fix *in_buff, fix *out_buff, int B);

#endif

