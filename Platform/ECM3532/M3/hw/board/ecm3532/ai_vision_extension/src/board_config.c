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
#ifdef CONFIG_SENSOR_MANAGER
#include "sensor_dev.h"
#endif
#include "print_util.h"

#if CONFIG_SPI_FLASH
#include "spi_flash.h"
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

#ifdef CONFIG_SPI_FLASH
    spiFlashRegister(CONFIG_SPI_FLASH_SPI_NUM, CONFIG_SPI_FLASH_CS_PIN);
#endif

#ifdef CONFIG_STUB_DRV
    SensorStubDriverBoardInit();
#endif

#ifdef CONFIG_CAM_HM0360
    HalGpioOutInit(PIN0, 1);
    HalGpioOutInit(PIN6, 0);
    HalGpioInInit(PIN12, 0);
#endif



#ifdef CONFIG_SENSOR_MANAGER
    SalInit();
#endif
}
