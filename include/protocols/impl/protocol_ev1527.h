#pragma once

#include "../core/protocol_interface.h"

// EV1527 Protocol Implementation
// Common 433MHz remote control protocol supporting 20-24 bit codes
// Note: With RC-Switch integration, actual encoding/decoding is handled by RC-Switch library
// This class now serves as a metadata wrapper for RC-Switch protocol #1
class EV1527Protocol : public IProtocol {
public:
    EV1527Protocol();
    virtual ~EV1527Protocol() = default;

    virtual const char* getName() const override;
    virtual int getProtocolId() const override;
    virtual bool supportsRx() const override;
    virtual bool supportsTx() const override;
};
