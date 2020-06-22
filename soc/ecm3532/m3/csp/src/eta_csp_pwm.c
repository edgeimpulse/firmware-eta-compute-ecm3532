/***************************************************************************//**
 *
 * @file eta_csp_pwm.c
 *
 * @brief This file contains eta_csp_pwm module implementations.
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
 * @addtogroup ecm3532pwm-m3 Pulse-width Modulation (PWM)
 * @ingroup ecm3532csp-m3
 * @{
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "eta_status.h"
#include "eta_chip.h"
#include "eta_csp.h"
#include "eta_csp_pwm.h"

/***************************************************************************//**
 *
 *  EtaCspPwmSoftReset - Perform a soft reset.
 *
 ******************************************************************************/
void
EtaCspPwmSoftReset(void)
{
    REG_S1(PWM_CTRL, SFTRST);
    REG_C1(PWM_CTRL, CLK_EN_OVR);
}

/***************************************************************************//**
 *
 *  EtaCspPwmInit - Initialize the PWM.
 *
 *  @param iClkSrc the desired clock source for the PWM.
 *  @param ui32FifoLow the Fifo low value.
 *
 ******************************************************************************/
tEtaStatus
EtaCspPwmInit(tPwmClkSrc iClkSrc, uint32_t ui32FifoLow)
{
    tEtaStatus iStatus = eEtaSuccess;

    if (ui32FifoLow > 15)
    {
        iStatus = eEtaParameterOutOfRange;
        ui32FifoLow = 15;
    }

    EtaCspPwmSoftReset();
    REG_M2(PWM_CTRL, CLKSRC, iClkSrc, CFIFO_LOW, ui32FifoLow);

    return(iStatus);
}

/***************************************************************************//**
 *
 *  EtaCspPwmModuleInit - Initialize a PWM module.
 *
 *  @param iModule PWM module.
 *  @param ui32Period desired period.
 *  @param ui32Duty desired duty cycle.
 *  @param iIdleDrive idle drive bit.
 *
 ******************************************************************************/
tEtaStatus
EtaCspPwmModuleInit(tPwmModule iModule, uint32_t ui32Period,
                    uint32_t ui32Width, uint32_t ui32Shift,
                    bit_t iIdleDrive)
{
    tEtaStatus iStatus = eEtaSuccess;

    iStatus = EtaCspPwmDutyCycleSet(iModule, ui32Period, ui32Width, ui32Shift,
                                    EtaCspPwmCfgRegGet(iModule), FALSE);

    if (iStatus) {
        return(iStatus);
    }
    else
    {
        EtaCspPwmIdleDriveSet(iModule, iIdleDrive);
        EtaCspPwmStart(iModule);
    }

    return(eEtaSuccess);
}

/***************************************************************************//**
 *
 *  EtaCspPwmDutyCycleSet - Set the PWM duty cycle.
 *
 *  @param iModule PWM module.
 *  @param ui32Period the period of the PWM in clock cycles.
 *  @param ui32Width is the width of the pulse in clock cycles.
 *  @param ui32Shift is the positive shift of the pulse in clock cycles.
 *  @param sPwmCfgReg PWM configuration structure.
 *  @param iWait wait for previous program to finish.
 *  @return Return the CSP status.
 *
 ******************************************************************************/
tEtaStatus
EtaCspPwmDutyCycleSet(tPwmModule iModule, uint32_t ui32Period,
                      uint32_t ui32Width, uint32_t ui32Shift,
                      tPwmCfgReg sPwmCfgReg, bit_t iWait)
{
    tEtaStatus iStatus = eEtaSuccess;

    uint32_t ui32End = ui32Shift + ui32Width;

    if( (ui32End > ui32Period) || (ui32Period == 0) ||
            (ui32Period > (PWM_MAX_COUNT+1)) )
    {
        iStatus = eEtaParameterOutOfRange;
    }
    else
    {
        EtaCspPwmCfgRegSet(iModule, sPwmCfgReg, iWait);
        EtaCspPwmPeriodRegSet(iModule, ui32Period, FALSE);
        EtaCspPwmPulseStartRegSet(iModule, ui32Shift, FALSE);
        EtaCspPwmPulseEndRegSet(iModule, ui32End, FALSE);
    }
    return(iStatus);
}

