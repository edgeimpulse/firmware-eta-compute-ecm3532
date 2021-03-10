// Common types, etc.
#include "reg.h"

// chip level simulatio test bench
#include "tb_regs.h"

// Chip specific registers
#include "reg_eta_ecm3532_dsp.h"


// NXP
#include "api-dma_controller.h"
#include "api-int_controller.h"

// MATHLIB
//#include "CoolFlux_defs.h"
//#include "mathlib.h"


// Big switch between simulation and validation
//#define FOR_SIMULATION

// Big switch between chips
#define ETA_ECM3532




//////////////////////////////////////////////////////////////////////
// Some non-ETA structures that needed hardcoded address
#define DMA_CONTROLLER_ADDRESS 0x8300
#define INT_CONTROLLER_ADDRESS 0x8000
// Some non-ETA structures that needed hardcoded address
//////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//// XMEM and YMEM depth are in bytes
//// PMEM is in bytes.
//// and 2048 bytes of stack.
//#define XMEM_DEPTH (unsigned long) 32768
//#define YMEM_DEPTH (unsigned long) 32768
//#define PMEM_DEPTH (unsigned long) 32768
//#define STACK_SIZE (unsigned long) 2048
//// 
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//// Test exit codes
//#define TEST_PASS (0)
//#define TEST_FAIL (-1)
//// Test exit codes
////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Memory / Register access
#define IOREG(xx) xx
#define YMEM(x)  *((volatile int chess_storage(YMEM)*)(x))
#define XMEM(x)  *((volatile int chess_storage(XMEM)*)(x))
#define IOMEM(x) *((volatile int chess_storage(IOMEM)*)(x))
// Memory / Register access
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// These are fixed addresses for the DSP memories as seen from AHB
#define DSP_PMEM_START (0x40840000)
#define DSP_XMEM_START (0x40880000)
#define DSP_YMEM_START (0x408c0000)
// These are fixed addresses for the DSP memories as seen from AHB
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// AHB Window

#define  AHB_WINDOW0_IOMEM_ADDR      (0*8192UL)   // This points to AHB_WINDOW0
#define  AHB_WINDOW1_IOMEM_ADDR      (1*8192UL)   // This points to AHB_WINDOW1
#define  AHB_WINDOW2_IOMEM_ADDR      (2*8192UL)   // This points to AHB_WINDOW2
#define  AHB_WINDOW3_IOMEM_ADDR      (3*8192UL)   // This points to AHB_WINDOW3
#define  AHB_WINDOW_IOMEM_ADDR(IDX)  (IDX*8192UL) // This points to AHB_WINDOW0-3

/***************************************************************************//**
 *
 *  MAP_BYTE_TO_AHBWINDOW - Convert Byte Address to value needed to be programmed in AHB Window.
 *
 *  @param byte - AHB byte address (typically 32 bit)
 *
 *  example:   eta_csp_set_ahb_window(0,MAP_BYTE_TO_AHBWINDOW(0x10002FEA));
 *
 ******************************************************************************/
#ifdef ETA_ECM3532
  #define  MAP_BYTE_TO_AHBWINDOW(byte) (byte)
#else // 3531
  #define  MAP_BYTE_TO_AHBWINDOW(byte) (byte & 0xffffc000UL)
#endif


/***************************************************************************//**
 *
 *  MAP_BYTE_AHBWIN_TO_IOMEM - Convert Byte Address, AHB window Number and Value in AHB Window Config to IOMEM address.
 *
 *  @param byte - AHB byte address (typically 32 bit)
 *  @param ahbwin - DSP AHB Window Number (0-3)
 *  @param ahbbyte - Value programmed in AHB Window
 *
 *  example:
 *      // This sets AHB Config (in 3531 mode) to 0x1000_0000 (i.e. 16KB aligned)
 *      eta_csp_set_ahb_window(0,MAP_BYTE_TO_AHBWINDOW(0x10002000));
 *
 *      dma_cmd.src_target=DMA_IOMEM_WINDOW;
 *      // This sets src_addr = ((((0x10002000 & 0xffffc000) - 0x1000_0000)/2) + ((0x10002000 & 0x3fff)/2) + (8192 * 0))
 *      //           src_addr = ((((0x1000_0000              - 0x1000_0000)/2) + (0x2000 /2) )
 *      //           src_addr = ((                                             + (0x1000   ) ) = 0x1000 halfword IOMEM
 *      dma_cmd.src_address=(unsigned int) MAP_BYTE_AHBWIN_TO_IOMEM(0x10002000, 0, MAP_BYTE_TO_AHBWINDOW(0x10002000));
 *
 *      dma_cmd.dst_target=DMA_IOMEM_WINDOW;
 *      // This sets src_addr = ((((0x10003000 & 0xffffc000) - 0x1000_0000)/2) + ((0x10003000 & 0x3fff)/2) + (8192 * 0))
 *      //           src_addr = ((((0x1000_0000              - 0x1000_0000)/2) + (0x3000 /2) )
 *      //           src_addr = ((                                             + (0x1800   ) ) = 0x1800 halfword IOMEM
 *      dma_cmd.dst_address=(unsigned int) MAP_BYTE_AHBWIN_TO_IOMEM(0x10003000, 0, MAP_BYTE_TO_AHBWINDOW(0x10002000));
 *
 ******************************************************************************/

#ifdef ETA_ECM3532
  #define  MAP_BYTE_AHBWIN_TO_IOMEM(byte, ahbwin, ahbbyte) (( (byte                 - ahbbyte)/2) + (8192UL * ahbwin))
#else // 3531
  #define  MAP_BYTE_AHBWIN_TO_IOMEM(byte, ahbwin, ahbbyte) ((((byte & 0xffffc000UL) - ahbbyte)/2) + ((byte & 0x3fffUL)/2) + (8192UL * ahbwin))
#endif



// AHB Window
//////////////////////////////////////////////////////////////////////


