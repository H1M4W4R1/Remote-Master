#pragma once

#include <cstdint>
#include <cstddef>

template<typename T, uint32_t SIZE>
class RingBuffer {
    static_assert(SIZE > 1, "RingBuffer SIZE must be greater than 1");

    private:
        uint32_t head = 0;
        uint32_t tail = 0;
        uint32_t count = 0;

        T buffer[SIZE];

    public:
        bool push(const T& item)
        {
            if (count == SIZE) {
                // overwrite oldest element
                tail = (tail + 1) % SIZE;
                count--;
            }

            buffer[head] = item;
            head = (head + 1) % SIZE;
            count++;
            return true;
        }

        bool pop()
        {
            if (count == 0)
                return false;

            tail = (tail + 1) % SIZE;
            count--;
            return true;
        }

        bool pop(T& out)
        {
            if (count == 0)
                return false;

            out = buffer[tail];
            tail = (tail + 1) % SIZE;
            count--;
            return true;
        }

        bool empty() const
        {
            return count == 0;
        }

        bool full() const
        {
            return count == SIZE;
        }

        uint32_t size() const
        {
            return count;
        }

        T getHistoryAt(const uint32_t offset) const
        {
            // offset 0 = most recently pushed element
            if (offset >= count)
                return T{}; // or assert / throw, depending on requirements

            uint32_t index =
                (head + SIZE - 1 - offset) % SIZE;

            return buffer[index];
        }

        T peek(T& out)
        {
            if (count == 0)
                return false;

            out = buffer[tail];
            return true;
        }
};