/***************************************************************************//**
 *
 *  EtaCspPwmOnCountGet - Get the on count for the desired PWM module.
 *
 *  @param iModule PWM module.
 *  @return Return the on count for the desired PWM module.
 *
 ******************************************************************************/
uint32_t
EtaCspPwmOnCountGet(tPwmModule iModule)
{
    uint32_t ui32OnCount, ui32Pend, ui32Pstrt;

    switch(iModule)
    {
        case ePwmModule0:
        {
            ui32Pend = REG_R(PWM_PEND0, PEND);
            ui32Pstrt = REG_R(PWM_PSTRT0, PSTRT);
            ui32OnCount = ui32Pend - ui32Pstrt;
            break;
        }

        case ePwmModule1:
        {
            ui32Pend = REG_R(PWM_PEND1, PEND);
            ui32Pstrt = REG_R(PWM_PSTRT1, PSTRT);
            ui32OnCount = ui32Pend - ui32Pstrt;
            break;
        }

        case ePwmModule2:
        {
            ui32Pend = REG_R(PWM_PEND2, PEND);
            ui32Pstrt = REG_R(PWM_PSTRT2, PSTRT);
            ui32OnCount = ui32Pend - ui32Pstrt;
            break;
        }

        case ePwmModule3:
        {
            ui32Pend = REG_R(PWM_PEND3, PEND);
            ui32Pstrt = REG_R(PWM_PSTRT3, PSTRT);
            ui32OnCount = ui32Pend - ui32Pstrt;
            break;
        }

        default:
        {
            // DB added -Wmaybe-uninitialized
            ui32Pend = REG_R(PWM_PEND0, PEND);
            ui32Pstrt = REG_R(PWM_PSTRT0, PSTRT);
            ui32OnCount = ui32Pend - ui32Pstrt;
            break;
        }
    }
    return(ui32OnCount);
}

/***************************************************************************//**
 *
 *  EtaCspPwmCmdFifoWrite - Write a Fifo command.
 *
 *  @param iModule PWM module.
 *  @param iReg PWM reg.
 *  @param iWait wait bit.
 *  @param ui32Data the data.
 *
 ******************************************************************************/
tEtaStatus
EtaCspPwmCmdFifoWrite(tPwmModule iModule, tPwmReg iReg, bit_t iWait,
                      uint32_t ui32Data)
{
    tEtaStatus iStatus = eEtaSuccess;

    switch(iReg)
    {
        case ePwmRegCfg:
        {
            ui32Data &= 0xFFFFF;
            break;
        }

        case ePwmRegPer:
        case ePwmRegPstrt:
        case ePwmRegPend:
        {
            if (ui32Data > PWM_MAX_COUNT) {
                ui32Data &= PWM_MAX_COUNT;
                iStatus = eEtaParameterOutOfRange;
            }
            break;
        }
    }

    REG_W4(PWM_CFIFO, SEL, iModule, REG, iReg, WAIT, iWait, DATA, ui32Data);

    return(iStatus);
}

/***************************************************************************//**
 *
 *  EtaCspPwmCfgRegSet - Set the PWM configure register.
 *
 *  @param iModule PWM module.
 *  @param sPwmCfgRed PWM configuration struct.
 *  @param iWait wait bit.
 *
 ******************************************************************************/
tEtaStatus
EtaCspPwmCfgRegSet(tPwmModule iModule, tPwmCfgReg sPwmCfgReg, bit_t iWait)
{
    return( EtaCspPwmCmdFifoWrite(iModule, ePwmRegCfg, iWait,
            sPwmCfgReg.ui32Value) );
}

