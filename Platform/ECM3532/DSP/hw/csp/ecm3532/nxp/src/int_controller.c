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


#include "api-int_controller.h"

/* 
Mapping a hardware interrupt to a CoolFlux interrupt line:
    - Address where the Interrupt Controller is mapped: p_intctrl.
    - Index of the hardware interrupt line: interrupt_idx.
    - CoolFlux interrupt line: interrupt_line.
*/
intctrl_err_T intctrl_associateInterruptLine(p_hw_intctrl_reg_T p_intctrl, unsigned int interrupt_idx, intline_T interrupt_line)
{
    unsigned int word[2];

    if (p_intctrl->enable & (1 << interrupt_idx))
    {
        return INTCTRL_ENABLED_INT_ERR;
    }

    /* Get LSB and store it in word[0] */
    word[0] = (interrupt_line & 0x01) << interrupt_idx;
    /* Get next bit and store it in word[1] */
    word[1] = ((interrupt_line >> 1) & 0x01) << interrupt_idx;

    p_intctrl->select[0] |= word[0];
    p_intctrl->select[1] |= word[1];

    return INTCTRL_SUCCESS;
}


/*
Enable or disable CoolFlux nested interrupt mode.
    - Address where the Interrupt Controller is mapped: p_intctrl.
    - Nested or unnested interrupt mode: nested_mode.
*/
intctrl_err_T intctrl_setNestedMode(p_hw_intctrl_reg_T p_intctrl, nested_mode_T nested_mode)
{ 
    if (p_intctrl->enable)
    {
        return INTCTRL_ENABLED_INT_ERR;
    }

    p_intctrl->interrupt_mode = nested_mode;
    return INTCTRL_SUCCESS;
}

/*
Configure which edge triggers a hardware interrupt:
    - Address where the Interrupt Controller is mapped: p_intctrl.
    - Index of the hardware interrupt: interrupt_idx.
    - Triggering edge (rising or falling): edge.
*/
intctrl_err_T intctrl_setPolarity(p_hw_intctrl_reg_T p_intctrl, unsigned int interrupt_idx, intctrl_edge_T edge)
{
    bitmask_T int_bitmask = 1 << interrupt_idx;
    if (p_intctrl->enable & int_bitmask)
    {
        return INTCTRL_ENABLED_INT_ERR;
    }

    /* clear the value */
    p_intctrl->polarity &= (~int_bitmask);

    /* set it to the new value */
    p_intctrl->polarity |= (int_bitmask & (edge << interrupt_idx));

    return INTCTRL_SUCCESS;
}

/*
Configure which edge triggers several hardware interrupt:
    - Address where the Interrupt Controller is mapped: p_intctrl.
    - Bitmask for the hardware interrupts to be configured: interrupt_bitmask.
    - Triggering edge (rising or falling): edge.
*/
intctrl_err_T intctrl_setPolarity_bitmask(p_hw_intctrl_reg_T p_intctrl, bitmask_T int_bitmask, intctrl_edge_T edge)
{
    if (p_intctrl->enable & int_bitmask)
    {
        return INTCTRL_ENABLED_INT_ERR;
    }

    bitmask_T edge_mask = (edge == INTCTRL_FALLING_EDGE)? 0xffffff : 0x000000;

    /* clear the value */
    p_intctrl->polarity &= (~int_bitmask);
    
    /* set it to the new value */
    p_intctrl->polarity |= (int_bitmask & edge_mask);

    return INTCTRL_SUCCESS;
}



/* Enable one hardware interrupt specified by its index. */
intctrl_err_T intctrl_enableInterrupt(p_hw_intctrl_reg_T p_intctrl, unsigned int interrupt_idx)
{
    bitmask_T int_bitmask = 1 << interrupt_idx;
    
    p_intctrl->enable |= int_bitmask;

    return INTCTRL_SUCCESS;
}

/* Disable one hardware interrupt specified by its index. */
intctrl_err_T intctrl_disableInterrupt(p_hw_intctrl_reg_T p_intctrl, unsigned int interrupt_idx)
{
    bitmask_T int_bitmask = 1 << interrupt_idx;
    
    p_intctrl->enable &= (~int_bitmask);
    
    return INTCTRL_SUCCESS;    
    
}


/* Disable all interrupts. */
intctrl_err_T intctrl_disableAllInterrupts(p_hw_intctrl_reg_T p_intctrl)
{
    p_intctrl->enable = 0;
    return INTCTRL_SUCCESS;
}

/* Set interrupts using a bitmask. */
intctrl_err_T intctrl_setInterrupts(p_hw_intctrl_reg_T p_intctrl, bitmask_T int_bitmask)
{
    p_intctrl->enable = int_bitmask;
    return INTCTRL_SUCCESS;
}

/* Get all active interrupts (enabled or disabled). */
bitmask_T intctrl_getInterruptStatus(p_hw_intctrl_reg_T p_intctrl)
{
    return p_intctrl->status;
}

/* Get a copy of the Interrupt Controller configuration registers */
intctrl_err_T intctrl_getHWRegistersSnapshot(p_hw_intctrl_reg_T p_intctrl, hw_intctrl_reg_T *p_hw_intctrl_reg_snapshot)
{
#if (0)
    p_hw_intctrl_reg_snapshot->status_int_line[0] = p_intctrl->status_int_line[0];
    p_hw_intctrl_reg_snapshot->status_int_line[1] = p_intctrl->status_int_line[1];
    p_hw_intctrl_reg_snapshot->status_int_line[2] = p_intctrl->status_int_line[2];
    p_hw_intctrl_reg_snapshot->enable = p_intctrl->enable;
    p_hw_intctrl_reg_snapshot->polarity = p_intctrl->polarity;
    p_hw_intctrl_reg_snapshot->select[0] = p_intctrl->select[0];
    p_hw_intctrl_reg_snapshot->select[1] = p_intctrl->select[1];
    p_hw_intctrl_reg_snapshot->interrupt_mode = p_intctrl->interrupt_mode;
    p_hw_intctrl_reg_snapshot->status = p_intctrl->status;
    p_hw_intctrl_reg_snapshot->trigger = p_intctrl->trigger;
#endif

    *p_hw_intctrl_reg_snapshot = *p_intctrl;
    

    return INTCTRL_SUCCESS;
}
