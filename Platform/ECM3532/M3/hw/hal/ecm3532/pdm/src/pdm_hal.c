/*******************************************************************************
 * @file pdm_hal.c
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
#include "config.h"
#include <stddef.h>
#include "errno.h"
#include "ecm3532.h"
#include "cm3.h"
#include "reg_eta_ecm3532_m3.h"
#include "eta_csp_common_pdm.h"
#include "eta_csp_socctrl.h"
#include "eta_csp_pdm.h"
#include "module_common.h"
#include "audio_common.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "shmem.h"
#include "rpc.h"
#include "pdm_hal.h"
#include "print_util.h"

#define FRAME_CNT (2)

#define PDM_INDEX(x) (x < CONFIG_PDM_COUNT ? x : x - 1)

typedef struct
{
  uint8_t st;
  tPCMFrameCb fAppCb;
  void *cbPtr;
  uint16_t fBLen;
  int16_t *fPtr;
}tPdmDev;

tPdmDev sPdmDev[CONFIG_PDM_COUNT];
SemaphoreHandle_t pdmLock = NULL;
SemaphoreHandle_t pdmDspRespSem = NULL;

static int audioDspNotifyResponse(uint32_t header, uint32_t data)
{
  // Give response semaphore
  xSemaphoreGive(pdmDspRespSem);
  return 0;
}

static int audioDspNotifyEvent(uint32_t header, uint32_t data)
{
  int16_t *fPtr;
  tPCMFrameCb fCb;
  void *cbPtr;
  uint8_t pdmNum;
  uint16_t bLen;

  pdmNum = GET_AUDIO_PDM_ID(header);
  fCb = sPdmDev[PDM_INDEX(pdmNum)].fAppCb;
  if ((data >= FRAME_CNT) || (pdmNum > CONFIG_PDM_COUNT))
  {
    ecm35xx_printf("Invaid Frame Pointer recevied\r\n");
  }
  else if (fCb)
  {
    fPtr = sPdmDev[PDM_INDEX(pdmNum)].fPtr;
    bLen = sPdmDev[PDM_INDEX(pdmNum)].fBLen;
    cbPtr = sPdmDev[PDM_INDEX(pdmNum)].cbPtr;
    fCb(cbPtr, &fPtr[(bLen * data)], bLen * 2);
  }
  return 0;
}
static int audioDspRpcInit(void)
{
  uint8_t evHMask = 0;

  /* Response will be aways serialized*/
  if (!pdmDspRespSem)
  {
    pdmDspRespSem = xSemaphoreCreateBinary();
    pdmLock = xSemaphoreCreateMutex();
    SET_MODULEID(evHMask, RPC_MODULE_ID_AUDIO);
    SET_EVT_RSP(evHMask, RPC_RESPONSE);
    // register with RPC notification  for all responses
    rpcRegisterEventCb(evHMask, audioDspNotifyResponse);
  }

  // register with RPC notification for events
  evHMask = 0;
  SET_MODULEID(evHMask, RPC_MODULE_ID_AUDIO);
  SET_EVT_RSP(evHMask, RPC_EVENT );
  SET_AUDIO_PDM_ID (evHMask, 1);

  rpcRegisterEventCb(evHMask, audioDspNotifyEvent);
  return 0;
}
/**
 * Initialize PDM channel
 * @param sPdm channel configuration
 * @param fPcmCb PCM fram callback function
 * @return 0 on success, non-zero error code on failure
 */
