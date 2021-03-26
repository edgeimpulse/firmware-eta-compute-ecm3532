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
#include "ei_device_eta_ecm3532.h"
#include "ei_eta_fs_commands.h"

#include "ei_camera.h"
#include "ei_inertialsensor.h"
#include "ei_microphone.h"

#include "eta_bsp.h"
#include "repl.h"
extern "C" {
#include "config.h"
#include "eta_csp_isr.h"
#include "eta_csp_timer.h"
#include "eta_csp_uart.h"
#include "executor_public.h"
#include "timer_hal.h"
#include "uart_hal.h"
}

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <math.h>

/** Memory location for the arduino device address */
#define DEVICE_ID_LSB_ADDR ((uint32_t)0x100000A4)
#define DEVICE_ID_MSB_ADDR ((uint32_t)0x100000A8)

/** Max size for device id array */
#define DEVICE_ID_MAX_SIZE 32

/** Sensors */
typedef enum
{
    MICROPHONE = 0,
    ACCELEROMETER
} used_sensors_t;

/** Data Output Baudrate */
const ei_device_data_output_baudrate_t ei_dev_max_data_output_baudrate = {
    "921600",
    eUartBaud921600,
};

const ei_device_data_output_baudrate_t ei_dev_default_data_output_baudrate = {
    "115200",
    eUartBaud115200,
};

/** Device type */
#if (CONFIG_AI_VISION_BOARD == 1)
static const char *ei_device_type = "ETA_EMC3532_VISION";
#else
static const char *ei_device_type = "ETA_EMC3532_AI_A"; /** @todo Get actual device type */
#endif

/** Device id array */
static char ei_device_id[DEVICE_ID_MAX_SIZE] = { "34:52:52:22:57:98" };

/** Device object, for this class only 1 object should exist */
EiDeviceEtaEcm3532 EiDevice;

static tEiState ei_program_state = eiStateIdle;

/** UART used for edge impulse communication */
tUart etaUart;
#define EI_USED_UART &etaUart

/* Private function declarations ------------------------------------------- */
static int get_id_c(uint8_t out_buffer[32], size_t *out_size);
static int get_type_c(uint8_t out_buffer[32], size_t *out_size);
static bool get_wifi_connection_status_c(void);
static bool get_wifi_present_status_c(void);
static void timer_callback(void *arg);
static bool read_sample_buffer(size_t begin, size_t length, void (*data_fn)(uint8_t *, size_t));
static int get_data_output_baudrate_c(ei_device_data_output_baudrate_t *baudrate);
static void set_max_data_output_baudrate_c();
static void set_default_data_output_baudrate_c();

/* Public functions -------------------------------------------------------- */

EiDeviceEtaEcm3532::EiDeviceEtaEcm3532(void)
{
}

/**
 * @brief      For the device ID, the BLE mac address is used.
 *             The mac address string is copied to the out_buffer.
 *
 * @param      out_buffer  Destination array for id string
 * @param      out_size    Length of id string
 *
 * @return     0
 */
int EiDeviceEtaEcm3532::get_id(uint8_t out_buffer[32], size_t *out_size)
{
    return get_id_c(out_buffer, out_size);
}

/**
 * @brief      Gets the identifier pointer.
 *
 * @return     The identifier pointer.
 */
const char *EiDeviceEtaEcm3532::get_id_pointer(void)
{
    return (const char *)ei_device_id;
}

/**
 * @brief      Copy device type in out_buffer & update out_size
 *
 * @param      out_buffer  Destination array for device type string
 * @param      out_size    Length of string
 *
 * @return     -1 if device type string exceeds out_buffer
 */
int EiDeviceEtaEcm3532::get_type(uint8_t out_buffer[32], size_t *out_size)
{
    return get_type_c(out_buffer, out_size);
}

/**
 * @brief      Gets the type pointer.
 *
 * @return     The type pointer.
 */
const char *EiDeviceEtaEcm3532::get_type_pointer(void)
{
    return (const char *)ei_device_type;
}

/**
 * @brief      No Wifi available for device.
 *
 * @return     Always return false
 */
bool EiDeviceEtaEcm3532::get_wifi_connection_status(void)
{
    return false;
}

/**
 * @brief      No Wifi available for device.
 *
 * @return     Always return false
 */
bool EiDeviceEtaEcm3532::get_wifi_present_status(void)
{
    return false;
}

