#pragma once

#include "Arduino.h"
#include <cstdint>
#include <cstddef>

template<typename T, uint32_t SIZE>
class RingBuffer {
    static_assert(SIZE > 1, "RingBuffer SIZE must be greater than 1");

    private:
        uint32_t head = 0;
        uint32_t tail = 0;
        uint32_t consumedUpTo = 0;  // Track consumption separately from tail for peek operations
        uint32_t lastPushTime = 0;  // Timestamp when last item was pushed (in milliseconds)

        T buffer[SIZE];

    public:
        bool push(const T& item)
        {
            buffer[head] = item;
            head = (head + 1) % SIZE;
            lastPushTime = millis();  // Update last push time
            return true;
        }

        // Original pop (backward compatible)
        bool pop()
        {
            if (tail == head) return false;
            tail = (tail + 1) % SIZE;
            return true;
        }

        // Original pop with output (backward compatible)
        bool pop(T& out)
        {
            if (tail == head) return false;
            out = buffer[tail];
            tail = (tail + 1) % SIZE;
            return true;
        }

        // Peek at element at offset from consumedUpTo without removing it
        // offset: 0 = first unconsumed, 1 = second unconsumed, etc.
        bool peek(uint32_t offset, T& out)
        {
            uint32_t pos = (consumedUpTo + offset) % SIZE;
            if (pos == head) return false;  // No more data
            out = buffer[pos];
            return true;
        }

        // Get number of unconsumed items available
        uint32_t getAvailableCount() const
        {
            if (head >= consumedUpTo) {
                return head - consumedUpTo;
            } else {
                return (SIZE - consumedUpTo) + head;
            }
        }

        // Mark count items as consumed, advancing consumedUpTo
        // Returns how many items were actually consumed (might be less than requested if not enough available)
        uint32_t consume(uint32_t count)
        {
            uint32_t available = getAvailableCount();
            uint32_t toConsume = (count > available) ? available : count;
            consumedUpTo = (consumedUpTo + toConsume) % SIZE;
            return toConsume;
        }

        // Get milliseconds since last item was pushed
        uint32_t getTimeSinceLastPush() const
        {
            return millis() - lastPushTime;
        }

        // Reset consumption pointer (start fresh collection)
        void resetConsumption()
        {
            consumedUpTo = head;
        }
};

// Provide millis() function for timing (must be defined elsewhere or in Arduino environment)
// This is typically available in Arduino/FreeRTOS contexts
#if !defined(millis)
extern uint32_t millis();
#endif
