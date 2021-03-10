/*******************************************************************************
*
* @file pdm_hal.h
*
* Copyright (C) 2020 Eta Compute, Inc
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
#ifndef H_PDM_HAL_
#define H_PDM_HAL_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/** PDM Channels */
typedef enum {
    /** pdm channel 0 */
    PDMCHAN0 = 0,
    /** pdm channel 1 */
    PDMCHAN1 = 1,
}tPDMChan;

/** PCM Sampling rate */
typedef enum {
    /** pcm 8 KHZ sampling */
    PCM8KHZ = 8,
    /** pcm 16 KHZ sampling */
    PCM16KHZ = 16,
}tSRate;

/** PCM channel mode */
typedef enum {
    DISABLE = 0,
    /** mono left */
    MONO_LEFT = 1,
    /** mono right */
    MONO_RIGHT = 2,
    /** stereo */
    STEREO = 3,
}tCMode;

/** pdm channel configuration */
typedef struct {
    /** pdm channel number 0/1 */
    uint8_t pdmNum;
    /** sampling rate in KHz 8/16 */
    uint8_t sRate;
    /** channel mode mono-l monr-r or stereo */
    uint8_t cMode;
	/** pdm_pcm frame length in msec, range <5-30>*/
    uint8_t rFLen;
}tPdmcfg;

/**
 * Function prototype for pcm frame.
 * called from interrupt context,
 * no blocking func should be called from callaback.
 * callback should return before frame length msec
 *
 * @param ptr void pointer of caller data struct
 * @param buf pointer to frame data
 * @param blen length of buffer in bytes
 */
typedef void (*tPCMFrameCb)(void *ptr, void *buf, uint16_t blen);

/**
 * Initialize PDM channel
 * @param sPdm channel configuration
 * @param fPcmCb PCM fram callback function
 * @param vCbptr callback pointer
 * @return 0 on success, non-zero error code on failure
*/
int ecm3532_pdm_init(tPdmcfg *sPdm, tPCMFrameCb fPcmCb, void *vCbptr);

/**
 * Start PDM PCM stream
 *
 * @param u8Chan PDM channel number
 * @return 0 on success, non-zero error code on failure
 */
int ecm3532_start_pdm_stream(uint8_t u8Chan);

/**
 * Stop PDM PCM stream
 *
 * @param u8Chan PDM channel number
 * @return 0 on success, non-zero error code on failure
 */
int ecm3532_stop_pdm_stream(uint8_t u8Chan);

/**
 * DeInit PDM, will free up resources
 *
 * @param pdmNum PDM channel number
 * @return 0 on success, non-zero error code on failure
 */
int ecm3532_pdm_deinit(uint8_t pdmNum);

#endif //H_PDM_HAL_