/***************************************************************************//**
 *
 *  EtaCspPwmCfgRegGet - Get the PWM configure register value.
 *
 *  @param iModule PWM module.
 *  @return Return the config register structure.
 *
 ******************************************************************************/
tPwmCfgReg
EtaCspPwmCfgRegGet(tPwmModule iModule)
{
    tPwmCfgReg sCfgReg;

    switch(iModule)
    {
        case ePwmModule0:
        {
            sCfgReg.ui32Value = REG_PWM_CFG0.V;
            break;
        }

        case ePwmModule1:
        {
            sCfgReg.ui32Value = REG_PWM_CFG1.V;
            break;
        }

        case ePwmModule2:
        {
            sCfgReg.ui32Value = REG_PWM_CFG2.V;
            break;
        }

        case ePwmModule3:
        {
            sCfgReg.ui32Value = REG_PWM_CFG3.V;
            break;
        }

        default:
        {
            sCfgReg.ui32Value = REG_PWM_CFG0.V; // DB added -Wmaybe-uninitialized
            break;
        }
    }
    return(sCfgReg);
}

/***************************************************************************//**
 *
 *  EtaCspPwmPeriodRegSet - Set the period register.
 *
 *  @param iModule PWM module.
 *  @param ui32PeriodCnt the period count.
 *  @param iWait wait bit.
 *
 ******************************************************************************/
tEtaStatus
EtaCspPwmPeriodRegSet(tPwmModule iModule, uint32_t ui32PeriodCnt,
                      bit_t iWait)
{
    ui32PeriodCnt--;

    if (ui32PeriodCnt > PWM_MAX_COUNT)
    {
        return(eEtaParameterOutOfRange);
    }

    reg32_t r32RegValue = FV1(PWM_PER0, PER, ui32PeriodCnt);

    return( EtaCspPwmCmdFifoWrite(iModule, ePwmRegPer, iWait, r32RegValue) );
}

/***************************************************************************//**
 *
 *  EtaCspPwmPeriodRegGet - Get the period register value.
 *
 *  @param iModule PWM module.
 *  @return Return the period register value.
 *
 ******************************************************************************/
uint32_t
EtaCspPwmPeriodRegGet(tPwmModule iModule)
{
    uint32_t ui32PeriodCnt = 0x0BAD0BAD; // DB added -Wmaybe-uninitialized

    switch(iModule)
    {
        case ePwmModule0:
        {
            ui32PeriodCnt = REG_R(PWM_PER0, PER);
            break;
        }

        case ePwmModule1:
        {
            ui32PeriodCnt = REG_R(PWM_PER1, PER);
            break;
        }

        case ePwmModule2:
        {
            ui32PeriodCnt = REG_R(PWM_PER2, PER);
            break;
        }

        case ePwmModule3:
        {
            ui32PeriodCnt = REG_R(PWM_PER3, PER);
            break;
        }

        default:
        {
            break;
        }
    }
    return(++ui32PeriodCnt);
}

/***************************************************************************//**
 *
 *  EtaCspPwmPulseStartRegSet - Set the pulse start register value.
 *
 *  @param iModule PWM module.
 *  @param ui32PulseStartCnt pulse start count value.
 *  @param iWait wait bit.
 *
 ******************************************************************************/
tEtaStatus
EtaCspPwmPulseStartRegSet(tPwmModule iModule,
                          uint32_t ui32PulseStartCnt, bit_t iWait)
{
    reg32_t r32RegValue = FV1(PWM_PSTRT0, PSTRT, ui32PulseStartCnt);

    return( EtaCspPwmCmdFifoWrite(iModule, ePwmRegPstrt, iWait, r32RegValue) );
}

/***************************************************************************//**
 *
 *  EtaCspPwmPulseStartRegGet - Get the pulse start value.
 *
 *  @param iModule PWM module.
 *  @return Return the pulse start value.
 *
 ******************************************************************************/
