/*******************************************************************************
 *
 * @file system_ecm3531.c
 *
 * @brief System startup functions
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
 ******************************************************************************/
#include "config.h"
#include "print_util.h"
#include "eta_csp.h"
#ifdef CONFIG_ECM3531
#include "ecm3531.h"
#include "ecm3531_pinconfig.h"
#elif CONFIG_ECM3532
#include "ecm3532.h"
#include "reg_eta_ecm3532_m3.h"
#endif
#include "eta_csp_buck.h"
#include "eta_csp_dsp.h"
#include "eta_csp_socctrl.h"
#include "eta_csp_hfo.h"
#include "uart_hal.h"
#include "timer_hal.h"
#include "i2c_hal.h"
#include "spi_hal.h"
#include "rtc_hal.h"
#include "pm.h"
#include "board.h"
#include "cm3.h"
#ifdef CONFIG_RPC
#include "rpc.h"
#include "dsp_helper.h"
#endif
#include <stdint.h>
#include "incbin.h"
int32_t HalTmrChInit(tHalTmrCh iTmrChan);

#if defined(CONFIG_RPC)
INCBIN_ETA_DSP();
#endif

static volatile uint32_t r0;
static volatile uint32_t r1;
static volatile uint32_t r2;
static volatile uint32_t r3;
static volatile uint32_t r12;
static volatile uint32_t lr;
static volatile uint32_t pc;
static volatile uint32_t psr;

#ifdef CONFIG_DEBUG_UART
void printReleaseInfo(void)
{
    ecm35xx_printf("\r\n=======================\r\n");
    ecm35xx_printf(" Release %s\r\n", CONFIG_SW_VERSION);
    ecm35xx_printf(" Runnning: %s\r\n", CONFIG_APP_NAME);
    ecm35xx_printf("=======================\r\n");
}
#endif

/******************************************************************************
 *
 *@brief Initialize the system.
 *
 *This function sets up the microcontroller system.
 *
 * @return None.
 *
 ****************************************************************************/
int32_t HalSpiSlaveInit(uint8_t ui8SpiNum);

static void prvSetupHardware( void )
{
#ifdef CONFIG_I2C_0
    HalI2cInit(0);
#endif
#ifdef CONFIG_I2C_1
    HalI2cInit(1);
#endif

#ifdef CONFIG_I2C_2
    HalI2cInit(1);
#endif

#ifdef CONFIG_UART_0
   /* Enable the UART 0 */
    HalUartInit(0);
#endif
#ifdef CONFIG_UART_1
    /* Enable the UART 0 */
    HalUartInit(1);
#endif
    /* Enable the HAL Timer 0 */
    HalTmrChInit(0);
#ifdef CONFIG_SPI_0_MASTER
    HalSpiInit(0);
#endif
#ifdef CONFIG_SPI_1_MASTER
    HalSpiInit(1);
#endif
#ifdef CONFIG_HAL_COMMON_RTC
    RtcInit();
#endif
#ifdef CONFIG_SPI_0_SLAVE
    HalSpiSlaveInit(0);
#endif
}
void
SystemInit(uint32_t ui32rVor)
{
    tBuckMemTarget iMemTarget;
    tEtaStatus cspSt;
    tBuckAoTarget iAoTarget;

    SCB->VTOR = (uint32_t)ui32rVor;
    EtaCspInit();
    EtaCspGpioInit();
    EtaCspGpioRelease();
    EtaCspBuckInit();
    if (CONFIG_M3_INIT_FREQ > 60)
    {
        iMemTarget = eBuckMem1100Mv;
        iAoTarget = eBuckAo700Mv;
    }
    else if (CONFIG_M3_INIT_FREQ >= 40)
    {
        iMemTarget = eBuckMem900Mv;
        iAoTarget = eBuckAo700Mv;
    }
    else
    {
        iMemTarget = eBuckMem900Mv;
        iAoTarget = eBuckAo600Mv;
    }

#ifdef CONFIG_RPC
    EtaCspDspPowerUp();
    /*DSP and M3 at same freq*/
    cspSt = EtaCspBuckAllSet(iAoTarget,
                eBuckM3FrequencyMode, (CONFIG_M3_INIT_FREQ * 1000),
                eBuckDspFrequencyMode,  (CONFIG_DSP_INIT_FREQ * 1000),
                iMemTarget, true, true);
#else
    cspSt = EtaCspBuckAllSet(iAoTarget,
                eBuckM3FrequencyMode, (CONFIG_M3_INIT_FREQ * 1000),
                eBuckDspOff, 0, iMemTarget, true, true);
#endif
    if (cspSt)
        ecm35xx_printf("Err in ret value EtaCspBuckSetAll\r\n");
    REG_W1(RTC_SPI0_CTRL, SPI0_MISO_RE, 1);
    EtaCspHfoTuneDefaultUpdate();

    board_init();
    prvSetupHardware();
#ifdef CONFIG_PM_ENABLE
    ecm35xx_pwrg_init(CONFIG_PM_TIMER_CH_NO);
#endif
#ifdef CONFIG_ENABLE_FRONT_PANEL
    /* Turn on M3 and DSP Frequency Counters */
    REG_SOCCTRL_FPANEL.V |= (BFV_SOCCTRL_FPANEL_DSP_OSC_ENABLE |
                             BFV_SOCCTRL_FPANEL_M3_OSC_ENABLE);

#endif

#ifdef CONFIG_DEBUG_UART
    printReleaseInfo();
#endif
#ifdef CONFIG_RPC
    rpcInit();
    LoadDsp();
    DspdbgInit();
#endif

}

__attribute__((section(".initSection"))) void prvGetRegistersFromStack(uint32_t *pulFaultStackAddress)
{
    r0 = pulFaultStackAddress[ 0 ];
    r1 = pulFaultStackAddress[ 1 ];
    r2 = pulFaultStackAddress[ 2 ];
    r3 = pulFaultStackAddress[ 3 ];

    r12 = pulFaultStackAddress[ 4 ];
    lr = pulFaultStackAddress[ 5 ];
    pc = pulFaultStackAddress[ 6 ];
    psr = pulFaultStackAddress[ 7 ];

    while(1);
}

void HardFault_Handler(void)
{
    __asm volatile
    (
        " tst lr, #4                                                \n"
        " ite eq                                                    \n"
        " mrseq r0, msp                                             \n"
        " mrsne r0, psp                                             \n"
        " ldr r1, [r0, #24]                                         \n"
        " ldr r2, handler2_address_const                            \n"
        " bx r2                                                     \n"
        " handler2_address_const: .word prvGetRegistersFromStack    \n"
    );
}

#if defined (CONFIG_FLASH_RELOCATE) ||  defined (CONFIG_FLASH_SHM_RELOCATE)
__attribute__((section(".ram_vectors")))
uint8_t IRQRamVectors[512];

/* relocate vector table to RAM */
void RelocateVectorsToRam(void)
{
    __asm volatile ( "cpsid i" );
    SCB->VTOR = (uint32_t) &IRQRamVectors;
    __DSB();
    __asm volatile ( "cpsie i" );
}
#endif
