
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

    // SQINFO("run, ckin=%d", clockIn);
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
        // SQINFO("ex|eq ci=%d", clockIn);
        returnedClock = true;
        // we need to  derive a new _nextOutTime
        updateTargetTime();
        ++_nextClockToOutput;
    }

    return returnedClock;
}

inline void ClockMult::updateTargetTime() {
   

    // new way
    // step 1 : number of samples until next client clock.
    // Note: this assumes mult == 1
    const int period = int(_freqMeasure.getPeriod());
    const int sampleTillNextClock = period / int(_mul);


    //assert(_mul == 1);      // imp _mul
    // step 2 : x = currentTime (samples) + samplesUntilNextClientClock
    auto x = _currentTime;

    x.addDelta(sampleTillNextClock, period);
    // step 3 : next target = x;
    _nextOutTime = x;
    // SQINFO("uptt mul=%f not=%d,%d stnc=%d curt=%d, %d", _mul, _nextOutTime._clocks, _nextOutTime._samples, sampleTillNextClock, _currentTime._clocks, _currentTime._samples);
}