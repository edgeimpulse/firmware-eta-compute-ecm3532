

#include "ioreg_tb.h"
#include "ioreg_fpga.h"

#ifndef ETA_DIAGS_MAX10
#define ETA_DIAGS_MAX10

// SPI Slave commands
#define MAX10_SPISLV_CMD_WRITE (0x0 << 4)
#define MAX10_SPISLV_CMD_READ  (0x1 << 4)


////////////////////////////////////////////////////////////////////////////////
// function prototypes for general MAX10 comm
////////////////////////////////////////////////////////////////////////////////

int max10_spislave_poll_until_rxhasdata(void);
int max10_spislave_poll_until_txrdyfordata(void);
int max10_spislave_poll_until_done(void);
int max10_spislave_initialize(void);
int max10_spislave_disable_cs(void);

int max10_spislave_addr_poll_status(uint8_t cmd, uint16_t addr);
int max10_spislave_rmw(uint16_t addr, uint32_t wrdata, uint32_t enable_newbit);
int max10_spislave_write(uint16_t addr, uint32_t wrdata);
int max10_spislave_read(uint16_t addr, uint32_t * rddata);
uint32_t max10_spislave_read_imm(uint16_t addr);

int max10_spislave_enable_nocs_mode(void);
int max10_spislave_disable_nocs_mode(void);

void ecm3532_max10_finish(uint32_t ui32RC);




////////////////////////////////////////////////////////////////////////////////
// function prototypes for pwm check
////////////////////////////////////////////////////////////////////////////////
uint32_t max10_setup_pwm(uint8_t ch_mask);
uint32_t max10_parse_data_pwm(bool exp_full, bool init_chk);
uint32_t max10_chk_pwm(uint8_t pwm_sel, uint32_t exp_per, uint32_t exp_hi, uint32_t tolerence);
uint32_t max10_get_pwm_hi_count(uint8_t pwm_sel);
uint32_t max10_get_pwm_lo_count(uint8_t pwm_sel);
uint32_t max10_get_pwm_first_hi_interval(uint8_t pwm_sel);
uint32_t max10_get_pwm_period_ignore_first_pulse(uint8_t pwm_sel);
uint32_t analyze_pwmchk(uint8_t pwmchk_select, uint8_t from_edge, uint8_t to_edge, uint8_t request, bool first_only, bool remove_first);

////////////////////////////////////////////////////////////////////////////////
// function prototypes gen_fifo
////////////////////////////////////////////////////////////////////////////////
// These functions use a global variable to store the space available / data available.
// That way, you don't need to check for more space/data until you run out.
extern uint16_t glob_gen_fifo0_space_to_push;
extern uint16_t glob_gen_fifo0_data_to_pop;

void max10_gen_fifo_init_efficient(void);
void max10_gen_fifo_push_efficient(uint32_t wrddata);
void max10_gen_fifo_pop_efficient(uint32_t *rddata);

////////////////////////////////////////////////////////////////////////////////
// Low Power Settings
////////////////////////////////////////////////////////////////////////////////
void max10_lower_power_now(void);
void max10_lower_power_dip7(void);
void max10_lower_power_dip7_pb1(void);
int  max10_stop_driving_gpio (void);


////////////////////////////////////////////////////////////////////////////////
// Useful Commands
////////////////////////////////////////////////////////////////////////////////
#define max10_spislave_trigger_fpga_reset() max10_spislave_write(IO_FPGA_RESET_POR_CTRL_WRITE_ADDRESS,0xdead0001);
#define max10_spislave_trigger_por()        max10_spislave_write(IO_FPGA_RESET_POR_CTRL_WRITE_ADDRESS,0xdead0002);
#define max10_spislave_trigger_reset()      max10_spislave_write(IO_FPGA_RESET_POR_CTRL_WRITE_ADDRESS,0xdead0004);
#define max10_spislave_trigger_wake()       max10_spislave_write(IO_FPGA_RESET_POR_CTRL_WRITE_ADDRESS,0xdead0008);







