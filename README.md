# Remote Master

A flexible, multi-protocol RF remote control system for ESP32 microcontrollers with on-device protocol encoding and
decoding.

## Features

- **Multi-Protocol Support**: Supports multiple RF protocols including EV1527 and proprietary variants
- **On-Device Decoding**: Decodes RF signals directly on the ESP32 without external dependencies
- **Protocol Registry**: Pluggable protocol architecture for easy addition of new protocols
- **UART Communication**: Serial interface for command and data transfer
- **FreeRTOS Integration**: Multi-threaded task-based architecture with dedicated RF RX/TX and UART handling
- **Modular Design**: Cleanly separated protocol, system, and core components

## Hardware

- **Microcontroller**: Wemos D1 R32
- **Framework**: Arduino with FreeRTOS
- **RF Modules**: Compatible with standard RF transmitter/receiver modules

### I/O Connections

| Interface      | Pin         | Function                            |
|----------------|-------------|-------------------------------------|
| RF Receiver    | GPIO 22     | RF data input (with interrupt)      |
| RF Transmitter | GPIO 26     | RF data output (PWM/timing control) |
| UART           | USB         | Serial data communication to host   |
| UART Baudrate  | -           | 2,000,000 bps (CH340G limits)       |

## Project Structure

```
src/
├── main.cpp                           # Application entry point
├── system/
│   ├── sys_rf_rx.cpp                 # RF receiver implementation
│   ├── sys_rf_tx.cpp                 # RF transmitter implementation
│   └── sys_uart.cpp                  # UART/serial communication
└── protocols/
    ├── core/
    │   ├── protocol_interface.h       # Base protocol interface
    │   ├── protocol_registry.cpp      # Protocol registration system
    │   ├── protocol_encoder.cpp       # Protocol encoding engine
    │   ├── protocol_decoder.cpp       # Protocol decoding engine
    │   └── frame_parser.cpp           # Frame parsing utilities
    └── impl/
        ├── protocol_ev1527.cpp        # EV1527 protocol implementation
        ├── protocol_proprietary_alpha.cpp
        └── protocol_proprietary_beta.cpp

include/                              # Header files mirroring src structure
```

## Getting Started

### Prerequisites

- PlatformIO IDE or CLI
- Wemos D1 R32 (or compatible ESP32 board)
- RF transmitter/receiver modules

### Installation

1. Clone the repository
2. Open in PlatformIO
3. Build and upload to your ESP32:
   ```bash
   platformio run -e esp32doit-devkit-v1 -t upload
   ```

## Building

```bash
platformio run -e esp32doit-devkit-v1
```

## Uploading

```bash
platformio run -e esp32doit-devkit-v1 -t upload
```

## Serial Monitor

Monitor device output at 2000000 baud:

```bash
platformio device monitor -e esp32doit-devkit-v1 --baud 2000000
```

## Architecture

The system uses a task-based architecture with three main FreeRTOS tasks:

- **RF RX Task**: Receives and decodes incoming RF signals
- **RF TX Task**: Encodes and transmits RF commands
- **UART Task**: Handles serial communication with the host

Protocol handling is abstracted through a registry system, allowing new protocols to be added by implementing the
`ProtocolInterface`.

## Extending

### Adding a New Protocol

To add support for a new RF protocol, follow these steps:

1. **Create the Protocol Header** (`include/protocols/impl/protocol_myprotocol.h`)
   ```cpp
   #pragma once
   #include "../core/protocol_interface.h"

   class MyProtocol : public IProtocol {
   public:
       const char* getName() const override;
       bool tryParse(const uint16_t* timings, size_t count,
                     ProtocolData& out, size_t& out_timingsConsumed) override;
       bool buildPacket(const ProtocolData& data, uint16_t* outTimings,
                        size_t& outCount, size_t maxCount) override;
   };
   ```

2. **Implement the Protocol** (`src/protocols/impl/protocol_myprotocol.cpp`)
   - **getName()**: Return a unique protocol identifier string
   - **tryParse()**: Decode RF timing pulse widths into `ProtocolData` (value and bit count)
     - Set `out.isValid = true` when a complete valid frame is decoded
     - Set `out_timingsConsumed` to the number of timing values processed
     - Return `true` on successful decode
   - **buildPacket()**: Encode `ProtocolData` into RF timing pulse values
     - Fill `outTimings` array with timing values
     - Set `outCount` to number of timings generated
     - Return `true` on successful encode

3. **Register the Protocol** (in `src/main.cpp`)
   ```cpp
   static MyProtocol g_myProtocol;

   void setup() {
       // ... existing code ...
       g_registry.registerProtocol(&g_myProtocol);
   }
   ```

### Protocol Data Format

The `ProtocolData` struct represents decoded protocol information:
- **isValid**: Whether the decoded data is valid
- **value**: The decoded command/data value (up to 64 bits)
- **bits**: Number of significant bits in the value

### Example: Protocol Frame Parsing

A typical RF protocol consists of:
- **Preamble**: Sync pulses to identify frame start
- **Data**: Encoded command/address bits as pulse width patterns
- **Postamble**: Optional termination pattern

Your `tryParse()` implementation should:
1. Detect the preamble pattern in the timing array
2. Decode the data bits using protocol-specific encoding
3. Validate checksum or integrity if applicable
4. Return the decoded data in `ProtocolData`

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details.
