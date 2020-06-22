/***************************************************************************//**
 *
 * @file eta_csp_nvic.c
 *
 * @brief This file contains eta_csp_nvic module implementations.
 *
 * Copyright (C) 2018 Eta Compute, Inc
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This is part of revision 1.0.0a6 of the Tensai Software Development Kit.
 *
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup ecm3532nvic-m3 Nested Vectored Interrupt Controller (NVIC)
 * @ingroup ecm3532csp-m3
 * @{
 ******************************************************************************/

#include "eta_chip.h"
#include "eta_csp_nvic.h"
#include "eta_csp.h"

/***************************************************************************//**
 *
 *  EtaCspNvicIntEnable - Enable an interrupt in the NVIC.
 *
 *  @param iIrqNum interrupt to enable.
 *
 ******************************************************************************/
void
EtaCspNvicIntEnable(tIrqNum iIrqNum)
{
    REG_NVIC_EISER0.V = (1 << iIrqNum);
}

/***************************************************************************//**
 *
 *  EtaCspNvicIntDisable - Disable an interrupt in the NVIC.
 *
 *  @param iIrqNum interrupt to disable.
 *
 ******************************************************************************/
void
EtaCspNvicIntDisable(tIrqNum iIrqNum)
{
    REG_NVIC_EICER0.V = (1 << iIrqNum);
}

/***************************************************************************//**
 *
 *  EtaCspNvicIntDisableAll - Disable all interrupts in the NVIC.
 *
 *
 ******************************************************************************/
void
EtaCspNvicIntDisableAll(void)
{
    REG_NVIC_EICER0.V = 0xFFFFFFFF;
}

/***************************************************************************//**
 *
 *  EtaCspNvicIntClear - Clear an interrupt in the NVIC.
 *
 *  @param iIrqNum interrupt to clear.
 *
 ******************************************************************************/
void
EtaCspNvicIntClear(tIrqNum iIrqNum)
{
    REG_NVIC_EICPR0.V = (1 << iIrqNum);
}

/***************************************************************************//**
 *
 *  EtaCspNvicIntClearAll - Clear all interrupts in the NVIC.
 *
 *  @param iIrqNum interrupt to clear.
 *
 ******************************************************************************/
void
EtaCspNvicIntClearAll(void)
{
    REG_NVIC_EICPR0.V = 0xFFFFFFFF;
}


/***************************************************************************//**
 *
 *  EtaCspNvicIntStatusSet - Set the interrupt pending bit.
 *
 *  @param iIrqNum - the interrupt for the pending bit to set.
 *
 ******************************************************************************/
void EtaCspNvicIntStatusSet(tIrqNum iIrqNum)
{
    REG_NVIC_EISPR0.V = (0x1 << iIrqNum);
}

/***************************************************************************//**
 *
 *  EtaCspNvicIntStatusGet - Get the interrupt status.
 *
 *  @return REturn the interrupt status.
 *
 ******************************************************************************/
reg_nvic_eispr0_t
EtaCspNvicIntStatusGet(void)
{
    return(REG_NVIC_EISPR0);
}

/***************************************************************************//**
 *
 *  EtaCspNvicM3Reset - Perform a M3 reset.
 *
 ******************************************************************************/
void
EtaCspNvicM3Reset(void)
{
    REG_W2(NVIC_AIRCR, VECTKEY, 0x05FA, VECTRESET, TRUE);
}

/***************************************************************************//**
 *
 *  EtaCspNvicSocReset - Perform a SOC reset.
 *
 ******************************************************************************/
void
EtaCspNvicSocReset(void)
{
    REG_W2(NVIC_AIRCR, VECTKEY, 0x05FA, SYSRESETREQ, TRUE);
}

/***************************************************************************//**
 *
 *  EtaCspNvicIntPrioritySet - set an external interrupt priority level.
 *
 *  @param iIrqNum the interrput to priority to set.
 *  @param ui32Priority the desired priority.
 *
 ******************************************************************************/
