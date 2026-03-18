#pragma once

#include <cstdint>
#include <cstddef>

// Protocol data structure - result of decoding RF timings or input for encoding
struct ProtocolData {
    bool isValid;
    uint64_t value;
    uint8_t bits;
};

// Base interface for all radio protocols
class IProtocol {
public:
    virtual ~IProtocol() = default;

    // Get the protocol name (e.g., "EV1527", "P-ALPHA")
    virtual const char* getName() const = 0;

    // Try to decode timings into protocol data
    // Returns true if a valid complete frame was successfully decoded
    // out_timingsConsumed: set to number of timing values consumed by this decode attempt
    // (allows decoder to advance buffer past processed timings)
    virtual bool tryParse(const uint16_t* timings, size_t count, ProtocolData& out, size_t& out_timingsConsumed) = 0;

    // Encode protocol data into RF timing pulses
    // Returns true if encoding was successful
    // outTimings: array to fill with timing values
    // outCount: set to number of timing values generated
    // maxCount: maximum number of elements that can fit in outTimings array
    virtual bool buildPacket(const ProtocolData& data, uint16_t* outTimings, size_t& outCount, size_t maxCount) = 0;
};
