#pragma once

#include <cstdint>
#include <tuple>
#include <vector>

#include <assert.h>
#include "SqLog.h"

class BitDelay {
public:
    friend class TestX;
    enum class Errors {
        NoError,
        ExceededDelaySize,
        LostClocks
    };
    bool process(bool InputClock, float delay, Errors* error = nullptr);
    void setMaxDelaySamples(unsigned samples);
    size_t getMaxDelaySize() const;
private:
    // delay memory as 32 bit unsigned, but accesses as a bit.
    std::vector<uint32_t> _delayMemory;

    // These are the virtual buffer pointers. Units are bits.
    unsigned _delayInSamples = 0;
  //  unsigned _delayOutSamples = 0;

   
    bool _getDelayOutput(float delay);
    void _insertDelayInput(bool data);

    std::tuple<unsigned, unsigned> _getIndexAndBit(unsigned bitIndex);
    static bool _extractBit(uint32_t word, unsigned bit);
    static uint32_t _packBit(uint32_t word, unsigned bit, bool value);
    void _nextDelayPointer(uint32_t& ptr);
    void _prevDelayPointer(uint32_t& ptr);
};

inline size_t BitDelay::getMaxDelaySize() const {
    return _delayMemory.size() * 32;
}

inline void BitDelay::_nextDelayPointer(uint32_t& ptr) {
    ++ptr;
    if (ptr >= getMaxDelaySize()) {
        ptr = 0;
    }
}

inline void BitDelay::_prevDelayPointer(uint32_t& ptr) {
    --ptr;
}

inline void BitDelay::_insertDelayInput(bool data) {
    const auto x = _delayMemory[0];
    const auto indexAndBit = _getIndexAndBit(_delayInSamples);
    const unsigned index = std::get<0>(indexAndBit);
    const unsigned bit = std::get<1>(indexAndBit);
    assert(index < _delayMemory.size());

    uint32_t word = _delayMemory.at(index);
    word = _packBit(word, bit, data);
    _delayMemory.at(index) = word;
    const auto y = _delayMemory[0];

   // _delayInSamples++;      // really needs wrapping logic here.
    _nextDelayPointer(_delayInSamples);
}

inline std::tuple<unsigned, unsigned> BitDelay::_getIndexAndBit(unsigned bitIndex) {
    unsigned index = _delayInSamples / 32;
    unsigned bit = _delayInSamples - index;
    return std::make_tuple(index, bit);
}

inline bool BitDelay::_extractBit(unsigned word, unsigned bit) {
    const unsigned mask = 1 << bit;
    const unsigned _and = word & mask;
    return bool(_and);
}

inline uint32_t BitDelay::_packBit(uint32_t word, unsigned bit, bool value) {
    // uint32_t uvalue = value;
    uint32_t mask = value << bit;
    uint32_t nonMask = ~(1 << bit);
    auto cleared = word & nonMask;
    auto combined = cleared | mask;
    return combined;
}

inline bool BitDelay::process(bool clock, float delay, Errors* error) {
    SQINFO("in process, delay mem=%u", _delayMemory.size());
    if (error) {
        *error = Errors::NoError;

        if (delay > getMaxDelaySize()) {
            *error = Errors::ExceededDelaySize;
            return false;
        }
    }

    const bool outputClock = _getDelayOutput(delay);
    _insertDelayInput(clock);
    return outputClock;
}

inline void BitDelay::setMaxDelaySamples(unsigned samples) {
    _delayMemory.resize(1 + samples / 32);
}

inline bool BitDelay::_getDelayOutput(float delay) {
    const auto indexAndBit = _getIndexAndBit(_delayInSamples);
    const unsigned index = std::get<0>(indexAndBit);
    const unsigned bit = std::get<1>(indexAndBit);
    unsigned x = _delayMemory.at(index);
    return _extractBit(x, bit);
}
