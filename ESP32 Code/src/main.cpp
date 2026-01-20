#include <Arduino.h>
#include "sys_rf_rx.h"
#include "sys_rf_tx.h"
#include "sys_uart.h"

void setup() {
    uart_init();
    rf_rx_init();

    txQueue = xQueueCreate(4, sizeof(TxCommand));

    xTaskCreatePinnedToCore(rf_rx_task, "rf_rx", 4096, nullptr, 4, nullptr, 0);
    xTaskCreatePinnedToCore(uart_task,  "uart", 4096, nullptr, 3, nullptr, 0);
    xTaskCreatePinnedToCore(rf_tx_task, "rf_tx", 4096, nullptr, 4, nullptr, 1);
}

void loop() {
    vTaskDelay(portMAX_DELAY);
}
