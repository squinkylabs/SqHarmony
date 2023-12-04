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

    /**
     * @brief retrieve by index
     * last one pushed == index 0.
     * one before that is index 1.
     */
    int at(int index) const;

    const int MAX_SIZE = 16;

private:
    int data[16];
    int inIndex = 0;
    int outIndex = 0;
#ifdef _DEBUG
    const bool _allowOverflow;
#endif
    int _size;

    void advance(int& index) const;
};

inline SqRingBuffer2::SqRingBuffer2(bool allowOverflow, int size) :
#ifdef _DEBUG
                                                                    _allowOverflow(allowOverflow),
#endif
                                                                    _size(size) {
}

inline bool SqRingBuffer2::empty() const {
    return inIndex == outIndex;
}

inline bool SqRingBuffer2::full() const {
    return size() == _size;
}

inline void SqRingBuffer2::push(int x) {
#ifdef _DEBUG
    assert((size() < _size) || _allowOverflow);
#endif

    if (size() >= _size) {
        pop();
    }
    assert(size() < _size);

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
        ret += (_size + 1);  // account for the extra data cell
    }
    return ret;
}

inline void SqRingBuffer2::advance(int& x) const {
    ++x;
    if (x > _size) {
        x = 0;
    }
}

inline void SqRingBuffer2::setSize(int x) {
    assert(x < MAX_SIZE);
    assert(x > 0);
    while (size() > x) {
        pop();
    }
    _size = x;
}

inline int SqRingBuffer2::at(int x) const {
    assert(size() > x);
    assert(x >= 0);

    int index = inIndex - 1;
    index -= x;
    if (index < 0) {
        index += (_size + 1);
    }
    return data[index];
}

class SqChordHistory {
public:
    SqChordHistory() : rb(true, 4) {
    }
    void onNewChord(int rank, int root) {
        const auto h = hash(rank, root);
        rb.push(h);
    }
    bool haveSeen(int rank, int root) const {
        const auto h = hash(rank, root);
        for (int i = 0; i < rb.size(); ++i) {
            if (rb.at(i) == h) {
                return true;
            }
        }
        return false;
    }
    void setSize(int x) {
        rb.setSize(x);
    }

private:
    SqRingBuffer2 rb;

    static int hash(int rank, int root) {
        assert(root < 8);
        assert(root >= 1);
        assert(rank >= 0);
        assert(rank < 1000);
        return (rank << 4) | root;
    }
};