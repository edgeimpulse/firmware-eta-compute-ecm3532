/* icm20602_spi.c - spi access routines for ICM20602 driver
 */

#include "gpio_hal.h"
#include "ecm3532_pinconfig.h"
#include "spi_hal.h"
#include "icm20602.h"

#include <stdint.h>
#include <stdlib.h>

// icm sensor connected on spi master 1
static uint8_t icm_spi_bus = 1;



/***************************************************************************//**
 *
 * _Icm20602Read - Read from the Icm20602
 *
 * @param icm_spi_bus SPI bus/instance id.
 * @param ui8Reg Register to read.
 * @param ui32NumBytes Number of bytes to read.
 *
 * @return Return the read value opf the requested register.
 *
 ******************************************************************************/
static void _Icm20602Read(uint8_t icm_spi_bus, uint8_t ui8Reg, uint8_t* pui8RxData, uint32_t ui32NumBytes)
{
    uint8_t pui8TxData[1];

    //
    // Setup the read.
    //
    pui8TxData[0] = ui8Reg | 0x80;

    //Enable CS of icm20602 sensor in SPI mode
    HalSpiAssertCS(icm_spi_bus, PIN30, CS_ACTIVE_LOW);
    HalSpiXfer(icm_spi_bus, pui8TxData, 1, pui8RxData, ui32NumBytes, NULL, NULL);
    //Disable CS of icm20602 sensor
    HalSpiDeAssertCS(icm_spi_bus, PIN30);
}

static int icm20602_spi_read_data(struct icm20602_data *data, uint8_t reg_addr,
				 uint8_t *value, uint8_t len)
{
    _Icm20602Read(icm_spi_bus, reg_addr, value, len);
    return 0;
}


/***************************************************************************//**
 *
 * _Icm20602Write - Write to the Icm20602
 *
 * @param icm_spi_bus SPI bus/instance id.
 * @param ui8Reg Register to write.
 * @param ui8Value TODO
 *
 ******************************************************************************/
static void _Icm20602Write(uint8_t icm_spi_bus, uint8_t ui8Reg, uint8_t ui8Value)
{
    uint8_t pui8TxData[2];

    //
    // Write the register.
    //
    pui8TxData[0] = ui8Reg & 0x7F;
    pui8TxData[1] = ui8Value;

    //Enable CS of icm20602 sensor in SPI mode
    HalSpiAssertCS(icm_spi_bus, PIN30, CS_ACTIVE_LOW);
    HalSpiXfer(icm_spi_bus, pui8TxData, 2, NULL, 0, NULL, NULL);
    //Disable CS of icm20602 sensor
    HalSpiDeAssertCS(icm_spi_bus, PIN30);
    vTaskDelay(10);
}


static int icm20602_spi_write_data(struct icm20602_data *data, uint8_t reg_addr,
				  uint8_t value)
{
    _Icm20602Write(icm_spi_bus, reg_addr, value);
    return 0;
}

static int icm20602_spi_read_reg(struct icm20602_data *data, uint8_t reg_addr,
				uint8_t *value)
{
    _Icm20602Read(icm_spi_bus, reg_addr, value, 1);
    return 0;
}

static int icm20602_spi_update_reg(struct icm20602_data *data, uint8_t reg_addr,
				  uint8_t mask, uint8_t value)
{
    uint8_t temp;
    _Icm20602Read(icm_spi_bus, reg_addr, &temp, 1);

    temp = temp & (~mask);
    temp |= (value & mask);

    _Icm20602Write(icm_spi_bus, reg_addr, temp);

    return 0;
}
static const struct icm20602_transfer_function icm20602_spi_transfer_fn = {
    .read_data = icm20602_spi_read_data,
    .write_data = icm20602_spi_write_data,
    .read_reg  = icm20602_spi_read_reg,
    .update_reg = icm20602_spi_update_reg,
};

int icm20602_spi_init(struct icm20602_data *data)
{
    // Power on the sensors.
    HalGpioOutInit(PIN11, 1);//ETA_BSP_SPI_PWR_PIN = eGpioBit11
    HalGpioOutInit(PIN31, 1);//Disble SPI Flash (optional?)
    HalGpioOutInit(PIN30, 1);//Initialze GPIO30

    data->hw_tf = &icm20602_spi_transfer_fn;
    return 0;
}
