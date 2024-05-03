#pragma once

#include <assert.h>

#include <cstdint>
#include <tuple>
#include <vector>

#include "SqLog.h"
extern int logLevel;

class BitDelay {
public:
    friend class TestX;
    enum class Errors {
        NoError,
        ExceededDelaySize,
        LostClocks
    };
    bool process(bool InputClock, unsigned delay, Errors* error = nullptr);
    void setMaxDelaySamples(unsigned samples);
    uint32_t getMaxDelaySize() const;

private:
    // Delay memory as 32 bit unsigned, but accesses as a bit.
    std::vector<uint32_t> _delayMemory;

    // This is the virtual buffer pointer. Units are bits.
    // Rename to "_currentLocation"?
    unsigned _currentLocation = 0;

    /**
     * get the data at the current location (_delayInSamples), and offset
     * by delay. Will not change the current location.
     */
    bool _getDelayOutput(unsigned delay, Errors* err = nullptr);

    /**
     * Set the bit at the current location to data. Current location
     * is _delayInSamples.
     * This will not change the current location.
     */
    void _insertDelayInput(bool data);

    static std::tuple<unsigned, unsigned> _getIndexAndBit(unsigned bitIndex);
    static bool _extractBit(uint32_t word, unsigned bit);
    static uint32_t _packBit(uint32_t word, unsigned bit, bool value);
    void _nextDelayPointer(uint32_t& ptr);
    void _prevDelayPointer(uint32_t& ptr);
};

inline uint32_t BitDelay::getMaxDelaySize() const {
    return uint32_t(_delayMemory.size() * 32);
}

inline void BitDelay::_nextDelayPointer(uint32_t& ptr) {
    ++ptr;
    if (ptr >= getMaxDelaySize()) {
        ptr = 0;
    }
}

inline void BitDelay::_prevDelayPointer(uint32_t& ptr) {
    if (ptr > 0) {
        --ptr;
    } else {
        ptr = getMaxDelaySize() - 1;
    }
}

inline void BitDelay::_insertDelayInput(bool data) {
    const auto indexAndBit = _getIndexAndBit(_currentLocation);
    const unsigned index = std::get<0>(indexAndBit);
    const unsigned bit = std::get<1>(indexAndBit);
    assert(index < _delayMemory.size());
    // SQINFO("_insertDelayInput index=%x", index);

    uint32_t word = _delayMemory.at(index);
 //  auto x = word;
    word = _packBit(word, bit, data);
    // SQINFO("_insertDelayInput was=%x, will be=%d", x, word);
    //assert(word == 0);
    _delayMemory.at(index) = word;
}

inline std::tuple<unsigned, unsigned> BitDelay::_getIndexAndBit(unsigned bitIndex) {
    unsigned index = bitIndex / 32;
    unsigned bit = bitIndex % 32;
    // SQINFO("bi=%d index=%d, bit=%d", bitIndex, index, bit);
    return std::make_tuple(index, bit);
}

inline bool BitDelay::_extractBit(unsigned word, unsigned bit) {
    const unsigned mask = 1 << bit;
    const unsigned _and = word & mask;
    return bool(_and);
}

inline uint32_t BitDelay::_packBit(uint32_t word, unsigned bit, bool value) {
    uint32_t mask = value << bit;
    uint32_t nonMask = ~(1 << bit);
    auto cleared = word & nonMask;
    auto combined = cleared | mask;
    return combined;
}

inline bool BitDelay::process(bool clock, unsigned delay, Errors* error) {
    if (logLevel) SQINFO("BitDelay::process, delay mem=%u. passedck=%d delay=%d this=%p", unsigned(_delayMemory.size()), clock, delay, this);
    if (error) {
        *error = Errors::NoError;

        if (delay > getMaxDelaySize()) {
            *error = Errors::ExceededDelaySize;
            return false;
        }
    }

    _insertDelayInput(clock);
    const bool outputClock = _getDelayOutput(delay);

    // We have added one, so move up the buffer pointer.
    _nextDelayPointer(_currentLocation);
    // SQINFO("BitDelay::process will return %d", outputClock);
    return outputClock;
}

inline void BitDelay::setMaxDelaySamples(unsigned samples) {
    size_t size = 1 + samples / 32;
    // SQINFO("setMaxDelaySamples %d -> size %d", samples, size);
    _delayMemory.resize(size, 0);
}

inline bool BitDelay::_getDelayOutput(unsigned delayOffset, Errors* err) {
    // SQNFO("get delay output %d", delayOffset);
    if (delayOffset >= getMaxDelaySize()) {
        if (err) {
            *err = Errors::ExceededDelaySize;
        }
        return false;
    }

    // Off by one. current loc was already incremented.
   // int combinedLoc = _currentLocation - (delayOffset + 1);
    int combinedLoc = _currentLocation + delayOffset;
    combinedLoc -= 1;

    // SQINFO("combined loc = %d", combinedLoc);
    if (combinedLoc < 0) {
        //  SQINFO("output delay mem size = %d, in bits %d", _delayMemory.size(), _delayMemory.size() * 32 );
        //  SQINFO(" ratio = %d", getMaxDelaySize() / 32);
        combinedLoc += (getMaxDelaySize() - 1);
    }
    //  SQINFO("final combined loc = %d", combinedLoc);
    const auto indexAndBit = _getIndexAndBit(combinedLoc);
    const unsigned index = std::get<0>(indexAndBit);
    const unsigned bit = std::get<1>(indexAndBit);
    unsigned x = _delayMemory.at(index);
    // SQINFO("got delay mem = %x", x);
    return _extractBit(x, bit);
}
