/* Cucumber Test Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "api.h"

/**
 * This is an example which receives commands from the wire-server.
 *
 * - Port: configured UART
 * - Receive (Rx) buffer: on
 * - Transmit (Tx) buffer: off
 * - Flow control: off
 * - Event queue: off
 * - Pin assignment: see defines below (See Kconfig)
 */

#define CUCUMBER_TEST_TXD (CONFIG_EXAMPLE_UART_TXD)
#define CUCUMBER_TEST_RXD (CONFIG_EXAMPLE_UART_RXD)
#define CUCUMBER_TEST_RTS (UART_PIN_NO_CHANGE)
#define CUCUMBER_TEST_CTS (UART_PIN_NO_CHANGE)

#define CUCUMBER_UART_PORT_NUM      (CONFIG_EXAMPLE_UART_PORT_NUM)
#define CUCUMBER_TASK_STACK_SIZE    (CONFIG_EXAMPLE_TASK_STACK_SIZE)

static const char *TAG = "RPC SERVER";

#define BUF_SIZE (1024)

static void cucumber_task(void *arg)
{
    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = 38400,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    int intr_alloc_flags = 0;

#if CONFIG_UART_ISR_IN_IRAM
    intr_alloc_flags = ESP_INTR_FLAG_IRAM;
#endif

    ESP_ERROR_CHECK(uart_driver_install(CUCUMBER_UART_PORT_NUM, BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(CUCUMBER_UART_PORT_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(CUCUMBER_UART_PORT_NUM, CUCUMBER_TEST_TXD, CUCUMBER_TEST_RXD, CUCUMBER_TEST_RTS, CUCUMBER_TEST_CTS));

    // Configure a temporary buffer for the incoming data
    char* command_buffer = (char*) malloc(BUF_SIZE);

    char chr;
    int32_t rsp = 0;
    int idx = 0;
    while (1) {
        rsp = uart_read_bytes(CUCUMBER_UART_PORT_NUM, &chr, 1, 20 / portTICK_PERIOD_MS);
        if (rsp > 0)
        {
            ESP_LOGI(TAG, "Read %c\r\n", chr);
            if (chr == '\r')
            {
                command_buffer[idx] = 0;
                idx = 0;
                ESP_LOGI(TAG, "About to call api_handler\r\n");
                char *ret = api_handler(command_buffer);
                ESP_LOGI(TAG, "api_handler returned %x\r\n",(int) ret);

                uart_write_bytes(CUCUMBER_UART_PORT_NUM, (const char *) ret, 1);
                uart_write_bytes(CUCUMBER_UART_PORT_NUM, "\n", 1);

            } else if(idx < BUF_SIZE)
            {
                command_buffer[idx++] = chr;
            } else
            {
                command_buffer[0] = 0;
                idx = 0;
                ESP_LOGI(TAG, "Ignoring overrun\r\n");
            }
        }
    }
}

void app_main(void)
{
    xTaskCreate(cucumber_task, "uart_cucumber_task", CUCUMBER_TASK_STACK_SIZE, NULL, 10, NULL);
}
