/* The Clear BSD License
 *
 * Copyright (c) 2025 EdgeImpulse Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted (subject to the limitations in the disclaimer
 * below) provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 *   * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 *   * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY
 * THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef EI_DEVICE_ETA_ECM3532
#define EI_DEVICE_ETA_ECM3532

/* Include ----------------------------------------------------------------- */
#include "config.h"
#include "ei_device_info.h"
#include "eta_csp_m3.h"

/** Number of sensors used */
#if (CONFIG_AI_VISION_BOARD == 1)
#define EI_DEVICE_N_SENSORS 1
#else
#define EI_DEVICE_N_SENSORS 2
#endif

#if defined(EI_CLASSIFIER_SENSOR) && EI_CLASSIFIER_SENSOR == EI_CLASSIFIER_SENSOR_CAMERA
#define EI_DEVICE_N_RESOLUTIONS     3
#else
#define EI_DEVICE_N_RESOLUTIONS     2
#endif

#define EI_DEVICE_N_RESIZE_RESOLUTIONS      4

typedef enum
{
    eiStateIdle = 0,
    eiStateErasingFlash,
    eiStateSampling,
    eiStateUploading,
    eiStateFinished

} tEiState;

/** C Callback types */
typedef int (*c_callback)(uint8_t out_buffer[32], size_t *out_size);
typedef bool (*c_callback_status)(void);
typedef bool (*c_callback_read_sample_buffer)(
    size_t begin,
    size_t length,
    void (*data_fn)(uint8_t *, size_t));

/**
 * @brief      Class description and implementation of device specific 
 * 			   characteristics
 */
class EiDeviceEtaEcm3532 : public EiDeviceInfo {
private:
    ei_device_sensor_t sensors[EI_DEVICE_N_SENSORS];
    ei_device_snapshot_resolutions_t snapshot_resolutions[EI_DEVICE_N_RESOLUTIONS];
    ei_device_resize_resolutions_t resize_resolutions[EI_DEVICE_N_RESIZE_RESOLUTIONS];
public:
    EiDeviceEtaEcm3532(void);

    int get_id(uint8_t out_buffer[32], size_t *out_size);
    const char *get_id_pointer(void);
    int get_type(uint8_t out_buffer[32], size_t *out_size);
    const char *get_type_pointer(void);
    bool get_wifi_connection_status(void);
    bool get_wifi_present_status();
    bool get_sensor_list(const ei_device_sensor_t **sensor_list, size_t *sensor_list_size);
#if (CONFIG_AI_VISION_BOARD == 1)
    bool get_snapshot_list(const ei_device_snapshot_resolutions_t **resolution_list, size_t *resolution_list_size,
                        const char **color_depth);
#endif
    bool get_resize_list(const ei_device_resize_resolutions_t **resize_list, size_t *resize_list_size);
    void delay_ms(uint32_t milliseconds);
    void setup_led_control(void);
    void set_state(tEiState state);
	int get_data_output_baudrate(ei_device_data_output_baudrate_t *baudrate);
	void set_default_data_output_baudrate();
	void set_max_data_output_baudrate();    

    c_callback get_id_function(void);
    c_callback get_type_function(void);
    c_callback_status get_wifi_connection_status_function(void);
    c_callback_status get_wifi_present_status_function(void);
    c_callback_read_sample_buffer get_read_sample_buffer_function(void);
};

/* Function prototypes ----------------------------------------------------- */
void ei_command_line_handle(void *args);
bool ei_user_invoke_stop(void);
void ei_serial_setup(void);
void ei_printf_ble(const char *format, ...);
void ei_printf(const char *format, ...);
void ei_printf_float(float f);
void ei_printfloat(int n_decimals, int n, ...);

void ei_write_string(char *data, int length);
void ei_putc(char cChar);

/* Reference to object for external usage ---------------------------------- */
extern EiDeviceEtaEcm3532 EiDevice;

#endif