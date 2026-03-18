#pragma once

#include "protocol_interface.h"
#include <cstddef>

// Simple protocol registry for RC-Switch integration
// Maps protocol IDs (from RC-Switch) to protocol names and metadata
class ProtocolRegistry {
private:
    struct ProtocolEntry {
        int protocolId;           // RC-Switch protocol ID (or negative for custom)
        const char* name;         // Protocol name (e.g., "EV1527")
        bool supportsRx;
        bool supportsTx;
        IProtocol* customImpl;     // Optional custom implementation for non-standard protocols
    };

    static const size_t MAX_PROTOCOLS = 16;
    ProtocolEntry protocols[MAX_PROTOCOLS];
    size_t protocolCount;

public:
    ProtocolRegistry();

    // Register a protocol by ID and name
    // For standard RC-Switch protocols: protocolId > 0, customImpl = nullptr
    // For custom protocols: protocolId < 0, customImpl = custom IProtocol implementation
    bool registerProtocol(int protocolId, const char* name, bool supportsRx, bool supportsTx, IProtocol* customImpl = nullptr);

    // Get protocol name by ID (from RC-Switch)
    // Returns nullptr if protocol not registered
    const char* getProtocolName(int protocolId);

    // Get protocol ID by name
    // Returns -1 if not found
    int getProtocolId(const char* name);

    // Get protocol by name (for advanced operations)
    // Returns nullptr if not found or no custom implementation
    IProtocol* getProtocolByName(const char* name);

    // Check if protocol supports RX
    bool supportsRx(int protocolId);

    // Check if protocol supports TX
    bool supportsTx(int protocolId);
};
