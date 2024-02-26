#pragma once

#include <assert.h>

#include <cstdint>

#include "BitDelay.h"
#include "FreqMeasure2.h"
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
     * @param triggers is the leading edge of the clock, used for sync.
     * @param clock is the clock we are delaying - the input clock. Only used for setting output clock width.
     * @param delay is normalized to 1 == one clocking input period.
     * @param error is where errors are returned to the caller. nullptr is legal.
     * @return the delayed clock.
     */
    bool process(bool trigger, bool clock, float delay, Errors* error = nullptr);
    void setMaxDelaySamples(unsigned samples);

    bool freqValid() const;

     static int llv;  // log level

private:
    BitDelay _bitDelay;
    FreqMeasure2 _freqMeasure;
};

inline bool ClockShifter6::freqValid() const {
    return _freqMeasure.freqValid();
}
inline bool ClockShifter6::process(bool trigger, bool clock, float delay, Errors* error) {
    _freqMeasure.process(trigger, clock);
     if (!_freqMeasure.freqValid()) {
        if (llv > 0) SQINFO("leaving unstable");
        return false;
    }
    SQINFO("!! ClockShifter6::process is fake");
    return false;
}

inline void ClockShifter6::setMaxDelaySamples(unsigned samples) {
    SQINFO("!! ClockShifter6::setMaxDelaySamples is fake");
}
