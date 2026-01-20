#include "sys_rf_rx.h"
#include "config.h"
#include "ring_buffer.h"
#include <Arduino.h>
#include "xtensa/core-macros.h"
#include "esp32-hal-cpu.h"
#include "sys_config.h"

static RingBuffer<uint16_t, RF_RX_TIMINGS_BUFFER_SIZE> rxBuffer;
static volatile uint32_t                       lastCycle    = 0;
static volatile uint64_t                       lastSignalUs = 0;

void IRAM_ATTR rf_rx_isr()
{
    const uint32_t now = micros();
    rxBuffer.push(now - lastCycle);
    lastCycle    = now;

}

void rf_rx_init()
{
    pinMode(RF_RX_PIN, INPUT);
    lastCycle   = micros();
    attachInterrupt(RF_RX_PIN, rf_rx_isr, CHANGE);
}

[[noreturn]] void rf_rx_task(void* arg)
{
    for (;;)
    {
        vTaskDelay(1);

        uint16_t deltaCycles;
        while (rxBuffer.pop(deltaCycles))
        {
            UART_PORT.print(deltaCycles, HEX);
            UART_PORT.print(':');
        }

        vTaskDelay(1);
    }
}
