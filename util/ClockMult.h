
#pragma once

#include "FreqMeasure.h"
#include "ShiftMath.h"

class ClockMult {
public:
    friend class TestClockMult;
    bool run(bool clockIn);
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

inline bool ClockMult::run(bool clockIn) {
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
    SQINFO("utt, cur t: %s\n", _currentTime.toString().c_str());
    SQINFO("utt, next: %s\n", _nextOutTime.toString().c_str());

    assert(_currentTime._samples == 0);
    const double clockFreq = 1.0 / double(int(_freqMeasure.getPeriod()));
    const int currentClock = _currentTime._clocks;
    const double outputClock = currentClock * clockFreq;
    const auto outputClockWithPhase = ShiftMath::ClockWithPhase(outputClock);
    _nextOutTime = ShiftMath::convert(outputClockWithPhase, int(_freqMeasure.getPeriod()));
    

    //assert(_currentTime != _nextOutTime);
    assert(false);
}