uint32_t
EtaCspPwmPulseStartRegGet(tPwmModule iModule)
{
    uint32_t ui32PulseStart = 0x0BAD0BAD; // DB added -Wmaybe-uninitialized

    switch(iModule)
    {
        case ePwmModule0:
        {
            ui32PulseStart = REG_R(PWM_PSTRT0, PSTRT);
            break;
        }

        case ePwmModule1:
        {
            ui32PulseStart = REG_R(PWM_PSTRT1, PSTRT);
            break;
        }

        case ePwmModule2:
        {
            ui32PulseStart = REG_R(PWM_PSTRT2, PSTRT);
            break;
        }

        case ePwmModule3:
        {
            ui32PulseStart = REG_R(PWM_PSTRT3, PSTRT);
            break;
        }

        default:
        {
            break;
        }
    }
    return(ui32PulseStart);
}

/***************************************************************************//**
 *
 *  EtaCspPwmPulseEndRegSet - Set the pulse end register count.
 *
 *  @param iModule PWM module
 *  @param ui32PulseEndCnt the pulse end count for the PWM.
 *  @param iWait wait bit.
 *
 ******************************************************************************/
tEtaStatus
EtaCspPwmPulseEndRegSet(tPwmModule iModule, uint32_t ui32PulseEndCnt,
                        bit_t iWait)
{
    reg32_t r32RegValue = FV1(PWM_PEND0, PEND, ui32PulseEndCnt);

    return( EtaCspPwmCmdFifoWrite(iModule, ePwmRegPend, iWait, r32RegValue) );
}

/***************************************************************************//**
 *
 *  EtaCspPwmPulseEndRegGet - Get the end pulse value.
 *
 *  @param iModule PWM module.
 *  @return Return the end pulse value.
 *
 ******************************************************************************/
uint32_t
EtaCspPwmPulseEndRegGet(tPwmModule iModule)
{
    uint32_t ui32PulseEnd = -8000000; // DB added -Wmaybe-uninitialized

    switch(iModule)
    {
        case ePwmModule0:
        {
            ui32PulseEnd = REG_R(PWM_PEND0, PEND);
            break;
        }

        case ePwmModule1:
        {
            ui32PulseEnd = REG_R(PWM_PEND1, PEND);
            break;
        }

        case ePwmModule2:
        {
            ui32PulseEnd = REG_R(PWM_PEND2, PEND);
            break;
        }

        case ePwmModule3:
        {
            ui32PulseEnd = REG_R(PWM_PEND3, PEND);
            break;
        }

        default:
        {
            break;
        }
    }
    return(ui32PulseEnd);
}

/***************************************************************************//**
 *
 *  EtaCspPwmModeSet - Set a PWM mode.
 *
 *  @param iModule PWM module.
 *  @param iMode desired PWM mode.
 *
 ******************************************************************************/
void
EtaCspPwmModeSet(tPwmModule iModule, tPwmMode iMode)
{
    if (iMode == ePwmModeNone)
    {
        return;
    }

    switch(iModule)
    {
        case ePwmModule0:
        {
            REG_W2(PWM_MODE, SET0, TRUE, MODE0, iMode);
            break;
        }

        case ePwmModule1:
        {
            REG_W2(PWM_MODE, SET1, TRUE, MODE1, iMode);
            break;
        }

        case ePwmModule2:
        {
            REG_W2(PWM_MODE, SET2, TRUE, MODE2, iMode);
            break;
        }

        case ePwmModule3:
        {
            REG_W2(PWM_MODE, SET3, TRUE, MODE3, iMode);
            break;
        }

        default:
        {
            break;
        }
    }
}

/***************************************************************************//**
 *
 *  EtaCspPwmRunStateGet - Get the current running state of a PWM.
 *
 *  @return Return current PWM mode.
 *
 ******************************************************************************/
