#include "../../include/system/sys_uart.h"
#include "../../include/system/sys_rf_tx.h"
#include "../../include/sys_config.h"
#include <Arduino.h>
#include <cctype>
#include "../../include/protocols/core/protocol_registry.h"

// Global protocol registry (initialized in main)
extern ProtocolRegistry* g_protocolRegistry;

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

        // Read one complete line (frame ends with newline)
        String input = UART_PORT.readStringUntil('\n');
        input.trim();

        if (input.length() == 0) {
            continue;
        }

        // Parse protocol frame format: "PROTOCOL:BITCOUNT:HEXVALUE"
        // Example: "EV1527:24:AABBCC"
        int colon1 = input.indexOf(':');
        int colon2 = input.lastIndexOf(':');

        if (colon1 > 0 && colon2 > colon1) {
            // Extract protocol name
            String protocolName = input.substring(0, colon1);
            String bitCountStr = input.substring(colon1 + 1, colon2);
            String valueStr = input.substring(colon2 + 1);

            // Parse bit count
            uint8_t bitCount = (uint8_t)bitCountStr.toInt();

            // Parse hex value - trim semicolon and whitespace first
            valueStr.trim();
            if (valueStr.endsWith(";")) {
                valueStr = valueStr.substring(0, valueStr.length() - 1);
            }
            valueStr.trim();

            uint64_t value = 0;
            for (int i = 0; i < valueStr.length(); i++) {
                char c = valueStr[i];
                // Skip non-hex characters
                if (!isxdigit(c)) {
                    continue;
                }
                value = value * 16;
                if (c >= '0' && c <= '9') {
                    value += (c - '0');
                } else if (c >= 'A' && c <= 'F') {
                    value += (c - 'A' + 10);
                } else if (c >= 'a' && c <= 'f') {
                    value += (c - 'a' + 10);
                }
            }

            // Look up protocol ID from registry
            if (g_protocolRegistry) {
                int protocolId = g_protocolRegistry->getProtocolId(protocolName.c_str());
                if (protocolId >= 0) {
                    // Check if protocol supports TX
                    if (g_protocolRegistry->supportsTx(protocolId)) {
                        // Create TX command and queue it
                        TxCommand command;
                        command.protocolId = protocolId;
                        command.value = value;
                        command.bitLength = bitCount;

                        xQueueSend(txQueue, &command, 0);
                    } else {
                        // Protocol doesn't support TX
                        UART_PORT.print("ERROR: Protocol ");
                        UART_PORT.print(protocolName);
                        UART_PORT.println(" does not support TX");
                    }
                } else {
                    // Unknown protocol
                    UART_PORT.print("ERROR: Unknown protocol: ");
                    UART_PORT.println(protocolName);
                }
            }
        }

        vTaskDelay(1);
    }
}
