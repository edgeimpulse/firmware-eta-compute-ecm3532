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
#ifndef H_SERIAL_PROTO_
#define H_SERIAL_PROTO_
#include "config.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "gpio_hal.h"
#include "uart_hal.h"
#include "print_util.h"
#include "crc.h"
#include "errno.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SOF (0xAB)

/**
 * Serial communication packet types
 */
typedef enum
{
  /** command packet */
  CMD = 1,
  /** command response */
  CMD_RESP,
  /** async event packet*/
  EVENT,
  /** data packet*/
  DATA,
  /** data packet ACK or NACK*/
  DATA_RESP,

} tPktType;

enum DataPktResp {
  CRC_PASS,
  CRC_FAIL,
};


/**
 * Serial communication packet header
 */
typedef struct pktHdr
{
  /** start of frame*/
  uint8_t sof;
  /** packet type*/
  uint8_t ptype;
  /** payload length*/
  uint8_t plen;
} tPktHdr;

/**
 * command packet header
 */
typedef struct
{
  /** command byte*/
  uint8_t cmd;
  /** argument pointer*/
  uint8_t *argPtr;
} tCmdPkt;

/**
 * command response packet
 */
typedef struct
{
  /** command byte*/
  uint8_t cmd;
  /** response value*/
  uint32_t resp;
} tRespPkt;

/**
 * Event packet
 */
typedef struct
{
  /** event type*/
  uint8_t event;
  /** event data*/
  uint32_t val;
} tEventPkt;


/**
 * Data packet
 */
typedef struct
{
  /** packet sequence*/
  uint8_t pktSeq;
  /** packet offset with in sequence*/
  uint16_t pktOffSet;
  /** packet length in sequence*/
  uint8_t pktLen;
  /** response request data pkt*/
  uint8_t resReq;
  /** CRC of pkt sequence*/
  crcType pktCrc;
} tDataPkt;

/**
 * Data packet Response
 */
typedef struct
{
  uint8_t pktSeq;
  uint8_t pktOffSet;
  uint8_t respVal;
} tDataRespPkt;


/**
 * form and send Command packet
 *
 * @param cmdVal command value
 * @param argCnt agument count
 * @param argArray agument pointer
 * @return 0 on success, non-zero error code on failure
 *
 */
int sendCmd(uint8_t cmdVal, uint8_t argCnt, uint8_t *argArray);

/**
 * form and send command Response packet
 *
 * @param cmdVal command value
 * @param respVal response value
 * @return 0 on success, non-zero error code on failure
 *
 */
int sendCmdResp(uint8_t cmdVal, uint32_t respVal);

/**
 * form and send Event packet
 *
 * @param event Event type
 * @param eventVal Event value
 * @return 0 on success, non-zero error code on failure
 *
 */
int sendEvent(uint8_t event, uint32_t eventVal);

/**
 * form and send Data packet
 *
 * @param buf data pointer
 * @param dataLen data length
 * @return 0 on success, non-zero error code on failure
 *
 */
int sendData(uint8_t *buf, uint16_t dataLen);

/**
 * Send response of data packet
 *
 * @param resp response packer
 * @return 0 on success, non-zero error code on failure
 *
 */
int sendDataResp(tDataRespPkt *resp);

/**
 * Function prototype for Command Packet Handler
 */
typedef void (*tCmdPktHdlr)(tCmdPkt *cmdPtr);

/**
 * Function prototype for Command Response Packet Handler
 */
typedef void (*tCmdRespHdlr)(tRespPkt *respPkt);

/**
 * Function prototype for Event Packet Handler
 */
typedef void (*tEventHdlr)(tEventPkt *evPkt);

/**
 * Function prototype for Data Packet Handler
 */
typedef void (*tDataPktHdlr)(tDataPkt *dataPkt);


/**
 * serial protcol callback function structure
 */
typedef struct
{
  tCmdPktHdlr CmdHandler;
  tCmdRespHdlr CmdRespHandler;
  tEventHdlr EventHandler;
  tDataPktHdlr DataHandler;
} tSPCbFuncs;

/**
 * Initialze Serial protocol layer, Xport Layer
 *
 * @param sSPCb serial protcol callback function structure pointer
 * @return 0 on success, non-zero error code on failure
 *
 */
int serialProtoInit(tSPCbFuncs *sSPCb);


#ifdef __cplusplus
}
#endif

#endif
