#pragma once
#include <assert.h>
#include <stdint.h>

#include "SqLog.h"

/**
 * A simple ring buffer.
 * Template arguments are for numeric type stored, and for size. Optional
 * third argument is NOINIT. If this is true, will skip the initialization of T and
 * assume that T is an object with a constructor.
 *
 * Not thread safe.
 * Guaranteed to be non-blocking. Adding or removing items will never
 * allocate or free memory.
 * Objects in RingBuffer are not owned by RingBuffer - they will not be destroyed.
 */
template <typename T, int SIZE, bool NOINIT = false>
class SqRingBuffer {
public:
    SqRingBuffer(bool allowOverflow) : _allowOverflow(allowOverflow) {
#if (NOINIT == true)
        for (int i = 0; i < SIZE; ++i) {
            memory[i] = 0;
        }
#endif
    }

    int size() const;
    void push(T);
    T pop();
    T peek() const;
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
    bool couldBeFull = false;  // full and empty are ambiguous, both are in--out
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

template <typename T, int SIZE, bool NOINIT>
inline void SqRingBuffer<T, SIZE, NOINIT>::push(T value) {
    assert(_allowOverflow || !full());
    if (full()) {
        pop();
    }
    memory[inIndex] = value;
    advance(inIndex);
    couldBeFull = true;
}

template <typename T, int SIZE, bool NOINIT>
inline T SqRingBuffer<T, SIZE, NOINIT>::pop() {
    assert(!empty());
    T value = memory[outIndex];
    advance(outIndex);
    couldBeFull = false;
    return value;
}

template <typename T, int SIZE, bool NOINIT>
inline T SqRingBuffer<T, SIZE, NOINIT>::peek() const {
    assert(!empty());
    T value = memory[outIndex];
    return value;
}

template <typename T, int SIZE, bool NOINIT>
inline bool SqRingBuffer<T, SIZE, NOINIT>::full() const {
    return (inIndex == outIndex) && couldBeFull;
}

template <typename T, int SIZE, bool NOINIT>
inline int SqRingBuffer<T, SIZE, NOINIT>::size() const {
    // return (inIndex == outIndex) && !couldBeFull;
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

template <typename T, int SIZE, bool NOINIT>
inline T SqRingBuffer<T, SIZE, NOINIT>::at(int x) const {
    assert(size() > x);
    assert(x >= 0);

    int index = inIndex - 1;
    index -= x;
    if (index < 0) {
        index += SIZE;
    }
    return memory[index];
}

template <typename T, int SIZE, bool NOINIT>
inline bool SqRingBuffer<T, SIZE, NOINIT>::empty() const {
    return (inIndex == outIndex) && !couldBeFull;
}

template <typename T, int SIZE, bool NOINIT>
inline void SqRingBuffer<T, SIZE, NOINIT>::advance(int &p) {
    if (++p >= SIZE) p = 0;
}
