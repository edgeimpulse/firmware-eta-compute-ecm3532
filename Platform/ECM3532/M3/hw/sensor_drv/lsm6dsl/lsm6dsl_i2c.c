/* lsm6dsl_i2c.c - I2C routines for LSM6DSL driver
 */

/*
 * Copyright (c) 2018 STMicroelectronics
 *
 * SPDX-License-Identifier: Apache-2.0
 */

//#include <string.h>
//#include <drivers/i2c.h>
//#include <logging/log.h>

#include "lsm6dsl.h"
#include "i2c_hal.h"
#include <stdint.h>
#include <stdlib.h>

//#ifdef DT_ST_LSM6DSL_BUS_I2C

static uint16_t lsm6dsl_i2c_slave_addr = 0x6B;
static uint8_t lsm_i2c_bus = 1;


static int lsm6dsl_i2c_read_data(struct lsm6dsl_data *data, uint8_t reg_addr,
				 uint8_t *value, uint8_t len)
{
    HalI2cRead(lsm_i2c_bus, lsm6dsl_i2c_slave_addr, reg_addr, 1, value, len, NULL, NULL);

    return 0;
}

static int lsm6dsl_i2c_write_data(struct lsm6dsl_data *data, uint8_t reg_addr,
				  uint8_t *value, uint8_t len)
{
    HalI2cWrite(lsm_i2c_bus, lsm6dsl_i2c_slave_addr, reg_addr, 1, value, len, NULL, NULL);
    return 0;
}

static int lsm6dsl_i2c_read_reg(struct lsm6dsl_data *data, uint8_t reg_addr,
				uint8_t *value)
{
    /*
	return i2c_reg_read_byte(data->comm_master, lsm6dsl_i2c_slave_addr,
				 reg_addr, value);
    */
    HalI2cRead(lsm_i2c_bus, lsm6dsl_i2c_slave_addr, reg_addr, 1, value, 1, NULL, NULL);
    return 0;
}

static int lsm6dsl_i2c_update_reg(struct lsm6dsl_data *data, uint8_t reg_addr,
				  uint8_t mask, uint8_t value)
{
    uint8_t temp;
	/*return i2c_reg_update_byte(data->comm_master, lsm6dsl_i2c_slave_addr,
				   reg_addr, mask, value);
                   */
    HalI2cRead(lsm_i2c_bus, lsm6dsl_i2c_slave_addr, reg_addr, 1, &temp, 1, NULL, NULL);

    temp = temp & (~mask);
    temp |= (value & mask);

    HalI2cWrite(lsm_i2c_bus, lsm6dsl_i2c_slave_addr, reg_addr, 1, &temp, 1, NULL, NULL);
    return 0;
}
static const struct lsm6dsl_transfer_function lsm6dsl_i2c_transfer_fn = {
	.read_data = lsm6dsl_i2c_read_data,
	.write_data = lsm6dsl_i2c_write_data,
	.read_reg  = lsm6dsl_i2c_read_reg,
	.update_reg = lsm6dsl_i2c_update_reg,
};

int lsm6dsl_i2c_init(struct lsm6dsl_data *data)
{
	data->hw_tf = &lsm6dsl_i2c_transfer_fn;
	return 0;
}

//#endif /* DT_ST_LSM6DSL_BUS_I2C */
