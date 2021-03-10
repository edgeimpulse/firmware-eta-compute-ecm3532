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
#include "config.h"
#include "FreeRTOS.h"
#include "task.h"
#include "gpio_hal.h"
#include "timer_hal.h"
#include "print_util.h"
#include "UARTCommandConsole.h"
#include <stdint.h>
#include "FreeRTOS_CLI.h"

#define APP_LED (CONFIG_APP_LED_GPIO)

int32_t t1_loop = 0;
static void vTask_Name_placeholderTask(void *pvParameters)
{
    HalGpioOutInit(APP_LED, 0);
    while (1) {
        ecm35xx_printf("Task_Name_placeholder loop cnt %d\r\n", t1_loop);
        t1_loop++;
#ifdef CONFIG_APP_LED
        HalGpioToggle(APP_LED);
#endif
        vTaskDelay(1000);
    }
}

int main( void )
{
	xTaskCreate(vTask_Name_placeholderTask, "Task_Name_placeholder", configMINIMAL_STACK_SIZE,
                    NULL, tskIDLE_PRIORITY + 2, NULL);
#if defined(CONFIG_CLI) && defined(CONFIG_DEBUG_UART)
    /* Start the task that manages the command console for FreeRTOS+CLI. */
	vUARTCommandConsoleStart((configMINIMAL_STACK_SIZE * 3),
                                tskIDLE_PRIORITY);
#endif
	/* Start the scheduler. */
	vTaskStartScheduler();
	return 0;
}
