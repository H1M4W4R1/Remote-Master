#include "protocols/core/protocol_registry.h"
#include <cstring>

ProtocolRegistry::ProtocolRegistry() : protocolCount(0) {
    for (size_t i = 0; i < MAX_PROTOCOLS; i++) {
        protocols[i] = nullptr;
    }
}

bool ProtocolRegistry::registerProtocol(IProtocol* protocol) {
    if (protocol == nullptr || protocolCount >= MAX_PROTOCOLS) {
        return false;
    }
    protocols[protocolCount++] = protocol;
    return true;
}

IProtocol* ProtocolRegistry::getProtocolByName(const char* name) {
    if (name == nullptr) {
        return nullptr;
    }
    for (size_t i = 0; i < protocolCount; i++) {
        if (strcmp(protocols[i]->getName(), name) == 0) {
            return protocols[i];
        }
    }
    return nullptr;
}

bool ProtocolRegistry::tryDetectAllProtocols(const uint16_t* timings, size_t count,
                                             IProtocol** outMatches, size_t& outCount,
                                             size_t* out_timingsConsumedByFirst) {
    outCount = 0;

    if (timings == nullptr || count == 0 || outMatches == nullptr) {
        return false;
    }

    size_t firstConsumedCount = 0;
    bool anyMatch = false;

    for (size_t i = 0; i < protocolCount; i++) {
        ProtocolData data;
        size_t timingsConsumed = 0;

        if (protocols[i]->tryParse(timings, count, data, timingsConsumed) && data.isValid) {
            outMatches[outCount++] = protocols[i];
            anyMatch = true;

            // Record how many timings the first successful protocol consumed
            if (out_timingsConsumedByFirst && outCount == 1) {
                firstConsumedCount = timingsConsumed;
                *out_timingsConsumedByFirst = firstConsumedCount;
            }
        }
    }

    return anyMatch;
}
