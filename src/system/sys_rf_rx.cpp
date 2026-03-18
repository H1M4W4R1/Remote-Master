#include "../../include/system/sys_rf_rx.h"
#include "../../include/sys_config.h"
#include <Arduino.h>
#include <RCSwitch.h>
#include <cstring>
#include <cstdio>
#include "../../include/protocols/core/frame_parser.h"
#include "../../include/protocols/core/protocol_registry.h"

// Global RC-Switch receiver instance (exposed for protocol configuration)
RCSwitch rcswitch_rx;

// Forward declarations for protocol registry
extern ProtocolRegistry* g_protocolRegistry;

void rf_rx_init()
{
    // Initialize RC-Switch receiver on GPIO22
    // enableReceive(pin) automatically sets up GPIO and interrupt handling
    rcswitch_rx.enableReceive(RF_RX_PIN);
}

void rf_rx_task(void* arg)
{
    // RF RX task - Uses RC-Switch's built-in interrupt handling
    // RC-Switch handles GPIO interrupts and signal decoding internally

    for (;;)
    {
        vTaskDelay(10);  // Check every 10ms - RC-Switch processes signals asynchronously

        // Check if RC-Switch has decoded a complete signal
        if (rcswitch_rx.available()) {
            uint64_t receivedValue = rcswitch_rx.getReceivedValue();
            int receivedBitlength = rcswitch_rx.getReceivedBitlength();
            int receivedProtocol = rcswitch_rx.getReceivedProtocol();

            if (receivedValue != 0) {
                // Create protocol frame from RC-Switch decoded data
                ProtocolFrame frame;

                // Get protocol name from registry based on protocol ID
                const char* protocolName = g_protocolRegistry->getProtocolName(receivedProtocol);
                if (protocolName != nullptr) {
                    strncpy(frame.protocolName, protocolName, sizeof(frame.protocolName) - 1);
                    frame.protocolName[sizeof(frame.protocolName) - 1] = '\0';
                } else {
                    // Unrecognized protocol - use generic name
                    snprintf(frame.protocolName, sizeof(frame.protocolName), "UNKNOWN:%d", receivedProtocol);
                }

                frame.bits = receivedBitlength;
                frame.value = receivedValue;

                // Serialize and output to UART
                char output[64];
                size_t len = serializeProtocolFrame(frame, output, sizeof(output));
                if (len > 0) {
                    UART_PORT.write((uint8_t*)output, len);
                    UART_PORT.write('\n');
                }
            }

            // Reset RC-Switch to receive next signal
            rcswitch_rx.resetAvailable();
        }
    }
}
