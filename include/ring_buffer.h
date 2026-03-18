#pragma once

#include <cstdint>
#include <cstddef>

template<typename T, uint32_t SIZE>
class RingBuffer {
    static_assert(SIZE > 1, "RingBuffer SIZE must be greater than 1");

    private:
        uint32_t head = 0;
        uint32_t tail = 0;


        T buffer[SIZE];

    public:
        bool push(const T& item)
        {
            buffer[head] = item;
            head = (head + 1) % SIZE;
            return true;
        }

        bool pop()
        {
            if (tail == head) return false;
            tail = (tail + 1) % SIZE;
            return true;
        }

        bool pop(T& out)
        {
            if (tail == head) return false;
            out = buffer[tail];
            tail = (tail + 1) % SIZE;
            return true;
        }

};
