/***************************************************************************//**
 *
 * @file eta_devices_opt3001.h
 *
 * @brief This file contains opt3001 ambient light sensor definitions.
 *
 * Copyright (C) 2018 Eta Compute, Inc
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
 * This is part of revision 1.0.0b1 of the Tensai Chip Support Package.
 *
 ******************************************************************************/
#ifndef H_ETA_DEVICES_OPT3001_
#define H_ETA_DEVICES_OPT3001_
#include <stdint.h>

/**
 * Covertion time : 100 ms
 */
#define OPT3001_CONVERSION_TIME_100MS 0
/**
 * Covertion time : 800 ms
 */
#define OPT3001_CONVERSION_TIME_800MS (1 << 11)

/**
 * Covertion mode : SHUTDOWN
 */
#define OPT3001_CONVERSION_MODE_SHUTDOWN        0
/**
 * Covertion mode : SINGLE SHOT
 */
#define OPT3001_CONVERSION_MODE_SINGLE_SHOT     (1 << 9)
/**
 * Covertion mode : CONTINOUS
 */
#define OPT3001_CONVERSION_MODE_CONTINOUS       (3 << 9)

/**
 * Scale mode : Automatically scale based on the light intensity
 */
#define OPT3001_AUTOMATIC_SCALE (0xC << 12)

/**
 * Latch type : Hysteresis
 */
#define OPT3001_LATCH_HYSTERESIS    0
/**
 * Latch type : Window mode
 */
#define OPT3001_LATCH_WINDOW        (1 << 4)

/**
 * Interrupt polarity : Active low
 */
#define OPT3001_POLARITY_ACTIVE_LOW     0
/**
 * Interrupt polarity : Active high
 */
#define OPT3001_POLARITY_ACTIVE_HIGH    (1 << 3)

/**
 * Fault count : Trigger interrupt when consecutive fault is 1
 */
#define OPT3001_FAULT_COUNT_1   0
/**
 * Fault count : Trigger interrupt when consecutive fault is 2
 */
#define OPT3001_FAULT_COUNT_2   1
/**
 * Fault count : Trigger interrupt when consecutive fault is 4
 */
#define OPT3001_FAULT_COUNT_4   2
/**
 * Fault count : Trigger interrupt when consecutive fault is 8
 */
#define OPT3001_FAULT_COUNT_8   3

/**
 * Manufacturer ID
 */
#define OPT3001_MANUFACTURER_ID 0x5449
/**
 * Device ID
 */
#define OPT3001_DEVICE_ID       0x3001

#define OPT3001_END_OF_CONVERSION_MODE 0xC000

/**
 * Internal HW register address : Result
 */
#define OPT3001_REG_RESULT  0x00
/**
 * Internal HW register address : Configuration
 */
#define OPT3001_REG_CONFIG  0x01
/**
 * Internal HW register address : Low Limit
 */
#define OPT3001_REG_LOW_LIMIT   0x02
/**
 * Internal HW register address : High Limit
 */
#define OPT3001_REG_HIGH_LIMIT  0x03
/**
 * Internal HW register address : Manufacturer ID
 */
#define OPT3001_REG_MANUFACTURER_ID  0x7E
/**
 * Internal HW register address : Device ID
 */
#define OPT3001_REG_DEVICE_ID   0x7F

/** OPT3001 Results */
typedef struct {
    /** mantessa part */
    uint16_t R : 12;
    /** exponent part */
    uint16_t E : 4;
}OPT3001_RESULT_t;

/** OPT3001 Config register */
typedef struct 
{
    /** Fault count field */
    uint16_t faultCount : 2;
    /** Mask exponent field */
    uint16_t maskExponent : 1;
    /** Polarity field */
    uint16_t intPolarity : 1;
    /** Latch field */
    uint16_t latch : 1;
    /** Flag low field */
    uint16_t flagLow : 1;
    /** Flag high field */
    uint16_t flagHigh : 1;
    /** Conversion ready field */
    uint16_t convertionReady : 1;
    /** Overflow flag field */
    uint16_t overflowFlag : 1;
    /** Mode of conversion operation field */
    uint16_t conversionMode : 2;
    /** Conversion time field */
    uint16_t convertionTime : 1;
    /** Range number field */
    uint16_t rangeNumber : 4;
}OPT3001_CONFIG_t;

/**
 * Read a register from chip
 * @param reg address of the register to be read
 * @return 16 bit register data
*/
extern uint16_t Opt3001RegRead(uint8_t reg);

/**
 * Write a data into register on chip
 * @param reg address of the register to be written
 * @param val value to be written to the address
*/
extern void Opt3001RegWrite(uint8_t reg, uint16_t val);

/**
 * Convert register data to lux
 * @param data data to be converted to lux
 * @return lux value
*/
extern double Opt3001RegtoLux(uint16_t data);
#endif