
#pragma once

#include "FreqMeasure.h"
#include "ShiftMath.h"

class ClockMult {
public:
    friend class TestClockMult;
    bool go(bool clockIn);
    void setMul(double m) {
        mul = m;
    }

private:
    FreqMeasure _freqMeasure;
    double mul = 1;
    ShiftMath::ClockWithSamples _currentTime;
    ShiftMath::ClockWithSamples _nextOutTime;

    void updateTargetTime();
};

inline bool ClockMult::go(bool clockIn) {
    _freqMeasure.onSample(clockIn);
    if (!_freqMeasure.freqValid()) {
        return false;         // ignore input until we are stable
    }
    if (clockIn) {
        _currentTime._clocks++;
        _currentTime._samples = 0;

    } else {
        _currentTime._samples++;
    }

    bool returnedClock = false;
    if (ShiftMath::exceedsOrEquals(_currentTime, _nextOutTime)) {
        returnedClock = true;
        // we need to  derive a new _nextOutTime
        updateTargetTime();
    }

    return returnedClock;
}

inline void ClockMult::updateTargetTime() {
    assert(false);
}