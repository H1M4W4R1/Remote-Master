#include "protocols/impl/protocol_proprietary_beta.h"

ProprietaryBetaProtocol::ProprietaryBetaProtocol() {
    // Constructor - uses default values
}

const char* ProprietaryBetaProtocol::getName() const {
    return "P-BETA";
}

bool ProprietaryBetaProtocol::verifyOne(uint16_t h, uint16_t l) {
    return h >= ONE_H_MIN && h <= ONE_H_MAX && l >= ONE_L_MIN && l <= ONE_L_MAX;
}

bool ProprietaryBetaProtocol::verifyZero(uint16_t h, uint16_t l) {
    return h >= ZERO_H_MIN && h <= ZERO_H_MAX && l >= ZERO_L_MIN && l <= ZERO_L_MAX;
}

bool ProprietaryBetaProtocol::verifySync(uint16_t sync) {
    return sync >= SYNC_MIN && sync <= SYNC_MAX;
}

bool ProprietaryBetaProtocol::tryParse(const uint16_t* timings, size_t count, ProtocolData& out, size_t& out_timingsConsumed) {
    // Need at least sync pulse + 1 data bit (2 timings)
    if (timings == nullptr || count < 2) {
        out.isValid = false;
        out_timingsConsumed = 0;
        return false;
    }

    // Check sync pulse (note: verifySync only checks first timing)
    if (!verifySync(timings[0])) {
        out.isValid = false;
        out_timingsConsumed = 0;
        return false;
    }

    uint64_t value = 0;
    uint8_t nBitsRegistered = 0;
    size_t n = 1;

    // Parse data bits (same condition as P-ALPHA - checks % 2)
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

bool ProprietaryBetaProtocol::buildPacket(const ProtocolData& data, uint16_t* outTimings, size_t& outCount, size_t maxCount) {
    if (outTimings == nullptr || maxCount < 2 || data.bits == 0 || data.bits > 64) {
        outCount = 0;
        return false;
    }

    size_t index = 0;

    // Add sync pulse (P-BETA uses two sync pulses)
    if (index + 1 >= maxCount) {
        outCount = 0;
        return false;
    }
    outTimings[index++] = SYNC_DEFAULT;
    outTimings[index++] = SYNC_DEFAULT;

    // Encode bits from MSB to LSB
    for (int n = data.bits - 1; n >= 0; n--) {
        if (index + 1 >= maxCount) {
            outCount = 0;
            return false; // Not enough space
        }

        if ((data.value & (1ULL << n)) > 0) {
            // Bit is 1: ONE_H and ONE_L
            outTimings[index++] = ONE_H_DEFAULT;
            outTimings[index++] = ONE_L_DEFAULT;
        } else {
            // Bit is 0: ZERO_H and ZERO_L
            outTimings[index++] = ZERO_H_DEFAULT;
            outTimings[index++] = ZERO_L_DEFAULT;
        }
    }

    outCount = index;
    return true;
}