////////////////////////////////////////////////////////////////////////////////
// SPI2C Bus Decodes
////////////////////////////////////////////////////////////////////////////////
#define SPI2C_MUXSEL_0to9(a,b,c,d,e,f,g,h,i,j) \
  (j << IO_FPGA_SPI2C_MUX_SEL_SPI2C_9_SEL_SHIFT) |  \
  (i << IO_FPGA_SPI2C_MUX_SEL_SPI2C_8_SEL_SHIFT) |  \
  (h << IO_FPGA_SPI2C_MUX_SEL_SPI2C_7_SEL_SHIFT) |  \
  (g << IO_FPGA_SPI2C_MUX_SEL_SPI2C_6_SEL_SHIFT) |  \
  (f << IO_FPGA_SPI2C_MUX_SEL_SPI2C_5_SEL_SHIFT) |  \
  (e << IO_FPGA_SPI2C_MUX_SEL_SPI2C_4_SEL_SHIFT) |  \
  (d << IO_FPGA_SPI2C_MUX_SEL_SPI2C_3_SEL_SHIFT) |  \
  (c << IO_FPGA_SPI2C_MUX_SEL_SPI2C_2_SEL_SHIFT) |  \
  (b << IO_FPGA_SPI2C_MUX_SEL_SPI2C_1_SEL_SHIFT) |  \
  (a << IO_FPGA_SPI2C_MUX_SEL_SPI2C_0_SEL_SHIFT)


typedef enum
{
    //
    //  L=loopback, Chipsel is for Chipsel, Tst is test bus     L     L  ChipSel  Tst
    //                                                        0 1 2 3 4  5 6 7 8  9
    eTpaSPI2CSelectNothing                = SPI2C_MUXSEL_0to9(3,3,3,3,3, 3,3,3,3, 3),

    // Direct to SPI Flash
    eTpaSPI2CSelectSpi0Cs0SpiFlash        = SPI2C_MUXSEL_0to9(0,3,3,0,3, 0,3,3,3, 0),
    eTpaSPI2CSelectSpi0Cs1SpiFlash        = SPI2C_MUXSEL_0to9(0,3,3,0,3, 3,3,0,3, 0),
    // skip SPI0 CS2 because that is MAX10 communication channel
    eTpaSPI2CSelectSpi0Cs3SpiFlash        = SPI2C_MUXSEL_0to9(0,3,3,0,3, 3,3,3,0, 0),

    eTpaSPI2CSelectSpi1Cs0SpiFlash        = SPI2C_MUXSEL_0to9(1,3,3,1,3, 1,3,3,3, 0), // SPI2 can use this too.
    eTpaSPI2CSelectSpi1Cs1SpiFlash        = SPI2C_MUXSEL_0to9(1,3,3,1,3, 3,3,1,3, 0), // SPI2 can use this too.

    eTpaSPI2CSelectSpi2Cs0SpiFlash        = eTpaSPI2CSelectSpi1Cs0SpiFlash,
    eTpaSPI2CSelectSpi2Cs1SpiFlash        = eTpaSPI2CSelectSpi1Cs1SpiFlash,
    eTpaSPI2CSelectSpi2Cs2SpiFlash        = SPI2C_MUXSEL_0to9(1,3,3,1,3, 3,3,3,1, 0),
    eTpaSPI2CSelectSpi2Cs3SpiFlash        = SPI2C_MUXSEL_0to9(1,3,3,1,3, 2,3,3,3, 0),

    eTpaSPI2CSelectSpi2Cs0AltSpiFlash     = SPI2C_MUXSEL_0to9(2,3,3,2,3, 1,3,3,3, 0),
    eTpaSPI2CSelectSpi2Cs1AltSpiFlash     = SPI2C_MUXSEL_0to9(2,3,3,2,3, 3,3,1,3, 0),
    eTpaSPI2CSelectSpi2Cs2AltSpiFlash     = SPI2C_MUXSEL_0to9(2,3,3,2,3, 3,3,3,1, 0),
    eTpaSPI2CSelectSpi2Cs3AltSpiFlash     = SPI2C_MUXSEL_0to9(2,3,3,2,3, 2,3,3,3, 0),

    // SPI0 CS0 Master, SPI1 Slave (or SPI2 Alt Slave)
    // or
    // SPI1 CS0 Master, SPI0 Slave
    eTpaSPI2CSelectSpi0Cs0Spi1Slv         = SPI2C_MUXSEL_0to9(3,3,3,3,3, 3,3,3,3, 3), // Validation board MUXs are wired incorrectly. Must use flyleads instead.
    eTpaSPI2CSelectSpi1Cs0Spi0Slv         = eTpaSPI2CSelectSpi0Cs0Spi1Slv,

    // SPI0 CS0 Master, SPI2 Slave (no support for SPI2 master to SPI0,1 slave, but HW / board supports)
    eTpaSPI2CSelectSpi0Cs0Spi2Slv         = eTpaSPI2CSelectSpi0Cs0Spi1Slv,
    eTpaSPI2CSelectSpi0Cs0Spi2AltSlv      = SPI2C_MUXSEL_0to9(0,2,3,0,2, 0,2,3,3, 3),

    // I2C
    eTpaSPI2CSelectI2c0Seeprom            = SPI2C_MUXSEL_0to9(0,3,3,3,3, 3,3,3,3, 2),
    eTpaSPI2CSelectI2c0AltSeeprom         = SPI2C_MUXSEL_0to9(3,3,0,3,3, 3,3,3,3, 2),
    eTpaSPI2CSelectI2c1Seeprom            = SPI2C_MUXSEL_0to9(1,3,3,3,3, 3,3,3,3, 2),
    eTpaSPI2CSelectI2c1AltSeeprom         = SPI2C_MUXSEL_0to9(3,3,1,3,3, 3,3,3,3, 2),
    eTpaSPI2CSelectI2c2Seeprom            = eTpaSPI2CSelectI2c1AltSeeprom,

    // FTDI Master, Various Slaves
    eTpaSPI2CSelectSpiFTDISpi0Slv         = SPI2C_MUXSEL_0to9(0,3,3,0,3, 0,3,3,3, 1),
    eTpaSPI2CSelectSpiFTDISpi1Slv         = SPI2C_MUXSEL_0to9(1,3,3,1,3, 1,3,3,3, 1),
    eTpaSPI2CSelectSpiFTDISpi2Slv         = eTpaSPI2CSelectSpiFTDISpi1Slv,
    eTpaSPI2CSelectSpiFTDISpi2AltSlv      = SPI2C_MUXSEL_0to9(2,3,3,2,3, 3,3,1,3, 1),

}
tEtaTpaSPI2CConfigSelect;













