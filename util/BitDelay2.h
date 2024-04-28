
#pragma once

#include <cstdint>

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
    unsigned _sizeSamplesForTest = 0;
};

inline bool BitDelay2::process(bool InputClock, unsigned delay, Errors* error) {
    if (error != nullptr) {
        *error = Errors::NoError;
    }
    return true;
}

inline void BitDelay2::setMaxDelaySamples(unsigned samples) {
    _sizeSamplesForTest = samples;
}

inline uint32_t BitDelay2::getMaxDelaySize() const {
    return _sizeSamplesForTest;
}