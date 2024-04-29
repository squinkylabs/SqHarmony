
#pragma once

#include <cstdint>

#include "SqLog.h"
#include "SqRingBuffer.h"

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
    bool _currentValue = false;
    unsigned _currentCount = 0;
 //   SqRingBuffer<unsigned, 200> _ringBuffer;

    static const int _delaySize = 200;
    unsigned _delayMemory[_delaySize];
    unsigned* _delayWritePointer = _delayMemory;
    unsigned* _delayReadPointer = _delayMemory;
    unsigned _validDelayEntries = 0;
    void _writeToRingBuffer();
    void _advance(unsigned** p);
    void _decrement(unsigned** p);


    bool _getDelay(unsigned samples);
  

    unsigned _sizeSamplesForTest = 0;
};

inline void BitDelay2::_advance(unsigned** p) {
    SQINFO("_advance");

    (*p)++;
    if (*p >= (_delayMemory + _delaySize)) {
        *p = _delayMemory;
        SQINFO("advance wrap");
    }
}

inline void BitDelay2::_decrement(unsigned** p) {
    SQINFO("_decrement");

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
        _advance(&_delayReadPointer);
        _validDelayEntries--;
    }

    // Write the new data and advance.
    *_delayWritePointer = _currentCount;
    _advance(&_delayWritePointer);
    _validDelayEntries++;

}

inline bool BitDelay2::process(bool inputClock, unsigned delay, Errors* error) {
    if (error != nullptr) {
        *error = Errors::NoError;
    }
    if (inputClock == _currentValue) {
        ++_currentCount;
    } else {
      //  assert(false);  // need to write to buffer.
        SQINFO("write to buffer because input=%d, cur=%d", inputClock, _currentValue);
        _writeToRingBuffer();
        _currentValue = inputClock;
        _currentCount = 1;
    }
    const bool output = _getDelay(delay);
    return output;
}

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

inline void BitDelay2::setMaxDelaySamples(unsigned samples) {
    _sizeSamplesForTest = samples;
}

inline uint32_t BitDelay2::getMaxDelaySize() const {
    return _sizeSamplesForTest;
}