///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Below this line is NGC code, which should be deprecated / moved above the line.








// Address Map
// apb_csr_slave
#define MAX10_CSR_SPISLAVE_CTRL       0x00
#define MAX10_CSR_PC_SPISLAVE_CTRL    0x01
#define MAX10_CSR_FPGA_REV_NUM        0x0f
#define MAX10_CSR_SEMAPHORE0          0x10
#define MAX10_CSR_RESET_CTRL          0x11
#define MAX10_CSR_MBOX_FTDI2CHIP_DATA 0x12 // A write to this loc , sets corresponding PEND
#define MAX10_CSR_MBOX_CHIP2FTDI_DATA 0x13 // A write to this loc , sets corresponding PEND
#define MAX10_CSR_MBOX_FTDI2CHIP_PEND 0x14 // PEND can be written (cleared) by either master but  is set when write to DATA
#define MAX10_CSR_MBOX_CHIP2FTDI_PEND 0x15 // PEND can be written (cleared) by either master but  is set when write to DATA
#define MAX10_LED_OVERRIDE            0x16

// apb_gpio_xactor
#define MAX10_CSR_GPIO_IN       0x40
#define MAX10_CSR_GPIO_OUT      0x41
#define MAX10_CSR_GPIO_OE       0x42
#define MAX10_CSR_OTHER_IN      0x43
#define MAX10_CSR_OTHER_OUT     0x44
#define MAX10_CSR_OTHER_OE      0x45
#define MAX10_CSR_DIPSW_IN      0x60
#define MAX10_CSR_DIPSW_OUT     0x61
#define MAX10_CSR_DIPSW_OE      0x62
#define MAX10_CSR_MUXCTRL_IN    0x63
#define MAX10_CSR_MUXCTRL_OUT   0x64
#define MAX10_CSR_MUXCTRL_OE    0x65
#define MAX10_CSR_POWER_CTRL    0x66
#define MAX10_CSR_BOOTMODE_REG  0x67
#define MAX10_CSR_LOWPOWER_NOW  0x68
#define MAX10_CSR_LOWPOWER_DIP7 0x69
#define MAX10_CSR_BUTTON_CTRL   0x70

