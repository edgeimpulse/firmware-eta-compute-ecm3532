#ifndef TB_REGS_H
#define  TB_REGS_H

  #include "eta_chip.h"

  #ifdef FOR_SIMULATION

  // TB Regs Space 0x8900 - 0x89FF
  #define TB_REGS_BASE (0x8900)    // This is an IOMEM addr
  #define TB_REGS_PUTC_ADDR                  TB_REGS_BASE+0x00
  #define TB_REGS_CHECKPT_ADDR               TB_REGS_BASE+0x01
  #define TB_REGS_EXIT_ADDR                  TB_REGS_BASE+0x04
  #define TB_REGS_RTOPT_ADDR                 TB_REGS_BASE+0x08
  #define TB_REGS_RTSEED_ADDR                TB_REGS_BASE+0x09
  #define TB_REGS_NONVOL_ADDR                TB_REGS_BASE+0x0a
  #define TB_REGS_DBG1_ADDR                  TB_REGS_BASE+0x10
  #define TB_REGS_DBG2_ADDR                  TB_REGS_BASE+0x11
  #define TB_REGS_DBG3_ADDR                  TB_REGS_BASE+0x12
  #define TB_REGS_DBG4_ADDR                  TB_REGS_BASE+0x13
  #define TB_REGS_DUMP_TCF_ADDR              TB_REGS_BASE+0x14 
  #define TB_REGS_INC_NUMBER_PASSED_ADDR     TB_REGS_BASE+0x15 
  #define TB_REGS_INC_NUMBER_FAILED_ADDR     TB_REGS_BASE+0x16 

  #define REG_TB_PUTCHAR_ADDRESS         (*(volatile uint16_t chess_storage(IOMEM)*)((TB_REGS_PUTC_ADDR             )))
  #define REG_TB_CHECKPT_ADDRESS         (*(volatile uint16_t chess_storage(IOMEM)*)((TB_REGS_CHECKPT_ADDR          )))
  #define REG_TB_EXIT_CODE               (*(volatile uint16_t chess_storage(IOMEM)*)((TB_REGS_EXIT_ADDR             )))
  #define REG_TB_RUNTIME_OPT             (*(volatile uint16_t chess_storage(IOMEM)*)((TB_REGS_RTOPT_ADDR            )))
  #define REG_TB_RUNTIME_SEED            (*(volatile uint16_t chess_storage(IOMEM)*)((TB_REGS_RTSEED_ADDR           )))
  #define REG_TB_NON_VOL_REG             (*(volatile uint16_t chess_storage(IOMEM)*)((TB_REGS_NONVOL_ADDR           )))
  #define REG_TB_DBG_REG1                (*(volatile uint16_t chess_storage(IOMEM)*)((TB_REGS_DBG1_ADDR             )))
  #define REG_TB_DBG_REG2                (*(volatile uint16_t chess_storage(IOMEM)*)((TB_REGS_DBG2_ADDR             )))
  #define REG_TB_DBG_REG3                (*(volatile uint16_t chess_storage(IOMEM)*)((TB_REGS_DBG3_ADDR             )))
  #define REG_TB_DBG_REG4                (*(volatile uint16_t chess_storage(IOMEM)*)((TB_REGS_DBG4_ADDR             )))
  #define REG_TB_DUMP_TCF                (*(volatile uint16_t chess_storage(IOMEM)*)((TB_REGS_DUMP_TCF_ADDR         )))
  #define REG_TB_INC_NUMBER_PASSED       (*(volatile uint16_t chess_storage(IOMEM)*)((TB_REGS_INC_NUMBER_PASSED_ADDR)))
  #define REG_TB_INC_NUMBER_FAILED       (*(volatile uint16_t chess_storage(IOMEM)*)((TB_REGS_INC_NUMBER_FAILED_ADDR)))

  #define INC_NUM_PASSED REG_TB_INC_NUMBER_PASSED=1;
  #define INC_NUM_FAILED REG_TB_INC_NUMBER_FAILED=1;


  #define I2S_XACTOR_BASE (0x8920)
  #define I2S_XACTOR_CFG_ADDR       I2S_XACTOR_BASE
  #define I2S_XACTOR_LDATA_ADDR     I2S_XACTOR_BASE+0x01
  #define I2S_XACTOR_RDATA_ADDR     I2S_XACTOR_BASE+0x02
  #define I2S_XACTOR_RX_STAT_ADDR   I2S_XACTOR_BASE+0x05

  #define REG_I2S_XACTOR_CFG      (*(volatile uint16_t chess_storage(IOMEM)*)((I2S_XACTOR_CFG_ADDR    )))
  #define REG_I2S_XACTOR_LDATA    (*(volatile uint16_t chess_storage(IOMEM)*)((I2S_XACTOR_LDATA_ADDR  )))
  #define REG_I2S_XACTOR_RDATA    (*(volatile uint16_t chess_storage(IOMEM)*)((I2S_XACTOR_RDATA_ADDR  )))
  #define REG_I2S_XACTOR_RX_STAT  (*(volatile uint16_t chess_storage(IOMEM)*)((I2S_XACTOR_RX_STAT_ADDR)))

  #else // FOR_SIMULATION

  #define REG_TB_PUTCHAR_ADDRESS  
  #define REG_TB_CHECKPT_ADDRESS  
  #define REG_TB_EXIT_CODE        
  #define REG_TB_RUNTIME_OPT      
  #define REG_TB_RUNTIME_SEED     
  #define REG_TB_NON_VOL_REG      
  #define REG_TB_DBG_REG1         
  #define REG_TB_DBG_REG2         
  #define REG_TB_DBG_REG3         
  #define REG_TB_DBG_REG4         
  #define REG_TB_DUMP_TCF         
  #define REG_TB_INC_NUMBER_PASSED
  #define REG_TB_INC_NUMBER_FAILED

  #define INC_NUM_PASSED
  #define INC_NUM_FAILED

  #endif // FOR_SIMULATION

#endif // TB_REGS_H
