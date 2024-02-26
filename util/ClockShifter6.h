#pragma once

#include <assert.h>

// #include <tuple>

// #include "FreqMeasure2.h"
// #include "OneShotSampleTimer.h"

#include <cstdint>
// #include <vector>

#include "BitDelay.h"
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
    BitDelay _bitDelay;
};

inline bool ClockShifter6::process(bool clock, float delay, Errors* error) {
    return false;
}

inline void ClockShifter6::setMaxDelaySamples(unsigned samples) {
}
