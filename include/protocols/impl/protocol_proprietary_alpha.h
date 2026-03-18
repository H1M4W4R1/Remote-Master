#pragma once

#include "../core/protocol_interface.h"

// Proprietary Alpha Protocol Implementation
// Proprietary 433MHz protocol supporting variable-length codes (8-16 bits)
class ProprietaryAlphaProtocol : public IProtocol {
private:
    uint16_t desiredSyncLength = 4400; // Default sync pulse length in microseconds

    // Protocol timing parameters (ratios) - different from EV1527
    static constexpr float SYNC_H_TO_DL_RATIO = 4400.0f / 350.0f;
    static constexpr float SYNC_H_TO_DL_MIN = SYNC_H_TO_DL_RATIO / 1.2f;
    static constexpr float SYNC_H_TO_DL_MAX = SYNC_H_TO_DL_RATIO * 1.2f;

    static constexpr float SYNC_H_TO_DH_RATIO = 4400.0f / 920.0f;
    static constexpr float SYNC_H_TO_DH_MIN = SYNC_H_TO_DH_RATIO / 1.2f;
    static constexpr float SYNC_H_TO_DH_MAX = SYNC_H_TO_DH_RATIO * 1.2f;

    static constexpr float GENERIC_RATIO = 920.0f / 350.0f;
    static constexpr float GENERIC_RATIO_MIN = GENERIC_RATIO / 1.2f;
    static constexpr float GENERIC_RATIO_MAX = GENERIC_RATIO * 1.2f;

    static constexpr int MIN_BITS = 8;
    static constexpr int DEFAULT_BITS = 16;

    // Helper functions for verification
    static bool verifyOne(uint16_t h, uint16_t l);
    static bool verifyZero(uint16_t h, uint16_t l);
    static bool verifySync(uint16_t sync, uint16_t data);

public:
    ProprietaryAlphaProtocol();
    virtual ~ProprietaryAlphaProtocol() = default;

    virtual const char* getName() const override;
    virtual bool tryParse(const uint16_t* timings, size_t count, ProtocolData& out, size_t& out_timingsConsumed) override;
    virtual bool buildPacket(const ProtocolData& data, uint16_t* outTimings, size_t& outCount, size_t maxCount) override;

    // Set the desired sync pulse length (for encoding)
    void setSyncLength(uint16_t newSyncLength);
};