/**
 * @brief      Create sensor list with sensor specs
 *             The studio and daemon require this list
 * @param      sensor_list       Place pointer to sensor list
 * @param      sensor_list_size  Write number of sensors here
 *
 * @return     False if all went ok
 */
bool EiDeviceEtaEcm3532::get_sensor_list(
    const ei_device_sensor_t **sensor_list,
    size_t *sensor_list_size)
{
    /* Calculate number of bytes available on flash for sampling, reserve 1 block for header + overhead */
    uint32_t available_bytes = (ei_eta_fs_get_n_available_sample_blocks() - 1) *
        ei_eta_fs_get_block_size();

    sensors[MICROPHONE].name = "Built-in microphone";
    sensors[MICROPHONE].start_sampling_cb = &ei_microphone_sample_start;
    sensors[MICROPHONE].max_sample_length_s = available_bytes / (16000 * 2);
    sensors[MICROPHONE].frequencies[0] = 16000.0f;

#if (CONFIG_AI_SENSOR_BOARD == 1)
    sensors[ACCELEROMETER].name = "Built-in accelerometer";
    sensors[ACCELEROMETER].start_sampling_cb = &ei_inertial_setup_data_sampling;
    sensors[ACCELEROMETER].max_sample_length_s = available_bytes / (100 * SIZEOF_N_AXIS_SAMPLED);
    sensors[ACCELEROMETER].frequencies[0] = 62.5f;
    sensors[ACCELEROMETER].frequencies[1] = 250.0f;
    sensors[ACCELEROMETER].frequencies[2] = 500.0f;
#endif

    *sensor_list = sensors;
    *sensor_list_size = EI_DEVICE_N_SENSORS;

    return false;
}

#if (CONFIG_AI_VISION_BOARD == 1)
/**
 * @brief      Create resolution list for snapshot setting
 *             The studio and daemon require this list
 * @param      snapshot_list       Place pointer to resolution list
 * @param      snapshot_list_size  Write number of resolutions here
 *
 * @return     False if all went ok
 */
bool EiDeviceEtaEcm3532::get_snapshot_list(
    const ei_device_snapshot_resolutions_t **snapshot_list,
    size_t *snapshot_list_size,
    const char **color_depth)
{
    snapshot_resolutions[0].width = CONFIG_IMAGE_COLUMN_COUNT;
    snapshot_resolutions[0].height = CONFIG_IMAGE_ROW_COUNT;
    snapshot_resolutions[1].width = 128;
    snapshot_resolutions[1].height = 96;

#if defined(EI_CLASSIFIER_SENSOR) && EI_CLASSIFIER_SENSOR == EI_CLASSIFIER_SENSOR_CAMERA
    snapshot_resolutions[2].width = EI_CLASSIFIER_INPUT_WIDTH;
    snapshot_resolutions[2].height = EI_CLASSIFIER_INPUT_HEIGHT;
#endif

    *snapshot_list = snapshot_resolutions;
    *snapshot_list_size = EI_DEVICE_N_RESOLUTIONS;
    *color_depth = "Grayscale";

    return false;
}
#endif

/**
 * @brief      Create resolution list for resizing
 * @param      resize_list       Place pointer to resolution list
 * @param      resize_list_size  Write number of resolutions here
 *
 * @return     False if all went ok
 */
bool EiDeviceEtaEcm3532::get_resize_list(
    const ei_device_resize_resolutions_t **resize_list,
    size_t *resize_list_size)
{
    resize_resolutions[0].width = 128;
    resize_resolutions[0].height = 96;

    resize_resolutions[1].width = 160;
    resize_resolutions[1].height = 120;

    resize_resolutions[2].width = 200;
    resize_resolutions[2].height = 150;

    resize_resolutions[3].width = 256;
    resize_resolutions[3].height = 192;

    *resize_list = resize_resolutions;
    *resize_list_size = EI_DEVICE_N_RESIZE_RESOLUTIONS;

    return false;
}

/**
 * @brief      Device specific delay ms implementation
 *
 * @param[in]  milliseconds  The milliseconds
 */
void EiDeviceEtaEcm3532::delay_ms(uint32_t milliseconds)
{
    EtaCspTimerDelayMs(milliseconds);
}

