#ifndef ETA_DIAGS_H
#define  ETA_DIAGS_H

#include "eta_chip.h"
#include "eta_csp.h"

#ifdef FOR_SIMULATION

  #define ETA_DIAGS_PUTS(STR)             etaPuts(STR)
  #define ETA_DIAGS_PRINTF(format, ...)   etaPrintf(format, ##__VA_ARGS__ )

  #define ETA_DIAGS_END_TEST(exit_value) \
                    REG_TB_EXIT_CODE=exit_value;

  #define ETA_DIAGS_PROGRESS(a) // Do nothing

  #define ETA_DIAGS_BEGIN_MAIN() \
                    REG_DSPCTRL_DSP_DSP_STACK_DEBUG0 = 0xffff; \
                    REG_DSPCTRL_DSP_DSP_STACK_DEBUG0 = 0x400;  \

  #define ETA_DIAGS_END_MAIN()   \
                    ETA_DIAGS_PRINTF ("Max Stack Usage: %d bytes\r\n", REG_DSPCTRL_DSP_DSP_STACK_DEBUG1<<1); \
                    ETA_DIAGS_PRINTF ("Max Hold Detect: %d cycles\r\n", REG_DSPCTRL_DSP_DSP_HOLD_DEBUG0 & BM_DSPCTRL_DSP_DSP_HOLD_DEBUG0_HOLD_COUNT); \

#else // FOR_SIMULATION

  #define ETA_DIAGS_PUTS(STR)             etaPuts(STR)
  #define ETA_DIAGS_PRINTF(format, ...)   etaPrintf(format, ##__VA_ARGS__ )

  #define ETA_DIAGS_END_TEST(exit_value) \
                        EtaCspFinish(exit_value); \
                        ETA_DIAGS_PRINTF ("DSP says end of test with exit code: %x\r\n", exit_value); \
                        while (1); // loop forever (keeps interrupts going)

  #define ETA_DIAGS_PROGRESS(a) // Do nothing

  #define ETA_DIAGS_BEGIN_MAIN() \
                    REG_DSPCTRL_DSP_DSP_STACK_DEBUG0 = 0xffff; \
                    REG_DSPCTRL_DSP_DSP_STACK_DEBUG0 = 0x400;  \
                    eta_csp_enable_verbose(); \
                    eta_csp_init_print_via_mbox();
 
                    // removed because its hard to tell if test rerun ... ETA_DIAGS_PRINTF("\033[2J\033[H");

  #define ETA_DIAGS_END_MAIN()   \
                    ETA_DIAGS_PRINTF ("Max Stack Usage: %d bytes\r\n", (REG_DSPCTRL_DSP_DSP_STACK_DEBUG1<<1) & 0xffff); \
                    ETA_DIAGS_PRINTF ("Max Hold Detect: %d cycles\r\n", REG_DSPCTRL_DSP_DSP_HOLD_DEBUG0 & BM_DSPCTRL_DSP_DSP_HOLD_DEBUG0_HOLD_COUNT); \
                    eta_csp_wait_for_print_credits(); 


#endif // FOR_SIMULATION

#endif // ETA_DIAGS_H
