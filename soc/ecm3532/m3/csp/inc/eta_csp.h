/***************************************************************************//**
 *
 * @file eta_csp.h
 *
 * @brief This file contains eta_csp module definitions.
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
 * @addtogroup ecm3532
 * @addtogroup ecm3532csp-m3
 *
 * @defgroup ecm3532 ECM3532 Documentation
 * @defgroup ecm3532csp-m3 M3 CSP (Chip Support Package)
 *
 * @ingroup ecm3532
 * @ingroup ecm3532csp-m3
 * @{
 ******************************************************************************/

#ifndef __ETA_CSP_H__
#define __ETA_CSP_H__

#include <stdint.h>
#include <stdbool.h>
#include "eta_status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PRINTF    printf
#define etaPrintf EtaCspIoPrintf
#define etaPutc   EtaCspIoPutc
#define etaPuts   EtaCspIoPuts


#define ETA_CSP_SRAM_BASE          (0x10000000)
#define ETA_CSP_SRAM_SIZE          (0x00040000)
#define ETA_CSP_SRAM_MAGIC0        (0x1003FFF8)
#define ETA_CSP_SRAM_MAGIC1        (0x1003FFFC)
#define ETA_CSP_SRAM_MAGIC_DEFAULT (0xC369A517)
#define ETA_CSP_SRAM_MAGIC_TUNE    (0x7C369A51)
#define ETA_CSP_SRAM_MAGIC_TRIM    (0xC6A13957)

#define ETA_CSP_TIMER_CLK_HZ      32000ULL
#define ETA_CSP_TIMER_MS(ui32Cnt) (ui32Cnt / 1ULL)

//
// Use UART0 for printf output.
//
#define ETA_CSP_UART_NUM_PRINTF (eUartNum0)

#define DEBUG_FUNCTION_ENTER() ETA_DIAGS_PRINTF("ENTER: %s\n", __FUNCTION__)
#define DEBUG_FUNCTION_EXIT()  ETA_DIAGS_PRINTF("EXIT: %s\n", __FUNCTION__)

#ifndef __LANGUAGE_ASM__

#ifdef NOTDEF
fixme remove
//
//! Vector table union.
//
typedef union
{
    //
    //! Value.
    //
    uint32_t ui32Value;

    //
    //! Register interupt structure.
    //
    struct tRegIntBits
    {
        uint32_t ui32GPIO0               : 1;
        uint32_t ui32GPIO1               : 1;
        uint32_t ui32TIMER0              : 1;
        uint32_t ui32TIMER1              : 1;
        uint32_t ui32UART0               : 1;
        uint32_t ui32UART1               : 1;
        uint32_t ui32SPI0                : 1;
        uint32_t ui32SPI1                : 1;
        uint32_t ui32I2C0                : 1;
        uint32_t ui32I2C1                : 1;
        uint32_t ui32RTC0                : 1;
        uint32_t ui32RTC1                : 1;
        uint32_t ui32DSP                 : 1;
        uint32_t ui32ADC                 : 1;
        uint32_t ui32PWM                 : 1;
        uint32_t ui32WDT                 : 1;
        uint32_t ui32RTC_TMR             : 1;
        uint32_t ui32SW0                 : 1;
        uint32_t ui32SW1                 : 1;
        uint32_t ui32RSVD0               : 13;
    }
    sRegIntBits;
}
tVTable;
#endif

//
// USE THIS BREAKPOINT OR SUPPLY YOUR OWN.
//
#ifndef ETA_CSP_BREAKPOINT
//
// Keil.
//
#ifdef __CC_ARM
#define ETA_CSP_BREAKPOINT __breakpoint(0);

//
// IAR.
//
#elif __ICCARM__
#define ETA_CSP_BREAKPOINT asm volatile("BKPT #0");

//
// GCC.
//
#else
#define ETA_CSP_BREAKPOINT __asm__ __volatile__ ("BKPT #0");
#endif
#endif

//
// General purpose CSP ASSERT (use this one or supply your own)
//
#ifndef ETA_CSP_ASSERT
#define ETA_CSP_ASSERT(VALUE,MSG) \
     if(VALUE) { etaPrintf("ETA_CSP_ASSERT: ERROR %s VALUE 0x%x Line %d in %s\r\n",\
                            MSG,VALUE,__LINE__,__FILE__); \
                ETA_CSP_BREAKPOINT;}
#endif


//
// CSP init function.
//
extern tEtaStatus EtaCspInit(void);
extern void EtaCspFrontPanelEnable(bool);

#endif // __LANGUAGE_ASM__

#ifdef __cplusplus
}
#endif


#endif // __ETA_CSP_H__

/** @}*/

