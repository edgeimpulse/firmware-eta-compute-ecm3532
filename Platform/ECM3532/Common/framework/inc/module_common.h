//////////////////////////////////////////////////////////////////////////
// The confidential and proprietary information contained in this file may
// only be used by a person authorized under and to the extent permitted
// by a subsisting licensing agreement from Eta Compute Inc.
//
//        (C) Copyright 2019 Eta Compute Inc
//                All Rights Reserved
//
// This entire notice must be reproduced on all copies of this file
// and copies of this file may only be made by a person if such person is
// permitted to do so under the terms of a subsisting license agreement
// from Eta Compute Inc.
//////////////////////////////////////////////////////////////////////////
#ifndef H_MODULE_COMMON_
#define H_MODULE_COMMON_

#define MAX_MODULE_ID  (1 << MODULE_ID_NUM_BITS)
//defines for various processing request offloaded to dsp  //8 bit
#define RPC_MODULE_ID_EXECUTOR  0x01
#define RPC_MODULE_ID_DEBUG     0x00  //Reserved for debug print communication.
#define RPC_MODULE_ID_SENSOR    0x02
#define RPC_MODULE_ID_AUDIO     0x03

#define RPC_EVENT               0x1
#define RPC_RESPONSE            0x0

 /*HEADER

D[31-24]     D[23-16]       D[15-8]    D[7-5]    D4 D3         D2         D1 D0
-----------  ------------ ------------------- --------------- ----------- - ----
| RSVD     |EVENT TYPE   |SENSOR TYPE | RSVD |HW SENSOR ID| EVNT/RSP |MODULE ID|
------------------------- ----------------------------------------- ------------
 ---------USER DEFINED-------------------------------------|----COMMON-----------
*/
#define LASTBITS(VAL,NBITS) ((VAL) & ((1<<(NBITS))-1))  // Last N bits of VAL
#define GETBITS(VAL,POS,NBITS) LASTBITS(((VAL)>>(POS)),NBITS) // Get N bits of VAL starting bit position POS : position starts from 0

#define GETMASK(POS, NBITS) (((1 << (NBITS)) - 1) << (POS))
// Extract N bits from  VAL and move them to variable DATA Starting from bit position POS
#define WRITETO(DATA, POS, NBITS, VAL) ((DATA) = ((DATA) & (~GETMASK((POS), (NBITS)))) | ((LASTBITS(VAL,NBITS)) << (POS)))

//Not using bit fields for portability across compilers
#define MODULE_ID_NUM_BITS          2
#define MODULE_ID_BIT_START_POS     0

#define GET_MODULEID(X) (GETBITS(X,MODULE_ID_BIT_START_POS,MODULE_ID_NUM_BITS) )
#define SET_MODULEID(X,moduleID) (WRITETO(X,MODULE_ID_BIT_START_POS,MODULE_ID_NUM_BITS,moduleID))

#define EVT_RSP_NUM_BITS          1
#define EVT_RSP_BIT_START_POS     2

#define GET_EVT_RSP(X) (GETBITS(X,EVT_RSP_BIT_START_POS,EVT_RSP_NUM_BITS) )
#define SET_EVT_RSP(X,evtRsp) (WRITETO(X,EVT_RSP_BIT_START_POS,EVT_RSP_NUM_BITS,evtRsp))

#endif  //H_MODULE_COMMON_
