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
        NoError = static_cast<int>(BitDelay::Errors::NoError),
        ExceededDelaySize = static_cast<int>(BitDelay::Errors::ExceededDelaySize),
        LostClocks = static_cast<int>(BitDelay::Errors::LostClocks)
    };
    ClockShifter6();
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
    unsigned getPeriod() const;

     static int llv;  // log level

private:
    BitDelay _bitDelay;
    FreqMeasure2 _freqMeasure;
};

inline  ClockShifter6:: ClockShifter6() {
    _bitDelay.setMaxDelaySamples(100000);
}

inline bool ClockShifter6::freqValid() const {
    return _freqMeasure.freqValid();
}

inline unsigned ClockShifter6::getPeriod() const {
    return _freqMeasure.getPeriod();
}

inline bool ClockShifter6::process(bool trigger, bool clock, float delay, Errors* error) {
    _freqMeasure.process(trigger, clock);
     if (!_freqMeasure.freqValid()) {
        if (llv > 0) SQINFO("leaving unstable");
        return false;
    }
    SQINFO("!! ClockShifter6::process is fake");
    const unsigned delayAbsolute = unsigned( float(_freqMeasure.getPeriod()) * delay);
    return _bitDelay.process(trigger, delayAbsolute);
}

inline void ClockShifter6::setMaxDelaySamples(unsigned samples) {
    _bitDelay.setMaxDelaySamples(samples);
}
