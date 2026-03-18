#include "../../include/system/sys_rf_rx.h"
#include "sys_config.h"
#include "../../include/core/ring_buffer.h"
#include <Arduino.h>
#include "xtensa/core-macros.h"
#include "esp32-hal-cpu.h"
#include "sys_config.h"
#include "../../include/protocols/core/protocol_decoder.h"
#include "../../include/protocols/core/frame_parser.h"

static RingBuffer<uint16_t, RF_RX_TIMINGS_BUFFER_SIZE> rxBuffer;
static volatile uint32_t lastCycle = 0;

// Global protocol decoder (initialized in main)
extern ProtocolDecoder* g_protocolDecoder;

void IRAM_ATTR rf_rx_isr()
{
    const uint32_t now = micros();
    rxBuffer.push(now - lastCycle);
    lastCycle = now;
}

void rf_rx_init()
{
    pinMode(RF_RX_PIN, INPUT_PULLUP);
    lastCycle = micros();
    attachInterrupt(RF_RX_PIN, rf_rx_isr, CHANGE);
}

void rf_rx_task(void* arg)
{
    // Work directly with ring buffer - no copying needed!
    const uint32_t FRAME_GAP_MS = 5;  // Gap of 5ms indicates frame boundary

    for (;;)
    {
        vTaskDelay(1);  // Check every 1ms for faster response

        uint32_t availableCount = rxBuffer.getAvailableCount();

        // Only attempt decode if:
        // 1. We have some data
        // 2. Either we have a lot of data OR there's been a gap since last timing
        if (availableCount > 0) {
            uint32_t timeSinceLastTiming = rxBuffer.getTimeSinceLastPush();

            // Attempt decode if we detect a frame boundary (gap) or have lots of data
            if (timeSinceLastTiming > FRAME_GAP_MS || availableCount > 200) {
                // Attempt to decode the collected timings
                if (g_protocolDecoder) {
                    // Create temporary array for decoder (small, ~100 bytes on stack)
                    uint16_t timingArray[256];
                    uint32_t timingCount = (availableCount < 256) ? availableCount : 256;

                    // Peek at all available timings without removing them
                    for (uint32_t i = 0; i < timingCount; i++) {
                        rxBuffer.peek(i, timingArray[i]);
                    }

                    // Try to decode
                    size_t frameCount = g_protocolDecoder->decodeTiming(timingArray, timingCount);

                    if (frameCount > 0) {
                        // Output all decoded frames to UART
                        for (size_t i = 0; i < frameCount; i++) {
                            const ProtocolFrame* frame = g_protocolDecoder->getFrame(i);
                            if (frame != nullptr) {
                                char output[64];
                                size_t len = serializeProtocolFrame(*frame, output, sizeof(output));
                                if (len > 0) {
                                    UART_PORT.write((uint8_t*)output, len);
                                    UART_PORT.write('\n');
                                }
                            }
                        }

                        // Consume exactly what was decoded by first protocol
                        size_t timingsConsumed = g_protocolDecoder->getTimingsConsumed();
                        if (timingsConsumed > 0) {
                            rxBuffer.consume(timingsConsumed);
                        }

                        // Continue immediately to decode any back-to-back frames
                    } else {
                        // No matches found - if we have too much data with no valid frame,
                        // discard oldest data to make room
                        if (availableCount > 240) {
                            rxBuffer.consume(10);  // Discard 10 oldest timings
                        } else {
                            // Wait for more data or frame boundary
                            rxBuffer.resetConsumption();
                        }
                    }
                }
            }
        }
    }
}
