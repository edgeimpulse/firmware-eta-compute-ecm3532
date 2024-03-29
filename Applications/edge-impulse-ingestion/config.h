/* Generated by Kconfiglib (https://github.com/ulfalizer/Kconfiglib) */
#define CONFIG_ECM3532 1
#define CONFIG_SOC "ecm3532"
#define CONFIG_BOOTLOADER_APP 1
#define CONFIG_CHIP_RAM_START 0x10000000
#define CONFIG_CHIP_RAM_SIZE 0x40000
#define CONFIG_CHIP_RAM_SIZE_INT 262144
#define CONFIG_CHIP_RAM_MAX 0x10040000
#define CONFIG_CHIP_FLASH_START 0x01000000
#define CONFIG_CHIP_FLASH_SIZE 0x80000
#define CONFIG_CHIP_FLASH_MAX 0x01080000
#define CONFIG_BOOTLOADER_RAM_START 0x1003D000
#define CONFIG_BOOTLOADER_RAM_SIZE 0x2060
#define CONFIG_BOOTLOADER_HEAP_START 0x1003F060
#define CONFIG_BOOTLOADER_HEAP_SIZE 4000
#define CONFIG_APP_MIN_HEAP_SIZE 12288
#define CONFIG_BOOTLOADER_APP_FLASH_START 0x01006000
#define CONFIG_BOOTLOADER_APP_FLASH_SIZE 0x7A000
#define CONFIG_DEFAULT_HEAP_SIZE 20000
#define CONFIG_BOOTLOADER_FLASH_SIZE 0x6000
#define CONFIG_M3_RAM_SIZE 0x40000
#define CONFIG_M3_FLASH_SIZE 0x7A000
#define CONFIG_AI_SENSOR_BOARD 1
#define CONFIG_ECM3532_TARGET_BOARD "ai_sensor"
#define CONFIG_BUTTON_GPIO 1
#define CONFIG_BUTTON_GPIO_NUM 10
#define CONFIG_PIN3_FUNC_UART1RX 1
#define CONFIG_PIN3_MUX 3
#define CONFIG_PIN4_FUNC_UART1RTS 1
#define CONFIG_PIN4_MUX 3
#define CONFIG_PIN5_FUNC_UART1CTS 1
#define CONFIG_PIN5_MUX 3
#define CONFIG_PIN7_FUNC_UART1TX 1
#define CONFIG_PIN7_MUX 3
#define CONFIG_PIN8_FUNC_I2C1SDA 1
#define CONFIG_PIN8_MUX 1
#define CONFIG_PIN9_FUNC_I2C1SCK 1
#define CONFIG_PIN9_MUX 1
#define CONFIG_PIN14_FUNC_PDM1CLK 1
#define CONFIG_PIN14_MUX 2
#define CONFIG_PIN15_FUNC_PDM1DATA 1
#define CONFIG_PIN15_MUX 2
#define CONFIG_PIN20_FUNC_GPIO20 1
#define CONFIG_PIN20_MUX 0
#define CONFIG_PIN27_FUNC_SPI1MISO 1
#define CONFIG_PIN27_MUX 1
#define CONFIG_PIN28_FUNC_SPI1MOSI 1
#define CONFIG_PIN28_MUX 1
#define CONFIG_PIN29_FUNC_SPI1CLK 1
#define CONFIG_PIN29_MUX 1
#define CONFIG_PIN30_FUNC_GPIO30 1
#define CONFIG_PIN30_MUX 0
#define CONFIG_PIN31_FUNC_GPIO31 1
#define CONFIG_PIN31_MUX 0
#define CONFIG_PIN_SPI0CLK_FUNC_SPI0CLK 1
#define CONFIG_PIN_SPI0CLK 0
#define CONFIG_PIN_SPI0MOSI_FUNC_MOSI 1
#define CONFIG_PIN_SPI0MOSI 0
#define CONFIG_PIN_SPI0MISO_FUNC_MISO 1
#define CONFIG_PIN_SPI0MISO 0
#define CONFIG_HAL_COMMON_UART 1
#define CONFIG_UART_ASYNC_RECV_MAX_CNT 2
#define CONFIG_UART_0 1
#define CONFIG_UART0_BRATE_115200 1
#define CONFIG_UART0_DEFAULT_BAUD_RATE 115200
#define CONFIG_UART0_DEFAULT_PARITY 2
#define CONFIG_UART0_DEFAULT_FLOW_CTRL 0
#define CONFIG_UART0_DEFAULT_STOP_BITS 2
#define CONFIG_UART_1 1
#define CONFIG_UART1_BRATE_115200 1
#define CONFIG_UART1_DEFAULT_BAUD_RATE 115200
#define CONFIG_UART1_DEFAULT_PARITY 2
#define CONFIG_UART1_DEFAULT_FLOW_CTRL 0
#define CONFIG_UART1_DEFAULT_STOP_BITS 2
#define CONFIG_UART_CNT 2
#define CONFIG_UART_INDEX_OFFSET 0
#define CONFIG_DEBUG_UART_PRINT 1
#define CONFIG_DEBUG_UART_1 1
#define CONFIG_DEBUG_UART 1
#define CONFIG_HAL_COMMON_GPIO 1
#define CONFIG_GPIO_PIN_CNT 31
#define CONFIG_GPIO_MAX_IRQ 3
#define CONFIG_HAL_COMMON_TIMER 1
#define CONFIG_TMR_CH_0 1
#define CONFIG_HAL_COMMON_RTC 1
#define CONFIG_HAL_COMMON_I2C 1
#define CONFIG_I2C_1 1
#define CONFIG_I2C1_FREQ_400_KHZ 1
#define CONFIG_I2C_1_FREQ 400
#define CONFIG_HAL_COMMON_SPI 1
#define CONFIG_SPI_0_MASTER 1
#define CONFIG_SPI_0_BRATE_1000_KHZ 1
#define CONFIG_SPI_0_BRATE_IN_KHZ 1000
#define CONFIG_SPI_1_MASTER 1
#define CONFIG_SPI_1_BRATE_1000_KHZ 1
#define CONFIG_SPI_1_BRATE_IN_KHZ 1000
#define CONFIG_HAL_PDM 1
#define CONFIG_PDM_1 1
#define CONFIG_PDM_COUNT 1
#define CONFIG_PDM_CLK_SRC 1
#define CONFIG_PGA_L 8
#define CONFIG_PGA_R 8
#define CONFIG_HP_FILTER 1
#define CONFIG_HP_COEF 11
#define CONFIG_M3_INIT_FREQ 100
#define CONFIG_DSP_INIT_FREQ 100
#define CONFIG_RPC 1
#define CONFIG_DSP_UART 1
#define CONFIG_DSP_PDM_FW 1
#define CONFIG_PRE_BUILD_DSP_FW_RELOCATED "dsp_fw_pdm_relocated.bin"
#define CONFIG_PRE_BUILD_DSP_FW "dsp_fw_pdm.bin"
#define CONFIG_DSP_PDM 1
#define CONFIG_BUILD_DSP_FW "pdm_test"
#define CONFIG_SPI_FLASH 1
#define CONFIG_SPI_FLASH_CS_PIN 31
#define CONFIG_SPI_FLASH_SPI_NUM 1
#define CONFIG_FLASH_MX25R6435F 1
#define CONFIG_OP_M3_CONV2D_Q7 1
#define CONFIG_OP_M3_AVPOOL2D_Q7 1
#define CONFIG_OP_M3_MAXPOOL_Q7 1
#define CONFIG_OP_M3_FULLY_CONNECTED_Q7 1
#define CONFIG_APP_NAME "firmware-eta-compute-ecm3532-internal"
#define CONFIG_SW_VERSION "TensaiFlow_rc_alpha2-0.2"
#define CONFIG_SW_VERSION_EXTRA ""
#define CONFIG_FLASH_SHM 1
#define CONFIG_DSP_IN_FLASH 1
#define CONFIG_DSP_FW_LENGTH 0x16000
#define CONFIG_PBUF_IN_FLASH 1
#define CONFIG_PBUF_LENGTH 0x0
#define CONFIG_DSP_FILE_SECTION ".dsp"
#define CONFIG_PBUF_FILE_SECTION ".pbuf"
#define CONFIG_SHM_LENGTH 0x1200
#define CONFIG_LINKER_FILE "flash_shm"
#define CONFIG_SYSTICK_ON_REF_CLK 1
#define CONFIG_TICK_RATE_HZ 500
#define CONFIG_SYSTICK_REF_CLOCK 1024
#define CONFIG_HEAP_SIZE 12288
