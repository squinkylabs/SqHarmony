#pragma once

// #include <cmath>
// #include <tuple>

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
};

size_t ClockShifter6::_maxDelaySize() {
    return _delayMemory.size() * 32;
}

inline bool ClockShifter6::process(bool clock, float delay, Errors* error) {
    SQINFO("in process, delay mem=%u", _delayMemory.size());
    if (error) {
        *error = Errors::NoError;

        if (delay > _maxDelaySize()) {
            *error = Errors::ExceededDelaySize;
        }
    }
    return false;
}

inline void ClockShifter6::setMaxDelaySamples(unsigned samples) {

    _delayMemory.resize((samples + 31) / 32);
}
