/***************************************************************************//**
 *
 * @file eta_csp_buck.h
 *
 * @brief This file contains eta_csp_buck module definitions.
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
 * @addtogroup ecm3532buck-m3 Buck
 * @ingroup ecm3532csp-m3
 * @{
 ******************************************************************************/

#ifndef __ETA_CSP_BUCK_H__
#define __ETA_CSP_BUCK_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __LANGUAGE_ASM__

//
// Defines used to correctly constrain frequency mode requirements
//
// FIXME - need to determine correct limits to replace the values below
#define ETA_BUCK_AO_0P6V_MEM_0P9V_MIN_FREQ  20000
#define ETA_BUCK_AO_0P6V_MEM_0P9V_MAX_FREQ  60000

#define ETA_BUCK_AO_0P7V_MEM_0P9V_MIN_FREQ  40000
#define ETA_BUCK_AO_0P7V_MEM_0P9V_MAX_FREQ  60000

#define ETA_BUCK_AO_0P7V_MEM_1P2V_MIN_FREQ  40000
#define ETA_BUCK_AO_0P7V_MEM_1P2V_MAX_FREQ  1000000

#define ETA_BUCK_MIN_M3_FREQ_TARGET         20000
#define ETA_BUCK_MAX_M3_FREQ_TARGET         100000

#define ETA_BUCK_MIN_DSP_FREQ_TARGET        20000
#define ETA_BUCK_MAX_DSP_FREQ_TARGET        100000

//
//! Set the HF CLK SOURCE to one of these
//
typedef enum
{
    eBuckHfClockSourceVhfo = 1,
    eBuckHfClockSourceHfo  = 0,
}
tBuckHfClkSrc;

//
//! Set the AO BUCK to provide the specified target voltage.
//
typedef enum
{
    eBuckAo600Mv   = 600,
    eBuckAo700Mv   = 700,
    eBuckAoInvalid = -1,
}
tBuckAoTarget;

//
//! Set the M3 buck to provide the specified target voltage
//
typedef enum
{
    //
    //! Voltage settings.
    //
    eBuckM3Voltage600Mv  = 600,
    eBuckM3Voltage700Mv  = 700,
    eBuckM3Voltage800Mv  = 800,
    eBuckM3FrequencyMode = -2,
    eBuckM3Invalid       = -1,
}
tBuckM3Target;

//
//! Set the DSP buck to provide the specified target voltage
//
typedef enum
{
    //
    //! Voltage settings.
    //
    eBuckDspOff           = 0,
    eBuckDspVoltage600Mv  = 600,
    eBuckDspVoltage700Mv  = 700,
    eBuckDspVoltage800Mv  = 800,
    eBuckDspFrequencyMode = -2,
    eBuckDspInvalid       = -1,
}
tBuckDspTarget;

//
//! Set the memory buck to provide the specified target voltage.
//
typedef enum
{
    eBuckMem900Mv    = 900,
    eBuckMem1100Mv   = 1100,
    eBuckMemInvalid  = -1,
}
tBuckMemTarget;

//
// Define state transitions for Buck programming
//
typedef enum
{
    eBuckIdle,
    eBuckSetAo,
    eBuckSetM3,
    eBuckSetM3Intermediate,
    eBuckSetDsp,
    eBuckSetDspIntermediate,
    eBuckSetMem,
}
tBuckState;

//
//! Define typedef to indicate a Set function should wait for the bucks to be stable or not
//
typedef enum
{
    eBuckStableNoWait,
    eBuckStableWait,
}
tBuckWait4Stable;

//
//!
//
typedef enum
{
    eBuckUseCallback,
    eBuckSetAllBucks,
}
tBuckAllAtOnce;

