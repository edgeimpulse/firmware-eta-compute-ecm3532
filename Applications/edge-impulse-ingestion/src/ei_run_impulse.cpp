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
#include "edge-impulse-sdk/classifier/ei_run_classifier.h"
#include "edge-impulse-sdk/dsp/numpy.hpp"
#include "ei_microphone.h"
#include "ei_inertialsensor.h"
#include "ei_camera.h"

#if defined(EI_CLASSIFIER_SENSOR) && EI_CLASSIFIER_SENSOR == EI_CLASSIFIER_SENSOR_ACCELEROMETER

/* Private variables ------------------------------------------------------- */
static float acc_buf[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE];
static int acc_sample_count = 0;

extern int base64_encode(const char *input, size_t input_size, char *output, size_t output_size);

#if (CONFIG_AI_SENSOR_BOARD == 1)
/**
 * @brief      Called by the inertial sensor module when a sample is received.
 *             Stores sample data in acc_buf
 * @param[in]  sample_buf  The sample buffer
 * @param[in]  byteLenght  The byte length
 *
 * @return     { description_of_the_return_value }
 */
static bool acc_data_callback(const void *sample_buf, uint32_t byteLength)
{
    float *buffer = (float *)sample_buf;
    for(uint32_t i = 0; i < (byteLength / sizeof(float)); i++) {
        acc_buf[acc_sample_count + i] = buffer[i];
    }
    
    return true;
}

/**
 * @brief      Sample data and run inferencing. Prints results to terminal
 *
 * @param[in]  debug  The debug
 */
void run_nn(bool debug) {

    bool stop_inferencing = false;
#if (CONFIG_BLE_A31R118 == 1)
    uint8_t prev_classification = EI_CLASSIFIER_LABEL_COUNT;
    ei_printf_ble("Start inference\r\n");
#endif

    // summary of inferencing settings (from model_metadata.h)
    ei_printf("Inferencing settings:\n");
    ei_printf("\tInterval: %.4f ms\n", (float)EI_CLASSIFIER_INTERVAL_MS);
    ei_printf("\tFrame size: %d\n", EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE);
    ei_printf("\tSample length: %.4f ms.\n", 1000.0f * static_cast<float>(EI_CLASSIFIER_RAW_SAMPLE_COUNT) /
                  (1000.0f / static_cast<float>(EI_CLASSIFIER_INTERVAL_MS)));
    ei_printf("\tNo. of classes: %d\n", sizeof(ei_classifier_inferencing_categories) / sizeof(ei_classifier_inferencing_categories[0]));

    ei_printf("Starting inferencing, press 'b' to break\n");

    ei_inertial_sample_start(&acc_data_callback, EI_CLASSIFIER_INTERVAL_MS);

    while (1) {
        ei_printf("Starting inferencing in 2 seconds...\n");

        // instead of wait_ms, we'll wait on the signal, this allows threads to cancel us...
        if (ei_sleep(2000) != EI_IMPULSE_OK) {
            break;
        }

        if(ei_user_invoke_stop()) {
            ei_printf("Inferencing stopped by user\r\n");
            EiDevice.set_state(eiStateIdle);
            break;
        }

        ei_printf("Sampling...\n");

        /* Run sampler */
        acc_sample_count = 0;
        for(int i = 0; i < EI_CLASSIFIER_RAW_SAMPLE_COUNT; i++) {
            ei_inertial_read_data();
            acc_sample_count += EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME;
        }

        // Create a data structure to represent this window of data
        signal_t signal;
        int err = numpy::signal_from_buffer(acc_buf, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal);
        if (err != 0) {
            ei_printf("ERR: signal_from_buffer failed (%d)\n", err); 
        }

        // run the impulse: DSP, neural network and the Anomaly algorithm
        ei_impulse_result_t result = { 0 };
        EI_IMPULSE_ERROR ei_error = run_classifier(&signal, &result, debug);
        if (ei_error != EI_IMPULSE_OK) {
            ei_printf("Failed to run impulse (%d)\n", ei_error);
            break;
        }

        // print the predictions
        ei_printf("Predictions (DSP: %d ms., Classification: %d ms., Anomaly: %d ms.): \n",
                  result.timing.dsp, result.timing.classification, result.timing.anomaly);
        for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
            ei_printf("    %s: \t%f\r\n", result.classification[ix].label, result.classification[ix].value);

#if (CONFIG_BLE_A31R118 == 1)
            if(result.classification[ix].value > 0.8 && prev_classification != ix) {
                ei_printf_ble(result.classification[ix].label);
                prev_classification = ix;
            }
#endif
        }
#if EI_CLASSIFIER_HAS_ANOMALY == 1
        ei_printf("    anomaly score: %f\r\n", result.anomaly);
#endif
        if(ei_user_invoke_stop()) {
            ei_printf("Inferencing stopped by user\r\n");
            EiDevice.set_state(eiStateIdle);
            break;
        }
    }
}
#else
void run_nn(bool debug) {
    ei_printf("Motion classification is not supported on Eta Compute AI Vision board\r\n");
}
#endif