#define MAX10_CSR_OTHER_GPIO_PM_MODE_MASK      (1<<0)
#define MAX10_CSR_OTHER_HFO_OVRD_MASK          (1<<1)
#define MAX10_CSR_OTHER_FRONT_PANEL_MASK       (1<<2)
#define MAX10_CSR_OTHER_SWDCLK_MASK            (1<<3)
#define MAX10_CSR_OTHER_SWDIO_MASK             (1<<4)
#define MAX10_CSR_OTHER_PMIC_AO_EN25_MASK      (1<<5)
#define MAX10_CSR_OTHER_PMIC_EN25_MASK         (1<<6)
#define MAX10_CSR_OTHER_PMIC_OVRD25_MASK       (1<<7)
#define MAX10_CSR_OTHER_POR_N_MASK             (1<<8)
#define MAX10_CSR_OTHER_POR_OVRD_MASK          (1<<9)
#define MAX10_CSR_OTHER_RESET_N_MASK           (1<<10)
#define MAX10_CSR_OTHER_SPI0_MISO_MASK         (1<<11)
#define MAX10_CSR_OTHER_SPI0_MOSI_MASK         (1<<12)
#define MAX10_CSR_OTHER_SPI0_SCLK_MASK         (1<<13)
#define MAX10_CSR_OTHER_TESTMODE_MASK          (1<<14)
#define MAX10_CSR_OTHER_UART0_CTS_MASK         (1<<15)
#define MAX10_CSR_OTHER_UART0_RTS_MASK         (1<<16)
#define MAX10_CSR_OTHER_UART0_RX_MASK          (1<<17)
#define MAX10_CSR_OTHER_UART0_TX_MASK          (1<<18)
#define MAX10_CSR_OTHER_WAKE_MASK              (1<<19)

#define MAX10_CSR_OTHER_GPIO_PM_MODE_SHIFT      0
#define MAX10_CSR_OTHER_HFO_OVRD_SHIFT          1
#define MAX10_CSR_OTHER_FRONT_PANEL_SHIFT       2
#define MAX10_CSR_OTHER_SWDCLK_SHIFT            3
#define MAX10_CSR_OTHER_SWDIO_SHIFT             4
#define MAX10_CSR_OTHER_PMIC_AO_EN25_SHIFT      5
#define MAX10_CSR_OTHER_PMIC_EN25_SHIFT         6
#define MAX10_CSR_OTHER_PMIC_OVRD25_SHIFT       7
#define MAX10_CSR_OTHER_POR_N_SHIFT             8
#define MAX10_CSR_OTHER_POR_OVRD_SHIFT          9
#define MAX10_CSR_OTHER_RESET_N_SHIFT           10
#define MAX10_CSR_OTHER_SPI0_MISO_SHIFT         11
#define MAX10_CSR_OTHER_SPI0_MOSI_SHIFT         12
#define MAX10_CSR_OTHER_SPI0_SCLK_SHIFT         13
#define MAX10_CSR_OTHER_TESTMODE_SHIFT          14
#define MAX10_CSR_OTHER_UART0_CTS_SHIFT         15
#define MAX10_CSR_OTHER_UART0_RTS_SHIFT         16
#define MAX10_CSR_OTHER_UART0_RX_SHIFT          17
#define MAX10_CSR_OTHER_UART0_TX_SHIFT          18
#define MAX10_CSR_OTHER_WAKE_SHIFT              19

#define MAX10_CSR_MUXCTRL_M3_CLK_SEL_MASK   (0x3<<0)
#define MAX10_CSR_MUXCTRL_HFO_CLK_SEL_MASK  (0x3<<2)
#define MAX10_CSR_MUXCTRL_DSP_CLK_SEL_MASK  (0x3<<4)
#define MAX10_CSR_MUXCTRL_SEL_TST0_MASK     (0x1<<8)
#define MAX10_CSR_MUXCTRL_EN_TST0_N_MASK    (0x1<<9)
#define MAX10_CSR_MUXCTRL_SPI0_CS_EN_N_MASK (0x1<<10)
#define MAX10_CSR_MUXCTRL_SEL_TST1_MASK     (0x1<<11)
#define MAX10_CSR_MUXCTRL_EN_TST1_N_MASK    (0x1<<12)
#define MAX10_CSR_MUXCTRL_SPI1_CS_EN_N_MASK (0x1<<13)
#define MAX10_CSR_MUXCTRL_CONFIG_SEL_MASK   (0x1<<14)

#define MAX10_CSR_MUXCTRL_M3_CLK_SEL_SHIFT      0
#define MAX10_CSR_MUXCTRL_HFO_CLK_SEL_SHIFT     2
#define MAX10_CSR_MUXCTRL_DSP_CLK_SEL_SHIFT     4
#define MAX10_CSR_MUXCTRL_SEL_TST0_SHIFT        8
#define MAX10_CSR_MUXCTRL_EN_TST0_N_SHIFT       9
#define MAX10_CSR_MUXCTRL_SPI0_CS_EN_N_SHIFT    10
#define MAX10_CSR_MUXCTRL_SEL_TST1_SHIFT        11
#define MAX10_CSR_MUXCTRL_EN_TST1_N_SHIFT       12
#define MAX10_CSR_MUXCTRL_SPI1_CS_EN_N_SHIFT    13
#define MAX10_CSR_MUXCTRL_CONFIG_SEL_SHIFT      14

