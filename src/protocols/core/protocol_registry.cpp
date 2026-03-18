#include "protocols/core/protocol_registry.h"
#include <cstring>

ProtocolRegistry::ProtocolRegistry() : protocolCount(0) {
    for (size_t i = 0; i < MAX_PROTOCOLS; i++) {
        protocols[i].protocolId = 0;
        protocols[i].name = nullptr;
        protocols[i].supportsRx = false;
        protocols[i].supportsTx = false;
        protocols[i].customImpl = nullptr;
    }
}

bool ProtocolRegistry::registerProtocol(int protocolId, const char* name, bool supportsRx, bool supportsTx, IProtocol* customImpl) {
    if (name == nullptr || protocolCount >= MAX_PROTOCOLS) {
        return false;
    }

    protocols[protocolCount].protocolId = protocolId;
    protocols[protocolCount].name = name;
    protocols[protocolCount].supportsRx = supportsRx;
    protocols[protocolCount].supportsTx = supportsTx;
    protocols[protocolCount].customImpl = customImpl;
    protocolCount++;

    return true;
}

const char* ProtocolRegistry::getProtocolName(int protocolId) {
    for (size_t i = 0; i < protocolCount; i++) {
        if (protocols[i].protocolId == protocolId) {
            return protocols[i].name;
        }
    }
    return nullptr;
}

int ProtocolRegistry::getProtocolId(const char* name) {
    if (name == nullptr) {
        return -1;
    }
    for (size_t i = 0; i < protocolCount; i++) {
        if (strcmp(protocols[i].name, name) == 0) {
            return protocols[i].protocolId;
        }
    }
    return -1;
}

IProtocol* ProtocolRegistry::getProtocolByName(const char* name) {
    if (name == nullptr) {
        return nullptr;
    }
    for (size_t i = 0; i < protocolCount; i++) {
        if (strcmp(protocols[i].name, name) == 0) {
            return protocols[i].customImpl;  // May be nullptr for standard RC-Switch protocols
        }
    }
    return nullptr;
}

bool ProtocolRegistry::supportsRx(int protocolId) {
    for (size_t i = 0; i < protocolCount; i++) {
        if (protocols[i].protocolId == protocolId) {
            return protocols[i].supportsRx;
        }
    }
    return false;
}

bool ProtocolRegistry::supportsTx(int protocolId) {
    for (size_t i = 0; i < protocolCount; i++) {
        if (protocols[i].protocolId == protocolId) {
            return protocols[i].supportsTx;
        }
    }
    return false;
}
