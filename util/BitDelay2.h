
#pragma once

#include <cstdint>

#include "SqLog.h"
#include "SqRingBuffer.h"

class BitDelay2 {
public:
    friend class TestX;

    BitDelay2();
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
    SqRingBuffer<unsigned, 200> _ringBuffer;

    bool _getDelay(unsigned samples) const;
    void _writeToRingBuffer();

    unsigned _sizeSamplesForTest = 0;
};

inline BitDelay2::BitDelay2() : _ringBuffer(true) {
}

inline void BitDelay2::_writeToRingBuffer() {
    if (_currentCount == 0) {
        SQINFO("nothing to write");
        return;
    }
    _ringBuffer.push(_currentCount);
}

inline bool BitDelay2::process(bool inputClock, unsigned delay, Errors* error) {
    if (error != nullptr) {
        *error = Errors::NoError;
    }
    if (inputClock == _currentValue) {
        ++_currentCount;
    } else {
      //  assert(false);  // need to write to buffer.
        _writeToRingBuffer();
        _currentValue = inputClock;
        _currentCount = 1;
    }
    const bool output = _getDelay(delay);
    return output;
}

 inline bool BitDelay2::_getDelay(unsigned samples) const {
    if (samples <= _currentCount) {
        return _currentValue;
    }
    SQINFO("getDelay2 fake");
    return false;
 }

inline void BitDelay2::setMaxDelaySamples(unsigned samples) {
    _sizeSamplesForTest = samples;
}

inline uint32_t BitDelay2::getMaxDelaySize() const {
    return _sizeSamplesForTest;
}