#define MAX10_CSR_DIPSW_LED_MODE_MASK                 (0x3<<0)
#define MAX10_CSR_DIPSW_SPI0_LOOPBACK_MASK            (0x1<<2)
#define MAX10_CSR_DIPSW_DISABLE_TIME0_POR_MASK        (0x1<<3)
#define MAX10_CSR_DIPSW_I2S_MODE_MASK                 (0x7<<4)
#define MAX10_CSR_DIPSW_DISABLE_SWD_FTDI_CHANA_MASK   (0x1<<7)
#define MAX10_CSR_DIPSW_BOOTMODE_MASK                 (0xf<<8)
#define MAX10_CSR_DIPSW_UART_SWITCH_MATRIX_MASK       (0x7<<13)

#define MAX10_CSR_DIPSW_LED_MODE_SHIFT                 0
#define MAX10_CSR_DIPSW_SPI0_LOOPBACK_SHIFT            2
#define MAX10_CSR_DIPSW_DISABLE_TIME0_POR_SHIFT        3
#define MAX10_CSR_DIPSW_I2S_MODE_SHIFT                 4
#define MAX10_CSR_DIPSW_DISABLE_SWD_FTDI_CHANA_SHIFT   7
#define MAX10_CSR_DIPSW_BOOTMODE_SHIFT                 8
#define MAX10_CSR_DIPSW_UART_SWITCH_MATRIX_SHIFT       13

#define MAX10_CSR_LOWPOWER_DSPCLK_MASK    (0x1<<0)
#define MAX10_CSR_LOWPOWER_HFOCLK_MASK    (0x1<<1)
#define MAX10_CSR_LOWPOWER_M3CLK_MASK     (0x1<<2)
#define MAX10_CSR_LOWPOWER_32KCLK_MASK    (0x1<<3)
#define MAX10_CSR_LOWPOWER_DIS_UART0_MASK (0x1<<4)
#define MAX10_CSR_LOWPOWER_DIS_UART1_MASK (0x1<<5)

#define MAX10_CSR_LOWPOWER_DSPCLK_SHIFT    0
#define MAX10_CSR_LOWPOWER_HFOCLK_SHIFT    1
#define MAX10_CSR_LOWPOWER_M3CLK_SHIFT     2
#define MAX10_CSR_LOWPOWER_32KCLK_SHIFT    3
#define MAX10_CSR_LOWPOWER_DIS_UART0_SHIFT 4
#define MAX10_CSR_LOWPOWER_DIS_UART1_SHIFT 5



// pwm_checker_top
#define MAX10_CSR_PWMCHK_SEL0             0x80
#define MAX10_CSR_PWMCHK_SEL1             0x81
#define MAX10_CSR_PWMCHK_SEL2             0x82
#define MAX10_CSR_PWMCHK_SEL3             0x83
#define MAX10_CSR_PWMCHK_SEL4             0x84
#define MAX10_CSR_PWMCHK_SEL5             0x85
#define MAX10_CSR_PWMCHK_SEL6             0x86
#define MAX10_CSR_PWMCHK_SEL7             0x87
#define MAX10_CSR_PWMCHK_CTRL             0x88
#define MAX10_CSR_PWMCHK_EVENT_READ       0x90
#define MAX10_CSR_PWMCHK_DELTA_READ       0x91

#define MAX10_CSR_PWMCHKCTRL_ENABLE_MASK               0x01
#define MAX10_CSR_PWMCHKCTRL_RESET_FIFO_MASK           0x02
#define MAX10_CSR_PWMCHKCTRL_START_SEL_EN_MASK         0x04
#define MAX10_CSR_PWMCHKCTRL_START_ON_PE_MASK          0x08
#define MAX10_CSR_PWMCHKCTRL_START_PWM_SEL_MASK        0x70
#define MAX10_CSR_PWMCHKCTRL_DISABLE_PE_MASK           0x100
#define MAX10_CSR_PWMCHKCTRL_DISABLE_NE_MASK           0x200
#define MAX10_CSR_PWMCHKCTRL_RESET_COUNTER_MASK        0x400
#define MAX10_CSR_PWMCHKCTRL_DELTA_IS_RAW_COUNTER_MASK 0x800

