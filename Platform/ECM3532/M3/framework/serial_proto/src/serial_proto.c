/*******************************************************************************
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
#include "serial_proto.h"
#include "uart_xport.h"

struct serProto
{
  SemaphoreHandle_t xSPLock;
  SemaphoreHandle_t xRespLock;
  uint8_t st;
  uint8_t pendResp;
  tDataRespPkt dataResp ;
};
/* current state */
enum spSt {
  DONE,
  CMD_ST,
  RESP_ST,
  EVENT_ST,
  DATA_ST,
};

struct serProto spDev;

/**
 * form and send command
 *
 * @param cmdVal command value
 * @param argCnt number of arguments
 * @param argCnt argArray argument arrary pointer
 * @return 0 on success, non-zero error code on failure
 *
 */
int sendCmd(uint8_t cmdVal, uint8_t argCnt, uint8_t *argArray)
{
  uint8_t bufLen, cmdLen, i;
  uint8_t *buf, *cmdPtr;
  tPktHdr *Pkt;

  cmdLen =  1 + argCnt;
  bufLen = (sizeof(tPktHdr)) + cmdLen;

  buf = pvPortMalloc(bufLen);

  if (buf)
  {
    xSemaphoreTake(spDev.xSPLock, portMAX_DELAY);
    spDev.st = CMD_ST;
    Pkt = (tPktHdr *)buf;
    Pkt->sof = SOF;
    Pkt->ptype = CMD;
    Pkt->plen = cmdLen;

    cmdPtr = buf + sizeof(tPktHdr);

    cmdPtr[0] = cmdVal;
    for (i = 0; i < argCnt; i++)
      cmdPtr[i + 1] =  argArray[i];

    xportSend(buf, bufLen);
    spDev.st = DONE;
    xSemaphoreGive(spDev.xSPLock);
    vPortFree(buf);
  }

  return 0;
}

/**
 * Send response of data packet
 *
 * @param resp response packer
 * @return 0 on success, non-zero error code on failure
 *
 */
int sendDataResp(tDataRespPkt *resp)
{
  tPktHdr pHdr;
  xSemaphoreTake(spDev.xSPLock, portMAX_DELAY);
  ecm35xx_printf("Send Data resp\r\n");

  pHdr.sof = SOF;
  pHdr.ptype = DATA_RESP;
  pHdr.plen = sizeof(tDataRespPkt);

  xportSend((uint8_t *)&pHdr, sizeof(tPktHdr));
  xportSend((uint8_t *)resp, sizeof(tDataRespPkt));
  xSemaphoreGive(spDev.xSPLock);
  return 0;
}

/**
 * form and send Response of command
 *
 * @param event value
 * @param val event data
 * @return 0 on success, non-zero error code on failure
 *
 */
int sendCmdResp(uint8_t cmd, uint32_t val)
{
  tRespPkt *respPkt;
  tPktHdr *pktHdr;
  uint8_t *buf;
  uint8_t bufLen;

  bufLen = sizeof(tPktHdr) +  sizeof(tRespPkt);
  buf = pvPortMalloc(bufLen);
  if (buf)
  {
    xSemaphoreTake(spDev.xSPLock, portMAX_DELAY);
    spDev.st = RESP_ST;
    pktHdr = (tPktHdr *)buf;
    pktHdr->sof = SOF;
    pktHdr->ptype = CMD_RESP;
    pktHdr->plen = sizeof(tRespPkt);

    respPkt = (tRespPkt *)(buf + sizeof(tPktHdr));
    respPkt->cmd = cmd;
    respPkt->resp = val;
    xportSend(buf, bufLen);
    spDev.st = DONE;
    xSemaphoreGive(spDev.xSPLock);
    vPortFree(buf);
  }
  return 0;
}

/**
 * form and send Event
 *
 * @param event value
 * @param val event data
 * @return 0 on success, non-zero error code on failure
 *
 */
int sendEvent(uint8_t event, uint32_t val)
{
  tEventPkt *eventPkt;
  tPktHdr *pktHdr;
  uint8_t *buf;
  uint8_t bufLen;

  bufLen = sizeof(tPktHdr) +  sizeof(tEventPkt);
  buf = pvPortMalloc(bufLen);
  if (buf)
  {
    xSemaphoreTake(spDev.xSPLock, portMAX_DELAY);
    spDev.st = EVENT_ST;
    pktHdr = (tPktHdr *)buf;
    pktHdr->sof = SOF;
    pktHdr->ptype = EVENT;
    pktHdr->plen = sizeof(tEventPkt);

    eventPkt = (tEventPkt *)(buf + sizeof(tPktHdr));
    eventPkt->event = event;
    eventPkt->val = val;
    xportSend(buf, bufLen);
    spDev.st = DONE;
    xSemaphoreGive(spDev.xSPLock);
    vPortFree(buf);
  }
  return 0;
}