tPwmMode
EtaCspPwmRunStateGet(tPwmModule iModule)
{
    tPwmMode iRunState = 0; // DB added -Wmaybe-uninitialized

    switch(iModule)
    {
        case ePwmModule0:
        {
            iRunState = (tPwmMode)REG_R(PWM_STAT, STATE0);
            break;
        }

        case ePwmModule1:
        {
            iRunState = (tPwmMode)REG_R(PWM_STAT, STATE1);
            break;
        }

        case ePwmModule2:
        {
            iRunState = (tPwmMode)REG_R(PWM_STAT, STATE2);
            break;
        }

        case ePwmModule3:
        {
            iRunState = (tPwmMode)REG_R(PWM_STAT, STATE3);
            break;
        }

        default:
        {
            break;
        }
    }
    return(iRunState);
}

/***************************************************************************//**
 *
 *  EtaCspPwmAllModeSet - Set all PWM modes.
 *
 *  @param iMode0 desired PWM0 mode.
 *  @param iMode1 desired PWM1 mode.
 *  @param iMode2 desired PWM2 mode.
 *  @param iMode3 desired PWM3 mode.
 *
 ******************************************************************************/
void
EtaCspPwmAllModeSet(tPwmMode iMode0, tPwmMode iMode1, tPwmMode iMode2,
                    tPwmMode iMode3)
{
    bit_t iSet[4];

    iSet[0] = (bit_t)(iMode0 != ePwmModeNone);
    iSet[1] = (bit_t)(iMode1 != ePwmModeNone);
    iSet[2] = (bit_t)(iMode2 != ePwmModeNone);
    iSet[3] = (bit_t)(iMode3 != ePwmModeNone);

    reg_pwm_mode_t reg_pwm_mode = {
        .BF = {
            .SET0  = iSet[0],
            .SET1  = iSet[1],
            .SET2  = iSet[2],
            .SET3  = iSet[3],
            .MODE0 = iMode0,
            .MODE1 = iMode1,
            .MODE2 = iMode2,
            .MODE3 = iMode3,
        }
    };

    REG_PWM_MODE = reg_pwm_mode;
}

/***************************************************************************//**
 *
 *  EtaCspPwmStart - Start a PWM.
 *
 *  @param iModule PWM module.
 *
 ******************************************************************************/
void
EtaCspPwmStart(tPwmModule iModule)
{
    EtaCspPwmModeSet(iModule, ePwmModeStart);
}

/***************************************************************************//**
 *
 *  EtaCspPwmStopImmediate - Stop the PWM immediately.
 *
 *  @param iModule PWM module.
 *
 ******************************************************************************/
void
EtaCspPwmStopImmediate(tPwmModule iModule)
{
    EtaCspPwmModeSet(iModule, ePwmModeStopImmediate);
}

/***************************************************************************//**
 *
 *  EtaCspPwmStopAfterCycle - Stop the PWM after the cycle.
 *
 *  @param iModule PWM module.
 *
 ******************************************************************************/
void
EtaCspPwmStopAfterCycle(tPwmModule iModule)
{
    EtaCspPwmModeSet(iModule, ePwmModeStopAfterCycle);
}

/***************************************************************************//**
 *
 *  EtaCspPwmStopAfterProgram - Stop the PWM after program.
 *
 *  @param iModule PWM module.
 *
 ******************************************************************************/
void
EtaCspPwmStopAfterProgram(tPwmModule iModule)
{
    EtaCspPwmModeSet(iModule, ePwmModeStopAfterProgram);
}

/***************************************************************************//**
 *
 *  EtaCspPwmStart - Start multiple PWMs simultaneously.
 *
 *  @param iPwm0 Start PWM0 module.
 *  @param iPwm1 Start PWM1 module.
 *  @param iPwm2 Start PWM2 module.
 *  @param iPwm3 Start PWM3 module.
 *
 ******************************************************************************/
