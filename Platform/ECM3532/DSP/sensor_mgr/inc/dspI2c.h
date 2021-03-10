#ifndef H_DSPI2C_
#define H_DSPI2C_

void i2c_setup(void);
int i2c_write_reg(char slaveAddr, char regAddr, char data);
int i2c_read_reg(char slaveAddr, char regAddr, char* rx_data, unsigned short int length);
int i2c_update_reg(char slaveAddr, char regAddr, char mask, char val);

#if 0
int write_reg(char address, char data);
int read_reg(char address, char* rx_data);
#endif

#define LSM6DSL_WHO_AM_I                ((0x0f))     /*!< Who_AM_I register */
#define LSM6DSL_CTRL1_XL 0x10
#define LSM6DSL_CTRL1_XL_VAL 0x28	// 4g 26 Hz
#define LSM6DSL_OUTX_L_XL               ((0x28))     /*!< Linear acceleration sensor X-axis output register L */

#endif //H_DSPI2C_
