#ifndef H_PDM_AUDIO_
#define H_PDM_AUDIO_

#include "reg.h"
#include "dsp_config.h"
#include "module_common.h"
#include "audio_common.h"
#include "utility.h"
#include "eta_csp_mailbox.h"
#include "eta_csp_dma.h"
#include "eta_csp_common_pdm.h"
#include "dsp_fw_cfg.h"
#include "m3Interface.h"
#include "dsp_task.h"
#include "dsp_msg.h"
#include "dsp_malloc.h"

#define AUDIO_DEMO_PDM_DMA_CHANNEL (1)
#define CONFIG_PDM_1_DMA_CHAN (1)
#define CONFIG_PDM_0_DMA_CHAN (0)
#define AUDIO_DEMO_M3_NUM_SAMPLES (240)
#define AUDIO_DEMO_M3_NUM_PKTS (1)
#define  AUDIO_DEMO_MEM_WIN (APP_MEM_WIN)

#define ACFG_INDEX(x) (x < CONFIG_PDM_COUNT ? x : x - 1)

/** PCM channel mode */
typedef enum {
    DISABLE = 0,
    /** mono left */
    MONO_LEFT = 1,
    /** mono right */
    MONO_RIGHT = 2,
    /** stereo */
    STEREO = 1,
}tCMode;


/* PCM Frame Buffer */
typedef struct {
    uint16_t dmaChan;
    uint32_t ahbAdr;
    tAudRPCfg *rAcfg;
} tAudCfg;


#endif
