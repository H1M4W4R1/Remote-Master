#pragma once

#include <cstdint>
#include <cstddef>

// Protocol frame structure for UART serialization
struct ProtocolFrame {
    char protocolName[16];      // e.g., "EV1527"
    uint8_t bits;               // e.g., 24
    uint64_t value;             // e.g., 0xAABBCC
};

// Parse UART input: "EV1527:24:AABBCC;" → ProtocolFrame
// input: null-terminated string to parse
// out: output ProtocolFrame
// Returns: true if parsing was successful
bool parseProtocolFrame(const char* input, ProtocolFrame& out);

// Serialize for UART output: ProtocolFrame → "EV1527:24:AABBCC;"
// frame: input ProtocolFrame to serialize
// output: buffer to write serialized frame (should be at least 64 bytes)
// maxLen: size of output buffer
// Returns: number of characters written (not including null terminator)
size_t serializeProtocolFrame(const ProtocolFrame& frame, char* output, size_t maxLen);
