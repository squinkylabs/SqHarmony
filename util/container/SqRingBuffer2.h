#pragma once
#include <assert.h>
#include <stdint.h>

#include "SqLog.h"

/**
 * @brief a non-atomic ring buffer, size can change on the fly
 * 
 * note: we always have one extra location, so in==out always means empty;
 */
class SqRingBuffer2 {
public:
    SqRingBuffer2(bool allowOverflow, int size);
    int size() const;
    void push(int);
    int pop();
    bool full() const;
    bool empty() const;
    void setSize(int);

    const int MAX_SIZE = 16;

private:
    int data[16];
    int inIndex = 0;
    int outIndex = 0;
    const bool _allowOverflow;
    int _size;

    void advance(int& index) const;
};

inline SqRingBuffer2::SqRingBuffer2(bool allowOverflow, int size) : 
    _allowOverflow(allowOverflow), _size(size) {
}

inline bool SqRingBuffer2::empty() const {
    return inIndex == outIndex;
}

inline bool SqRingBuffer2::full() const {
    return size() == _size;
}

inline void SqRingBuffer2::push(int x) {
    assert((size() < _size) || _allowOverflow);

    data[inIndex] = x;
    advance(inIndex);
}

inline int SqRingBuffer2::pop() {
    assert(!empty());
  
    int ret = data[outIndex];
    advance(outIndex);
    return ret;
}

inline int SqRingBuffer2::size() const {
    int ret = inIndex - outIndex;
    if (ret < 0) {
        ret += (_size + 1);         // account for the extra data cell
    }
    return ret;
}

inline void SqRingBuffer2::advance(int &x) const {
    ++x;
    if (x > _size) {
        x = 0;
    }
}

inline void SqRingBuffer2::setSize(int x) {
    assert(x < MAX_SIZE);
    while (size() > x) {
        pop();
    }
    _size = x;
}