void EiDeviceEtaEcm3532::setup_led_control(void)
{
    tHalTmr *AppTmr = NULL;
    /* Create Periodic timer */
    AppTmr = HalTmrCreate(HalTmrCh0, HalTmrPeriodic, 1000, timer_callback, AppTmr);
    if (!AppTmr)
        ei_printf("TImer Creatation Failed\n");
    else
        HalTmrStart(AppTmr);
}

void EiDeviceEtaEcm3532::set_state(tEiState state)
{
    ei_program_state = state;

    if (state == eiStateFinished) {
        EtaBspLedsClearAll();
        EtaCspTimerDelayMs(100);
        EtaBspLedSet(ETA_BSP_LED0);
        EtaCspTimerDelayMs(100);
        EtaBspLedSet(ETA_BSP_LED1);
#if (CONFIG_AI_SENSOR_BOARD == 1)
        EtaCspTimerDelayMs(100);
        EtaBspLedSet(ETA_BSP_LED4);
        EtaCspTimerDelayMs(100);
        EtaBspLedSet(ETA_BSP_LED3);
        EtaCspTimerDelayMs(100);
        EtaBspLedSet(ETA_BSP_LED2);
#endif
        EtaCspTimerDelayMs(200);
        EtaBspLedsClearAll();
        EtaCspTimerDelayMs(200);
        EtaBspLedsSetAll();
        EtaCspTimerDelayMs(200);
        EtaBspLedsClearAll();

        ei_program_state = eiStateIdle;
    }
}

/**
 * @brief      Get the data output baudrate
 *
 * @param      baudrate    Baudrate used to output data
 *
 * @return     0
 */
int EiDeviceEtaEcm3532::get_data_output_baudrate(ei_device_data_output_baudrate_t *baudrate)
{
    return get_data_output_baudrate_c(baudrate);
}

/**
 * @brief      Set output baudrate to max
 *
 */
void EiDeviceEtaEcm3532::set_max_data_output_baudrate()
{
    set_max_data_output_baudrate_c();
}

/**
 * @brief      Set output baudrate to default
 *
 */
void EiDeviceEtaEcm3532::set_default_data_output_baudrate()
{
    set_default_data_output_baudrate_c();
}

/**
 * @brief      Get a C callback for the get_id method
 *
 * @return     Pointer to c get function
 */
c_callback EiDeviceEtaEcm3532::get_id_function(void)
{
#define FLASH_INFO_OFFSET_UNIQUE_ID (0x1f0 >> 2)

    uint32_t ui32Buffer[ETA_CSP_FLASH_PAGE_SIZE_WORDS];

    EtaCspFlashInfoGet(0x01000000, (uint8_t *)ui32Buffer, ETA_CSP_FLASH_PAGE_SIZE_BYTES);

    /* Setup device ID */
    snprintf(
        &ei_device_id[0],
        DEVICE_ID_MAX_SIZE,
        "%02X:%02X:%02X:%02X:%02X:%02X",
        (uint8_t)((ui32Buffer[FLASH_INFO_OFFSET_UNIQUE_ID + 0] >> 0) & 0xFF),
        (uint8_t)((ui32Buffer[FLASH_INFO_OFFSET_UNIQUE_ID + 0] >> 8) & 0xFF),
        (uint8_t)((ui32Buffer[FLASH_INFO_OFFSET_UNIQUE_ID + 0] >> 16) & 0xFF),
        (uint8_t)((ui32Buffer[FLASH_INFO_OFFSET_UNIQUE_ID + 0] >> 24) & 0xFF),
        (uint8_t)((ui32Buffer[FLASH_INFO_OFFSET_UNIQUE_ID + 1] >> 0) & 0xFF),
        (uint8_t)((ui32Buffer[FLASH_INFO_OFFSET_UNIQUE_ID + 1] >> 8) & 0xFF));

    return &get_id_c;
}

/**
 * @brief      Get a C callback for the get_type method
 *
 * @return     Pointer to c get function
 */
c_callback EiDeviceEtaEcm3532::get_type_function(void)
{
    return &get_type_c;
}

/**
 * @brief      Get a C callback for the get_wifi_connection_status method
 *
 * @return     Pointer to c get function
 */
c_callback_status EiDeviceEtaEcm3532::get_wifi_connection_status_function(void)
{
    return &get_wifi_connection_status_c;
}

/**
 * @brief      Get a C callback for the wifi present method
 *
 * @return     The wifi present status function.
 */
c_callback_status EiDeviceEtaEcm3532::get_wifi_present_status_function(void)
{
    return &get_wifi_present_status_c;
}

