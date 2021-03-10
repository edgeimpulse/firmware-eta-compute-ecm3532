#include "pdm_audio.h"
tAudCfg audCfg[CONFIG_PDM_COUNT];

void setPdmDmaHandler(uint16_t pdm, uint16_t dmaChan);
void clearPdmDmaHandler(uint16_t pdm, uint16_t dmaChan);
void audio_start_dma(uint16_t pdmNum);

/** Send Event to DSP */
void sendAudioEventData(uint16_t pdmNum, uint16_t pkt_num)
{
    uint32_t mboxHeader = 0;
    uint32_t mboxData = 0;

    SET_MODULEID(mboxHeader, RPC_MODULE_ID_AUDIO);
    SET_EVT_RSP(mboxHeader, RPC_EVENT);

    SET_AUDIO_PDM_ID(mboxHeader, (pdmNum & 1));
    SET_AUDIO_EVENT_TYPE(mboxHeader, EVENT_DMA_BUF_ID);
    SET_AUDIO_EVENT_DATA(mboxHeader, 0);
    mboxData = pkt_num;
    EtaCspMboxDsp2M3((uint16_t)mboxHeader, mboxData);
    return;
}


/** Common DMA ISR func */
void pcm_dma_int_handler(tIrqNum iIrqNum, intline_T int_line, uint16_t pdmNum)
{
    static uint16_t pkt_num;
    EtaCspIsrIntClear(iIrqNum, int_line);
    pkt_num = (pkt_num +  1 & 1);
    sendAudioEventData(pdmNum, pkt_num);
}

/** DMA ISR handler */
void audio_pdm1_dma_int_handler(tIrqNum iIrqNum, intline_T int_line)
{
  pcm_dma_int_handler(iIrqNum, int_line, 1);
}

/** DMA ISR handler */
void audio_pdm0_dma_int_handler(tIrqNum iIrqNum, intline_T int_line)
{
  pcm_dma_int_handler(iIrqNum, int_line, 0);
}


/** set DMA ISR handler */
void setPdmDmaHandler(uint16_t pdm, uint16_t dmaChan)
{
  etaPrintf("pdm %d dmaChan %d\r\n",pdm, dmaChan);

  EtaCspIsrExtIntEnable(eIrqNumDma0 + dmaChan, DMA_ISR_LINE);
  if (pdm)
    EtaCspIsrHandlerSet(eIrqNumDma0 +  dmaChan, DMA_ISR_LINE,
      audio_pdm1_dma_int_handler);
  else
    EtaCspIsrHandlerSet(eIrqNumDma0 +  dmaChan, DMA_ISR_LINE,
      audio_pdm0_dma_int_handler);
}


/** Clear DMA ISR handler */
void clearPdmDmaHandler(uint16_t pdm, uint16_t dmaChan)
{
  EtaCspIsrExtIntDisable(eIrqNumDma0 + dmaChan, DMA_ISR_LINE);
  EtaCspIsrIntClear(eIrqNumDma0 + dmaChan, DMA_ISR_LINE);
  EtaCspIsrDefaultHandlerSet(eIrqNumDma0 +  dmaChan, DMA_ISR_LINE);
}

/** Send Response to M3*/
static void sendResponseFromAudio(uint8_t moduleId, uint8_t evtRsp)
{
  uint32_t mboxHeader = 0;
  SET_MODULEID(mboxHeader, moduleId);
  SET_EVT_RSP(mboxHeader, evtRsp);
  EtaCspMboxDsp2M3((uint16_t)mboxHeader, 0x00);

  etaPrintf("Send Resp\r\n");
}

/** Start DMA */
void audio_start_dma(uint16_t pdmNum)
{
  tDmaCmd dma_cmd = EtaCspDmaCmdGetDefault();

  audCfg[ACFG_INDEX(pdmNum)].dmaChan = EtaCspGetFreeDmaCh();
  setPdmDmaHandler(pdmNum, audCfg[ACFG_INDEX(pdmNum)].dmaChan);

  etaPrintf("start pdm %d dmaChan %d\r\n",pdmNum, audCfg[ACFG_INDEX(pdmNum)].dmaChan);

  dma_cmd.ui8Channel = audCfg[ACFG_INDEX(pdmNum)].dmaChan;
  dma_cmd.iSrcTarget = eDmaTargetIoMemPdm1;
  dma_cmd.iDstTarget = eDmaTargetIoMemWindow;
  dma_cmd.ui32SrcAddress = 0;
  dma_cmd.ui32DstAddress = AHB_WINDOW_IOMEM_ADDR(AUDIO_DEMO_MEM_WIN) +
    ((audCfg[ACFG_INDEX(pdmNum)].ahbAdr - M3_SRAM_WIN3_ADDR) / 2);
  dma_cmd.ui16XferLength = audCfg[ACFG_INDEX(pdmNum)].rAcfg->bInfo.sFbuf.Flen;

  dma_cmd.cmd_reload_per_xfer = 1;
  dma_cmd.ui8CircBuffTrig = 2;
  dma_cmd.circ_buff_dst_reload = 1;
  EtaCspDmaLaunch(&dma_cmd);
}
/** audio task handler */
      uint32_t ahbAdr;
