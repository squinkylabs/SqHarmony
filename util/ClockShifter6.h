#pragma once

// #include <cmath>
#include <assert.h>
#include <tuple>

// #include "FreqMeasure2.h"
// #include "OneShotSampleTimer.h"

#include <cstdint>
#include <vector>

#include "SqLog.h"

class ClockShifter6 {
public:
    friend class TestX;
    enum class Errors {
        NoError,
        ExceededDelaySize,
        LostClocks
    };
    bool process(bool clock, float delay, Errors* error = nullptr);
    void setMaxDelaySamples(unsigned samples);

private:
    // delay memory as 32 bit unsigned, but accesses as a bit.
    std::vector<uint32_t> _delayMemory;

    size_t _maxDelaySize();
    unsigned _delayInSamples=0;
    unsigned _delayOutSamples=0;
    bool _getDelayOutput(float delay);
    void _insertDelayInput(bool data);

    std::tuple<unsigned, unsigned> _getIndexAndBit(unsigned bitIndex);
    static bool _extractBit(uint32_t word, unsigned bit);
    static uint32_t _packBit(uint32_t word, unsigned bit, bool value);
};

size_t ClockShifter6::_maxDelaySize() {
    return _delayMemory.size() * 32;
}

inline void ClockShifter6::_insertDelayInput(bool data) {
    const auto indexAndBit = ClockShifter6::_getIndexAndBit(_delayInSamples);
    assert(false);
}

inline std::tuple<unsigned, unsigned> ClockShifter6::_getIndexAndBit(unsigned bitIndex) {
    unsigned index = _delayInSamples / 32;
    unsigned bit = _delayInSamples - index;
    return std::make_tuple(index, bit);
}

inline bool ClockShifter6::_extractBit(unsigned word, unsigned bit) {
    const unsigned mask = 1 << bit;
    const unsigned _and = word & mask;
    return bool(_and);
}

inline  uint32_t ClockShifter6::_packBit(uint32_t word, unsigned bit, bool value) {
   // uint32_t uvalue = value;
    uint32_t mask = value << bit;
    uint32_t nonMask = ~ (1 << bit);
    auto cleared = word & nonMask;
    auto combined = cleared | mask;
    return combined;
}

inline bool ClockShifter6::process(bool clock, float delay, Errors* error) {
    SQINFO("in process, delay mem=%u", _delayMemory.size());
    if (error) {
        *error = Errors::NoError;

        if (delay > _maxDelaySize()) {
            *error = Errors::ExceededDelaySize;
            return false;
        }
    }

    const bool outputClock = _getDelayOutput(delay);
    _insertDelayInput(clock);
    return outputClock;
}

inline void ClockShifter6::setMaxDelaySamples(unsigned samples) {
    _delayMemory.resize((samples + 31) / 32);
}

inline  bool ClockShifter6::_getDelayOutput(float delay) {
    const auto indexAndBit =  _getIndexAndBit(_delayInSamples); 
    const unsigned index = std::get<0>(indexAndBit);
    const unsigned bit = std::get<1>(indexAndBit);
    unsigned x = _delayMemory.at(index);
    return _extractBit(x, bit);
}