int ecm3532_pdm_init(tPdmcfg *sPdm, tPCMFrameCb fCb, void *cbPtr)
{
  tPdmClkSrc pdmClk;
  tAudRPCfg *audRcfg;
  void *aBuf;
  uint16_t bLen = 0;

  if (!sPdm || !fCb)
    return -EINVAL;
  else if ((sPdm->pdmNum > CONFIG_PDM_COUNT) || (sPdm->sRate > PCM16KHZ) ||
      (sPdm->cMode > STEREO))
  {
    ecm35xx_printf ("Invalid configuration\r\n");
    return -EINVAL;
  }
  else if (sPdmDev[PDM_INDEX(sPdm->pdmNum)].st)
  {
    return -EINPROGRESS;
  }

  pdmClk = CONFIG_PDM_CLK_SRC;
  switch(pdmClk)
  {
    case (ePdmClkSrc2MhzXtal):
      {
        // Power up the 16MHz oscillator
        REG_M1(RTC_OSC_CSR2, VHFO_PD_N, 1);
        // Fence
        EtaCspPdmFence();
        EtaCspSocCtrlAdcFreqSet(eSocCtrlAdcSelXtal2MHz);
        break;
      }

    case (ePdmClkSrc2MhzHfo):
      {
        EtaCspSocCtrlAdcFreqSet(eSocCtrlAdcSelHfo2MHz);
        break;
      }

    case (ePdmClkSrc1MhzHfo):
      {
        EtaCspSocCtrlAdcFreqSet(eSocCtrlAdcSelHfo1MHz);
        break;
      }

    case (ePdmClkSrcFromPad):
      {
        // Select ACLK as clock source
        EtaCspSocCtrlAdcFreqSet(eSocCtrlAdcSelPad2MHz);
        break;
      }

    default:
      {
        return(ePdmConfigInvalidConfig);
        break;
      }
  }

  REGN_M1(sPdm->pdmNum, PDM_PCMA_CTRL, SFTRST, 1);
  vTaskDelay(50);
  /* confifure PDM */
  REGN_M1(sPdm->pdmNum, PDM_PDM_CORE_CONF, PGA_R, CONFIG_PGA_R);
  REGN_M1(sPdm->pdmNum, PDM_PDM_CORE_CONF, PGA_L, CONFIG_PGA_L);
#ifdef CONFIG_HP_FILTER
  REGN_M2(sPdm->pdmNum, PDM_PDM_CORE_CONF, ADCHPD, 1,
            HPGAIN,CONFIG_HP_COEF);
#endif
  REGN_M1(sPdm->pdmNum, PDM_PDM_CORE_CONF, SINC_RATE, 0x40);
  REGN_M1(sPdm->pdmNum, PDM_PDM_CORE_CONF, MCLKDIV, 0x0);
  REGN_M1(sPdm->pdmNum, PDM_PDM_CORE2_CONF, PDMCKO_DLY, 0x3);

  if(sPdm->sRate == PCM16KHZ)
  {
    REGN_M1(sPdm->pdmNum, PDM_PDM_CORE2_CONF, DIV_MCLKQ,
        BFV_PDM_PDM_CORE2_CONF_DIV_MCLKQ_BYPASS);
    bLen = (16 * sPdm->rFLen);
  }
  else
  {
    REGN_M1(sPdm->pdmNum, PDM_PDM_CORE2_CONF, DIV_MCLKQ,
        BFV_PDM_PDM_CORE2_CONF_DIV_MCLKQ_DIV_BY_1);
    bLen = (8 * sPdm->rFLen);
  }

  if (sPdm->cMode == STEREO)
  {
    bLen = bLen * 2;
  }

  audRcfg = SharedMemAlloc(sizeof(tAudRPCfg));
  if (!audRcfg) {
    ecm35xx_printf("Fail to alloc shared memory\r\n");
    configASSERT(0);
  }

  audioDspRpcInit();

  aBuf = SharedMemAlloc(bLen * sizeof(int16_t) * FRAME_CNT);
  if (!aBuf)
  {
    ecm35xx_printf("shmem mallox failed for PCM buffer\r\n");
    configASSERT(0);
  }

  sPdmDev[PDM_INDEX(sPdm->pdmNum)].fAppCb = fCb;
  sPdmDev[PDM_INDEX(sPdm->pdmNum)].cbPtr = cbPtr;
  sPdmDev[PDM_INDEX(sPdm->pdmNum)].fBLen = bLen;
  sPdmDev[PDM_INDEX(sPdm->pdmNum)].fPtr = aBuf;

  audRcfg->pdmNum = sPdm->pdmNum;
  audRcfg->cMode = sPdm->cMode;

  /* need only 2 frame in pingpong mode */
  audRcfg->bInfo.fCnt = FRAME_CNT;
  audRcfg->bInfo.sFbuf.Flen = bLen;
  audRcfg->bInfo.sFbuf.fPtrHi = ((int)(aBuf) >> 16);
  audRcfg->bInfo.sFbuf.fPtrLo = ((int)(aBuf)& 0xFFFF);

  rpcSubmitWork(RPC_MODULE_ID_AUDIO, AUDIO_INIT, audRcfg);
  xSemaphoreTake(pdmDspRespSem, portMAX_DELAY);
  SharedMemFree(audRcfg);
  sPdmDev[PDM_INDEX(sPdm->pdmNum)].st = AUDIO_INIT;
  return 0;
}