static void audioTask(tdspLocalMsg* msg, void* pArg)
{
  uint16_t pdmNum;
  uint32_t ui32Reg;
  tPdmChanConfig chanCfg;
  tEtaStatus st;
  volatile void chess_storage(IOMEM)* address;

  chess_dont_warn_dead(pArg);
  switch(msg->msgId)
  {
    case AUDIO_INIT:
    {
      tAudRPCfg *acfg;
      acfg = Dsp_Xmem_Alloc(sizeof(tAudRPCfg));

      address = (void chess_storage(IOMEM)*)getSharedMemoryAddress(msg->data[0]);
      memcpy_io_mem_to_xmem((void*)acfg, (void chess_storage(IOMEM)*)address,
          (uint16_t)sizeof(tAudRPCfg));

      if (acfg->cMode ==  MONO_LEFT)
        chanCfg = ePdmChanConfigLeftOnly;
      else if (acfg->cMode ==  MONO_RIGHT)
        chanCfg = ePdmChanConfigRightOnly;
      else if (acfg->cMode ==  STEREO)
        chanCfg = ePdmChanConfigBoth;

      etaPrintf("Audio Init pdm  %d\r\n", acfg->pdmNum);
      ui32Reg = REG_PDM_DSP_CSR(acfg->pdmNum);
      ui32Reg &= ~(BM_PDM_DSP_CSR_RX_CH_MODE | BM_PDM_DSP_CSR_PDM_EN);
      ui32Reg |= (chanCfg << BP_PDM_DSP_CSR_RX_CH_MODE);

      REG_PDM_DSP_CSR(acfg->pdmNum) = ui32Reg;

      /* assign to global ptr */
      audCfg[ACFG_INDEX(acfg->pdmNum)].rAcfg = acfg;

       // Set up the windows
      ahbAdr = acfg->bInfo.sFbuf.fPtrHi;
      ahbAdr = ((ahbAdr << 16) | acfg->bInfo.sFbuf.fPtrLo);

      audCfg[ACFG_INDEX(acfg->pdmNum)].ahbAdr = ahbAdr;

      sendResponseFromAudio(RPC_MODULE_ID_AUDIO, RPC_RESPONSE);
      break;
    }

    case AUDIO_START:
    {
      address = (void chess_storage(IOMEM)*)getSharedMemoryAddress(msg->data[0]);
      memcpy_io_mem_to_xmem((void*)&pdmNum, (void chess_storage(IOMEM)*)address,
          (uint16_t)sizeof(pdmNum));

      ui32Reg = REG_PDM_DSP_CSR(pdmNum);
      ui32Reg |= (1 << BP_PDM_DSP_CSR_PDM_EN);
      REG_PDM_DSP_CSR(pdmNum) = ui32Reg;


      etaPrintf("Start Audio %d\r\n", pdmNum);
      audio_start_dma(pdmNum);
      sendResponseFromAudio(RPC_MODULE_ID_AUDIO, RPC_RESPONSE);
      break;
    }

    case AUDIO_STOP:
    {
      address = (void chess_storage(IOMEM)*)getSharedMemoryAddress(msg->data[0]);
      memcpy_io_mem_to_xmem((void*)&pdmNum, (void chess_storage(IOMEM)*)address,
          (uint16_t)sizeof(pdmNum));

      ui32Reg = REG_PDM_DSP_CSR(pdmNum);
      ui32Reg &= (~(1 << BP_PDM_DSP_CSR_PDM_EN));
      REG_PDM_DSP_CSR(pdmNum) = ui32Reg;
      etaPrintf("Stop Audio Stream %d\r\n", pdmNum);
      EtaCspDmaChTerminate(audCfg[ACFG_INDEX(pdmNum)].dmaChan);
      clearPdmDmaHandler(pdmNum, audCfg[ACFG_INDEX(pdmNum)].dmaChan);
      sendResponseFromAudio(RPC_MODULE_ID_AUDIO, RPC_RESPONSE);
      break;
    }
   case AUDIO_DEINIT:
    {
      address = (void chess_storage(IOMEM)*)getSharedMemoryAddress(msg->data[0]);
      memcpy_io_mem_to_xmem((void*)&pdmNum, (void chess_storage(IOMEM)*)address,
          (uint16_t)sizeof(pdmNum));
      etaPrintf("Deinit  %d\r\n", pdmNum);
      Dsp_Xmem_Free(audCfg[ACFG_INDEX(pdmNum)].rAcfg);
      break;
    }
  }

}

/** Create Audio Task*/
void createAudioTask(void)
{
  int8_t status;
  uint8_t audioTaskId;

  status = DspTaskCreate(audioTask, 0, &audioTaskId);
  if (status == -1)
  {
    TRACE("Issue in creating Audio Task:\n\r");
    return;// status;
  }
  bindTasktoRPCModule(RPC_MODULE_ID_AUDIO, audioTaskId);

  return;// status;
}