#define MAX10_CSR_PWMCHKCTRL_ENABLE_SHIFT                 0
#define MAX10_CSR_PWMCHKCTRL_RESET_FIFO_SHIFT             1
#define MAX10_CSR_PWMCHKCTRL_START_SEL_EN_SHIFT           2
#define MAX10_CSR_PWMCHKCTRL_START_ON_PE_SHIFT            3
#define MAX10_CSR_PWMCHKCTRL_START_PWM_SEL_SHIFT          4
#define MAX10_CSR_PWMCHKCTRL_DISABLE_PE_SHIFT             8
#define MAX10_CSR_PWMCHKCTRL_DISABLE_NE_SHIFT             9
#define MAX10_CSR_PWMCHKCTRL_RESET_COUNTER_SHIFT          10
#define MAX10_CSR_PWMCHKCTRL_DELTA_IS_RAW_COUNTER_SHIFT   11

#define MAX10_PWMCHKEVENT_PE        1
#define MAX10_PWMCHKEVENT_NE        2
#define MAX10_PWMCHKEVENT_S1        3
#define MAX10_PWMCHKEVENT_S0        0

#define MAX10_CSR_PWMCHKEVENT_PWM0_SHIFT        0
#define MAX10_CSR_PWMCHKEVENT_PWM1_SHIFT        2
#define MAX10_CSR_PWMCHKEVENT_PWM2_SHIFT        4
#define MAX10_CSR_PWMCHKEVENT_PWM3_SHIFT        6
#define MAX10_CSR_PWMCHKEVENT_PWM4_SHIFT        8
#define MAX10_CSR_PWMCHKEVENT_PWM5_SHIFT        10
#define MAX10_CSR_PWMCHKEVENT_PWM6_SHIFT        12
#define MAX10_CSR_PWMCHKEVENT_PWM7_SHIFT        14
#define MAX10_CSR_PWMCHKEVENT_ENTRIES_SHIFT     16

#define MAX10_CSR_PWMCHKEVENT_PWM0_MASK        0x00000003
#define MAX10_CSR_PWMCHKEVENT_PWM1_MASK        0x0000000c
#define MAX10_CSR_PWMCHKEVENT_PWM2_MASK        0x00000030
#define MAX10_CSR_PWMCHKEVENT_PWM3_MASK        0x000000c0
#define MAX10_CSR_PWMCHKEVENT_PWM4_MASK        0x00000300
#define MAX10_CSR_PWMCHKEVENT_PWM5_MASK        0x00000c00
#define MAX10_CSR_PWMCHKEVENT_PWM6_MASK        0x00003000
#define MAX10_CSR_PWMCHKEVENT_PWM7_MASK        0x0000c000
#define MAX10_CSR_PWMCHKEVENT_ENTRIES_MASK     0xffff0000