#elif defined(EI_CLASSIFIER_SENSOR) && EI_CLASSIFIER_SENSOR == EI_CLASSIFIER_SENSOR_MICROPHONE
void run_nn(bool debug) {

    bool stop_inferencing = false;
#if (CONFIG_BLE_A31R118 == 1)
    uint8_t prev_classification = EI_CLASSIFIER_LABEL_COUNT;
    ei_printf_ble("Start inference\r\n");
#endif

    // summary of inferencing settings (from model_metadata.h)
    ei_printf("Inferencing settings:\n");
    ei_printf("\tInterval: %.4f ms.\n", (float)EI_CLASSIFIER_INTERVAL_MS);
    ei_printf("\tFrame size: %d\n", EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE);
    ei_printf("\tSample length: %d ms.\n", EI_CLASSIFIER_RAW_SAMPLE_COUNT / 16);
    ei_printf("\tNo. of classes: %d\n", sizeof(ei_classifier_inferencing_categories) / sizeof(ei_classifier_inferencing_categories[0]));

    if(ei_microphone_inference_start(EI_CLASSIFIER_RAW_SAMPLE_COUNT) == false) {
        ei_printf("ERR: Failed to setup audio sampling\r\n");
        return;
    }

    ei_printf("Starting inferencing, press 'b' to break\n");

    while (1) {
        ei_printf("Starting inferencing in 2 seconds...\n");

        // instead of wait_ms, we'll wait on the signal, this allows threads to cancel us...
        if (ei_sleep(2000) != EI_IMPULSE_OK) {
            break;
        }
        
        if(ei_user_invoke_stop()) {
            ei_printf("Inferencing stopped by user\r\n");
            EiDevice.set_state(eiStateIdle);
            break;
        }
        
        ei_printf("Recording...\n");

        ei_microphone_inference_reset_buffers();
        bool m = ei_microphone_inference_record();
        if (!m) {
            ei_printf("ERR: Failed to record audio...\n");
            break;
        }

        ei_printf("Recording done\n");

        signal_t signal;
        signal.total_length = EI_CLASSIFIER_RAW_SAMPLE_COUNT;
        signal.get_data = &ei_microphone_audio_signal_get_data;
        ei_impulse_result_t result = { 0 };

        EI_IMPULSE_ERROR r = run_classifier(&signal, &result, debug);
        if (r != EI_IMPULSE_OK) {
            ei_printf("ERR: Failed to run classifier (%d)\n", r);
            break;
        }

        // print the predictions
        ei_printf("Predictions (DSP: %d ms., Classification: %d ms., Anomaly: %d ms.): \n",
                  result.timing.dsp, result.timing.classification, result.timing.anomaly);
        for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
            ei_printf("    %s: \t%f\r\n", result.classification[ix].label, result.classification[ix].value);

#if (CONFIG_BLE_A31R118 == 1)
            if(result.classification[ix].value > 0.8 && prev_classification != ix) {
                ei_printf_ble(result.classification[ix].label);
                prev_classification = ix;
            }
#endif
        }
#if EI_CLASSIFIER_HAS_ANOMALY == 1
        ei_printf("    anomaly score: %f\r\n", result.anomaly);
#endif

        if(ei_user_invoke_stop()) {
            ei_printf("Inferencing stopped by user\r\n");
            EiDevice.set_state(eiStateIdle);
            break;
        }
    }

    ei_microphone_inference_end();
}

