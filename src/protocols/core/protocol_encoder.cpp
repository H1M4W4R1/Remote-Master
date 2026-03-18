#include "../../../include/protocols/core/protocol_encoder.h"
#include <cstring>

ProtocolEncoder::ProtocolEncoder(ProtocolRegistry& reg) : registry(reg) {
}

bool ProtocolEncoder::encodeFrame(const char* input, TxCommand& outCommand) {
    if (input == nullptr) {
        return false;
    }

    ProtocolFrame frame;
    if (!parseProtocolFrame(input, frame)) {
        return false;
    }

    return encodeProtocolData(frame, outCommand);
}

bool ProtocolEncoder::encodeProtocolData(const ProtocolFrame& frame, TxCommand& outCommand) {
    // Look up protocol by name
    IProtocol* protocol = registry.getProtocolByName(frame.protocolName);
    if (protocol == nullptr) {
        return false;
    }

    // Create ProtocolData from frame
    ProtocolData data;
    data.isValid = true;
    data.value = frame.value;
    data.bits = frame.bits;

    // Encode to timings
    size_t timingCount = 0;
    if (!protocol->buildPacket(data, outCommand.timings, timingCount, RF_TX_TIMINGS_BUFFER_SIZE)) {
        return false;
    }

    // Fill rest of timings array with zeros (sentinel value for end of data)
    for (size_t i = timingCount; i < RF_TX_TIMINGS_BUFFER_SIZE; i++) {
        outCommand.timings[i] = 0;
    }

    return true;
}
