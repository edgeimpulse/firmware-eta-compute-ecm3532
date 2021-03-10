/***************************************************************************//**
 *
 * @file i2c_test3/src/main.c
 *
 * @brief This file contains the main function for i2c_test3.
 *
 * Copyright (C) 2020 Eta Compute, Inc
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This is part of revision ${version} of the Tensai Software Development Kit.
 *
 ******************************************************************************/

/*
 * i2c_test3 main.c
 *
 * CSP variant:
 *
 * Polling testing of both I2C ports in master mode.
 * Also has example code for writing / reading an external SEEPROM.
 *
 */


#include "eta_csp_io.h"
#include "dspI2c.h"


#define IFDBG if(1)
#define BYTES_TO_WRITE 4

#define LSM6DSL_SLAVE_ADDR1 0x6B   //7bit slave address if SD0/SA0 connected to VCC
#define LSM6DSL_SLAVE_ADDR2 0x6A   //7bit slave address if SD0/SA0 connected to GND
#define LSM6DSL_SLAVE_ADDR  0x6B

#define I2C_SLAVE_WR_ADDR   LSM6DSL_SLAVE_ADDR
#define I2C_SLAVE_RD_ADDR   LSM6DSL_SLAVE_ADDR


////////////////////////////////////////////////////////////////////////////////
// MAIN!!!
////////////////////////////////////////////////////////////////////////////////

int main() {
  int ui32RC = 0x0; // placeholder for your test return code
  int i;
  short int Accel_xyz[3];
  char act_data [BYTES_TO_WRITE];
  
  int result;

  EtaCspIoPrintfViaDspUart(true); //enable prints.
  etaPrintf("\r\ni2c unit test freeRTOS (lsm6dsl) starting\r\n");

  //EtaCspI2cInit(eI2cClk400kHz, eI2cSrcClk8MHz);
  i2c_setup();

  act_data[0] = 0xAA;
  etaPrintf("\r\nreading LSM6DSL_WHO_AM_I\r\n");
  result = i2c_read_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_WHO_AM_I, &act_data[0], 0x01);
  etaPrintf("\r\n result = %d, readValue = %d\r\n", result, act_data[0]);

  act_data[0] = 0xAA;
  etaPrintf("\r\nWriting ODR and Range register val = %x\r\n", LSM6DSL_CTRL1_XL_VAL);
  result = i2c_write_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_CTRL1_XL, LSM6DSL_CTRL1_XL_VAL);
  result = i2c_read_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_CTRL1_XL, &act_data[0], 0x01);
  etaPrintf("\r\n result = %d, readValue = %d\r\n", result, act_data[0]);

  for (i = 0; i < 1000; i++)
  {
      result = i2c_read_reg(LSM6DSL_SLAVE_ADDR, LSM6DSL_OUTX_L_XL, (char*)(&Accel_xyz[0]), 6);
      etaPrintf("\r\n i=%d result = %d, Ax = %d, Ay = %d, Az = %d\r\n", i, result, (int)Accel_xyz[0], (int)Accel_xyz[1], (int)Accel_xyz[2]);
      //add delay
      for (int j = 0; j < 1000; j++)
      {
          ;//nop
      }
  }

  volatile int temp_timeout = 1000;
  while (temp_timeout--);

  return ui32RC;
}

