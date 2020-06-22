/***************************************************************************//**
 *
 * @file eta_utils_state.c
 *
 * @brief Functions to help with saving/restoring state.
 *
 * Copyright (C) 2019 Eta Compute, Inc
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
 * @addtogroup etautils-state State Save/Restore
 * @ingroup etautils
 * @{
 ******************************************************************************/

#include <stdio.h>
#include "eta_utils_state.h"
#include "eta_utils_stdio.h"
#include "eta_chip.h"
#include "memio.h"

#define __CORTEX_M (3)

//
// Cortex-(M3 || M4) without FPU.
//
#if ((__CORTEX_M == 3) || ((__CORTEX_M == 4) && (__FPU_USED == 0)))

//
// Keil
//
#if defined(__CC_ARM)

/***************************************************************************//**
 *
 * EtaUtilsStateRestoreHelper - static helper restores from supplied stack.
 *
 * @return does not return.
 *
 ******************************************************************************/
static void
EtaUtilsStateRestoreHelper(uint32_t *pui32SpSaveAddress)
{
}

/***************************************************************************//**
 *
 * EtaUtilsStateNewStack - TODO
 *
 * @param pui64NewStackTop TODO
 * @param pfnAppMain Function call aftter swapping stacks.
 *
 ******************************************************************************/
void
EtaUtilsStateNewStack(tpfEtaUtilsStateAppMain pfnAppMain,
                      uint32_t *pui32NewStackTop)
{
}

/***************************************************************************//**
 *
 * Conditionally restore the processor state based on cold/warm start.
 *     COLD: Execution returns from this function for cold start.
 *     WARM: Execution Resumes where EtaUtilsStateSave would have returned.
 *
 * @param pui32SpSaveAddress Address of storage location for other stack ptr.
 *                           points to either RTC_NVRAMA,B,C,D or global sram.
 *
 ******************************************************************************/
void
EtaUtilsStateRestore(uint32_t *pui32SpSaveAddress)
{
    //
    // NOTE: THIS FUNCTION NEVER RETURNS.
    //
    EtaUtilsStateRestoreHelper(pui32SpSaveAddress);
}

/***************************************************************************//**
 *
 * EtaUtilsStateSavePwrDown - Save the processor state on the current stack,
 *                            save the SP and power down.
 *
 * @param pui32SpSaveAddress Address of storage location for other stack ptr.
 *                           points to either RTC_NVRAMA,B,C,D or global sram.
 *
 ******************************************************************************/
void
EtaUtilsStateSavePwrDown(uint32_t *pui32SpSaveAddress)
{
}

//
// IAR.
//
#elif defined(__ICCARM__)

/***************************************************************************//**
 *
 * EtaUtilsStateRestoreHelper - static helper restores from supplied stack.
 *
 * @return does not return.
 *
 ******************************************************************************/
static void
EtaUtilsStateRestoreHelper(uint32_t *pui32SpSaveAddress)
{
}

/***************************************************************************//**
 *
 * EtaUtilsStateNewStack - TODO
 *
 * @param pui64NewStackTop TODO
 * @param pfnAppMain Function call aftter swapping stacks.
 *
 ******************************************************************************/
void
EtaUtilsStateNewStack(tpfEtaUtilsStateAppMain pfnAppMain,
                      uint32_t *pui32NewStackTop)
{
}

/***************************************************************************//**
 *
 * Conditionally restore the processor state based on cold/warm start.
 *     COLD: Execution returns from this function for cold start.
 *     WARM: Execution Resumes where EtaUtilsStateSave would have returned.
 *
 * @param pui32SpSaveAddress Address of storage location for other stack ptr.
 *                           points to either RTC_NVRAMA,B,C,D or global sram.
 *
 ******************************************************************************/
void
EtaUtilsStateRestore(uint32_t *pui32SpSaveAddress)
{
    //
    // NOTE: THIS FUNCTION NEVER RETURNS.
    //
    EtaUtilsStateRestoreHelper(pui32SpSaveAddress);
}

/***************************************************************************//**
 *
 * EtaUtilsStateSavePwrDown - Save the processor state on the current stack,
 *                            save the SP and power down.
 *
 * @param pui32SpSaveAddress Address of storage location for other stack ptr.
 *                           points to either RTC_NVRAMA,B,C,D or global sram.
 *
 ******************************************************************************/
void
EtaUtilsStateSavePwrDown(uint32_t *pui32SpSaveAddress)
{
}

//
// GCC
//
#elif defined(__GNUC__)

/***************************************************************************//**
 *
 * EtaUtilsStateRestoreHelper - static helper restores from supplied stack.
 *
 * @return does not return.
 *
 ******************************************************************************/
static void
EtaUtilsStateRestoreHelper(uint32_t *pui32SpSaveAddress)
{
    //
    // Restore state and go execute there. This function will not return.
    // R0 == ppui32OldSpAddress.
    //
    __asm("ldr  sp, [r0]\n"
          "pop  {r2, r3}\n"
          "msr  control, r2\n"
          "pop  {r4-r11}\n"
          "bx   r3");
}

/***************************************************************************//**
 *
 * EtaUtilsStateNewStack - TODO
 *
 * @param pui32NewStackTop Pointer to the new stack top.
 * @param pfnAppMain Function call aftter swapping stacks.
 *
 ******************************************************************************/
void
EtaUtilsStateNewStack(tpfEtaUtilsStateAppMain pfnAppMain,
                      uint32_t *pui32NewStackTop)
{
    //
    // R1 == ppui32OldSpAddress
    //
    __asm("mov  sp, r1\n");

    //
    // Call the application main.
    //
    pfnAppMain();
}

/***************************************************************************//**
 *
 * Conditionally restore the processor state based on cold/warm start.
 *     COLD: Execution returns from this function for cold start.
 *     WARM: Execution Resumes where EtaUtilsStateSave would have returned.
 *
 * @param pui32SpSaveAddress Address of storage location for other stack ptr.
 *                           points to either RTC_NVRAMA,B,C,D or global sram.
 *
 ******************************************************************************/
void
EtaUtilsStateRestore(uint32_t *pui32SpSaveAddress)
{
    //
    // NOTE: This function does not return.
    //
    EtaUtilsStateRestoreHelper(pui32SpSaveAddress);
}

/***************************************************************************//**
 *
 * EtaUtilsStateSavePwrDown - Save the processor state on the current stack,
 *                            save the SP and power down.
 *
 * @param pui32SpSaveAddress Address of storage location for other stack ptr.
 *                           points to either RTC_NVRAMA,B,C,D or global sram.
 *
 ******************************************************************************/
void
EtaUtilsStateSavePwrDown(uint32_t *pui32SpSaveAddress)
{
    __asm __volatile__ ("push {r4-r11}\n"
                        "mrs  r2, control\n"
                        "push {r2, lr}\n"
                        "str  sp, [r0]\n"); // r0 == pui32SpSaveAddress

    //
    // Power down without saving context.
    //
    EtaCspPwrSocShutdown();
}

#else
#error "NO IDE SELECTED"

#endif
#else
#error "NOT M3 or M4"
#endif

/** @}*/

