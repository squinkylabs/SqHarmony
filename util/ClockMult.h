
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
    double mul=1;


    ShiftMath::ClockWithSamples _currentTime;
};

inline bool ClockMult::go(bool clockIn) {
    if (clockIn) {
        _currentTime._clocks++;
        _currentTime._samples = 0;
        
    } else {
        _currentTime._samples++;
    }
    _freqMeasure.onSample(clockIn);
    return false;
}