void
EtaCspNvicIntPrioritySet(tIrqNum iIrqNum, uint32_t ui32Priority)
{
    switch(iIrqNum)
    {
       case  0: REG_M1(NVIC_EIPLR0, PRI_V0, ui32Priority<<5); break;
       case  1: REG_M1(NVIC_EIPLR0, PRI_V1, ui32Priority<<5); break;
       case  2: REG_M1(NVIC_EIPLR0, PRI_V2, ui32Priority<<5); break;
       case  3: REG_M1(NVIC_EIPLR0, PRI_V3, ui32Priority<<5); break;
       case  4: REG_M1(NVIC_EIPLR1, PRI_V4, ui32Priority<<5); break;
       case  5: REG_M1(NVIC_EIPLR1, PRI_V5, ui32Priority<<5); break;
       case  6: REG_M1(NVIC_EIPLR1, PRI_V6, ui32Priority<<5); break;
       case  7: REG_M1(NVIC_EIPLR1, PRI_V7, ui32Priority<<5); break;
       case  8: REG_M1(NVIC_EIPLR2, PRI_V8, ui32Priority<<5); break;
       case  9: REG_M1(NVIC_EIPLR2, PRI_V9, ui32Priority<<5); break;
       case 10: REG_M1(NVIC_EIPLR2, PRI_V10, ui32Priority<<5); break;
       case 11: REG_M1(NVIC_EIPLR2, PRI_V11, ui32Priority<<5); break;
       case 12: REG_M1(NVIC_EIPLR3, PRI_V12, ui32Priority<<5); break;
       case 13: REG_M1(NVIC_EIPLR3, PRI_V13, ui32Priority<<5); break;
       case 14: REG_M1(NVIC_EIPLR3, PRI_V14, ui32Priority<<5); break;
       case 15: REG_M1(NVIC_EIPLR3, PRI_V15, ui32Priority<<5); break;
       case 16: REG_M1(NVIC_EIPLR4, PRI_V16, ui32Priority<<5); break;
       case 17: REG_M1(NVIC_EIPLR4, PRI_V17, ui32Priority<<5); break;
       case 18: REG_M1(NVIC_EIPLR4, PRI_V18, ui32Priority<<5); break;
       case 19: REG_M1(NVIC_EIPLR4, PRI_V19, ui32Priority<<5); break;
       case 20: REG_M1(NVIC_EIPLR5, PRI_V20, ui32Priority<<5); break;
       case 21: REG_M1(NVIC_EIPLR5, PRI_V21, ui32Priority<<5); break;
       case 22: REG_M1(NVIC_EIPLR5, PRI_V22, ui32Priority<<5); break;
       case 23: REG_M1(NVIC_EIPLR5, PRI_V23, ui32Priority<<5); break;
#ifdef NOTDEF
fixme remove there are only 24 bits in ecm3532
       case 24: REG_M1(NVIC_EIPLR6, PRI_V24, ui32Priority<<5); break;
       case 25: REG_M1(NVIC_EIPLR6, PRI_V25, ui32Priority<<5); break;
       case 26: REG_M1(NVIC_EIPLR6, PRI_V26, ui32Priority<<5); break;
       case 27: REG_M1(NVIC_EIPLR6, PRI_V27, ui32Priority<<5); break;
       case 28: REG_M1(NVIC_EIPLR7, PRI_V28, ui32Priority<<5); break;
       case 29: REG_M1(NVIC_EIPLR7, PRI_V29, ui32Priority<<5); break;
       case 30: REG_M1(NVIC_EIPLR7, PRI_V30, ui32Priority<<5); break;
       case 31: REG_M1(NVIC_EIPLR7, PRI_V31, ui32Priority<<5); break;
#else
       default: break;
#endif
    }
}

/***************************************************************************//**
 *
 *  EtaCspNvicIntPriorityGet - TODO not implemented.
 *
 *  @param iIrqNum the interrput to priority to get.
 *  @return Return the interrupt priority.
 *
 ******************************************************************************/
