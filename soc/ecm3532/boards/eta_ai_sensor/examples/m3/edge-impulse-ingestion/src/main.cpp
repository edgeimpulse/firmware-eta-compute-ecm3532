/* Edge Impulse ingestion SDK
 * Copyright (c) 2020 EdgeImpulse Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/* Include ----------------------------------------------------------------- */
#include "eta_bsp.h"
#include "ei_device_eta_ecm3532.h"
#include "ei_run_impulse.h"
#include "ei_eta_fs_commands.h"
#include "at_cmds.h"
#include "ei_inertialsensor.h"
#include "ei_microphone.h"


/** ETA DSP binary */
extern tDspMem dsp_mem;

/* Private function prototypes --------------------------------------------- */
static void enable_irq(void);

/**
 * @brief      Main function
 *
 * @return     { description_of_the_return_value }
 */
int main(void)
{
    /* Init the board */
    EtaBspInit();
    
    /* Set M3 to 80MHz */
    if(EtaCspBuckAllSet(eBuckAo700Mv, eBuckM3FrequencyMode, 80000,
                        eBuckDspOff, 0, eBuckMem1100Mv, eBuckSetAllBucks,
                        eBuckStableWait) != eEtaSuccess)
    {
        EtaUtilsStdioPrintf("\r\nERROR: Failed to update the Frequency.\r\n");
        while(1)
        {
        }
    }

    ei_serial_setup();
    enable_irq();

    ei_printf("Hello from Edge Impulse Device SDK.\r\n"
              "Compiled on %s %s\r\n", __DATE__, __TIME__);

    /* Setup the inertial sensor */
    if(ei_inertial_init() == false) {
        ei_printf("Inerial sensor communication error occured\r\n");
    }    

    /* Intialize configuration */
    ei_config_ctx_t config_ctx = { 0 };
    config_ctx.get_device_id = EiDevice.get_id_function();
    config_ctx.get_device_type = EiDevice.get_type_function();
    config_ctx.wifi_connection_status = EiDevice.get_wifi_connection_status_function();
    config_ctx.wifi_present = EiDevice.get_wifi_present_status_function();
    config_ctx.load_config = &ei_eta_fs_load_config;
    config_ctx.save_config = &ei_eta_fs_save_config;
    config_ctx.list_files = NULL;    
    config_ctx.read_buffer = EiDevice.get_read_sample_buffer_function();


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
    ei_printf("Type AT+HELP to see a list of commands.\r\n> ");

    /* Init and load the DSP */
    EtaCspDspPrintViaM3Init();
    EtaCspDspPowerUp();
    EtaCspBuckDspFrequencySet(30000, eBuckStableWait);
    EtaCspDspLoaderSimple(&dsp_mem);

    /* Run the LEDs to indicate we're here */
    EiDevice.setup_led_control();
    EiDevice.set_state(eiStateFinished);

    /* Load and start the DSP once more */
    EtaCspDspPowerDown();
    EtaCspRtcTmrDelayMs(1000);
    EtaCspDspPowerUp();
    EtaCspDspLoaderSimple(&dsp_mem);
    ei_microphone_init();

    /* Check and handle command line input */
    while(1)
    {
        ei_command_line_handle();
    }

    return 0;
}

/**
 * @brief      Globally enable interrupts for the M3 core
 */
static void enable_irq(void)
{
//
// Keil compiler
//
#ifdef __CC_ARM
    uint32_t ui32Temp;
    __asm("  movs  ui32Temp,      #0");
    __asm("  msr   primask, ui32Temp");

//
// Everything else.
//
#else
    // fixme this needs to save/restore the enable state.
    __asm("  movs  r0,      #0");
    __asm("  msr   primask, r0");
#endif
}