/**
 * @brief      Get a C callback to the read sample buffer function
 *
 * @return     The read sample buffer function.
 */
c_callback_read_sample_buffer EiDeviceEtaEcm3532::get_read_sample_buffer_function(void)
{
    return &read_sample_buffer;
}

#if (CONFIG_BLE_A31R118 == 1)
/**
 * @brief      Read in characters from BLE and parse to repl handler
 */
static void ei_ble_command_line(void)
{
    static char bl_msg = 0;

    while (EtaCspUartRxFifoDepthGet(CONFIG_BLE_UART)) {

        rx_callback(EtaCspUartGetc(CONFIG_BLE_UART, 1));
        bl_msg = 1;
    }

    /* Use character rx timeout to append carriage return */
    if (bl_msg) {
        if (++bl_msg > 100) {
            rx_callback('\r');
            bl_msg = 0;
        }
    }
}

/**
 * @brief      Print over Uart BLE
 *
 * @param[in]  format     Text string
 * @param[in]  Variable argument list
 */
void ei_printf_ble(const char *format, ...)
{
    char print_buf[64] = { 0 };

    va_list args;
    va_start(args, format);
    int r = vsnprintf(print_buf, sizeof(print_buf), format, args);
    va_end(args);

    if (r > 0) {
        EtaCspUartPuts(CONFIG_BLE_UART, print_buf);
    }
}

#endif

/**
 * @brief      Get characters for uart pheripheral and send to repl
 */
void ei_command_line_handle(void *args)
{

    ei_microphone_init();

    while (1) {
        while (EtaCspUartRxFifoDepthGet(EI_USED_UART)) {
            rx_callback(EtaCspUartGetc(EI_USED_UART, 1));
        }        
#if (CONFIG_BLE_A31R118 == 1)
        ei_ble_command_line();
#endif
    }
}

/**
 * @brief      Call this function periocally during inference to 
 *             detect a user stop command
 *
 * @return     true if user requested stop
 */
bool ei_user_invoke_stop(void)
{
    bool stop_found = false;

    while (EtaCspUartRxFifoDepthGet(EI_USED_UART)) {
        if (EtaCspUartGetc(EI_USED_UART, 1) == 'b') {
            stop_found = true;
            break;
        }
    }

/* Listen for stop trigger on BLE */
#if (CONFIG_BLE_A31R118 == 1)
    while (EtaCspUartRxFifoDepthGet(CONFIG_BLE_UART)) {
        if (EtaCspUartGetc(CONFIG_BLE_UART, 1) == 'b') {
            stop_found = true;
            break;
        }
    }
#endif

    return stop_found;
}

/**
 * @brief      Setup the serial port
 */
void ei_serial_setup(void)
{
    EtaCspUartInit(&etaUart, (tUartNum)CONFIG_DEBUG_UART, eUartBaud115200, eUartFlowControlNone);
}

/**
 * @brief      Printf function uses vsnprintf and output using Eta lib function
 *
 * @param[in]  format     Variable argument list
 */
void ei_printf(const char *format, ...)
{
    char print_buf[1024] = { 0 };

    va_list args;
    va_start(args, format);
    int r = vsnprintf(print_buf, sizeof(print_buf), format, args);
    va_end(args);

    if (r > 0) {
        EtaCspUartPuts(EI_USED_UART, print_buf);
    }
}

/**
 * @brief      Print a float value, bypassing the stdio %f
 *             Uses standard serial out
 *
 * @param[in]  f     Float value to print.
 */
void ei_printf_float(float f)
{
    float n = f;

    static double PRECISION = 0.00001;
    static int MAX_NUMBER_STRING_SIZE = 32;

    char s[MAX_NUMBER_STRING_SIZE];

    if (n == 0.0) {
        ei_printf("0.00000");
    }
    else {
        int digit, m; //, m1;
        char *c = s;
        int neg = (n < 0);
        if (neg) {
            n = -n;
        }
        // calculate magnitude
        m = log10(n);
        if (neg) {
            *(c++) = '-';
        }
        if (m < 1.0) {
            m = 0;
        }
        // convert the number
        while (n > PRECISION || m >= 0) {
            double weight = pow(10.0, m);
            if (weight > 0 && !isinf(weight)) {
                digit = floor(n / weight);
                n -= (digit * weight);
                *(c++) = '0' + digit;
            }
            if (m == 0 && n > 0) {
                *(c++) = '.';
            }
            m--;
        }
        *(c) = '\0';
        ei_write_string(s, c - s);
    }
}