void
EtaCspPwmMultiStart(bit_t iPwm0, bit_t iPwm1, bit_t iPwm2, bit_t iPwm3)
{
    tPwmMode iMode[4] = { ePwmModeNone, ePwmModeNone,
                          ePwmModeNone, ePwmModeNone };

    if (iPwm0) iMode[0] = ePwmModeStart;
    if (iPwm1) iMode[1] = ePwmModeStart;
    if (iPwm2) iMode[2] = ePwmModeStart;
    if (iPwm3) iMode[3] = ePwmModeStart;

    EtaCspPwmAllModeSet(iMode[0], iMode[1], iMode[2], iMode[3]);
}

/***************************************************************************//**
 *
 *  EtaCspPwmMultiStopImmediate - Stop multiple PWMs simultaneously.
 *
 *  @param iPwm0 Stop PWM0 module.
 *  @param iPwm1 Stop PWM1 module.
 *  @param iPwm2 Stop PWM2 module.
 *  @param iPwm3 Stop PWM3 module.
 *
 ******************************************************************************/
void
EtaCspPwmMultiStopImmediate(bit_t iPwm0, bit_t iPwm1, bit_t iPwm2, bit_t iPwm3)
{
    tPwmMode iMode[4] = { ePwmModeNone, ePwmModeNone,
                          ePwmModeNone, ePwmModeNone };

    if (iPwm0) iMode[0] = ePwmModeStopImmediate;
    if (iPwm1) iMode[1] = ePwmModeStopImmediate;
    if (iPwm2) iMode[2] = ePwmModeStopImmediate;
    if (iPwm3) iMode[3] = ePwmModeStopImmediate;

    EtaCspPwmAllModeSet(iMode[0], iMode[1], iMode[2], iMode[3]);
}

/***************************************************************************//**
 *
 *  EtaCspPwmIdleDriveSet - Set the PWM idle drive.
 *
 *  @param iModule PWM module.
 *  @param iIdleDrive PWM idle drive value.
 *
 ******************************************************************************/
void
EtaCspPwmIdleDriveSet(tPwmModule iModule, bit_t iIdleDrive)
{
    switch(iModule)
    {
        case ePwmModule0:
        {
            REG_M1(PWM_CTRL, DFLT0, iIdleDrive);
            break;
        }

        case ePwmModule1:
        {
            REG_M1(PWM_CTRL, DFLT1, iIdleDrive);
            break;
        }

        case ePwmModule2:
        {
            REG_M1(PWM_CTRL, DFLT2, iIdleDrive);
            break;
        }

        case ePwmModule3:
        {
            REG_M1(PWM_CTRL, DFLT3, iIdleDrive);
            break;
        }

        default:
        {
            break;
        }
    }
}

/***************************************************************************//**
 *
 *  EtaCspPwmOutputStateGet - Get the current output state of a PWM.
 *
 *  @return Return **true** if output is high and **false** otherwise.
 *
 ******************************************************************************/
bool
EtaCspPwmOutputStateGet(tPwmModule iModule)
{
    bool bOutputState = FALSE;

    switch(iModule)
    {
        case ePwmModule0:
        {
            bOutputState = (REG_R(PWM_STAT, VAL0) == TRUE);
            break;
        }

        case ePwmModule1:
        {
            bOutputState = (REG_R(PWM_STAT, VAL1) == TRUE);
            break;
        }

        case ePwmModule2:
        {
            bOutputState = (REG_R(PWM_STAT, VAL2) == TRUE);
            break;
        }

        case ePwmModule3:
        {
            bOutputState = (REG_R(PWM_STAT, VAL3) == TRUE);
            break;
        }

        default:
        {
            break;
        }
    }
    return(bOutputState);
}

/***************************************************************************//**
 *
 *  EtaCspPwmCfgMuxChanSet - Configure the mux channel.
 *
 *  @param iChan mux channel.
 *  @param iSync synchronous transition bit.
 *  @param iDfltDrive default drive.
 *
 ******************************************************************************/
