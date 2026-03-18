#pragma once

#include "protocol_interface.h"
#include <cstddef>

class ProtocolRegistry {
private:
    static const size_t MAX_PROTOCOLS = 8;
    IProtocol* protocols[MAX_PROTOCOLS];
    size_t protocolCount;

public:
    ProtocolRegistry();

    // Register a new protocol
    // Returns true if successful, false if registry is full
    bool registerProtocol(IProtocol* protocol);

    // Get protocol by name
    // Returns nullptr if not found
    IProtocol* getProtocolByName(const char* name);

    // Try to decode with ALL registered protocols
    // Returns array of successful decodings and count
    // outMatches: array to store matching protocol pointers (must be large enough)
    // outCount: set to number of matches found
    // Returns true if at least one protocol matched
    bool tryDetectAllProtocols(const uint16_t* timings, size_t count,
                               IProtocol** outMatches, size_t& outCount,
                               size_t* out_timingsConsumedByFirst = nullptr);
};
