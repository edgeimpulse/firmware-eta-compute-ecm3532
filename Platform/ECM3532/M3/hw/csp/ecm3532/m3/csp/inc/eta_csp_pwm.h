/***************************************************************************//**
 *
 * @file eta_csp_pwm.h
 *
 * @brief This file contains eta_csp_pwm module definitions.
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
 * This is part of revision 1.0.0b1 of the Tensai Chip Support Package.
 *
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup ecm3532pwm-m3 Pulse-width Modulation (PWM)
 * @ingroup ecm3532csp-m3
 * @{
 ******************************************************************************/

#ifndef __ETA_CSP_PWM_H__
#define __ETA_CSP_PWM_H__

#include <stdint.h>
#include <stdbool.h>
#include "eta_csp.h"
#include "eta_status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PWM_MAX_COUNT ((1 << 24) - 1)

//
//! PWM module number.
//
typedef enum
{
    //
    //! PWM modules 0-3.
    //
    ePwmModule0 = 0,
    ePwmModule1 = 1,
    ePwmModule2 = 2,
    ePwmModule3 = 3,
}
tPwmModule;

//
//! PWM reg config typedef.
//
typedef enum
{
    //
    //! Reg values.
    //
    ePwmRegCfg   = 0,
    ePwmRegPer   = 1,
    ePwmRegPstrt = 2,
    ePwmRegPend  = 3,
}
tPwmReg;

//
//! PWM clock source typedef.
//
typedef enum
{
    //
    //! Source the PWM from 32kHz.
    //
    ePwmClkSrc32KHz = 0,

    //
    //! Source the PWM from 2MHz.
    //
    ePwmClkSrc2MHz = 1,
}
tPwmClkSrc;

//
//! PWM mode typedef.
//
typedef enum
{
    //
    //! Do not set mode.
    //
    ePwmModeNone = -1,

    //
    //! PWM stop immediately.
    //
    ePwmModeStopImmediate = 0,

    //
    //! PWM stop after cycle.
    //
    ePwmModeStopAfterCycle = 1,

    //
    //! PWM stop after program.
    //
    ePwmModeStopAfterProgram = 2,

    //
    //! PWM start.
    //
    ePwmModeStart = 3,
}
tPwmMode;

//
//! PWM channel typedef.
//
typedef enum
{
    //
    //! PWM channel mux select 0-7.
    //
    ePwmChanMuxSel0 = 0,
    ePwmChanMuxSel1 = 1,
    ePwmChanMuxSel2 = 2,
    ePwmChanMuxSel3 = 3,
    ePwmChanMuxSel4 = 4,
    ePwmChanMuxSel5 = 5,
    ePwmChanMuxSel6 = 6,
    ePwmChanMuxSel7 = 7,
}
tPwmChanMux;

//
//! The PWM interrupt union typedef.
//
typedef union
{
    //
    //! The value.
    //
    uint32_t ui32Value;

    //
    //! The PWM interrupt status bits structure.
    //
    struct tPwmIntStatBits
    {
        //
        //! PWM module 0-3 program interrupt status.
        //
        bool iProgInt0   : 1;
        bool iProgInt1   : 1;
        bool iProgInt2   : 1;
        bool iProgInt3   : 1;

        //
        //! PWM module 0-3 cycle interrupt status.
        //
        bool iCycleInt0  : 1;
        bool iCycleInt1  : 1;
        bool iCycleInt2  : 1;
        bool iCycleInt3  : 1;

        //
        //! FIFO interrupt status.
        //
        bool iFifoInt    : 1;

        //
        //! Reserved
        //
        uint32_t ui32RSVD : 23;
    }
    sPwmIntStatBits;
}
tPwmIntStatReg;

//
//! The PWM config union typedef.
//
typedef union
{
    //
    //! The value.
    //
    uint32_t ui32Value;

    //
    //! The PWM interrupt status bits structure.
    //
    struct tPwmCfgBits
    {
        //
        //! PWM cycles for program.
        //
        uint32_t ui32Cycles : 16;

        //
        //! Stop after ui32Count cycles.
        //
        bool iOneshot      : 1;

        //
        //! Pulse polarity (1 = HIGH, 0 = LOW).
        //
        bool iPulseDrive   : 1;

        //
        //! Program and cycle interrupt enables.
        //
        bool iProgInt      : 1;
        bool iCycleInt     : 1;

        //
        //! Reserved
        //
        uint32_t ui32RSVD   : 12;
    }
    sPwmCfgBits;
}
tPwmCfgReg;


//
// Perform a soft reset.
//
extern void EtaCspPwmSoftReset(void);

//
// Initialize the PWM.
//
extern tEtaStatus EtaCspPwmInit(tPwmClkSrc iClkSrc, uint32_t ui32FifoLow);

//
// Initialize a PWM module.
//
extern tEtaStatus EtaCspPwmModuleInit(tPwmModule iModule, uint32_t ui32Period,
                                      uint32_t ui32Width, uint32_t ui32Shift,
                                      bool iIdleDrive);

