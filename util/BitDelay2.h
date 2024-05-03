
#pragma once

#include <cstdint>

#include "SqLog.h"
#include "SqRingBuffer.h"

/* OLD
 * Some notes on how it should work. First of all, imagine that it's not a bit delay, but a regular
 * multi-bit delay (it will make it easier to understand).
 * put into it a sequence of "stuff": a, c, c, q.....
 * now, if it is set for a delay of 'n' we should get out: a(t=n), b(t=n+1), c(t=n+2), q(t> n+2)
 * translating to our API:
 *  delay(a, 0);
 *  delay(b, 0);
 *  delay(c, 0);
 *  delay(q, 1) == a
 *  delay(q, 2 +1) == b
 *  delay(q, 3 + 1 + 1) == c
 *  delay(q, 4 + 1 + 1 + 1) == q
 */

/*
 * Some notes on how it should work. First of all, imagine that it's not a bit delay, but a regular
 * multi-bit delay (it will make it easier to understand).
 * put into it a sequence of "stuff": a, b, c, q.....
 * now, if it is set for a delay of 'n' we should get out: a(t=n), b(t=n+1), c(t=n+2), q(t> n+2)
 * translating to our API:
 *  delay(a, 0);
 *  delay(b, 0);
 *  delay(c, 0);
 * then next:
 *  delay(q, 1) === c
 *  delay(q, 2) === b
 *  delay(3, 3) === a;
 * then next:
 *  delay(q, 2) === c;
 *  delay(q, 3) === b;
 *  delay(q, 4) === a;
 */
class BitDelay2 {
public:
    friend class TestX;

    enum class Errors {
        NoError,
        ExceededDelaySize,
        LostClocks
    };
    bool process(bool InputClock, unsigned delay, Errors* error = nullptr);

    // These are legacy functions. Don't use them.
    void setMaxDelaySamples(unsigned samples);
    uint32_t getMaxDelaySize() const;

private:
    class BitPacket {
    public:
        int count = 0;
        bool value = 0;
    };

    bool _currentValue = false;
    unsigned _currentCount = 0;

    static const int _delaySize = 200;
    BitPacket _delayMemory[_delaySize];

    // The pointer is advanced as new data is written.
    // That is to say new input goes at a higher address, until it wraps.
    BitPacket* _delayPointer = _delayMemory;
    unsigned _validDelayEntries = 0;
    void _writeToRingBuffer();
    void _advance();
    void _decrement();
    void _decrement2(BitPacket**);
    bool _getDelay(unsigned samples, bool currentInput);
    unsigned _sizeSamplesForTest = 0;
};

inline void BitDelay2::_advance() {
    SQINFO("_advance");

    _delayPointer++;
    if (_delayPointer >= (_delayMemory + _delaySize)) {
        _delayPointer = _delayMemory;
        SQINFO("advance wrap");
    }
}
inline void BitDelay2::_decrement() {
    SQINFO("_decrement");
    _decrement2(&_delayPointer);
}
inline void BitDelay2::_decrement2(BitPacket** p) {
    SQINFO("_decrement2");

    (*p)--;
    if (*p < _delayMemory) {
        *p = _delayMemory + (_delaySize - 1);
        SQINFO("decrement wrap");
    }
}

inline void BitDelay2::_writeToRingBuffer() {
    if (_currentCount == 0) {
        SQINFO("nothing to write");
        return;
    }

    // if memory if full, dump the old stuff
    if (_validDelayEntries >= (_delaySize - 1)) {
        SQINFO("write now writing something");
        _advance();
        _validDelayEntries--;
    }

    // Write the new data and advance.
    BitPacket& packet = *_delayPointer;
    packet.value = _currentValue;
    packet.count = _currentCount;
    //  *_delayPointer = _currentCount;
    _advance();
    _validDelayEntries++;
}

inline bool BitDelay2::process(bool inputClock, unsigned delay, Errors* error) {
    if (error != nullptr) {
        *error = Errors::NoError;
    }

    // First fetch the output.
    const bool output = _getDelay(delay, inputClock);

    // Then save away the input.
    if (inputClock == _currentValue) {
        ++_currentCount;
    } else {
        //  assert(false);  // need to write to buffer.
        SQINFO("write to buffer because input=%d, cur=%d", inputClock, _currentValue);
        _writeToRingBuffer();
        _currentValue = inputClock;
        _currentCount = 1;
    }

    return output;
}

inline bool BitDelay2::_getDelay(unsigned delaySamples, bool input) {
    // zero delay is a special case.
    if (delaySamples == 0) {
        return input;
    }

    // we don't need this....
//    unsigned totalDelay = _currentCount;
    unsigned totalDelay = 0;
    unsigned blocksToParse = _validDelayEntries;

    // we used to pick off the current stuff first - that's wrong

    // as <=, trying something new
    // < makes one test pass, but I think that's a bad test???
    // ok, all the stuff in test passes with <=, but trying to get PartTwo to work,
    // so try < . well, part 2 worked more, but part 1 stopped. let's go back.
    if (delaySamples <= this->_currentCount) {
         return _currentValue;
     }

    // and this..... only one of them. and maybe and off by one error
     delaySamples -= _currentCount;

    BitPacket* block = _delayPointer;
    if (blocksToParse) {
        _decrement2(&block);
    }

    while (blocksToParse) {
        // Get data in current block
        totalDelay += block->count;
        if (totalDelay >= delaySamples) {
            // this is the clock for us.
            return block->value;
        }
        blocksToParse--;
        _decrement2(&block);
    }

    // Now look in current values
    delaySamples -= totalDelay;

    // was <= but By2 was failing at the start.
    if (delaySamples < this->_currentCount) {
        return _currentValue;
    }

    SQINFO("fall off end of delay");
    return false;
}

#if 0  // old way
 inline bool BitDelay2::_getDelay(unsigned samples) {
    if (samples < _currentCount) {
        return _currentValue;
    }

    unsigned totalDelay = _currentCount - 1;

    unsigned blocksToParse = _validDelayEntries;
   //
   // assert(blocksToParse > 0);
    
   //  unsigned* block = _decrement(_delayWritePointer);
    unsigned* block = _delayWritePointer;
    if (blocksToParse) {
        _decrement(&block);
    }
    bool value = !_currentValue;     // because we know it's in a different block..
    while (blocksToParse) {
        // Get data in current block
        totalDelay += *block;
        if (totalDelay >= samples) {
            // this is the clock for us.
            return value;
        }
        blocksToParse--;
        _decrement(&block);
        value = !value;
    }

    
    SQINFO("!! getDelay2 past end");
  //  assert(false);
    return false;
 }
#endif

inline void BitDelay2::setMaxDelaySamples(unsigned samples) {
    _sizeSamplesForTest = samples;
}

inline uint32_t BitDelay2::getMaxDelaySize() const {
    return _sizeSamplesForTest;
}