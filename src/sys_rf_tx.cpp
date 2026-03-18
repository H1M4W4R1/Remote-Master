#include "sys_rf_tx.h"
#include <Arduino.h>
#include "config.h"

#include "sys_config.h"
#include "driver/gpio.h"

QueueHandle_t txQueue;
uint8_t tx_gpio_level;

[[noreturn]] void rf_tx_task(void* arg)
{
    pinMode(RF_TX_PIN, OUTPUT);


    TxCommand cmd;
    for (;;)
    {
        if (xQueueReceive(txQueue, &cmd, portMAX_DELAY))
        {
            gpio_set_level((gpio_num_t) RF_TX_PIN, HIGH);
            tx_gpio_level = 0;

            uint32_t index = 0;

            while (index < RF_TX_TIMINGS_BUFFER_SIZE)
            {
                const uint16_t timing = cmd.timings[index];
                if (timing == 0) break;

                delayMicroseconds(timing);
                tx_gpio_level ^= 1;
                gpio_set_level((gpio_num_t) RF_TX_PIN, tx_gpio_level);
                index++;
            }

            gpio_set_level((gpio_num_t) RF_TX_PIN, LOW);

            vTaskDelay(1);
        }
    }
}
