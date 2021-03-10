#include "config.h"
#define STR2(x) #x
#define STR(x) STR2(x)
#ifndef CONFIG_BOOTLOADER_APP
#define BIN_SEC CONFIG_BIN_FILE_SECTION
#else
#define BIN_SEC CONFIG_PBUF_FILE_SECTION
#endif
#define BIN_SEC_DSP CONFIG_DSP_FILE_SECTION

#define INCBIN_ETA_DSP()  INCBINDSP(CONFIG_PRE_BUILD_DSP_FW_RELOCATED)

#define INCBIN(name, file) \
     __asm__(".section " BIN_SEC ",\"ax\"\n" \
            ".global incbin_" STR(name) "_start\n" \
            ".type incbin_" STR(name) "_start, %object\n" \
            ".balign 16\n" \
            "incbin_" STR(name) "_start:\n" \
            ".incbin \"" file "\"\n" \
            \
            ".global incbin_" STR(name) "_end\n" \
            ".type incbin_" STR(name) "_end, %object\n" \
            ".balign 1\n" \
            "incbin_" STR(name) "_end:\n" \
            ".byte 0\n" \
    ); \
    extern const  __attribute__((section("BIN_SEC"))) __attribute__((aligned(16))) void* incbin_ ## name ## _start; \
    extern const void*  __attribute__((section("BIN_SEC"))) incbin_ ## name ## _end;

#define INCBINDSP(file) \
     __asm__(".section " BIN_SEC_DSP ",\"ax\"\n" \
            ".global incbin_dsp_start\n" \
            ".type incbin_dsp_start, %object\n" \
            "incbin_dsp_start:\n" \
            ".incbin \"" file "\"\n" \
            \
            ".global incbin_dsp_end\n" \
            ".type incbin_dsp_end, %object\n" \
            ".balign 1\n" \
            "incbin_dsp_end:\n" \
            ".byte 0\n" \
    ); \
    extern const  __attribute__((section("BIN_SEC_DSP"))) __attribute__((aligned(16))) void* incbin_dsp_start; \
    extern const void*  __attribute__((section("BIN_SEC_DSP"))) incbin_dsp_end;
