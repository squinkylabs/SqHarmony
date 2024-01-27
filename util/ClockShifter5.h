#pragma once

#include "FreqMeasure2.h"
#include "OneShotSampleTimer.h"

class ClockShifter5 {
public:
    bool process(bool trigger, bool clock, float shift);
    bool freqValid() const;
private:
};

inline
bool ClockShifter5::process(bool trigger, bool clock, float shift) {
    return false;
}

inline
bool ClockShifter5::freqValid() const {
    return false;
}