//
// Set the PWM duty cycle.
//
extern tEtaStatus EtaCspPwmDutyCycleSet(tPwmModule iModule,
                                        uint32_t ui32Period,
                                        uint32_t ui32Width,
                                        uint32_t ui32Shift,
                                        tPwmCfgReg sPwmCfgReg,
                                        bool iWait);

//
// Get the on count for the desired PWM module.
//
extern uint32_t EtaCspPwmOnCountGet(tPwmModule iModule);

//
// Write a Fifo command.
//
extern tEtaStatus EtaCspPwmCmdFifoWrite(tPwmModule iModule, tPwmReg iReg,
                                        bool iWait, uint32_t ui32Data);

//
// Set the PWM configure register.
//
extern tEtaStatus EtaCspPwmCfgRegSet(tPwmModule iModule, tPwmCfgReg sPwmCfgReg,
                                     bool iWait);

//
// Get the PWM configure register value.
//
extern tPwmCfgReg EtaCspPwmCfgRegGet(tPwmModule iModule);

//
// Set the period register.
//
extern tEtaStatus EtaCspPwmPeriodRegSet(tPwmModule iModule,
                                        uint32_t ui32PeriodCnt, bool iWait);

//
// Get the period register value.
//
extern uint32_t EtaCspPwmPeriodRegGet(tPwmModule iModule);

//
// Set the pulse start register value.
//
extern tEtaStatus EtaCspPwmPulseStartRegSet(tPwmModule iModule,
                                            uint32_t ui32PulseStartCnt,
                                            bool iWait);

//
// Get the pulse start value.
//
extern uint32_t EtaCspPwmPulseStartRegGet(tPwmModule iModule);

//
// Set the pulse end register count.
//
extern tEtaStatus EtaCspPwmPulseEndRegSet(tPwmModule iModule,
                                          uint32_t ui32PulseEndCnt, bool iWait);

//
// Get the pulse end period value.
//
extern uint32_t EtaCspPwmPulseEndRegGet(tPwmModule iModule);

//
// Set a PWM mode.
//
extern void EtaCspPwmModeSet(tPwmModule iModule, tPwmMode iMode);

//
// Get the current running state of a PWM.
//
extern tPwmMode EtaCspPwmRunStateGet(tPwmModule iModule);

//
// Set all PWM modes.
//
extern void EtaCspPwmAllModeSet(tPwmMode iMode0, tPwmMode iMode1,
                                tPwmMode iMode2, tPwmMode iMode3);

//
// Start a PWM.
//
extern void EtaCspPwmStart(tPwmModule iModule);

//
// Stop the PWM immediately.
//
extern void EtaCspPwmStopImmediate(tPwmModule iModule);

//
// Stop the PWM after the cycle.
//
extern void EtaCspPwmStopAfterCycle(tPwmModule iModule);

//
// Stop the PWM after program.
//
extern void EtaCspPwmStopAfterProgram(tPwmModule iModule);

//
// Start multiple PWMs simultaneously.
//
extern void EtaCspPwmMultiStart(bool iPwm0, bool iPwm1,
                                bool iPwm2, bool iPwm3);

//
// Stop multiple PWMs simultaneously.
//
extern void EtaCspPwmMultiStopImmediate(bool iPwm0, bool iPwm1,
                                        bool iPwm2, bool iPwm3);

//
// Set the PWM idle drive.
//
extern void EtaCspPwmIdleDriveSet(tPwmModule iModule, bool iIdleDrive);

//
// Get the current output state of a PWM.
//
extern bool EtaCspPwmOutputStateGet(tPwmModule iModule);

//
// Configure the mux channel.
//
extern void EtaCspPwmCfgMuxChanSet(tPwmChanMux iChan, bool iSync,
                                   bool iDfltDrive);

//
// Enable the mux channel.
//
extern void EtaCspPwmMuxChanEnable(tPwmModule iModule, tPwmChanMux iChan);

//
// Disable the mux channel.
//
extern void EtaCspPwmMuxChanDisable(tPwmModule iModule, tPwmChanMux iChan);

//
// Checks if the FIFO is full.
//
extern bool EtaCspPwmIsFifoFull(void);

//
// Checks if the FIFO is empty.
//
extern bool EtaCspPwmIsFifoEmpty(void);

//
// Get the FIFO usage.
//
extern uint32_t EtaCspPwmFifoUsageGet(void);

//
// Enable the PWM low interrupt.
//
extern tEtaStatus EtaCspPwmFifoLowIntEnable(uint32_t ui32FifoLowLevel);

//
// Disable the PWM low interrupt.
//
extern void EtaCspPwmFifoLowIntDisable(void);

//
// Get interrupt status.
//
extern tPwmIntStatReg EtaCspPwmIntGet(void);

#ifdef __cplusplus
}
#endif

#endif // __ETA_CSP_PWM_H__

