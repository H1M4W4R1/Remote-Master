# Remote Master

A reliable RF remote control system for ESP32 microcontrollers using the RC-Switch library for proven 433MHz protocol handling.

## Features

- **12 RC-Switch Protocols**: Full support for all 12 RC-Switch protocols (Standard variants, HT6P20B, HS2303/AUKEY, Conrad RS-200, 1ByOne Doorbell, HT12E, SM5212)
- **Full TX/RX Support**: All protocols support both transmission and reception on 433MHz frequency
- **Reliable RF Handling**: Uses battle-tested RC-Switch library for accurate timing and signal encoding/decoding
- **UART Communication**: Simple text-based serial interface (`Protocol:BitCount:HEXVALUE`) for commanding and monitoring
- **FreeRTOS Integration**: Multi-threaded task-based architecture with dedicated RF RX/TX and UART handling
- **High Transmission Reliability**: Hardware timer-based timing eliminates jitter and timing precision issues
- **Modular Design**: Clean separation of RF, protocol, and UART layers

## Hardware & Dependencies

### Microcontroller & Framework
- **Board**: Wemos D1 R32 (ESP32-based)
- **Framework**: Arduino with FreeRTOS
- **RF Modules**: Standard 433MHz RF transmitter/receiver modules

### Libraries
- **RC-Switch**: `sui77/rc-switch@^2.6.2` - Proven RF protocol handling library

### GPIO Connections

| Interface      | Pin         | Function                                    |
|----------------|-------------|---------------------------------------------|
| RF Receiver    | GPIO 22     | RF signal input (interrupt-driven capture)  |
| RF Transmitter | GPIO 26     | RF signal output (RC-Switch managed timing) |
| UART           | USB (CH340) | Serial communication at 2,000,000 baud      |

### Key Performance Specs
- **Protocols**: 12 RC-Switch protocols (IDs 1-12, variable bit widths)
- **Frequency**: 433 MHz (315 MHz compatible)
- **Transmission Reliability**: High (RC-Switch hardware timer-based timing)
- **Baud Rate**: 2,000,000 bps (USB interface limitation)
- **Range**: Device dependent, typically 20-100 meters in open space

## Project Structure

```
Remote Master/
├── src/
│   ├── main.cpp                       # Application entry point & FreeRTOS task setup
│   ├── system/
│   │   ├── sys_rf_rx.cpp             # RF receiver (RC-Switch based)
│   │   ├── sys_rf_tx.cpp             # RF transmitter (RC-Switch based)
│   │   └── sys_uart.cpp              # UART command parsing & output
│   └── protocols/
│       ├── core/
│       │   ├── protocol_registry.cpp  # Protocol name→ID mapping
│       │   └── frame_parser.cpp       # UART frame serialization
│       └── impl/
│           └── protocol_impl.cpp      # Protocol metadata & registry setup (RC-Switch handles encoding)
│
├── include/                           # Header files (mirrors src structure)
├── platformio.ini                     # Build configuration (includes RC-Switch library)
└── README.md                          # This file
```

### Key Components

- **sys_rf_rx.cpp**: Initializes RC-Switch receiver on GPIO22, handles interrupt-driven signal capture
- **sys_rf_tx.cpp**: Initializes RC-Switch transmitter on GPIO26, queues transmission commands
- **sys_uart.cpp**: Parses UART commands (`EV1527:24:AABBCC;`), routes to protocol registry
- **protocol_registry**: Maps protocol names to RC-Switch protocol IDs

## Getting Started

### Prerequisites

- PlatformIO IDE or CLI (version 6.0+)
- Wemos D1 R32 or compatible ESP32 board
- 433MHz RF transmitter and receiver modules
- USB cable for ESP32 programming and serial communication

### Quick Start

1. **Clone the repository**
   ```bash
   git clone https://github.com/yourusername/remote-master.git
   cd remote-master
   ```

2. **Build and upload**
   ```bash
   platformio run -e esp32doit-devkit-v1 -t upload
   ```

3. **Monitor UART output**
   ```bash
   platformio device monitor -e esp32doit-devkit-v1 --baud 2000000
   ```

### Dependencies

The `platformio.ini` file automatically handles library installation:
- **RC-Switch** (sui77/rc-switch@^2.6.2) - Automatically installed by PlatformIO