#define MAX10_PWMCHK_SEL_GPIO00       ( 0  + 0  )
#define MAX10_PWMCHK_SEL_GPIO01       ( 0  + 1  )
#define MAX10_PWMCHK_SEL_GPIO02       ( 0  + 2  )
#define MAX10_PWMCHK_SEL_GPIO03       ( 0  + 3  )
#define MAX10_PWMCHK_SEL_GPIO04       ( 0  + 4  )
#define MAX10_PWMCHK_SEL_GPIO05       ( 0  + 5  )
#define MAX10_PWMCHK_SEL_GPIO06       ( 0  + 6  )
#define MAX10_PWMCHK_SEL_GPIO07       ( 0  + 7  )
#define MAX10_PWMCHK_SEL_GPIO08       ( 0  + 8  )
#define MAX10_PWMCHK_SEL_GPIO09       ( 0  + 9  )
#define MAX10_PWMCHK_SEL_GPIO10       ( 0  + 10 )
#define MAX10_PWMCHK_SEL_GPIO11       ( 0  + 11 )
#define MAX10_PWMCHK_SEL_GPIO12       ( 0  + 12 )
#define MAX10_PWMCHK_SEL_GPIO13       ( 0  + 13 )
#define MAX10_PWMCHK_SEL_GPIO14       ( 0  + 14 )
#define MAX10_PWMCHK_SEL_GPIO15       ( 0  + 15 )
#define MAX10_PWMCHK_SEL_GPIO16       ( 0  + 16 )
#define MAX10_PWMCHK_SEL_GPIO17       ( 0  + 17 )
#define MAX10_PWMCHK_SEL_GPIO18       ( 0  + 18 )
#define MAX10_PWMCHK_SEL_GPIO19       ( 0  + 19 )
#define MAX10_PWMCHK_SEL_GPIO20       ( 0  + 20 )
#define MAX10_PWMCHK_SEL_GPIO21       ( 0  + 21 )
#define MAX10_PWMCHK_SEL_GPIO22       ( 0  + 22 )
#define MAX10_PWMCHK_SEL_GPIO23       ( 0  + 23 )
#define MAX10_PWMCHK_SEL_GPIO24       ( 0  + 24 )
#define MAX10_PWMCHK_SEL_GPIO25       ( 0  + 25 )
#define MAX10_PWMCHK_SEL_GPIO26       ( 0  + 26 )
#define MAX10_PWMCHK_SEL_GPIO27       ( 0  + 27 )
#define MAX10_PWMCHK_SEL_GPIO28       ( 0  + 28 )
#define MAX10_PWMCHK_SEL_GPIO29       ( 0  + 29 )
#define MAX10_PWMCHK_SEL_GPIO30       ( 0  + 30 )
#define MAX10_PWMCHK_SEL_GPIO31       ( 0  + 31 )
#define MAX10_PWMCHK_SEL_GPIO_PM_MODE ( 32 + 0  )
#define MAX10_PWMCHK_SEL_HFO_OVRD     ( 32 + 1  )
#define MAX10_PWMCHK_SEL_FRONT_PANEL  ( 32 + 2  )
#define MAX10_PWMCHK_SEL_SWDCLK       ( 32 + 3  )
#define MAX10_PWMCHK_SEL_SWDIO        ( 32 + 4  )
#define MAX10_PWMCHK_SEL_PMIC_AO_EN25 ( 32 + 5  )
#define MAX10_PWMCHK_SEL_PMIC_EN25    ( 32 + 6  )
#define MAX10_PWMCHK_SEL_PMIC_OVRD25  ( 32 + 7  )
#define MAX10_PWMCHK_SEL_POR_N        ( 32 + 8  )
#define MAX10_PWMCHK_SEL_POR_OVRD     ( 32 + 9  )
#define MAX10_PWMCHK_SEL_RESET_N      ( 32 + 10 )
#define MAX10_PWMCHK_SEL_SPI0_MISO    ( 32 + 11 )
#define MAX10_PWMCHK_SEL_SPI0_MOSI    ( 32 + 12 )
#define MAX10_PWMCHK_SEL_SPI0_SCLK    ( 32 + 13 )
#define MAX10_PWMCHK_SEL_TESTMODE     ( 32 + 14 )
#define MAX10_PWMCHK_SEL_UART0_CTS    ( 32 + 15 )
#define MAX10_PWMCHK_SEL_UART0_RTS    ( 32 + 16 )
#define MAX10_PWMCHK_SEL_UART0_RX     ( 32 + 17 )
#define MAX10_PWMCHK_SEL_UART0_TX     ( 32 + 18 )
#define MAX10_PWMCHK_SEL_WAKE         ( 32 + 19 )
#define MAX10_PWMCHK_SEL_DISABLE      ( 32 + 31 )

// gen_fifo
#define MAX10_CSR_GENFIFO0_RESET        0xc0
#define MAX10_CSR_GENFIFO0_WRDDATA_PUSH 0xc1
#define MAX10_CSR_GENFIFO0_RDDATA_POP   0xc2
#define MAX10_CSR_GENFIFO0_PUSH_STATUS  0xc3
#define MAX10_CSR_GENFIFO0_POP_STATUS   0xc4

#define MAX10_CSR_GENFIFOPUSHSTAT_OVERFLOW_SHIFT          31
#define MAX10_CSR_GENFIFOPUSHSTAT_UNDERFLOW_SHIFT         30
#define MAX10_CSR_GENFIFOPUSHSTAT_HAS_DATA_TO_POP_SHIFT   29
#define MAX10_CSR_GENFIFOPUSHSTAT_HAS_SPACE_TO_PUSH_SHIFT 28
#define MAX10_CSR_GENFIFOPUSHSTAT_SPACE_TO_PUSH_SHIFT     0

#define MAX10_CSR_GENFIFOPUSHSTAT_OVERFLOW_MASK           (0x1U<<31)
#define MAX10_CSR_GENFIFOPUSHSTAT_UNDERFLOW_MASK          (0x1U<<30)
#define MAX10_CSR_GENFIFOPUSHSTAT_HAS_DATA_TO_POP_MASK    (0x1U<<29)
#define MAX10_CSR_GENFIFOPUSHSTAT_HAS_SPACE_TO_PUSH_MASK  (0x1U<<28)
#define MAX10_CSR_GENFIFOPUSHSTAT_SPACE_TO_PUSH_MASK      (0xffffU<<0)

#define MAX10_CSR_GENFIFOPOPSTAT_OVERFLOW_SHIFT           31
#define MAX10_CSR_GENFIFOPOPSTAT_UNDERFLOW_SHIFT          30
#define MAX10_CSR_GENFIFOPOPSTAT_HAS_DATA_TO_POP_SHIFT    29
#define MAX10_CSR_GENFIFOPOPSTAT_HAS_SPACE_TO_PUSH_SHIFT  28
#define MAX10_CSR_GENFIFOPOPSTAT_ENTRIES_TO_POP_SHIFT     0

