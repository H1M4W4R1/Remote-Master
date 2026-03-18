#include "protocols/impl/protocol_proprietary_alpha.h"

ProprietaryAlphaProtocol::ProprietaryAlphaProtocol() {
    // Constructor - uses default values
}

const char* ProprietaryAlphaProtocol::getName() const {
    return "P-ALPHA";
}

bool ProprietaryAlphaProtocol::verifyOne(uint16_t h, uint16_t l) {
    float ratio = (float)h / l;
    return ratio > GENERIC_RATIO_MIN && ratio < GENERIC_RATIO_MAX;
}

bool ProprietaryAlphaProtocol::verifyZero(uint16_t h, uint16_t l) {
    float ratio = (float)l / h;
    return ratio > GENERIC_RATIO_MIN && ratio < GENERIC_RATIO_MAX;
}

bool ProprietaryAlphaProtocol::verifySync(uint16_t sync, uint16_t data) {
    float ratio = (float)sync / data;
    return (ratio > SYNC_H_TO_DH_MIN && ratio < SYNC_H_TO_DH_MAX) ||
           (ratio > SYNC_H_TO_DL_MIN && ratio < SYNC_H_TO_DL_MAX);
}

bool ProprietaryAlphaProtocol::tryParse(const uint16_t* timings, size_t count, ProtocolData& out, size_t& out_timingsConsumed) {
    // Need at least sync pulse + 1 data bit (2 timings)
    if (timings == nullptr || count < 2) {
        out.isValid = false;
        out_timingsConsumed = 0;
        return false;
    }

    // Check sync pulse
    if (!verifySync(timings[0], timings[1])) {
        out.isValid = false;
        out_timingsConsumed = 0;
        return false;
    }

    uint64_t value = 0;
    uint8_t nBitsRegistered = 0;
    size_t n = 1;

    // Parse data bits (different condition than EV1527 - checks % 2 instead of % 4)
    for (; n < count; n += 2) {
        if (n + 1 >= count) {
            break; // Not enough data for another bit
        }

        uint16_t h = timings[n];
        uint16_t l = timings[n + 1];

        if (verifyOne(h, l)) {
            value = value << 1;
            value |= 1;
            nBitsRegistered++;
        } else if (verifyZero(h, l)) {
            value = value << 1;
            nBitsRegistered++;
        } else if (nBitsRegistered >= MIN_BITS && nBitsRegistered % 2 == 0) {
            // Valid frame end (checks for even number of bits)
            break;
        } else {
            // Invalid bit pattern
            out.isValid = false;
            out_timingsConsumed = 0;
            return false;
        }
    }

    // Check if we got a valid frame
    if (nBitsRegistered >= MIN_BITS && nBitsRegistered % 2 == 0) {
        out.isValid = true;
        out.value = value;
        out.bits = nBitsRegistered;
        out_timingsConsumed = n + 1; // +1 because we've processed up to index n
        return true;
    }

    out.isValid = false;
    out_timingsConsumed = 0;
    return false;
}

bool ProprietaryAlphaProtocol::buildPacket(const ProtocolData& data, uint16_t* outTimings, size_t& outCount, size_t maxCount) {
    if (outTimings == nullptr || maxCount < 2 || data.bits == 0 || data.bits > 64) {
        outCount = 0;
        return false;
    }

    uint16_t shortPulse = (uint16_t)(desiredSyncLength / SYNC_H_TO_DL_RATIO);
    uint16_t longPulse = (uint16_t)(shortPulse * GENERIC_RATIO);

    size_t index = 0;

    // Add sync pulse
    if (index + 1 >= maxCount) {
        outCount = 0;
        return false;
    }
    outTimings[index++] = desiredSyncLength;
    outTimings[index++] = shortPulse;

    // Encode bits from MSB to LSB
    for (int n = data.bits - 1; n >= 0; n--) {
        if (index + 1 >= maxCount) {
            outCount = 0;
            return false; // Not enough space
        }

        if ((data.value & (1ULL << n)) > 0) {
            // Bit is 1: long high, short low
            outTimings[index++] = longPulse;
            outTimings[index++] = shortPulse;
        } else {
            // Bit is 0: short high, long low
            outTimings[index++] = shortPulse;
            outTimings[index++] = longPulse;
        }
    }

    outCount = index;
    return true;
}

void ProprietaryAlphaProtocol::setSyncLength(uint16_t newSyncLength) {
    desiredSyncLength = newSyncLength;
}
