/*******************************************************************************
 *
 * Copyright (C) 2019 Eta Compute, Inc
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

/* Include ------------------------------------------------------------------ */
#include "FreeRTOS.h"
#include "config.h"
#include "gpio_hal.h"
#include "queue.h"
#include "task.h"
#include "timer_hal.h"
#include "uart_hal.h"
#include <stdint.h>

extern "C" {
#include "eta_bsp.h"
#include "eta_devices_icm20602.h"
#if (CONFIG_BLE_A31R118 == 1)
#include "eta_devices_a31r118.h"
#endif
}

#include "ei_device_eta_ecm3532.h"
#include "ei_eta_fs_commands.h"
#include "at_cmds.h"
#include "ei_camera.h"
#include "ei_inertialsensor.h"
#include "ei_microphone.h"
#include "ei_run_impulse.h"

#include "spi_flash.h"


/* Private function prototypes --------------------------------------------- */
static void init_ai_hardware(void);

int main(void)
{

    EtaCspTimerDelayMs(1000);

    init_ai_hardware();

#if (CONFIG_BLE_A31R118 == 1)
    bleEnable();
    EtaCspTimerDelayMs(500);
    bleInit();
#endif

    ei_serial_setup();

    ei_printf(
        "Hello from Edge Impulse Device SDK.\r\n"
        "Compiled on %s %s\r\n",
        __DATE__,
        __TIME__);

#if (CONFIG_AI_SENSOR_BOARD == 1)
    /* Setup the inertial sensor */
    if (ei_inertial_init() == false) {
        ei_printf("Inertial sensor communication error occured\r\n");
    }
#else
    ei_camera_init();
#endif

    /* Intialize configuration */
    static ei_config_ctx_t config_ctx = { 0 };
    config_ctx.get_device_id = EiDevice.get_id_function();
    config_ctx.get_device_type = EiDevice.get_type_function();
    config_ctx.wifi_connection_status = EiDevice.get_wifi_connection_status_function();
    config_ctx.wifi_present = EiDevice.get_wifi_present_status_function();
    config_ctx.load_config = &ei_eta_fs_load_config;
    config_ctx.save_config = &ei_eta_fs_save_config;
    config_ctx.list_files = NULL;
    config_ctx.read_buffer = EiDevice.get_read_sample_buffer_function();
#if (CONFIG_AI_VISION_BOARD == 1)
    config_ctx.take_snapshot = &ei_camera_take_snapshot_encode_and_output;
    config_ctx.start_snapshot_stream = &ei_camera_start_snapshot_stream_encode_and_output;
#endif

    EI_CONFIG_ERROR cr = ei_config_init(&config_ctx);

    if (cr != EI_CONFIG_OK) {
        ei_printf("Failed to initialize configuration (%d)\n", cr);
    }
    else {
        ei_printf("Loaded configuration\n");
    }

    /* Setup the command line commands */
    ei_at_register_generic_cmds();
    ei_at_cmd_register("RUNIMPULSE", "Run the impulse", run_nn_normal);
    ei_at_cmd_register("RUNIMPULSEDEBUG", "Run the impulse with extra debug output", run_nn_debug);
    ei_at_cmd_register("RUNIMPULSECONT", "Run the impulse in continuous mode", run_nn_continuous_normal);
    ei_printf("Type AT+HELP to see a list of commands.\r\n> ");

    /* Run the LEDs to indicate we're here */
    EiDevice.setup_led_control();
    EiDevice.set_state(eiStateFinished);


    xTaskCreate(ei_command_line_handle, "CLI Handle", 2048, NULL, tskIDLE_PRIORITY + 1, NULL);

    /* Start the scheduler. */
    vTaskStartScheduler();

    return 0;
}

/**
 * @brief      Sensors / Flash and Leds init
 */
static void init_ai_hardware(void)
{
    tSpiConfig sSpiConfig;
    tIcm20602Cfg sIcm20602Config;

    /* Power on the sensors */
    EtaCspGpioOutputEnableBitSet(ETA_BSP_SPI_PWR_PIN);
    EtaCspGpioOutputBitSet(ETA_BSP_SPI_PWR_PIN);

#if (CONFIG_AI_SENSOR_BOARD == 1)
    /* Configure the SPI pins */
    EtaCspSocCtrlPadMuxSet(ETA_BSP_ICM20602_CLK, eSocCtrlPadMux1);
    EtaCspSocCtrlPadMuxSet(ETA_BSP_ICM20602_MOSI, eSocCtrlPadMux1);
    EtaCspSocCtrlPadMuxSet(ETA_BSP_ICM20602_MISO, eSocCtrlPadMux1);
    EtaCspSocCtrlPadMuxSet(ETA_BSP_ICM20602_SPI_CS, eSocCtrlPadMux1);
#endif

    EtaCspSocCtrlPadMuxSet((tGpioBit)CONFIG_SPI_FLASH_CS_PIN, eSocCtrlPadMux1);

    /* Fill in the SPI config structure */
    sSpiConfig.spiByteEndian = eSpiByteEndianLSB;
    sSpiConfig.spiBitEndian = eSpiBitEndianMSb;
    sSpiConfig.spiClkPhase = eSpiClkLeadEdge;
    sSpiConfig.spiClkPol = eSpiClkPolPos;
    sSpiConfig.spiClkFreq = ETA_BSP_ICM20602_SPI_SPEED;
    sSpiConfig.ui8IntSize = 0x4;
    sSpiConfig.ui8ICG = 0x0;
    sSpiConfig.ui8IBG = 0xff;

    /* Initialize the SPI */
    if (EtaCspSpiInit((tSpiNum)CONFIG_SPI_FLASH_SPI_NUM, sSpiConfig) != eEtaSuccess) {
        ei_printf("Error: SPI Init failed.\r\n");
    }

    EtaBspLedsInit();

#if (CONFIG_AI_SENSOR_BOARD == 1)
    /* Initialize the icm20602 */
    sIcm20602Config.iSpiInstance = eSpi1;
    sIcm20602Config.iSpiChipSel = ETA_BSP_ICM20602_SPI_CS_NUM;

    /* Initialize the ICM20602 */
    EtaDevicesIcm20602Init(&sIcm20602Config);

    /* Init the PDM */
    EtaCspGpioOutputEnableBitSet(ETA_BSP_PDM_PWR_PIN);
    EtaCspGpioDriveHighSet(ETA_BSP_PDM_PWR_PIN);
    EtaCspGpioOutputBitSet(ETA_BSP_PDM_PWR_PIN);
#endif
}