#pragma once
#include <assert.h>
#include <stdint.h>

#include "SqLog.h"

/**
 * A simple ring buffer.
 * Template arguments are for numeric type stored, and for size.
 * Not thread safe.
 * Guaranteed to be non-blocking. Adding or removing items will never
 * allocate or free memory.
 * Objects in RingBuffer are not owned by RingBuffer - they will not be destroyed.
 */
template <typename T, int SIZE>
class SqRingBuffer
{
public:
    SqRingBuffer(bool allowOverflow) : _allowOverflow(allowOverflow)
    {
        for (int i = 0; i < SIZE; ++i) {
            memory[i] = 0;
        }
    }

    int size() const;
    void push(T);
    T pop();
    bool full() const;
    bool empty() const;

    /**
     * @brief retrieve by index
     * last one pushed == index 0.
     * one before that is index 1. 
     */
    T at(int index) const;

    void _dump();

private:

#if 0
    // this constructor does not try to initialize
    SqRingBuffer(bool b)
    {
        assert(!b);
    }
#endif

    T memory[SIZE];
    bool couldBeFull = false;           // full and empty are ambiguous, both are in--out
    int inIndex = 0;
    int outIndex = 0;
    const bool _allowOverflow;

    /** Move up 'p' (a buffer index), wrap around if we hit the end
     * (this is the core of the circular ring buffer).
     */
    void advance(int &p);
};

template <>
inline void SqRingBuffer<uint16_t, 3>::_dump() {
    SQINFO("*** ring buffer dump.");
    SQINFO("in=%d out=%d", inIndex, outIndex);
    for (int i = 0; i < 3; ++i) {
        SQINFO("buffer[%d] = %x", i, memory[i]);
    }
}

template <typename T, int SIZE>
inline void SqRingBuffer<T, SIZE>::push(T value)
{
    assert(_allowOverflow || !full());
    if (full()) {
        pop();
    }
    memory[inIndex] = value;
    advance(inIndex);
    couldBeFull = true;
}

template <typename T, int SIZE>
inline T SqRingBuffer<T, SIZE>::pop()
{
    assert(!empty());
    T value = memory[outIndex];
    advance(outIndex);
    couldBeFull = false;
    return value;
}

template <typename T, int SIZE>
inline bool SqRingBuffer<T, SIZE>::full() const
{
    return (inIndex == outIndex) && couldBeFull;
}

template <typename T, int SIZE>
inline int SqRingBuffer<T, SIZE>::size() const
{
    //return (inIndex == outIndex) && !couldBeFull;
    if (empty()) {
        return 0;
    }
    if (full()) {
        return SIZE;
    }
    int x = inIndex - outIndex;
    if (x < 0) {
        x += SIZE;
    }
    return x;
}

template <typename T, int SIZE>
inline T SqRingBuffer<T, SIZE>::at(int x) const
{
    assert(size() > x);
    assert(x >= 0);
    
    int index = inIndex - 1;
    index -= x;
    if (index < 0) {
        index += SIZE;
    }
    return memory[index];
}

template <typename T, int SIZE>
inline bool SqRingBuffer<T, SIZE>::empty() const
{
    return (inIndex == outIndex) && !couldBeFull;
}

template <typename T, int SIZE>
inline void SqRingBuffer<T, SIZE>::advance(int &p)
{
    if (++p >= SIZE) p = 0;
}



