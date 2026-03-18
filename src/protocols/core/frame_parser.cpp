#include "protocols/core/frame_parser.h"
#include <cstring>
#include <cstdio>
#include <cctype>

bool parseProtocolFrame(const char* input, ProtocolFrame& out) {
    if (input == nullptr) {
        return false;
    }

    // Find first colon (protocol name separator)
    const char* colon1 = strchr(input, ':');
    if (colon1 == nullptr) {
        return false;
    }

    // Extract protocol name
    size_t nameLen = colon1 - input;
    if (nameLen == 0 || nameLen >= sizeof(out.protocolName)) {
        return false;
    }
    strncpy(out.protocolName, input, nameLen);
    out.protocolName[nameLen] = '\0';

    // Find second colon (bits separator)
    const char* colon2 = strchr(colon1 + 1, ':');
    if (colon2 == nullptr) {
        return false;
    }

    // Parse bits (decimal)
    int bitsValue = 0;
    const char* bitsStr = colon1 + 1;
    char bitsBuffer[16] = {0};
    size_t bitsLen = colon2 - bitsStr;
    if (bitsLen == 0 || bitsLen >= sizeof(bitsBuffer)) {
        return false;
    }
    strncpy(bitsBuffer, bitsStr, bitsLen);
    bitsBuffer[bitsLen] = '\0';

    // Parse decimal bits value
    if (sscanf(bitsBuffer, "%d", &bitsValue) != 1 || bitsValue <= 0 || bitsValue > 255) {
        return false;
    }
    out.bits = (uint8_t)bitsValue;

    // Find frame end marker ';'
    const char* semicolon = strchr(colon2 + 1, ';');
    if (semicolon == nullptr) {
        return false;
    }

    // Parse hex value
    const char* valueStr = colon2 + 1;
    size_t valueLen = semicolon - valueStr;
    if (valueLen == 0 || valueLen > 16) { // max 16 hex digits for uint64_t
        return false;
    }

    char valueBuffer[32] = {0};
    strncpy(valueBuffer, valueStr, valueLen);
    valueBuffer[valueLen] = '\0';

    // Parse hex value
    uint64_t hexValue = 0;
    if (sscanf(valueBuffer, "%llx", &hexValue) != 1) {
        return false;
    }
    out.value = hexValue;

    return true;
}

size_t serializeProtocolFrame(const ProtocolFrame& frame, char* output, size_t maxLen) {
    if (output == nullptr || maxLen < 32) {
        return 0;
    }

    // Format: "PROTOCOL:BITS:HEXVALUE;"
    int written = snprintf(output, maxLen, "%s:%d:%llx;",
                           frame.protocolName,
                           frame.bits,
                           (unsigned long long)frame.value);

    if (written < 0 || written >= (int)maxLen) {
        return 0;
    }

    return (size_t)written;
}
