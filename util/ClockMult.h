
#pragma once

#include "FreqMeasure.h"
#include "ShiftMath.h"

class ClockMult {
public:
    friend class TestClockMult;
    bool run(bool clockIn);
    void setMul(double m) {
        _mul = m;
    }

private:
    bool _isFirstClock = true;
    FreqMeasure _freqMeasure;
    double _mul = 1;
    int _nextClockToOutput = 1;
    ShiftMath::ClockWithSamples _currentTime;
    ShiftMath::ClockWithSamples _nextOutTime;

    void updateTargetTime();
};

inline bool ClockMult::run(bool clockIn) {
    _freqMeasure.onSample(clockIn);
    if (!_freqMeasure.freqValid()) {
        return false;  // ignore input until we are stable
    }
    if (_isFirstClock) {
        _nextClockToOutput = 1;
        updateTargetTime();
        _isFirstClock = false;
        return true;
    }

    if (clockIn) {
        _currentTime._clocks++;
        _currentTime._samples = 0;

    } else {
        // TODO: is current time even used?
        _currentTime._samples++;
    }

    bool returnedClock = false;

    // note - this means we could emit two clocks "at the same time",
    // especially at start.
    if (ShiftMath::exceedsOrEquals(_currentTime, _nextOutTime)) {
        returnedClock = true;
        // we need to  derive a new _nextOutTime
        updateTargetTime();
        ++_nextClockToOutput;
    }

    return returnedClock;
}

inline void ClockMult::updateTargetTime() {
    assert(_mul == 1);      // imp _mul
    const double clockPeriod = double(int(_freqMeasure.getPeriod()));
    const double outputClockTime = _nextClockToOutput * clockPeriod;
    const auto outputClockWithPhase = ShiftMath::ClockWithPhase(outputClockTime);
    _nextOutTime = ShiftMath::convert(outputClockWithPhase, int(_freqMeasure.getPeriod()));
}