void
EtaCspPwmCfgMuxChanSet(tPwmChanMux iChan, bit_t iSync, bit_t iDfltDrive)
{
    switch(iChan)
    {
        case ePwmChanMuxSel0:
        {
            REG_M2(PWM_CHAN_CFG, SYNC0, iSync, DFLT0, iDfltDrive);
            break;
        }

        case ePwmChanMuxSel1:
        {
            REG_M2(PWM_CHAN_CFG, SYNC1, iSync, DFLT1, iDfltDrive);
            break;
        }

        case ePwmChanMuxSel2:
        {
            REG_M2(PWM_CHAN_CFG, SYNC2, iSync, DFLT2, iDfltDrive);
            break;
        }

        case ePwmChanMuxSel3:
        {
            REG_M2(PWM_CHAN_CFG, SYNC3, iSync, DFLT3, iDfltDrive);
            break;
        }

        case ePwmChanMuxSel4:
        {
            REG_M2(PWM_CHAN_CFG, SYNC4, iSync, DFLT4, iDfltDrive);
            break;
        }

        case ePwmChanMuxSel5:
        {
            REG_M2(PWM_CHAN_CFG, SYNC5, iSync, DFLT5, iDfltDrive);
            break;
        }

        case ePwmChanMuxSel6:
        {
            REG_M2(PWM_CHAN_CFG, SYNC6, iSync, DFLT6, iDfltDrive);
            break;
        }

        case ePwmChanMuxSel7:
        {
            REG_M2(PWM_CHAN_CFG, SYNC7, iSync, DFLT7, iDfltDrive);
            break;
        }

        default:
        {
            break;
        }
    }
}

/***************************************************************************//**
 *
 *  EtaCspPwmMuxChanEnable - Enable the mux channel.
 *
 *  @param iModule PWM module.
 *  @param iChan mux channel.
 *
 ******************************************************************************/
void
EtaCspPwmMuxChanEnable(tPwmModule iModule, tPwmChanMux iChan)
{
    switch(iChan)
    {
        case ePwmChanMuxSel0:
        {
            REG_M2(PWM_CHAN_MUX, EN0, TRUE, SEL0, iModule);
            break;
        }

        case ePwmChanMuxSel1:
        {
            REG_M2(PWM_CHAN_MUX, EN1, TRUE, SEL1, iModule);
            break;
        }

        case ePwmChanMuxSel2:
        {
            REG_M2(PWM_CHAN_MUX, EN2, TRUE, SEL2, iModule);
            break;
        }

        case ePwmChanMuxSel3:
        {
            REG_M2(PWM_CHAN_MUX, EN3, TRUE, SEL3, iModule);
            break;
        }

        case ePwmChanMuxSel4:
        {
            REG_M2(PWM_CHAN_MUX, EN4, TRUE, SEL4, iModule);
            break;
        }

        case ePwmChanMuxSel5:
        {
            REG_M2(PWM_CHAN_MUX, EN5, TRUE, SEL5, iModule);
            break;
        }

        case ePwmChanMuxSel6:
        {
            REG_M2(PWM_CHAN_MUX, EN6, TRUE, SEL6, iModule);
            break;
        }

        case ePwmChanMuxSel7:
        {
            REG_M2(PWM_CHAN_MUX, EN7, TRUE, SEL7, iModule);
            break;
        }

        default:
        {
            break;
        }
    }
}

/***************************************************************************//**
 *
 *  EtaCspPwmMuxChanDisable - Disable the mux channel.
 *
 *  @param iModule PWM module.
 *  @param iChan mux channel.
 *
 ******************************************************************************/
