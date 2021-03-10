#include "spi_hal.h"
#include "eta_csp_spi.h"
#include "config.h"

#if CONFIG_FLASH_MX25R6435F
#include "MX25R6435F.h"
#else
#error "SPI Flash chip not defined"
#endif

#include "FreeRTOS.h"
#include "semphr.h"

#define SPI_PWR_CNTRL_PIN 11

/**
 * Unique identifier for different wrapper functions for debugging purpose
 */
typedef enum
{
    WRAPPER_NONE = 0,
    WRAPPER_SECTOR_ERASE,
    WRAPPER_BLOCK32_ERASE,
    WRAPPER_BLOCK_ERASE,
    WRAPPER_CHIP_ERASE,
    WRAPPER_READ,
    WRAPPER_WRITE
} spiFlashWrapperIdentifier_t;

/**
 * Enable chip select
 */
#define ASSERT_CS(flashChipSel) HalGpioWrite((flashChipSel), 0)
/**
 * Disable chip select
 */
#define RELEASE_CS(flashChipSel) HalGpioWrite((flashChipSel), 1)

/**
 * Lock SPI flash mutex
 */
#define LOCK_SPI_FLASH()    {\
                            configASSERT(spiFlashConfig.lock != NULL);\
                            xSemaphoreTake(spiFlashConfig.lock, portMAX_DELAY);\
                           }
/**
 * Release SPI flash mutex
 */
#define UNLOCK_SPI_FLASH()  xSemaphoreGive(spiFlashConfig.lock)

/**
 * External SPI flash config internal structure.
 * This structure is populated based on menuconfig configuration
 */
typedef struct
{
    /**
     * SPI number
     */
    tSpiNum spiNum;
    /**
     * Chipselect GPIO
     */
    tSpiChipSel chipSel;
    /**
     * Mutex for access control
     */
    SemaphoreHandle_t lock;
}spiFlashConfig_t;

/**
 * Enable write
 *
 * @return tEtaStatus
 */
tEtaStatus spiFlashWriteEnable(void);

/**
 * Disable write
 *
 * @return tEtaStatus
 */
tEtaStatus spiFlashWriteDisable(void);

/**
 * Read device ID
 *
 * @param rdid_res : address to which device ID is to be written
 *
 * @return tEtaStatus
 */
tEtaStatus spiFlashReadDeviceID(RDID_RES_t *rdid_res);

/**
 * Read status register
 *
 * @param rdsr_res : address to which status register value is to be written
 *
 * @return tEtaStatus
 */
tEtaStatus spiFlashReadStatusRegister(void *rdsr_res);

/**
 * Read control register
 *
 * @param rdcr_res : address to which control register value is to be written
 *
 * @return tEtaStatus
 */
tEtaStatus spiFlashReadControlRegister(RDCR_RES_t *rdcr_res);

/**
 * Write status and control register
 *
 * @param wrsr : status and control register data to be written
 *
 * @return tEtaStatus
 */
tEtaStatus spiFlashWriteStatusControlRegister(WRSR_t *wrsr);

/**
 * Fast read data from offset
 *
 * @param offset : starting address from which data is to be read
 * @param length  : length to be read
 * @param buff   : pointer to which the read data should be placed
 *
 * @return tEtaStatus
 */
tEtaStatus spiFlashFastReadData(uint32_t offset, uint32_t length, void *buff);

/**
 * Erase 4K sector in which the offset is present
 *
 * @param offset : any address of the block to be erased
 *
 * @return tEtaStatus
 */
tEtaStatus spiFlashSectorErase(uint32_t offset);

/**
 * Erase 32K block in which the offset is present
 *
 * @param offset : any address of the block to be erased
 *
 * @return tEtaStatus
 */
tEtaStatus spiFlashBlockErase32K(uint32_t offset);

/**
 * Erase 64K block in which the offset is present
 *
 * @param offset : any address of the block to be erased
 *
 * @return tEtaStatus
 */
tEtaStatus spiFlashBlockErase(uint32_t offset);

/**
 * Enter deep power down mode
 *
 * @return tEtaStatus
 */
tEtaStatus spiFlashDeepPowerDownMode(void);

/**
 * Exit from deep power down mode
 *
 * @return tEtaStatus
 */
tEtaStatus spiFlashExitDeepPowerDownMode(void);

/**
 * Initialize the specified Spi based on configuration structure
 *
 * @param iNum       : spi port number
 * @param spiChipSel : Gpio pin number for chip select pin
 *
 * @return tEtaStatus
 */
tEtaStatus spiFlashRegister(tSpiNum iNum, tSpiChipSel spiChipSel);

/**
 * Read data from a particluar address
 *
 * @param offset   : flash memory address from which data has to be read
 * @param buff      : pointer to which the read data should be placed
 * @param length    : number of bytes to be read
 *
 * @return tEtaStatus
 */
tEtaStatus spiFlashReadData(uint32_t offset, uint32_t length, void *buff);

/**
 * Update the data at a particular address range, expects the location to be in erased state.
 * If not in erased state, only 1's will be converted to 0's, reverse will have no effect
 *
 * @param offset   : flash memory address to which data is to be written
 * @param data      : pointer to data to be written
 * @param length    : number of bytes to be written, max : 4096 bytes
 *
 * @return tEtaStatus
 */
tEtaStatus spiFlashWriteData(uint32_t offset, uint32_t length, void *data);

/**
 * Erase the whole chip
 *
 * @return tEtaStatus
 */
tEtaStatus spiFlashChipErase(void);

/**
 * Write data to a sectors
 *
 * @param data         : pointer to data source
 * @param sectorNumber : sector number from which write should start
 * @param sectorCount  : number of sectors to be written
 *
 * @return tEtaStatus
 */
tEtaStatus spiFlashWriteSectors(void* data, uint16_t sectorNumber, uint16_t sectorCount);

/**
 * Read data from sectors
 *
 * @param data         : pointer to data destination
 * @param sectorNumber : sector number from which read should start
 * @param sectorCount  : number of sectors to be read
 *
 * @return tEtaStatus
 */
tEtaStatus spiFlashReadSectors(void* data, uint16_t sectorNumber, uint16_t sectorCount);

/**
 * Erase data from sectors, erases are be optimized using Block32 and Block command
 *
 * @param sectorNumber : sector number from which erase should start
 * @param sectorCount  : number of sectors to be erased
 *
 * @return tEtaStatus
 */
tEtaStatus spiFlashEraseSectors(uint16_t sectorNumber, uint16_t sectorCount);