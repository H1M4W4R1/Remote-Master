#include "../../../include/protocols/core/protocol_decoder.h"
#include <cstring>

ProtocolDecoder::ProtocolDecoder(ProtocolRegistry& reg)
    : registry(reg), timingBufferCount(0), frameCount(0), timingsConsumedByFirst(0) {
    memset(timingBuffer, 0, sizeof(timingBuffer));
    memset(frames, 0, sizeof(frames));
}

size_t ProtocolDecoder::decodeTiming(const uint16_t* timings, size_t count) {
    if (timings == nullptr || count == 0) {
        return 0;
    }

    // Copy timings to internal buffer (up to max size)
    timingBufferCount = count < TIMING_BUFFER_SIZE ? count : TIMING_BUFFER_SIZE;
    memcpy(timingBuffer, timings, timingBufferCount * sizeof(uint16_t));

    // Clear previous frames
    clearFrames();
    timingsConsumedByFirst = 0;

    // Try to decode with all protocols
    IProtocol* matchingProtocols[8];
    size_t matchCount = 0;

    if (registry.tryDetectAllProtocols(timingBuffer, timingBufferCount, matchingProtocols, matchCount)) {
        // For each matching protocol, store the decoded frame
        for (size_t i = 0; i < matchCount && frameCount < MAX_FRAMES; i++) {
            ProtocolData data;
            size_t timingsConsumed = 0;

            if (matchingProtocols[i]->tryParse(timingBuffer, timingBufferCount, data, timingsConsumed) && data.isValid) {
                // Store the timing consumed by first successful protocol
                if (frameCount == 0) {
                    timingsConsumedByFirst = timingsConsumed;
                }

                // Store the decoded frame
                strncpy(frames[frameCount].protocolName, matchingProtocols[i]->getName(), sizeof(frames[frameCount].protocolName) - 1);
                frames[frameCount].protocolName[sizeof(frames[frameCount].protocolName) - 1] = '\0';
                frames[frameCount].bits = data.bits;
                frames[frameCount].value = data.value;
                frameCount++;
            }
        }
    }

    return frameCount;
}

const ProtocolFrame* ProtocolDecoder::getFrame(size_t index) const {
    if (index >= frameCount) {
        return nullptr;
    }
    return &frames[index];
}

size_t ProtocolDecoder::getFrameCount() const {
    return frameCount;
}

size_t ProtocolDecoder::getTimingsConsumed() const {
    return timingsConsumedByFirst;
}

void ProtocolDecoder::clearFrames() {
    frameCount = 0;
    timingsConsumedByFirst = 0;
    memset(frames, 0, sizeof(frames));
}