void
EtaCspPwmMuxChanDisable(tPwmModule iModule, tPwmChanMux iChan)
{
    switch(iChan)
    {
        case ePwmChanMuxSel0:
        {
            REG_M2(PWM_CHAN_MUX, EN0, FALSE, SEL0, iModule);
            break;
        }

        case ePwmChanMuxSel1:
        {
            REG_M2(PWM_CHAN_MUX, EN1, FALSE, SEL1, iModule);
            break;
        }

        case ePwmChanMuxSel2:
        {
            REG_M2(PWM_CHAN_MUX, EN2, FALSE, SEL2, iModule);
            break;
        }

        case ePwmChanMuxSel3:
        {
            REG_M2(PWM_CHAN_MUX, EN3, FALSE, SEL3, iModule);
            break;
        }

        case ePwmChanMuxSel4:
        {
            REG_M2(PWM_CHAN_MUX, EN4, FALSE, SEL4, iModule);
            break;
        }

        case ePwmChanMuxSel5:
        {
            REG_M2(PWM_CHAN_MUX, EN5, FALSE, SEL5, iModule);
            break;
        }

        case ePwmChanMuxSel6:
        {
            REG_M2(PWM_CHAN_MUX, EN6, FALSE, SEL6, iModule);
            break;
        }

        case ePwmChanMuxSel7:
        {
            REG_M2(PWM_CHAN_MUX, EN7, FALSE, SEL7, iModule);
            break;
        }

        default:
        {
            break;
        }
    }
}

/***************************************************************************//**
 *
 *  EtaCspPwmIsFifoFull - Checks if the FIFO is full.
 *
 *  @return Return **true** if the FIFO is full and **false** otherwise.
 *
 ******************************************************************************/
bool
EtaCspPwmIsFifoFull(void)
{
    return(REG_R(PWM_STAT, CFIFO_FULL) == TRUE);
}

/***************************************************************************//**
 *
 *  EtaCspPwmIsFifoEmpty - Checks if the FIFO is empty.
 *
 *  @return Return **true** if the FIFO is empty and **false** otherwise.
 *
 ******************************************************************************/
bool
EtaCspPwmIsFifoEmpty(void)
{
    return(REG_R(PWM_STAT, CFIFO_EMPTY) == TRUE);
}

/***************************************************************************//**
 *
 *  EtaCspPwmFifoUsageGet - Get the FIFO usage.
 *
 *  @return Return the FIFO usage.
 *
 ******************************************************************************/
uint32_t
EtaCspPwmFifoUsageGet(void)
{
    reg_pwm_stat_t iPwmStat = REG_PWM_STAT;

    return((iPwmStat.BF.CFIFO_FULL << 4) | iPwmStat.BF.CFIFO_SIZE);
}

/***************************************************************************//**
 *
 *  EtaCspPwmFifoLowIntEnable - Enable the PWM low interrupt.
 *
 *  @param ui32FifoLowLevel the low leverl threshold for the interrupt.
 *
 ******************************************************************************/
tEtaStatus
EtaCspPwmFifoLowIntEnable(uint32_t ui32FifoLowLevel)
{
    tEtaStatus iStatus = eEtaSuccess;

    if (ui32FifoLowLevel > 0xF) {
        iStatus = eEtaParameterOutOfRange;
    }
    else {
        REG_M2(PWM_CTRL, CFIFO_LOW, ui32FifoLowLevel, CFIFO_INT, TRUE);
    }

    return(iStatus);
}

/***************************************************************************//**
 *
 *  EtaCspPwmFifoLowIntDisable - Disable the PWM low interrupt.
 *
 ******************************************************************************/
void
EtaCspPwmFifoLowIntDisable(void)
{
    REG_C1(PWM_CTRL, CFIFO_INT);
}

/***************************************************************************//**
 *
 *  EtaCspPwmIntGet - Get interrupt status.
 *
 *  @return Return interrupt status struct
 *
 ******************************************************************************/
tPwmIntStatReg
EtaCspPwmIntGet(void)
{
    tPwmIntStatReg iPwmIntStat;
    iPwmIntStat.ui32Value = REG_PWM_INT_STAT.V;

    return(iPwmIntStat);
}


