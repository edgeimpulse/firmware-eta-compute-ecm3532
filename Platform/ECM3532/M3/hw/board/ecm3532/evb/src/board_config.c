/*******************************************************************************
 *
 * @file board_config.c
 *
 * @brief board pin mux setting
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
 ******************************************************************************/
#include "config.h"
#include "board.h"
#include "gpio_hal.h"
#include "reg_eta_ecm3532_m3.h"
#include "ecm3532_pinconfig.h"
#include "ioreg_tb.h"
#ifdef CONFIG_LSM6DSL_M3
#include "lsm6dsl.h"
#endif
#ifdef CONFIG_ICM20602_M3
#include "icm20602.h"
#endif

#ifdef CONFIG_SENSOR_MANAGER
#include "sensor_dev.h"
#endif
#include "print_util.h"

#ifdef CONFIG_LSM6DSL_M3
void Lsm6dslBoardInit(void)
{
    ecm3532_pin_config(PIN1, PIN1_FUNC_GPIO1);
    /* Do board specific init before calling Driver Init*/
    //Lsm6dslDrvInit();
}
#endif

#ifdef CONFIG_ICM20602_M3
void icm20602BoardInit(void)
{
    /* Do board specific init before calling Driver Init*/
    //icm20602DrvInit();
}
#endif


#ifdef CONFIG_RPC
void Lsm6dslDspBoardInit(void)
{
    HalGpioOutInit(0, 1);
}
#endif

#ifdef CONFIG_STUB_DRV
void SensorStubDriverInit(void);
void SensorStubDriverBoardInit(void)
{
    /* Do board specific init before calling Driver Init*/
    SensorStubDriverInit();
}
#endif

void board_init(void)
{
   ecm3532_set_pinmux();

#ifdef CONFIG_STUB_DRV
    SensorStubDriverBoardInit();
#endif

#ifdef CONFIG_LSM6DSL_M3
    Lsm6dslBoardInit();
#endif

#ifdef CONFIG_ICM20602_M3
    icm20602BoardInit();
#endif


#ifdef CONFIG_RPC
    Lsm6dslDspBoardInit();
#endif
#ifdef CONFIG_SENSOR_MANAGER
    SalInit();
#endif
}
