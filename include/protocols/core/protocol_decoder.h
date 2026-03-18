#pragma once

#include "protocol_registry.h"
#include "frame_parser.h"
#include "../../core/ring_buffer.h"
#include <cstddef>

class ProtocolDecoder {
private:
    ProtocolRegistry& registry;

    // Temporary buffer for collected timings (max size)
    static const size_t TIMING_BUFFER_SIZE = 512;
    uint16_t timingBuffer[TIMING_BUFFER_SIZE];
    size_t timingBufferCount;

    // Output buffer for frames
    static const size_t MAX_FRAMES = 8;
    ProtocolFrame frames[MAX_FRAMES];
    size_t frameCount;

    // How many timings were consumed by the first successful protocol
    size_t timingsConsumedByFirst;

public:
    ProtocolDecoder(ProtocolRegistry& reg);

    // Process timing data from ring buffer and attempt decode
    // timings: array of timing delta values from RF receiver
    // count: number of timing values in array
    // Returns: number of protocol matches found (0 if no match, >1 if ambiguous)
    size_t decodeTiming(const uint16_t* timings, size_t count);

    // Get decoded frames
    // index: which frame to retrieve (0 to frameCount-1)
    // Returns: pointer to ProtocolFrame, or nullptr if index out of range
    const ProtocolFrame* getFrame(size_t index) const;

    // Get number of decoded frames
    size_t getFrameCount() const;

    // Get number of timing values consumed by the first successful protocol match
    size_t getTimingsConsumed() const;

    // Clear the internal buffer
    void clearFrames();
};
