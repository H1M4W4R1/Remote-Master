#pragma once
#include <stdint.h>
// ReSharper disable once CppUnusedIncludeDirective
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

#include "sys_config.h"

struct TxCommand
{
    uint16_t timings[RF_TX_TIMINGS_BUFFER_SIZE];
};

extern QueueHandle_t txQueue;
void rf_tx_task(void* arg);
