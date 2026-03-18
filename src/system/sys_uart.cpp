#include "../../include/system/sys_uart.h"
#include "../../include/system/sys_rf_tx.h"
#include "../../include/sys_config.h"
#include <Arduino.h>
#include "../../include/sys_config.h"
#include "../../include/protocols/core/protocol_encoder.h"
#include "../../include/protocols/core/protocol_registry.h"

// Global protocol encoder (initialized in main)
extern ProtocolEncoder* g_protocolEncoder;

void uart_init()
{
    UART_PORT.begin(UART_BAUDRATE);
}

void uart_task(void* arg)
{
    for (;;)
    {
        // Wait for data from UART
        while (!UART_PORT.available())
        {
            vTaskDelay(1);
        }

        // Read one complete line (frame ends with ';')
        String input = UART_PORT.readStringUntil('\n');
        input.trim();

        if (input.length() == 0) {
            continue;
        }

        // Check if this is a protocol frame (contains colons and semicolon)
        if (input.indexOf(':') != -1 && input.indexOf(';') != -1) {
            // Encode the protocol frame
            TxCommand command = {};
            if (g_protocolEncoder && g_protocolEncoder->encodeFrame(input.c_str(), command)) {
                // Send to RF TX queue
                xQueueSend(txQueue, &command, 0);
            } else {
                // Encoding failed - could send error message, but silent fail for now
            }
        }

        vTaskDelay(1);
    }
}
