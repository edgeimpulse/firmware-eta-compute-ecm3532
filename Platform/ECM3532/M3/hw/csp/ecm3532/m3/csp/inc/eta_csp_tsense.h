/***************************************************************************//**
 *
 * @file eta_csp_tsense.h
 *
 * @brief This file contains eta_csp_tsense module definitions.
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
 * This is part of revision 1.0.0b1 of the Tensai Chip Support Package.
 *
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup ecm3532tsense-m3 Temperature Sensor (TSENSE)
 * @ingroup ecm3532csp-m3
 * @{
 ******************************************************************************/

#ifndef __ETA_CSP_TSENSE_H__
#define __ETA_CSP_TSENSE_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __LANGUAGE_ASM__

#endif // __LANGUAGE_ASM__

/***************************************************************************//**
 *
 *  EtaCspTSenseInit - Run at least once to setup temp conversion datsa.
 *
 ******************************************************************************/
extern void EtaCspTSenseInit(void);

/***************************************************************************//**
 *
 *  EtaCspTSenseStart - Start a temperature measurement
 *
 *  @return EtaSuccess if the LDO is ready to go and the tsense has started.
 ******************************************************************************/
extern tEtaStatus EtaCspTSenseStart(void);

/***************************************************************************//**
 *
 *  EtaCspTSenseOff - Turn off the temp sensor until next time.
 *
 ******************************************************************************/
extern void EtaCspTSenseOff(void);

/***************************************************************************//**
 *
 *  EtaCspTSenseIsReady - Check  temperature sensor ready state.
 *
 *  @return True if it is ready.
 *
 ******************************************************************************/
extern bool EtaCspTSenseIsReady(void);

/***************************************************************************//**
 *
 *  EtaCspTSenseGet - Return  temperature sensor value in degrees C.
 *
 *  @return temp in degree C or -273  degrees C if it is in error.
 *          the value returned is  in deci-degrees C so -270C is -2700
 *
 ******************************************************************************/
extern int32_t EtaCspTSenseGet(void);

#ifdef __cplusplus
}
#endif

#endif // __ETA_CSP_TSENSE_H__

