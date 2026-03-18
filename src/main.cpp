#include <Arduino.h>
#include <RCSwitch.h>
#include "../include/system/sys_rf_rx.h"
#include "system/sys_rf_tx.h"
#include "../include/system/sys_uart.h"
#include "../include/protocols/core/protocol_registry.h"

// Global protocol registry (used by RX/TX tasks)
ProtocolRegistry* g_protocolRegistry = nullptr;

void setup() {
    // Initialize UART and RF hardware
    uart_init();
    rf_rx_init();
    rf_tx_init();

    // Create TX queue
    txQueue = xQueueCreate(4, sizeof(TxCommand));

    // Initialize protocol registry
    g_protocolRegistry = new ProtocolRegistry();

    // Register all RC-Switch supported protocols (IDs 1-12)
    // All protocols support both TX and RX

    // Protocol 1: Standard (~575µs) - EV1527/SC5262/HX2262 family
    g_protocolRegistry->registerProtocol(1, "EV1527", true, true, nullptr);
    g_protocolRegistry->registerProtocol(1, "SC5262", true, true, nullptr);
    g_protocolRegistry->registerProtocol(1, "HX2262", true, true, nullptr);
    g_protocolRegistry->registerProtocol(1, "Standard", true, true, nullptr);

    // Protocol 2: Standard+650U - PT2262/PT2272 family
    g_protocolRegistry->registerProtocol(2, "PT2262+650U", true, true, nullptr);
    g_protocolRegistry->registerProtocol(2, "PT2272+650U", true, true, nullptr);
    g_protocolRegistry->registerProtocol(2, "EV1527+650U", true, true, nullptr);
    g_protocolRegistry->registerProtocol(2, "Standard+650U", true, true, nullptr);

    // Protocol 3: Standard+100U - Compact device variant
    g_protocolRegistry->registerProtocol(3, "SC5262+100U", true, true, nullptr);
    g_protocolRegistry->registerProtocol(3, "HX2262+100U", true, true, nullptr);
    g_protocolRegistry->registerProtocol(3, "Standard+100U", true, true, nullptr);

    // Protocol 4: Standard+380U - Legacy device variant
    g_protocolRegistry->registerProtocol(4, "EV1527+380U", true, true, nullptr);
    g_protocolRegistry->registerProtocol(4, "SC5262+380U", true, true, nullptr);
    g_protocolRegistry->registerProtocol(4, "Standard+380U", true, true, nullptr);

    // Protocol 5: Standard+500U - Generic 433MHz variant
    g_protocolRegistry->registerProtocol(5, "PT2262+500U", true, true, nullptr);
    g_protocolRegistry->registerProtocol(5, "EV1527+500U", true, true, nullptr);
    g_protocolRegistry->registerProtocol(5, "Standard+500U", true, true, nullptr);

    // Protocol 6: HT6P20B chipset
    g_protocolRegistry->registerProtocol(6, "HT6P20B", true, true, nullptr);
    g_protocolRegistry->registerProtocol(6, "HT6P20", true, true, nullptr);

    // Protocol 7: HS2303/AUKEY
    g_protocolRegistry->registerProtocol(7, "HS2303", true, true, nullptr);
    g_protocolRegistry->registerProtocol(7, "AUKEY", true, true, nullptr);
    g_protocolRegistry->registerProtocol(7, "HS2303_AUKEY", true, true, nullptr);

    // Protocol 8: Conrad RS-200 RX mode
    g_protocolRegistry->registerProtocol(8, "Conrad_RS200_RX", true, true, nullptr);
    g_protocolRegistry->registerProtocol(8, "RS200_RX", true, true, nullptr);

    // Protocol 9: Conrad RS-200 TX mode
    g_protocolRegistry->registerProtocol(9, "Conrad_RS200_TX", true, true, nullptr);
    g_protocolRegistry->registerProtocol(9, "RS200_TX", true, true, nullptr);

    // Protocol 10: 1ByOne Doorbell
    g_protocolRegistry->registerProtocol(10, "1ByOne_Doorbell", true, true, nullptr);
    g_protocolRegistry->registerProtocol(10, "1ByOne", true, true, nullptr);

    // Protocol 11: HT12E decoder chip
    g_protocolRegistry->registerProtocol(11, "HT12E", true, true, nullptr);
    g_protocolRegistry->registerProtocol(11, "HT12", true, true, nullptr);

    // Protocol 12: SM5212 chipset
    g_protocolRegistry->registerProtocol(12, "SM5212", true, true, nullptr);
    g_protocolRegistry->registerProtocol(12, "SM52", true, true, nullptr);

    // Create FreeRTOS tasks
    xTaskCreatePinnedToCore(rf_rx_task, "rf_rx", 4096, nullptr, 4, nullptr, 0);
    xTaskCreatePinnedToCore(uart_task,  "uart", 4096, nullptr, 3, nullptr, 0);
    xTaskCreatePinnedToCore(rf_tx_task, "rf_tx", 4096, nullptr, 4, nullptr, 1);
}

void loop() {
    vTaskDelay(portMAX_DELAY);
}
