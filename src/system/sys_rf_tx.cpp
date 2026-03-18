#include "../../include/system/sys_rf_tx.h"
#include <Arduino.h>
#include <RCSwitch.h>
#include "../../include/sys_config.h"

QueueHandle_t txQueue;

// Global RC-Switch transmitter instance (exposed for protocol configuration)
RCSwitch rcswitch_tx;

void rf_tx_init()
{
    // Initialize RC-Switch transmitter on GPIO26
    // enableTransmit(pin) automatically sets up GPIO for output
    rcswitch_tx.enableTransmit(RF_TX_PIN);

    // Set transmission repeat count (how many times to repeat the signal)
    // Higher values = more reliable transmission at cost of longer transmission time
    rcswitch_tx.setRepeatTransmit(10);  // 10 repetitions
}

void rf_tx_task(void* arg)
{
    TxCommand cmd;

    for (;;)
    {
        if (xQueueReceive(txQueue, &cmd, portMAX_DELAY))
        {
            // Use RC-Switch to send the signal
            // rcswitch.send(value, bitLength, protocol)
            // The protocol parameter is optional if using default
            // RC-Switch handles all timing precision internally
            rcswitch_tx.send(cmd.value, cmd.bitLength);

            // Small delay between consecutive transmissions
            // RC-Switch handles the internal timing, we just need to space out TX commands
            vTaskDelay(pdMS_TO_TICKS(100));
        }
        else
        {
            vTaskDelay(10);
        }
    }
}
