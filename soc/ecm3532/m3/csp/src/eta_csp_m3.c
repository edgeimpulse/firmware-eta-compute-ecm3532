/***************************************************************************//**
 *
 * @file eta_csp_m3.c
 *
 * @brief This file contains eta_csp_m3 module implementations.
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
 * @addtogroup ecm3532m3-m3 M3
 * @ingroup ecm3532csp-m3
 * @{
 ******************************************************************************/

#include "eta_csp_m3.h"

/***************************************************************************//**
 *
 *  EtaCspM3IntDisable - Disable interrupts in the M3.
 *
 *  @note Does not support nesting.
 *
 ******************************************************************************/
void
EtaCspM3IntDisable(void)
{
//
// Keil compiler
//
#ifdef __CC_ARM
    uint32_t ui32Temp;
    __asm("  movs  ui32Temp,      #1");
    __asm("  msr   primask, ui32Temp");

//
// Everything else.
//
#else
    // fixme this needs to save/restore the enable state.
    __asm("  movs  r0,      #1");
    __asm("  msr   primask, r0");
#endif
}

/***************************************************************************//**
 *
 *  EtaCspM3IntEnable - Enable interrupts in the M3.
 *
 *  @note Does not support nesting.
 *
 ******************************************************************************/
void
EtaCspM3IntEnable(void)
{
//
// Keil compiler
//
#ifdef __CC_ARM
    uint32_t ui32Temp;
    __asm("  movs  ui32Temp,      #0");
    __asm("  msr   primask, ui32Temp");

//
// Everything else.
//
#else
    // fixme this needs to save/restore the enable state.
    __asm("  movs  r0,      #0");
    __asm("  msr   primask, r0");
#endif
}

/***************************************************************************//**
 *
 *  EtaCspM3IntBaseLevel - Set Base Enable interrupts level in the M3.
 *
 *  @param iDisableLevel - disable interrupts with this priortiy level and up.
 *
 *  @note Does not support nesting.
 *
 ******************************************************************************/
void 
EtaCspM3IntBaseLevel(uint8_t iDisableLevel)
{
    // disable interrupts with priortiy level 1 and up ( 3 bits priority: levels 0-7);
    // Note: Shift the priority value left by (8-NVIC_PRI_BITS = 8-3 = 5)
//
// Keil compiler
//
#ifdef __CC_ARM
  uint32_t ui32Temp;
  if( iDisableLevel == 0){
    __asm("  movs  ui32Temp,      #0x0");
    __asm("  msr   primask, ui32Temp");
  }
  else if( iDisableLevel == 1){
    __asm("  movs  ui32Temp,      #0x20");
    __asm("  msr   primask, ui32Temp");
  }
  else if( iDisableLevel == 2){
    __asm("  movs  ui32Temp,      #0x40");
    __asm("  msr   primask, ui32Temp");
  }
  else if( iDisableLevel == 3){
    __asm("  movs  ui32Temp,      #0x60");
    __asm("  msr   primask, ui32Temp");
  }
  else if( iDisableLevel == 4){
    __asm("  movs  ui32Temp,      #0x80");
    __asm("  msr   primask, ui32Temp");
  }
  else if( iDisableLevel == 5){
    __asm("  movs  ui32Temp,      #0xa0");
    __asm("  msr   primask, ui32Temp");
  }
  else if( iDisableLevel == 6){
    __asm("  movs  ui32Temp,      #0xc0");
    __asm("  msr   primask, ui32Temp");
  }
  else if( iDisableLevel == 7){
    __asm("  movs  ui32Temp,      #0xe0");
    __asm("  msr   primask, ui32Temp");
  }

//
// Everything else.
//
#else
  if( iDisableLevel == 0){
    __asm("    movs     r0,       #0x0\n"
          "    MSR      BASEPRI,  r0\n");
  }
  else if( iDisableLevel == 1){
    __asm("    movs     r0,       #0x20\n"
          "    MSR      BASEPRI,  r0\n");
  }
  else if( iDisableLevel == 2){
    __asm("    movs     r0,       #0x40\n"
          "    MSR      BASEPRI,  r0\n");
  }
  else if( iDisableLevel == 3){
    __asm("    movs     r0,       #0x60\n"
          "    MSR      BASEPRI,  r0\n");
  }
  else if( iDisableLevel == 4){
    __asm("    movs     r0,       #0x80\n"
          "    MSR      BASEPRI,  r0\n");
  }
  else if( iDisableLevel == 5){
    __asm("    movs     r0,       #0xa0\n"
          "    MSR      BASEPRI,  r0\n");
  }
  else if( iDisableLevel == 6){
    __asm("    movs     r0,       #0xc0\n"
          "    MSR      BASEPRI,  r0\n");
  }
  else if( iDisableLevel == 7){
    __asm("    movs     r0,       #0xe0\n"
          "    MSR      BASEPRI,  r0\n");
  }
#endif
}

/***************************************************************************//**
 *
 *  EtaCspM3WFI - Issue WFI instruction inside critical section.
 *
 *
 ******************************************************************************/
#if defined(__CC_ARM)
__asm void
EtaCspM3WFI(void)
{
    cpsid i
    wfi
     cpsie i
    bx      lr;
}
#else
void __attribute__((naked))
EtaCspM3WFI(void)
{
    __asm(" cpsid i\n"
	  " wfi \n"
	  " cpsie i \n"
          " bx    lr"
	  );
}
#endif