/**
 * @brief      Write serial data with length to Serial output
 *
 * @param      data    The data
 * @param[in]  length  The length
 */
void ei_write_string(char *data, int length)
{
    for (int i = 0; i < length; i++) {
        EtaCspUartPutc(EI_USED_UART, *(data++));
    }
}

/**
 * @brief      Write single character to serial output
 *
 * @param[in]  cChar  The character
 */
void ei_putc(char cChar)
{
    EtaCspUartPutc(EI_USED_UART, cChar);
}

/* Private functions ------------------------------------------------------- */
static void timer_callback(void *arg)
{
    static char toggle = 0;

    if (toggle) {
        switch (ei_program_state) {
        case eiStateErasingFlash:
            EtaBspLedSet(ETA_BSP_LED1);
            break;
        case eiStateSampling:
#if (CONFIG_AI_SENSOR_BOARD == 1)
            EtaBspLedSet(ETA_BSP_LED2);
#else
            EtaBspLedSet(ETA_BSP_LED0);
#endif
            break;
        case eiStateUploading:
#if (CONFIG_AI_SENSOR_BOARD == 1)
            EtaBspLedSet(ETA_BSP_LED3);
#else
            EtaBspLedSet(ETA_BSP_LED0);
            EtaBspLedSet(ETA_BSP_LED1);
#endif
            break;
        default:
            break;
        }
    }
    else {
        if (ei_program_state != eiStateFinished) {
            EtaBspLedsClearAll();
        }
    }
    toggle ^= 1;
}

static int get_id_c(uint8_t out_buffer[32], size_t *out_size)
{
    size_t length = strlen(ei_device_id);

    if (length < 32) {
        memcpy(out_buffer, ei_device_id, length);

        *out_size = length;
        return 0;
    }

    else {
        *out_size = 0;
        return -1;
    }
}

static int get_type_c(uint8_t out_buffer[32], size_t *out_size)
{
    size_t length = strlen(ei_device_type);

    if (length < 32) {
        memcpy(out_buffer, ei_device_type, length);

        *out_size = length;
        return 0;
    }

    else {
        *out_size = 0;
        return -1;
    }
}

static bool get_wifi_connection_status_c(void)
{
    return false;
}

static bool get_wifi_present_status_c(void)
{
    return false;
}

static int get_data_output_baudrate_c(ei_device_data_output_baudrate_t *baudrate)
{
    size_t length = strlen(ei_dev_max_data_output_baudrate.str);

    if (length < 32) {
        memcpy(baudrate, &ei_dev_max_data_output_baudrate, sizeof(ei_device_data_output_baudrate_t));
        return 0;
    }
    else {
        return -1;
    }
}

static void set_max_data_output_baudrate_c()
{
    EtaCspUartBaudSet(&etaUart, (tUartBaud)ei_dev_max_data_output_baudrate.val);
}

static void set_default_data_output_baudrate_c()
{
    EtaCspUartBaudSet(&etaUart, (tUartBaud)ei_dev_default_data_output_baudrate.val);
}

/**
 * @brief      Read samples from sample memory and send to data_fn function
 *
 * @param[in]  begin    Start address
 * @param[in]  length   Length of samples in bytes
 * @param[in]  data_fn  Callback function for sample data
 *
 * @return     false on flash read function
 */
static bool read_sample_buffer(size_t begin, size_t length, void (*data_fn)(uint8_t *, size_t))
{
    size_t pos = begin;
    size_t bytes_left = length;
    bool retVal;

    EiDevice.set_state(eiStateUploading);

    // we're encoding as base64 in AT+READFILE, so this needs to be divisable by 3
    uint8_t buffer[513];
    while (1) {
        size_t bytes_to_read = sizeof(buffer);
        if (bytes_to_read > bytes_left) {
            bytes_to_read = bytes_left;
        }
        if (bytes_to_read == 0) {
            retVal = true;
            break;
        }

        int r = ei_eta_fs_read_sample_data(buffer, pos, bytes_to_read);
        if (r != 0) {
            retVal = false;
            break;
        }
        data_fn(buffer, bytes_to_read);

        pos += bytes_to_read;
        bytes_left -= bytes_to_read;
    }

    EiDevice.set_state(eiStateFinished);

    return retVal;
}