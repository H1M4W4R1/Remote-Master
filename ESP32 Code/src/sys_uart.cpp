#include "sys_uart.h"
#include "sys_rf_tx.h"
#include "config.h"
#include <Arduino.h>
#include "sys_config.h"

uint16_t uart_tx_timings_cache[RF_TX_TIMINGS_BUFFER_SIZE];
uint16_t uart_tx_timings_index = 0;

void uart_init()
{
    UART_PORT.begin(UART_BAUDRATE);
    memset(uart_tx_timings_cache, 0, sizeof(uart_tx_timings_cache));
}

[[noreturn]] void uart_task(void* arg)
{
    for (;;)
    {
        while (!UART_PORT.available())
        {
            vTaskDelay(1);
        }

        String text = UART_PORT.readStringUntil('\n');
        text.trim(); // removes \r, \n, and spaces

        uart_tx_timings_index = 0;

        int startIndex = 0;
        int colonIndex;
        memset(uart_tx_timings_cache, 0, sizeof(uart_tx_timings_cache));

        while ((colonIndex = text.indexOf(':', startIndex)) != -1)
        {
            String hexString = text.substring(startIndex, colonIndex);
            hexString.trim();

            const uint16_t timing = static_cast<uint16_t>(strtoul(hexString.c_str(), nullptr, 16));

            uart_tx_timings_cache[uart_tx_timings_index++] = timing;

            startIndex = colonIndex + 1;
        }

        // Parse the final value after the last colon
        if (startIndex < text.length())
        {
            String hexString = text.substring(startIndex);
            hexString.trim();

            const uint16_t timing = static_cast<uint16_t>(strtoul(hexString.c_str(), nullptr, 16));

            uart_tx_timings_cache[uart_tx_timings_index++] = timing;
        }

        TxCommand command;
        memcpy(command.timings, uart_tx_timings_cache, sizeof(uart_tx_timings_cache));

        xQueueSend(txQueue, &command, 0);
        vTaskDelay(1);
    }
}
