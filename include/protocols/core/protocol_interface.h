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
// Note: With RC-Switch integration, protocols are simplified to name/ID mappings
// The actual encoding/decoding is handled by RC-Switch library
class IProtocol {
public:
    virtual ~IProtocol() = default;

    // Get the protocol name (e.g., "EV1527")
    virtual const char* getName() const = 0;

    // Get the RC-Switch protocol ID (or custom protocol ID for non-standard protocols)
    // RC-Switch protocol IDs: 1=EV1527, etc. Return negative for custom implementations
    virtual int getProtocolId() const = 0;

    // Check if this protocol supports RX (receiving/decoding)
    virtual bool supportsRx() const = 0;

    // Check if this protocol supports TX (transmitting/encoding)
    virtual bool supportsTx() const = 0;

    // For RX: Extract protocol data from RC-Switch receiver
    // This is used for non-standard protocols that RC-Switch doesn't support natively
    // For standard protocols, this method may not be called (RC-Switch handles it directly)
    virtual bool decodeFromRcSwitch(uint64_t value, uint8_t bitlength, ProtocolData& out) {
        out.value = value;
        out.bits = bitlength;
        out.isValid = true;
        return true;  // Default: pass through RC-Switch decoded value
    }

    // For TX: Send this protocol via RC-Switch
    // The actual transmission is done by calling RCSwitch::send()
    // This method prepares/validates the data before transmission
    virtual bool encodeForRcSwitch(const ProtocolData& data) {
        return true;  // Default: data is ready to send as-is
    }
};
