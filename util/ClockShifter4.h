#pragma once

#include "FreqMeasure.h"

class ClockShifter4 {
public:
    void setShift(float);
    bool run(bool input);
    bool freqValid() const;
private:

    FreqMeasure _freqMeasure;
};

inline void ClockShifter4::setShift(float) {
}

inline bool ClockShifter4::run(bool clock) {
    _freqMeasure.onSample(clock);
    return clock;
}

inline bool ClockShifter4::freqValid() const {
    return _freqMeasure.freqValid();
}