If manually managing dependencies, ensure RC-Switch is in your `lib/` directory.

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

### System Overview

```
┌─────────────────────────────────────────────────────────────┐
│                    Remote Master ESP32                       │
├─────────────────────────────────────────────────────────────┤
│                                                               │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐      │
│  │ RF RX Task   │  │ UART Task    │  │ RF TX Task   │      │
│  │ (Priority 4) │  │ (Priority 3) │  │ (Priority 4) │      │
│  │ Core: 0      │  │ Core: 0      │  │ Core: 1      │      │
│  └──────────────┘  └──────────────┘  └──────────────┘      │
│        ↓                  ↓                   ↓               │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐      │
│  │RC-Switch RX  │  │Frame Parser  │  │RC-Switch TX  │      │
│  │ GPIO22       │  │Protocol Reg  │  │ GPIO26       │      │
│  │ Interrupt    │  │Queue Handler │  │ Transmission │      │
│  └──────────────┘  └──────────────┘  └──────────────┘      │
│        ↓                  ↓                   ↑               │
│  ┌──────────────────────────────────────────────────┐       │
│  │         RF Signal Handler (RC-Switch)            │       │
│  │    433MHz / 315MHz (12 Protocols, Dual TX/RX)    │       │
│  └──────────────────────────────────────────────────┘       │
│        ↓                                   ↑                 │
└─────────────────────────────────────────────────────────────┘
        ↓                                   ↑
   [RF Receiver]                      [RF Transmitter]
   GPIO 22 Input                      GPIO 26 Output
        ↓                                   ↑
   [433MHz Signal]                    [433MHz Signal]
```

### FreeRTOS Tasks

- **RF RX Task** (Priority 4, Core 0):
  - Monitors RC-Switch receiver for available signals
  - Decodes received values and outputs via UART

- **UART Task** (Priority 3, Core 0):
  - Parses incoming UART commands (`EV1527:24:AABBCC;`)
  - Routes to protocol registry and TX queue
  - Outputs RX results and status messages

- **RF TX Task** (Priority 4, Core 1):
  - Dequeues transmission commands
  - Calls RC-Switch.send() with proper protocol and timing
  - Handles repetition and timing accuracy

### RC-Switch Integration

The system uses the **RC-Switch library** for all RF encoding/decoding:
- **Proven reliability**: Battle-tested on millions of ESP32 devices
- **Accurate timing**: Hardware timer-based pulse generation (no FreeRTOS jitter)
- **12 Protocol support**: All 12 RC-Switch protocols with full 433MHz/315MHz compatibility
- **Simple API**: `rcswitch.send(value, bitLength)` for transmission, `rcswitch.available()` for reception
- **Auto-detection**: Receiver automatically detects protocol type from received signal
- **Flexible bit widths**: Support for variable code lengths (typically 20-32 bits)

## UART Interface

### Command Format

Remote Master uses a simple text-based command format:

```
[PROTOCOL]:[BITCOUNT]:[HEXVALUE];
```

**Important:** The HEXVALUE is always interpreted as **hexadecimal** (without `0x` prefix).

### Examples

**Send EV1527 standard 24-bit code:**
```
EV1527:24:AABBCC;
```

This sends:
- Protocol: EV1527 (RC-Switch Protocol ID 1)
- Bit count: 24 bits
- Value: 0xAABBCC (hex)

**Send SC5262 with 650µs pulse variant:**
```
SC5262+650U:24:AABBCC;
```

**Send PT2262 protocol with 32-bit hex code:**
```
PT2262+650U:32:12345678;
```
This sends 0x12345678 (hexadecimal), **not** 12,345,678 (decimal)

**Send HT6P20B protocol:**
```
HT6P20B:32:FEDCBA98;
```

**Response for received signal (chip auto-detected):**
```
EV1527:24:6F65B0;
```
(Received value is 0x6F65B0 in hex)

Or if 650µs variant detected:
```
PT2262+650U:24:6F65B0;
```

**Send Conrad RS-200 RX variant with 20-bit code:**
```
RS200_RX:20:A1C2E;
```

### Serial Configuration

- **Baud rate**: 2,000,000 bps
- **Data bits**: 8
- **Stop bits**: 1
- **Parity**: None
- **Flow control**: None

## Supported Protocols

