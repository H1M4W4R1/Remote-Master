#include <Arduino.h>
#include "../include/system/sys_rf_rx.h"
#include "system/sys_rf_tx.h"
#include "../include/system/sys_uart.h"
#include "../include/protocols/core/protocol_registry.h"
#include "../include/protocols/core/protocol_encoder.h"
#include "../include/protocols/core/protocol_decoder.h"
#include "../include/protocols/impl/protocol_ev1527.h"
#include "../include/protocols/impl/protocol_proprietary_alpha.h"
#include "../include/protocols/impl/protocol_proprietary_beta.h"

// Global protocol components
static ProtocolRegistry g_registry;
ProtocolEncoder* g_protocolEncoder = nullptr;
ProtocolDecoder* g_protocolDecoder = nullptr;

// Protocol instances
static EV1527Protocol g_ev1527;
static ProprietaryAlphaProtocol g_propAlpha;
static ProprietaryBetaProtocol g_propBeta;

void setup() {
    // Initialize UART and RF hardware
    uart_init();
    rf_rx_init();

    // Create TX queue
    txQueue = xQueueCreate(4, sizeof(TxCommand));

    // Register all available protocols
    g_registry.registerProtocol(&g_ev1527);
    g_registry.registerProtocol(&g_propAlpha);
    g_registry.registerProtocol(&g_propBeta);

    // Create encoder and decoder with the registry
    g_protocolEncoder = new ProtocolEncoder(g_registry);
    g_protocolDecoder = new ProtocolDecoder(g_registry);

    // Create FreeRTOS tasks
    xTaskCreatePinnedToCore(rf_rx_task, "rf_rx", 4096, nullptr, 4, nullptr, 0);
    xTaskCreatePinnedToCore(uart_task,  "uart", 4096, nullptr, 3, nullptr, 0);
    xTaskCreatePinnedToCore(rf_tx_task, "rf_tx", 4096, nullptr, 4, nullptr, 1);
}

void loop() {
    vTaskDelay(portMAX_DELAY);
}
