#pragma once

#include "../core/protocol_interface.h"

// Proprietary Beta Protocol Implementation
// Proprietary protocol using absolute timing windows instead of ratios
class ProprietaryBetaProtocol : public IProtocol {
private:
    // Timing windows in microseconds (min/max bounds)
    static constexpr uint16_t SYNC_MIN = 6500;
    static constexpr uint16_t SYNC_MAX = 7500;
    static constexpr uint16_t SYNC_DEFAULT = 7000;

    static constexpr uint16_t ONE_H_MIN = 1300;
    static constexpr uint16_t ONE_H_MAX = 1900;
    static constexpr uint16_t ONE_H_DEFAULT = 1600;

    static constexpr uint16_t ONE_L_MIN = 600;
    static constexpr uint16_t ONE_L_MAX = 1200;
    static constexpr uint16_t ONE_L_DEFAULT = 900;

    static constexpr uint16_t ZERO_H_MIN = 400;
    static constexpr uint16_t ZERO_H_MAX = 1000;
    static constexpr uint16_t ZERO_H_DEFAULT = 700;

    static constexpr uint16_t ZERO_L_MIN = 2550;
    static constexpr uint16_t ZERO_L_MAX = 3150;
    static constexpr uint16_t ZERO_L_DEFAULT = 2850;

    static constexpr int MIN_BITS = 8;
    static constexpr int DEFAULT_BITS = 18;

    // Helper functions for verification
    static bool verifyOne(uint16_t h, uint16_t l);
    static bool verifyZero(uint16_t h, uint16_t l);
    static bool verifySync(uint16_t sync);

public:
    ProprietaryBetaProtocol();
    virtual ~ProprietaryBetaProtocol() = default;

    virtual const char* getName() const override;
    virtual bool tryParse(const uint16_t* timings, size_t count, ProtocolData& out, size_t& out_timingsConsumed) override;
    virtual bool buildPacket(const ProtocolData& data, uint16_t* outTimings, size_t& outCount, size_t maxCount) override;
};
