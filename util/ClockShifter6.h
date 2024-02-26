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
        NoError = BitDelay::Errors::NoError,
        ExceededDelaySize = BitDelay::Errors::ExceededDelaySize,
        LostClocks = BitDelay::Errors::LostClocks
    };
    /**
     * @brief 
     * 
     * @param clock is the clock we are delaying - the input clock.
     * @param delay is normalized to 1 == one clocking input period.
     * @param error is where errors are returned to the caller. nullptr is legal.
     * @return the delayed clock.
     */
    bool process(bool clock, float delay, unsigned masterClockPeriod, Errors* error);
    void setMaxDelaySamples(unsigned samples);

private:
    BitDelay _bitDelay;
};

inline bool ClockShifter6::process(bool clock, float delay, unsigned masterClockPeriod, Errors* error) {
    SQINFO("!! ClockShifter6::process is fake");
    return false;
}

inline void ClockShifter6::setMaxDelaySamples(unsigned samples) {
    SQINFO("!! ClockShifter6::setMaxDelaySamples is fake");
}
