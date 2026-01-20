#pragma once

#define RF_RX_PIN 22
#define RF_TX_PIN 26

#define UART_PORT Serial
#define UART_BAUDRATE 2e6

#define RF_RX_TIMINGS_BUFFER_SIZE 256
#define RF_TX_TIMINGS_BUFFER_SIZE 256
#define RF_IDLE_TIMEOUT_US 1e5

#define PACKET_VERSION 0x1