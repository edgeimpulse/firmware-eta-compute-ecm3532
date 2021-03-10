/* ----------------------------------------------------------------------------------- */
/* Copyright (c) 2010 NXP B.V.                                                         */
/* All rights reserved.                                                                */
/* This source code and any compilation or derivative thereof is the                   */
/* proprietary information of NXP B.V. and                                             */
/* is confidential in nature.                                                          */
/*                                                                                     */
/* Under no circumstances is this software to be combined with any Open                */
/* Source Software in any way or placed under an Open Source License of                */
/* any type without the express written permission of                                  */
/* NXP B.V.                                                                            */
/*
/* Version 1.3.0
/* ----------------------------------------------------------------------------------- */


/*--------------------------------------------------------------------**
    Purpose:  unique header file implementing the API of the module
**--------------------------------------------------------------------*/

#ifndef API_INT_CONTROLLER_H
#define API_INT_CONTROLLER_H

#include "cf6_chess.h"

typedef struct
{
  unsigned int status_int_line[3];
  unsigned int select[2];
  unsigned int enable;
  unsigned int interrupt_mode;
  unsigned int trigger;
  unsigned int polarity;
  unsigned int status;
} hw_intctrl_reg_T;


typedef volatile hw_intctrl_reg_T chess_storage(IOMEM) *p_hw_intctrl_reg_T;

typedef enum {INTCTRL_SUCCESS = 0, INTCTRL_ENABLED_INT_ERR, INTCTRL_DISABLED_INT_ERR} intctrl_err_T;
typedef enum {CF_INT_LINE_0 = 1, CF_INT_LINE_1 = 2, CF_INT_LINE_2 = 3} intline_T;
typedef enum {UNNESTED_INTERRUPTS = 0, NESTED_INTERRUPTS = 1} nested_mode_T;
typedef enum {INTCTRL_RISING_EDGE = 0, INTCTRL_FALLING_EDGE = 1} intctrl_edge_T;
typedef unsigned int bitmask_T;

#define BITMASK(i) (1 << (i))

/* 
Mapping a hardware interrupt to a CoolFlux interrupt line:
    - Address where the Interrupt Controller is mapped: p_intctrl.
    - Index of the hardware interrupt line: interrupt_idx.
    - CoolFlux interrupt line: interrupt_line.
*/
intctrl_err_T intctrl_associateInterruptLine(p_hw_intctrl_reg_T p_intctrl, unsigned int interrupt_idx, intline_T interrupt_line);

/*
Enable or disable CoolFlux nested interrupt mode.
    - Address where the Interrupt Controller is mapped: p_intctrl.
    - Nested or unnested interrupt mode: nested_mode.
*/
intctrl_err_T intctrl_setNestedMode(p_hw_intctrl_reg_T p_intctrl, nested_mode_T nested_mode);

/*
Configure which edge triggers a hardware interrupt:
    - Address where the Interrupt Controller is mapped: p_intctrl.
    - Index of the hardware interrupt: interrupt_idx.
    - Triggering edge (rising or falling): edge.
*/
intctrl_err_T intctrl_setPolarity(p_hw_intctrl_reg_T p_intctrl, unsigned int interrupt_idx, intctrl_edge_T edge);

/*
Configure which edge triggers several hardware interrupt:
    - Address where the Interrupt Controller is mapped: p_intctrl.
    - Bitmask for the hardware interrupts to be configured: interrupt_bitmask.
    - Triggering edge (rising or falling): edge.
*/
intctrl_err_T intctrl_setPolarity_bitmask(p_hw_intctrl_reg_T p_intctrl, bitmask_T interrupt_bitmask, intctrl_edge_T edge);

/* Enable one hardware interrupt specified by its index. */
intctrl_err_T intctrl_enableInterrupt(p_hw_intctrl_reg_T p_intctrl, unsigned int interrupt_idx);

/* Set interrupts using a bitmask. */
intctrl_err_T intctrl_setInterrupts(p_hw_intctrl_reg_T p_intctrl, bitmask_T int_bitmask);

/* Disable one hardware interrupt specified by its index. */
intctrl_err_T intctrl_disableInterrupt(p_hw_intctrl_reg_T p_intctrl, unsigned int interrupt_idx);

/* Disable all interrupts. */
intctrl_err_T intctrl_disableAllInterrupts(p_hw_intctrl_reg_T p_intctrl);


/* Trigger one interrupt. */
static inline intctrl_err_T intctrl_triggerInterrupt(p_hw_intctrl_reg_T p_intctrl, unsigned int interrupt_idx)
{
    p_intctrl->trigger |= (1 << interrupt_idx);
    return INTCTRL_SUCCESS;
}


/* Trigger several intterupts. */
static inline intctrl_err_T intctrl_triggerInterrupts_bitmask(p_hw_intctrl_reg_T p_intctrl, bitmask_T bitmask)
{
    p_intctrl->trigger |= bitmask;
    return INTCTRL_SUCCESS;
}

/* Get active interupts for an interrupt line. */
static inline bitmask_T intctrl_getActiveInterruptsBitmask(p_hw_intctrl_reg_T p_intctrl, intline_T interrupt_line)
{
    return p_intctrl->status_int_line[interrupt_line - 1];
}

/* Clear interrups for an interrupt line. */
static inline intctrl_err_T intctrl_clearInterrupts_bitmask(p_hw_intctrl_reg_T p_intctrl, intline_T interrupt_line, bitmask_T bitmask)
{
    p_intctrl->status_int_line[interrupt_line - 1] = bitmask;
    return INTCTRL_SUCCESS;
}

/* Clear pending hardware interrupt, if any. */
static inline intctrl_err_T intctrl_clearInterruptStatus_bitmask(p_hw_intctrl_reg_T p_intctrl, bitmask_T bitmask)
{
    p_intctrl->status = bitmask;
    return INTCTRL_SUCCESS;
}

/* Get all active interrupts (enabled or disabled). */
bitmask_T intctrl_getInterruptStatus(p_hw_intctrl_reg_T p_intctrl);

/* Get a copy of the Interrupt Controller configuration registers */
intctrl_err_T intctrl_getHWRegistersSnapshot(p_hw_intctrl_reg_T p_intctrl, hw_intctrl_reg_T *p_hw_intctrl_reg_snapshot);

#endif