#define MAX10_CSR_GENFIFOPOPSTAT_OVERFLOW_MASK            (0x1U<<31)
#define MAX10_CSR_GENFIFOPOPSTAT_UNDERFLOW_MASK           (0x1U<<30)
#define MAX10_CSR_GENFIFOPOPSTAT_HAS_DATA_TO_POP_MASK     (0x1U<<29)
#define MAX10_CSR_GENFIFOPOPSTAT_HAS_SPACE_TO_PUSH_MASK   (0x1U<<28)
#define MAX10_CSR_GENFIFOPOPSTAT_ENTRIES_TO_POP_MASK      (0xffffU<<0)




// max10_clr_dipswitch_settings
// Clears all MAX10 driven settings on DIPSW lines and stops driving them
void max10_clr_dipswitch_settings (void);
// max10_clr_muxctrl_settings
// Clears all MAX10 driven settings on MUXCTRL lines and stops driving them
void max10_clr_muxctrl_settings (void);

// max10_set_dipswitch
// Generic set value to dipswitch fields. Pass in SHIFT address (e.g MAX10_CSR_DIPSW_UART_SWITCH_MATRIX_SHIFT) and new value that field should be.
// returns 0 if passed, returns -1 if SHIFT field is invalid / unsupported.
int max10_set_dipswitch(uint32_t newvalue, uint32_t shift);

// max10_set_muxctrl
// Generic set value to dipswitch fields. Pass in SHIFT address (e.g MAX10_CSR_MUXCTRL_DSP_CLK_SEL_SHIFT) and new value that field should be.
// returns 0 if passed, returns -1 if SHIFT field is invalid / unsupported.
int max10_set_muxctrl(uint32_t newvalue, uint32_t shift);

#define max10_set_uart_switch_matrix(setting)          max10_set_dipswitch(setting, MAX10_CSR_DIPSW_UART_SWITCH_MATRIX_SHIFT);
#define max10_set_i2s_mode(setting)                    max10_set_dipswitch(setting, MAX10_CSR_DIPSW_I2S_MODE_SHIFT);

#define max10_disable_ftdi_swd_chana()                 max10_set_dipswitch(1, MAX10_CSR_DIPSW_UART_SWITCH_MATRIX_SHIFT);
#define max10_enable_ftdi_swd_chana()                  max10_set_dipswitch(0, MAX10_CSR_DIPSW_UART_SWITCH_MATRIX_SHIFT);
#define max10_enable_spi0_mosi_miso_loopback()         max10_set_dipswitch(1, MAX10_CSR_DIPSW_SPI0_LOOPBACK_MASK);
#define max10_disable_spi0_mosi_miso_loopback()        max10_set_dipswitch(0, MAX10_CSR_DIPSW_SPI0_LOOPBACK_MASK);

#define max10_set_m3_clk_sel(setting)                  max10_set_dipswitch(setting, MAX10_CSR_MUXCTRL_M3_CLK_SEL_SHIFT  );
#define max10_set_hfo_clk_sel(setting)                 max10_set_dipswitch(setting, MAX10_CSR_MUXCTRL_HFO_CLK_SEL_SHIFT );
#define max10_set_dsp_clk_sel(setting)                 max10_set_dipswitch(setting, MAX10_CSR_MUXCTRL_DSP_CLK_SEL_SHIFT );

void max10_setup_for_spi0_tst_bus (void);
void max10_setup_for_spi1_tst_bus (void);
void max10_setup_for_i2c0_tst_bus (void);
void max10_setup_for_i2c1_tst_bus (void);
void max10_dump_all_config_reg (void);

#define max10_spislave_trigger_fpga_reset() max10_spislave_write(IO_FPGA_RESET_POR_CTRL_WRITE_ADDRESS,0xdead0001);
#define max10_spislave_trigger_ngc_por()    max10_spislave_write(IO_FPGA_RESET_POR_CTRL_WRITE_ADDRESS,0xdead0002);
#define max10_spislave_trigger_ngc_reset()  max10_spislave_write(IO_FPGA_RESET_POR_CTRL_WRITE_ADDRESS,0xdead0004);
#define max10_spislave_trigger_ngc_wake()   max10_spislave_write(IO_FPGA_RESET_POR_CTRL_WRITE_ADDRESS,0xdead0008);




#endif  //ETA_DIAGS_MAX10
