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
#include "uart_xport.h"
#include "serial_proto.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#define NOT_PENDING (0xFF)

tSPCbFuncs *spOps;

struct txQParam {
    uint8_t mType;
    uint8_t *bPtr;
};

struct txQParam txQP;

static QueueHandle_t txQ = NULL;

void xportSend(uint8_t *xmitBuf, uint16_t xmitLen)
{
  HalUartWriteBuf(CONFIG_XPORT_UART, xmitBuf, xmitLen, NULL, NULL);
}

void uartXportCbHandler(void *arg)
{
  struct txQParam txPara;
  tCmdPkt *cmd;
  tEventPkt *eventPkt;
  tDataPkt *dataPkt;
  tRespPkt *respPkt;
  while (1)
  {
    xQueueReceive(txQ, &txPara, portMAX_DELAY);
    switch (txPara.mType)
    {
      case CMD:
        cmd = (tCmdPkt *)txPara.bPtr;
        if(spOps->CmdHandler)
          spOps->CmdHandler(cmd);
        vPortFree(cmd);
        break;
      case EVENT:
        eventPkt = (tEventPkt *)txPara.bPtr;
        if(spOps->EventHandler)
          spOps->EventHandler(eventPkt);
        vPortFree(eventPkt);
        break;
      case DATA:
        dataPkt = (tDataPkt *)txPara.bPtr;
        if(spOps->DataHandler)
          spOps->DataHandler(dataPkt);
        vPortFree(dataPkt);
        break;
      case CMD_RESP:
        respPkt = (tRespPkt *)txPara.bPtr;
        if (spOps->CmdRespHandler)
          spOps->CmdRespHandler(respPkt);
        vPortFree(respPkt);
      default :

        break;
    }
  }

}


void uartXportRxHandler(void *arg)
{
    tPktHdr pktHdr;
    tDataRespPkt dataResp;
    uint8_t pkt;
    uint16_t len, i;
    uint8_t *buf;
    ecm35xx_printf("Uart Xport Task port %d\r\n",CONFIG_XPORT_UART);
    while (1)
    {
        HalUartReadBuf(CONFIG_XPORT_UART, (uint8_t *)&pktHdr,
                        sizeof(tPktHdr), NULL, NULL);
        pkt = pktHdr.ptype;
        len = pktHdr.plen;
        ecm35xx_printf("recv pkt id %d\r\n", pkt);

        if (pktHdr.sof != SOF)
        {
          ecm35xx_printf("Not valid SOF %d\r\n", pktHdr.sof);
          continue;
        }
        switch(pkt)
        {
          case CMD:
          {
            tCmdPkt *cmd;
            cmd = pvPortMalloc(len);
            HalUartReadBuf(CONFIG_XPORT_UART, (uint8_t *)cmd,
                            len, NULL, NULL);
            if (len > 1)
              cmd->argPtr = (uint8_t *)&cmd[1];
            else
              cmd->argPtr = NULL;

            txQP.mType = CMD;
            txQP.bPtr =  (uint8_t *)cmd;
            xQueueSendToFront(txQ, &txQP, portMAX_DELAY);

            break;
          }
          case  EVENT:
          {
            tEventPkt *eventPkt;
            eventPkt = pvPortMalloc(sizeof(tEventPkt));
            HalUartReadBuf(CONFIG_XPORT_UART, (uint8_t *)eventPkt,
                            sizeof(tEventPkt), NULL, NULL);

            txQP.mType = EVENT;
            txQP.bPtr =  (uint8_t *)eventPkt;
            xQueueSendToFront(txQ, &txQP, portMAX_DELAY);

            break;
          }
          case  CMD_RESP:
          {
            tRespPkt *respPkt;
            respPkt = pvPortMalloc(sizeof(tRespPkt));
            HalUartReadBuf(CONFIG_XPORT_UART, (uint8_t *)respPkt,
                            sizeof(tRespPkt), NULL, NULL);
            txQP.mType = CMD_RESP;
            txQP.bPtr =  (uint8_t *)respPkt;
            xQueueSendToFront(txQ, &txQP, portMAX_DELAY);

            break;
          }
          case DATA:
          {
            tDataPkt *dPkt;
            tDataRespPkt *Resp;
            uint8_t cnt;
            crcType crc;
            uint8_t i = 0;
            static uint8_t pendSeq =  NOT_PENDING;
            static uint8_t pendOffset =  NOT_PENDING;

            buf = pvPortMalloc(len);
            if (buf )
            {
              HalUartReadBuf(CONFIG_XPORT_UART, buf,
                              sizeof(tDataPkt), NULL, NULL);
              dPkt = (tDataPkt *)buf;
              HalUartReadBuf(CONFIG_XPORT_UART, buf + sizeof(tDataPkt),
                              dPkt->pktLen, NULL, NULL);

              crc = calCrc(buf + sizeof(tDataPkt), dPkt->pktLen);
              if (crc == dPkt->pktCrc)
              {
                if ((pendSeq == NOT_PENDING) ||
                    ((pendSeq == dPkt->pktSeq) && (pendOffset == dPkt->pktOffSet)))
                {
                  //spOps->DataHandler(buf);
                  dataResp.respVal = CRC_PASS;
                  dataResp.pktSeq = dPkt->pktSeq;
                  dataResp.pktOffSet = dPkt->pktOffSet;
                  pendSeq = NOT_PENDING;
                  pendOffset = NOT_PENDING;

                  txQP.mType = DATA;
                  txQP.bPtr =  (uint8_t *)buf;
                  xQueueSendToFront(txQ, &txQP, portMAX_DELAY);
                }
                else
                {
                  // drop packet;
                  vPortFree(buf);
                }

              }
              else
              {
                dataResp.pktSeq = dPkt->pktSeq;
                dataResp.pktOffSet = dPkt->pktOffSet;
                dataResp.respVal = CRC_FAIL;
                /* Retranmit request set */
                pendSeq = dPkt->pktSeq;
                pendOffset = dPkt->pktOffSet;
                ecm35xx_printf("CRC Missmatch rexmit request\r\n");

                // drop packet;
                vPortFree(buf);
              }

              if ((dataResp.respVal == CRC_FAIL) || dPkt->resReq)
              {
                Resp = pvPortMalloc(sizeof(tDataRespPkt));
                Resp->pktSeq = dataResp.pktSeq;
                Resp->pktOffSet = dataResp.pktOffSet;
                Resp->respVal = dataResp.respVal;
                sendDataResp(Resp);
                vPortFree(Resp);
              }

            }
            break;
          }
            case DATA_RESP:
          {
            HalUartReadBuf(CONFIG_XPORT_UART, (uint8_t *)&dataResp,
                            sizeof(tDataRespPkt), NULL, NULL);
            DataRespHdlr(&dataResp);
            break;
          }
          default :
            ecm35xx_printf("Inavlid pkt\r\n");

            break;
        }
    }

}

void xportInit (tSPCbFuncs *sOps)
{
	xTaskCreate(uartXportRxHandler, "uartXportRx", configMINIMAL_STACK_SIZE,
                        NULL, tskIDLE_PRIORITY + 2, NULL);

	xTaskCreate(uartXportCbHandler, "uartXportCbH", configMINIMAL_STACK_SIZE,
                        NULL, tskIDLE_PRIORITY + 2, NULL);

  txQ = xQueueCreate(2, sizeof(txQP));
  spOps = sOps;
  return;
}