uint32_t
EtaCspNvicIntPriorityGet(tIrqNum iIrqNum)
{
    switch(iIrqNum)
    {
       case  0: return (REG_NVIC_EIPLR0.BF.PRI_V0)>>5; break;
       case  1: return (REG_NVIC_EIPLR0.BF.PRI_V1)>>5; break;
       case  2: return (REG_NVIC_EIPLR0.BF.PRI_V2)>>5; break;
       case  3: return (REG_NVIC_EIPLR0.BF.PRI_V3)>>5; break;
       case  4: return (REG_NVIC_EIPLR1.BF.PRI_V4)>>5; break;
       case  5: return (REG_NVIC_EIPLR1.BF.PRI_V5)>>5; break;
       case  6: return (REG_NVIC_EIPLR1.BF.PRI_V6)>>5; break;
       case  7: return (REG_NVIC_EIPLR1.BF.PRI_V7)>>5; break;
       case  8: return (REG_NVIC_EIPLR2.BF.PRI_V8)>>5; break;
       case  9: return (REG_NVIC_EIPLR2.BF.PRI_V9)>>5; break;
       case 10: return (REG_NVIC_EIPLR2.BF.PRI_V10)>>5; break;
       case 11: return (REG_NVIC_EIPLR2.BF.PRI_V11)>>5; break;
       case 12: return (REG_NVIC_EIPLR3.BF.PRI_V12)>>5; break;
       case 13: return (REG_NVIC_EIPLR3.BF.PRI_V13)>>5; break;
       case 14: return (REG_NVIC_EIPLR3.BF.PRI_V14)>>5; break;
       case 15: return (REG_NVIC_EIPLR3.BF.PRI_V15)>>5; break;
       case 16: return (REG_NVIC_EIPLR4.BF.PRI_V16)>>5; break;
       case 17: return (REG_NVIC_EIPLR4.BF.PRI_V17)>>5; break;
       case 18: return (REG_NVIC_EIPLR4.BF.PRI_V18)>>5; break;
       case 19: return (REG_NVIC_EIPLR4.BF.PRI_V19)>>5; break;
       case 20: return (REG_NVIC_EIPLR5.BF.PRI_V20)>>5; break;
       case 21: return (REG_NVIC_EIPLR5.BF.PRI_V21)>>5; break;
       case 22: return (REG_NVIC_EIPLR5.BF.PRI_V22)>>5; break;
#ifdef NOTDEF
fixme remove there are only 24 bits in ecm3532
       case 23: return (REG_NVIC_EIPLR5.BF.PRI_V23)>>5; break;
       case 24: return (REG_NVIC_EIPLR6.BF.PRI_V24)>>5; break;
       case 25: return (REG_NVIC_EIPLR6.BF.PRI_V25)>>5; break;
       case 26: return (REG_NVIC_EIPLR6.BF.PRI_V26)>>5; break;
       case 27: return (REG_NVIC_EIPLR6.BF.PRI_V27)>>5; break;
       case 28: return (REG_NVIC_EIPLR7.BF.PRI_V28)>>5; break;
       case 29: return (REG_NVIC_EIPLR7.BF.PRI_V29)>>5; break;
       case 30: return (REG_NVIC_EIPLR7.BF.PRI_V30)>>5; break;
       case 31: return (REG_NVIC_EIPLR7.BF.PRI_V31)>>5; break;
#else
       default: // fall through and return the same value
       case 23: return (REG_NVIC_EIPLR5.BF.PRI_V23)>>5; break;
#endif
    }
}

/***************************************************************************//**
 *
 *  EtaCspNvicIntSoftTrigger - Issue a software triggered interrupt
 *
 *  @param iIrqNum the interrput to trigger by software write.
 *
 ******************************************************************************/
void
EtaCspNvicIntSoftTrigger(tIrqNum iIrqNum)
{
    REG_NVIC_STIR.V = iIrqNum;
}