/**
 * Start PCM stream
 *
 * @param pdmNum PDM channel number
 * @return 0 on success, non-zero error code on failure
 */
int ecm3532_start_pdm_stream(uint8_t pdmNum)
{
  uint16_t *data;
  xSemaphoreTake(pdmLock, portMAX_DELAY);
  /* start PDM block */
  REGN_M1(pdmNum, PDM_PCMA_CTRL, EN, 1);
  vTaskDelay(50);
  data = SharedMemAlloc(sizeof(uint16_t));
  data[0] = pdmNum;
  rpcSubmitWork(RPC_MODULE_ID_AUDIO, AUDIO_START, data);
  xSemaphoreTake(pdmDspRespSem, portMAX_DELAY);
  SharedMemFree(data);
  sPdmDev[PDM_INDEX(pdmNum)].st = AUDIO_START;
  xSemaphoreGive(pdmLock);
  return 0;
}

/**
 * Stop PCM stream
 *
 * @param pdmNum PDM channel number
 * @return 0 on success, non-zero error code on failure
 */
int ecm3532_stop_pdm_stream(uint8_t pdmNum)
{
  uint16_t *data;
  xSemaphoreTake(pdmLock, portMAX_DELAY);
  data = SharedMemAlloc(sizeof(uint16_t));
  data[0] = pdmNum;
  rpcSubmitWork(RPC_MODULE_ID_AUDIO, AUDIO_STOP, data);
  xSemaphoreTake(pdmDspRespSem, portMAX_DELAY);
  SharedMemFree(data);
  /* stop PDM block */
  REGN_M1(pdmNum, PDM_PCMA_CTRL, EN, 0);
  sPdmDev[PDM_INDEX(pdmNum)].st = AUDIO_STOP;
  xSemaphoreGive(pdmLock);
  return 0;
}

/**
 * DeInit PDM, will free up resources
 *
 * @param u8Chan PDM channel number
 * @return 0 on success, non-zero error code on failure
 */
int ecm3532_pdm_deinit(uint8_t pdmNum)
{
  uint16_t *data;
  xSemaphoreTake(pdmLock, portMAX_DELAY);

  if (sPdmDev[PDM_INDEX(pdmNum)].st == AUDIO_START)
  {
    xSemaphoreGive(pdmLock);
    return -EINPROGRESS;
  }
  data = SharedMemAlloc(sizeof(uint16_t));
  data[0] = pdmNum;
  rpcSubmitWork(RPC_MODULE_ID_AUDIO, AUDIO_DEINIT, data);
  xSemaphoreTake(pdmDspRespSem, portMAX_DELAY);
  SharedMemFree(data);

  vPortFree(sPdmDev[PDM_INDEX(pdmNum)].fPtr);
  sPdmDev[PDM_INDEX(pdmNum)].st = AUDIO_DEINIT;

  xSemaphoreGive(pdmLock);

  return 0;
}
