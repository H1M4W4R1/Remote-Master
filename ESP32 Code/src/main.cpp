#include <Arduino.h>

#include "sys_config.h"
#include "sys_rf_rx.h"

void setup() {
    rf_rx_init();

    UART_PORT.begin(UART_BAUDRATE);
    xTaskCreatePinnedToCore(rf_rx_task, "rf_rx", 4096, nullptr, 4, nullptr, 1);
}

void loop() {
    vTaskDelay(portMAX_DELAY);
}
