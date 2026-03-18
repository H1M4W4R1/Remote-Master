#pragma once

#include "protocol_registry.h"
#include "frame_parser.h"
#include "../../system/sys_rf_tx.h"
#include <cstddef>

class ProtocolEncoder {
private:
    ProtocolRegistry& registry;

public:
    ProtocolEncoder(ProtocolRegistry& reg);

    // Encode a protocol frame to RF timings
    // input: string like "EV1527:24:AABBCC;"
    // outCommand: TxCommand structure to fill with encoded timings
    // Returns: true if encoding was successful
    bool encodeFrame(const char* input, TxCommand& outCommand);

    // Encode protocol data directly
    // frame: parsed ProtocolFrame
    // outCommand: TxCommand structure to fill with encoded timings
    // Returns: true if encoding was successful
    bool encodeProtocolData(const ProtocolFrame& frame, TxCommand& outCommand);
};
