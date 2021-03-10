#ifndef H_AUDIO_COMMON_
#define H_AUDIO_COMMON_

#include <stdint.h>
#include "config.h"

/** Processing type */
typedef enum {
    /** raw data*/
    RAW = 0,
    /** process type 1*/
    PTYPE1 = 1,
}tPtype;

/* PCM Frame Buffer */
typedef struct{
    uint16_t Flen;
    uint16_t fPtrHi;
    uint16_t fPtrLo;
} tAudFBuf;

/* PCM Buffer Descriptor */
typedef struct {
    uint16_t fCnt;
    tAudFBuf sFbuf;
} tAudioBufInfo;

typedef struct {
    /** pdm channel number 0/1 */
    uint16_t pdmNum;
    /** channel mode 0/1 for mono or stereo */
    uint16_t cMode;
    /** processing type */
    uint16_t pType;
    /** output buffer  */
    tAudioBufInfo bInfo;
}tAudRPCfg;

//Audio RPC MSG TYPES (8 bits)
#define AUDIO_INIT 0x01
#define AUDIO_START  0x02
#define AUDIO_STOP 0x03
#define AUDIO_DEINIT 0x0

#define EVENT_DMA_BUF_ID (1)
/*
 *
 * HEADER FORMAT
 *
 * D[31-24]     D[23-16]       D[15-8]      D[7-5]        D4 D3        D2         D1 D0
 * -------------------------- ------------- ------------ ------------ ---------- -----------
 *  |        RSVD            | EVENT DATA |  EVENT TYPE ||  PDM ID |EVNT/RSP |  MODULE ID |
 * -------------------------- ------------- ------------ ------------ ---------- -----------
 *
 *
 */

#define AUDIO_PDM_ID_START_POS    3
#define AUDIO_PDM_ID_NUM_BITS     2

#define GET_AUDIO_PDM_ID(X) (GETBITS(X,AUDIO_PDM_ID_START_POS, AUDIO_PDM_ID_NUM_BITS) )
#define SET_AUDIO_PDM_ID(X,pdmId) (WRITETO(X,AUDIO_PDM_ID_START_POS,AUDIO_PDM_ID_NUM_BITS,pdmId))

#define EVENT_AUDIO_TYPE_NUM_BITS     3
#define EVENT_AUDIO_TYPE_START_POS    5

#define GET_AUDIO_EVENT_TYPE(X) (GETBITS(X,EVENT_AUDIO_TYPE_START_POS,EVENT_AUDIO_TYPE_NUM_BITS) )
#define SET_AUDIO_EVENT_TYPE(X,eventType) (WRITETO(X,EVENT_AUDIO_TYPE_START_POS,EVENT_AUDIO_TYPE_NUM_BITS,eventType))

#define EVENT_AUDIO_DATA_NUM_BITS     8
#define EVENT_AUDIO_DATA_START_POS    8

#define GET_AUDIO_EVENT_DATA(X) (GETBITS(X,EVENT_AUDIO_DATA_START_POS,EVENT_AUDIO_DATA_NUM_BITS))
#define SET_AUDIO_EVENT_DATA(X,evd) (WRITETO(X,EVENT_AUDIO_DATA_START_POS,EVENT_AUDIO_DATA_NUM_BITS,evd))

#endif  //H_AUDIO_COMMON_
