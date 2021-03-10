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

#ifndef CFX_PRNG_H
#define CFX_PRNG_H

#include "cf6_chess.h"
#include "mathlib.h"
//#include "coolFlux_defs.h"

// and mask to select only the last 8 bits from the seed
#define PRNG_AND_MASK 0xFF

typedef struct
{
    uint_lfix seed;
    uint_lfix poly;

} prng_Instance;


void cfx_prng_Init( prng_Instance *pInstance,
                    uint_lfix seed,
                    uint_lfix poly );

void cfx_prng_uniform_Process( prng_Instance *pInstance,
                               fix *x,
                               int x_len );

void cfx_prng_normal_Process( prng_Instance *pInstance,
                              fix *x,
                              int x_len );

extern const fix aria0;
extern const fix ziggurat[512];
extern const fix lines[512];

#endif