#elif defined(EI_CLASSIFIER_SENSOR) && EI_CLASSIFIER_SENSOR == EI_CLASSIFIER_SENSOR_CAMERA

extern int base64_encode(const char *input, size_t input_size, char *output, size_t output_size);

#if (CONFIG_AI_VISION_BOARD == 1)

void run_nn(bool debug) {

    // static uint8_t image_buffer[EI_CLASSIFIER_INPUT_WIDTH * EI_CLASSIFIER_INPUT_HEIGHT];
    bool stop_inferencing = false;
#if (CONFIG_BLE_A31R118 == 1)
    uint8_t prev_classification = EI_CLASSIFIER_LABEL_COUNT;
    ei_printf_ble("Start inference\r\n");
#endif

    // summary of inferencing settings (from model_metadata.h)
    ei_printf("Inferencing settings:\n");
    ei_printf("\tImage resolution: %dx%d\n", EI_CLASSIFIER_INPUT_WIDTH, EI_CLASSIFIER_INPUT_HEIGHT);
    ei_printf("\tFrame size: %d\n", EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE);
    ei_printf("\tNo. of classes: %d\n", sizeof(ei_classifier_inferencing_categories) / sizeof(ei_classifier_inferencing_categories[0]));

    uint8_t image_data [EI_CLASSIFIER_INPUT_WIDTH*EI_CLASSIFIER_INPUT_HEIGHT] __attribute__((aligned(32)));

    if (ei_camera_init() == false) {
        ei_printf("ERR: Failed to initialize image sensor\r\n");
        return;
    }

    while(stop_inferencing == false) {
        ei_printf("Starting inferencing in 2 seconds...\n");

        // instead of wait_ms, we'll wait on the signal, this allows threads to cancel us...
        if (ei_sleep(2000) != EI_IMPULSE_OK) {
            break;
        }

        ei::signal_t signal;
        signal.total_length = EI_CLASSIFIER_INPUT_WIDTH * EI_CLASSIFIER_INPUT_HEIGHT;
        signal.get_data = &ei_camera_cutout_get_data;

        ei_printf("Taking photo...\n");

        if (ei_camera_capture((size_t)EI_CLASSIFIER_INPUT_WIDTH, (size_t)EI_CLASSIFIER_INPUT_HEIGHT, image_data) == false) {
            ei_printf("Failed to capture image\r\n");
            break;
        }

        if (debug) {
            ei_printf("Framebuffer: ");

            size_t signal_chunk_size = 1024;

            // loop through the signal
            float *signal_buf = (float*)ei_malloc(signal_chunk_size * sizeof(float));
            if (!signal_buf) {
                ei_printf("ERR: Failed to allocate signal buffer\n");
                return;
            }

            uint8_t *per_pixel_buffer = (uint8_t*)ei_malloc(513); // 171 x 3 pixels
            if (!per_pixel_buffer) {
                ei_free(signal_buf);
                ei_printf("ERR: Failed to allocate per_pixel buffer\n");
                return;
            }

            size_t per_pixel_buffer_ix = 0;

            for (size_t ix = 0; ix < signal.total_length; ix += signal_chunk_size) {
                size_t items_to_read = signal_chunk_size;
                if (items_to_read > signal.total_length - ix) {
                    items_to_read = signal.total_length - ix;
                }

                int r = signal.get_data(ix, items_to_read, signal_buf);
                if (r != 0) {
                    ei_printf("ERR: Failed to get data from signal (%d)\n", r);
                    break;
                }

                for (size_t px = 0; px < items_to_read; px++) {
                    uint32_t pixel = static_cast<uint32_t>(signal_buf[px]);

                    // grab rgb
                    uint8_t r = static_cast<float>(pixel >> 16 & 0xff);
                    uint8_t g = static_cast<float>(pixel >> 8 & 0xff);
                    uint8_t b = static_cast<float>(pixel & 0xff);

                    // is monochrome anyway now, so just print 1 pixel at a time
                    const bool print_rgb = false;

                    if (print_rgb) {
                        per_pixel_buffer[per_pixel_buffer_ix + 0] = r;
                        per_pixel_buffer[per_pixel_buffer_ix + 1] = g;
                        per_pixel_buffer[per_pixel_buffer_ix + 2] = b;
                        per_pixel_buffer_ix += 3;
                    }
                    else {
                        per_pixel_buffer[per_pixel_buffer_ix + 0] = r;
                        per_pixel_buffer_ix++;
                    }

                    if (per_pixel_buffer_ix >= 513) {
                        const size_t base64_output_size = 684;

                        char *base64_buffer = (char*)ei_malloc(base64_output_size);
                        if (!base64_buffer) {
                            ei_printf("ERR: Cannot allocate base64 buffer of size %lu, out of memory\n", base64_output_size);
                            ei_free(signal_buf);
                            ei_free(per_pixel_buffer);
                            return;
                        }

                        int r = base64_encode((const char*)per_pixel_buffer, per_pixel_buffer_ix, base64_buffer, base64_output_size);
                        if (r < 0) {
                            ei_printf("ERR: Failed to base64 encode (%d)\n", r);
                            ei_free(signal_buf);
                            ei_free(per_pixel_buffer);
                            return;
                        }

                        ei_write_string(base64_buffer, r);
                        per_pixel_buffer_ix = 0;
                        ei_free(base64_buffer);
                    }
                    EiDevice.set_state(eiStateUploading);
                }
            }

            const size_t new_base64_buffer_output_size = floor(per_pixel_buffer_ix / 3 * 4) + 4;
            char *base64_buffer = (char*)ei_malloc(new_base64_buffer_output_size);
            if (!base64_buffer) {
                ei_free(signal_buf);
                ei_free(per_pixel_buffer);
                ei_printf("ERR: Cannot allocate base64 buffer of size %lu, out of memory\n", new_base64_buffer_output_size);
                return;
            }

            int r = base64_encode((const char*)per_pixel_buffer, per_pixel_buffer_ix, base64_buffer, new_base64_buffer_output_size);
            if (r < 0) {
                ei_free(signal_buf);
                ei_free(per_pixel_buffer);
                ei_printf("ERR: Failed to base64 encode (%d)\n", r);
                return;
            }

            ei_write_string(base64_buffer, r);
            ei_printf("\r\n");

            ei_free(signal_buf);
            ei_free(per_pixel_buffer);
            ei_free(base64_buffer);
        }

        // run the impulse: DSP, neural network and the Anomaly algorithm
        ei_impulse_result_t result = { 0 };

        EI_IMPULSE_ERROR ei_error = run_classifier(&signal, &result, debug);
        if (ei_error != EI_IMPULSE_OK) {
            ei_printf("Failed to run impulse (%d)\n", ei_error);
            break;
        }

        // print the predictions
        ei_printf("Predictions (DSP: %d ms., Classification: %d ms., Anomaly: %d ms.): \n",
                  result.timing.dsp, result.timing.classification, result.timing.anomaly);
        for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
            ei_printf("    %s: \t%f\r\n", result.classification[ix].label, result.classification[ix].value);

#if (CONFIG_BLE_A31R118 == 1)
            if(result.classification[ix].value > 0.8 && prev_classification != ix) {
                ei_printf_ble(result.classification[ix].label);
                prev_classification = ix;
            }
#endif
        }
#if EI_CLASSIFIER_HAS_ANOMALY == 1
        ei_printf("    anomaly score: %f\r\n", result.anomaly);
#endif

        if(ei_user_invoke_stop()) {
            ei_printf("Inferencing stopped by user\r\n");
            EiDevice.set_state(eiStateIdle);
            break;
        }
    }
}

#else
void run_nn(bool debug) {

    ei_printf("Image classification is not supported on Eta Compute AI Sensor board\r\n");
}
#endif

#endif

void run_nn_normal(void) {
    run_nn(false);
}

void run_nn_debug(void) {
    run_nn(true);
}