/** Data packet Ack/Nack */
void  DataRespHdlr (tDataRespPkt *dataRespPkt)
{
  if (spDev.st == DATA_ST)
  {
      spDev.dataResp.pktSeq = dataRespPkt->pktSeq;
      spDev.dataResp.pktOffSet = dataRespPkt->pktOffSet;
      spDev.dataResp.respVal = dataRespPkt->respVal;
      spDev.pendResp = 1;
      xSemaphoreGive(spDev.xRespLock);
  }

}
/** Packet Fragmentation */
static int sendDataPkt(uint8_t *buf, uint8_t pktLen, bool waitForRsp,
                          uint8_t seq, uint8_t pktOffSet)
{
  tDataPkt *dataPkt;
  tPktHdr *pktHdr;
  uint8_t *hdr;
  uint8_t hdrLen;
#ifdef CONFIG_DATA_CRC_CHECK
  crcType crc;
  crc = calCrc(buf, pktLen);
#endif
  hdrLen = sizeof(tPktHdr) +  sizeof(tDataPkt);
  hdr = pvPortMalloc(hdrLen);
  if (hdr)
  {
    pktHdr = (tPktHdr *)hdr;
    pktHdr->sof = SOF;
    pktHdr->ptype = DATA;
    pktHdr->plen = pktLen + sizeof(tDataPkt);

    dataPkt = (tDataPkt *)(hdr + sizeof(tPktHdr));
    dataPkt->pktLen = pktLen;
    dataPkt->resReq = waitForRsp;
    dataPkt->pktSeq = seq;
    dataPkt->pktOffSet = pktOffSet;
#ifdef CONFIG_DATA_CRC_CHECK
    dataPkt->pktCrc = crc;
#endif
    xportSend(hdr, hdrLen);
    xportSend(buf, pktLen);
    vPortFree(hdr);
  }
  return 0;
}


/**
 * form and send Data packet
 *
 * @param buf data pointer
 * @param dataLen data length
 * @return 0 on success, non-zero error code on failure
 *
 */
int sendData(uint8_t *buf, uint16_t dataLen)
{
  uint16_t pktLen, pktCnt, pktOffset = 0;
  static uint16_t pktSeq = 0;
  uint8_t reXmit = 0, mod;
  uint8_t *bufIdxPtr;
  bool waitForRsp = 0;

  if (!buf || !dataLen)
    return -EINVAL;

  mod  = dataLen % CONFIG_DATA_PKT_LEN;
  pktCnt  =  dataLen / CONFIG_DATA_PKT_LEN;

  /**
   * last packet to be xmit will be == mod
   */
  if (mod)
  {
    pktCnt++;
  }
  else
  {
  /**
   * last packet to be xmit will be always mod, assing moe to packet lenght
   * when mod is actually zero, to have come code for both case
   */
    mod = CONFIG_DATA_PKT_LEN;
  }

  xSemaphoreTake(spDev.xSPLock, portMAX_DELAY);
  spDev.st = DATA_ST;

  do
  {
    if (dataLen > CONFIG_DATA_PKT_LEN)
    {
      pktLen =  CONFIG_DATA_PKT_LEN;
    }
    else
    {
      pktLen =  dataLen;
#ifdef CONFIG_DATA_CRC_CHECK
      /* last packet in seq, request for response */
      waitForRsp = 1;
#endif
    }
    bufIdxPtr = buf + (pktOffset * CONFIG_DATA_PKT_LEN);
    sendDataPkt(bufIdxPtr, pktLen, waitForRsp, pktSeq, pktOffset++);
#ifdef CONFIG_DATA_CRC_CHECK
    if (waitForRsp)
    {
      /*Sync Respose of Datapacket */
      xSemaphoreTake(spDev.xRespLock, portMAX_DELAY);
      if ((spDev.dataResp.respVal == CRC_FAIL))
      {
        reXmit = 1;

      }

    }
    /*Asysc Respose of Datapacket, check if response is recv without waiting for
     * it
     * */
    else  if (xSemaphoreTake(spDev.xRespLock, 0) == pdTRUE)
    {
      if ((spDev.pendResp) && (spDev.dataResp.respVal == CRC_FAIL))
      {
        ecm35xx_printf("Asysc Resp recv for DATA Pkt\r\n");
        reXmit = 1;

      }
    }

    if (reXmit && (spDev.dataResp.pktOffSet < pktCnt))
    {
      pktOffset = spDev.dataResp.pktOffSet;
      reXmit = 0;
      spDev.pendResp = 0;
    }

#endif
    dataLen =  ((pktCnt - (pktOffset + 1)) * CONFIG_DATA_PKT_LEN) + mod;
  } while (pktOffset <  pktCnt);

  spDev.st = DONE;
  ecm35xx_printf("sent data pkt seq %d \r\n", pktSeq);
  pktSeq++;
  xSemaphoreGive(spDev.xSPLock);
  return  0;
}

/**
 * Initialize Serial protocol
 *
 * @param sOps functions poniiters for serial protocol operation
 * on recevied packet
 * @return 0 on success, non-zero error code on failure
 *
 */
int serialProtoInit(tSPCbFuncs *sOps)
{
  spDev.xSPLock = xSemaphoreCreateMutex();
#ifdef CONFIG_DATA_CRC_CHECK
  spDev.xRespLock = xSemaphoreCreateBinary();
  crcInit();
#endif
  xportInit(sOps);
  return 0;
}

/*TODO*/
int serialProtoDeInit(uint8_t xportType)
{
  return 0;
}