Remote Master supports **all 12 RC-Switch protocols** with full TX/RX capability. Each protocol includes multiple chip name aliases for convenience:

| ID | Primary Names | Aliases | Description |
|----|---|---|-----------|
| 1 | `EV1527`, `SC5262`, `HX2262` | `Standard` | Basic pulse width (~575µs) for EV1527/SC5262/HX2262 family |
| 2 | `PT2262+650U`, `PT2272+650U`, `EV1527+650U` | `Standard+650U` | Pulse width variant (~650µs) for PT2262/PT2272 family |
| 3 | `SC5262+100U`, `HX2262+100U` | `Standard+100U` | Short pulse variant (~100µs) for compact devices |
| 4 | `EV1527+380U`, `SC5262+380U` | `Standard+380U` | Medium pulse variant (~380µs) for legacy devices |
| 5 | `PT2262+500U`, `EV1527+500U` | `Standard+500U` | Pulse width variant (~500µs) for generic 433MHz remotes |
| 6 | `HT6P20B`, `HT6P20` | — | HT6P20B chipset protocol for HT6P20B-based remotes |
| 7 | `HS2303`, `AUKEY`, `HS2303_AUKEY` | — | AUKEY/HS2303 protocol for AUKEY brand remotes |
| 8 | `Conrad_RS200_RX`, `RS200_RX` | — | Conrad RS-200 receiver mode |
| 9 | `Conrad_RS200_TX`, `RS200_TX` | — | Conrad RS-200 transmitter mode |
| 10 | `1ByOne_Doorbell`, `1ByOne` | — | 1ByOne wireless doorbell protocol |
| 11 | `HT12E`, `HT12` | — | HT12E decoder chip protocol |
| 12 | `SM5212`, `SM52` | — | SM5212 chipset protocol |

### Using Different Protocols

All protocols are automatically registered at startup with chip name aliases. Use the UART interface with any registered name:

**Standard Protocol (ID 1) variants:**
```
EV1527:24:AABBCC;         # EV1527 chip variant
SC5262:24:AABBCC;         # SC5262 chip variant
HX2262:24:AABBCC;         # HX2262 chip variant
Standard:24:AABBCC;       # Generic Standard alias
```

**650µs Standard variant (ID 2):**
```
EV1527+650U:24:AABBCC;    # EV1527 with 650µs pulse
PT2262+650U:24:AABBCC;    # PT2262 with 650µs pulse
PT2272+650U:24:AABBCC;    # PT2272 with 650µs pulse
Standard+650U:24:AABBCC;  # Generic 650µs alias
```

**Other protocols:**
```
HT6P20B:32:FEDCBA98;      # HT6P20B chipset (hex)
AUKEY:20:FEDCBA;          # AUKEY remote (HS2303 variant, hex)
RS200_RX:24:ABCDEF;       # Conrad RS-200 RX mode (hex)
1ByOne:24:123456;         # 1ByOne doorbell (hex: 0x123456)
HT12E:24:AABBCC;          # HT12E decoder chip (hex)
SM5212:24:AABBCC;         # SM5212 chipset (hex)
```

**Note:** All values are hexadecimal. For example:
- `123456` = 0x123456 (not 123,456 decimal)
- `FEDCBA` = 0xFEDCBA = 16,702,650 decimal
- `A1C2E` = 0xA1C2E = 663,598 decimal

## Troubleshooting

### Common Issues

**No signals received:**
- Verify RF receiver module is connected to GPIO 22
- Check RF module power supply
- Ensure antenna is properly connected and positioned
- Verify frequency matches remote (433 MHz or 315 MHz)
- If receiving but protocol unrecognized, check supported protocols table above

**Transmission not working:**
- Verify RF transmitter module is connected to GPIO 26
- Check module power supply and ground
- Ensure antenna is connected
- Test with a known working remote to verify frequency (433 MHz standard)
- Verify protocol name is correct (use `Standard` not `EV1527`)
- Check bit count matches your device's code length

**Received protocol not matching expected:**
- RC-Switch auto-detects protocol; verify with UART output
- Some devices may use variants (e.g., `Standard+650U` instead of `Standard`)
- Try all Standard variants if protocol unrecognized
- Check remote battery level

**Garbled UART output:**
- Verify baud rate is 2,000,000 bps
- Check USB cable quality
- Ensure USB to UART Bridge driver is installed (Windows)

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details.
