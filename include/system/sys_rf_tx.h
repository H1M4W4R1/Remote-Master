#pragma once
#include <stdint.h>
// ReSharper disable once CppUnusedIncludeDirective
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

// RC-Switch compatible TX command structure
struct TxCommand
{
    int protocolId;        // RC-Switch protocol ID (e.g., 1 for EV1527)
    uint64_t value;        // Protocol data value
    uint8_t bitLength;     // Number of bits in value
};

extern QueueHandle_t txQueue;
void rf_tx_init();
void rf_tx_task(void* arg);
