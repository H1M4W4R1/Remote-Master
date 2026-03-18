#pragma once

#include "../core/protocol_interface.h"

// EV1527 Protocol Implementation
// Common 433MHz remote control protocol supporting 20-24 bit codes
class EV1527Protocol : public IProtocol {
private:
    uint16_t desiredSyncLength = 11600; // Default sync pulse length in microseconds

    // Protocol timing parameters (ratios)
    static constexpr float SYNC_H_TO_DL_RATIO = 31.0f;
    static constexpr float SYNC_H_TO_DL_MIN = SYNC_H_TO_DL_RATIO / 1.2f;
    static constexpr float SYNC_H_TO_DL_MAX = SYNC_H_TO_DL_RATIO * 1.2f;

    static constexpr float SYNC_H_TO_DH_RATIO = 10.33f;
    static constexpr float SYNC_H_TO_DH_MIN = SYNC_H_TO_DH_RATIO / 1.2f;
    static constexpr float SYNC_H_TO_DH_MAX = SYNC_H_TO_DH_RATIO * 1.2f;

    static constexpr float GENERIC_RATIO = 3.0f;
    static constexpr float GENERIC_RATIO_MIN = GENERIC_RATIO / 1.2f;
    static constexpr float GENERIC_RATIO_MAX = GENERIC_RATIO * 1.2f;

    static constexpr int MIN_BITS = 20;
    static constexpr int DEFAULT_BITS = 24;

    // Helper functions for verification
    static bool verifyOne(uint16_t h, uint16_t l);
    static bool verifyZero(uint16_t h, uint16_t l);
    static bool verifySync(uint16_t sync, uint16_t data);

public:
    EV1527Protocol();
    virtual ~EV1527Protocol() = default;

    virtual const char* getName() const override;
    virtual bool tryParse(const uint16_t* timings, size_t count, ProtocolData& out, size_t& out_timingsConsumed) override;
    virtual bool buildPacket(const ProtocolData& data, uint16_t* outTimings, size_t& outCount, size_t maxCount) override;

    // Set the desired sync pulse length (for encoding)
    void setSyncLength(uint16_t desiredSyncLength);
};