//
// Define a structure to store the current state of the Bucks
//
typedef struct
{
    reg_rtc_pmic_ao_csr_t   sregAoCsr;                  // Shadow copy of AO CSR
    reg_rtc_pmic_m3_csr_t   sregM3Csr;                  // Shadow copy of M3 CSR
    reg_rtc_pmic_m3_csr2_t  sregM3Csr2;                 // Shadow copy of M3 CSR2
    reg_rtc_buck_dsp_csr_t  sregDspCsr;                 // Shadow copy of DSP CSR
    reg_rtc_buck_dsp_csr2_t sregDspCsr2;                // Shadow copy of DSP CSR2
    reg_rtc_pmic_mem_csr_t  sregMemCsr;                 // Shadow copy of MEM CSR
    uint32_t                ui32M3Frequency;            // Current M3 Buck Frequency (kHz)
    uint32_t                ui32M3FrequencyPreStall;    // M3 Buck Frequency Target Pre-Stall
    uint32_t                ui32DspFrequency;           // Current DSP Buck Frequency (kHz)
    uint32_t                ui32DspFrequencyPreStall;   // M3 Buck Frequency Target Pre-Stall
    uint32_t                ui32M3FrequencyTarget;      // New Target M3 Buck Frequency (kHz)
    uint32_t                ui32DspFrequencyTarget;     // New Target DSP Buck Frequency (kHz)
    uint8_t                 ui8AoVregTrim;              // AO Buck VREG_TRIM copied from Flash
    uint8_t                 ui8M3VregTrim;              // M3 Buck VREG_TRIM copied from Flash
    uint8_t                 ui8DspVregTrim;             // DSP Buck VREG_TRIM copied from Flash
    uint8_t                 ui8MemVregTrim;             // MEM Buck VREG_TRIM copied from Flash
    int8_t                  i8M3FreqOffset;             // Constant offset for M3 Buck freq_target
    int8_t                  i8DspFreqOffset;            // Constant offset for DSP Buck freq_target
    tBuckAoTarget           sregAoVoltageTarget;        // New Target AO Buck Voltage
    tBuckM3Target           sregM3VoltageTarget;        // New Target M3 Buck Voltage
    tBuckM3Target           sregM3VoltagePreStall;      // M3 Voltage setting when entering stall
    tBuckDspTarget          sregDspVoltageTarget;       // New Target DSP Buck Voltage
    tBuckDspTarget          sregDspVoltagePreStall;     // DSP Voltage setting when entering stall
    tBuckMemTarget          sregMemVoltageTarget;       // New Target MEM Buck Voltage
    tBuckHfClkSrc           sregHfClockSource;          // Indicates whether the HFO or VHFO is the high frequency clock source
    bool                    bAoValid;                   // Indicates whether AO Buck Needs to be programmed
    bool                    bAoUp;                      // New Target for AO Buck is higher or lower than current
    bool                    bM3Valid;                   // Indicates whether M3 Buck Needs to be programmed
    bool                    bM3Up;                      // New Target for M3 Buck is higher or lower than current
    bool                    bDspValid;                  // Indicates whether DSP Buck Needs to be programmed
    bool                    bDspUp;                     // New Target for DSP Buck is higher or lower than current
    bool                    bMemValid;                  // Indicates whether MEM Buck Needs to be programmed
    bool                    bMemUp;                     // New Target for MEM Buck is higher or lower than current
    bool                    bM3FrequencyMode;           // Indicates that the M3 Buck is operating in frequency mode
    bool                    bDspFrequencyMode;          // Indicates that the DSP Buck is operating in frequency mode
    bool                    bM3StallValid;              // Indicates that M3PreStall has been called and state captured
    bool                    bDspStallValid;             // Indicates that DspPreStall has been called and state captured
    bool                    bTargetsQualified;          // Indicates that new targets are qualified by SetAll function
    bool                    bEnterSleep;                // Indicates that the PrePowerDown function has been called to prepare for sleep
}
tBuckAllState;

#endif // __LANGUAGE_ASM__


//
// Set the AO target
//
extern tEtaStatus EtaCspBuckAoVoltageSet(tBuckAoTarget      iAoTarget,
                                         tBuckWait4Stable   iWaitForStable);

//
// Set the M3 target in voltage mode
//
extern tEtaStatus EtaCspBuckM3VoltageSet(tBuckM3Target      iM3Target,
                                         tBuckWait4Stable   iWaitForStable);

//
// Set the M3 target in frequency mode
//
extern tEtaStatus EtaCspBuckM3FrequencySet(uint32_t         ui32M3Frequency,
                                           tBuckWait4Stable iWaitForStable);

//
// Set the DSP target in voltage mode
//
extern tEtaStatus EtaCspBuckDspVoltageSet(tBuckDspTarget    iDspTarget,
                                          tBuckWait4Stable  iWaitForStable);

//
// Set the DSP target in frequency mode
//
extern tEtaStatus EtaCspBuckDspFrequencySet(uint32_t            ui32DspFrequency,
                                            tBuckWait4Stable    iWaitForStable);

//
// Set the MEM target
//
extern tEtaStatus EtaCspBuckMemVoltageSet(tBuckMemTarget        iMemTarget,
                                          tBuckWait4Stable      iWaitForStable);

//
// Prepare the DSP for Sleep/Powerdown
//
extern tEtaStatus EtaCspBuckDspPrePowerDown(void);

//
// Prepare the Bucks for Sleep/Powerdown
//
extern tEtaStatus EtaCspBuckPrePowerDown(void);

//
// Prepare the M3 Buck for Stall
//
extern tEtaStatus EtaCspBuckM3PreStall(void);

//
// Prepare the M3 Buck after Stall
//
extern tEtaStatus EtaCspBuckM3PostStall(void);

//
// Prepare the DSP Buck for Stall
//
extern tEtaStatus EtaCspBuckDspPreStall(void);

//
// Prepare the DSP Buck after Stall
//
extern tEtaStatus EtaCspBuckDspPostStall(void);

//
// Initialize the bucks.
//
extern tEtaStatus EtaCspBuckInit(void);

//
// Set all of the Bucks
//
extern tEtaStatus EtaCspBuckAllSet(tBuckAoTarget        iAoTarget,
                                   tBuckM3Target        iM3Target,
                                   uint32_t             ui32M3Frequency,
                                   tBuckDspTarget       iDspTarget,
                                   uint32_t             ui32DspFrequency,
                                   tBuckMemTarget       iMemTarget,
                                   tBuckAllAtOnce       iAllAtOnce,
                                   tBuckWait4Stable     iWaitForStable);

//
// Callback function to continue Buck programming while allowing
// upper level code to perform other tasks.
//
extern tEtaStatus EtaCspBuckAllSetWait(tBuckWait4Stable   iWaitForStable);

//
// Report when the Bucks have reached a stable state
// after programming.
//
extern bool EtaCspBuckStable(void);

//
// Select the Oscillator source of the high frequency
// reference clock (HFO or VHFO)
//
extern void EtaCspBuckHfClkSrcSelect(tBuckHfClkSrc iSource);

#ifdef __cplusplus
}
#endif

#endif // __ETA_CSP_BUCK_H__

