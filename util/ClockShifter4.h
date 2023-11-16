#pragma once

#include "FreqMeasure2.h"

class ClockShifter4 {
public:
    void setShift(float);
    bool process(bool trigger, bool clock);
    bool freqValid() const;

private:
    int _clocksSinceReset = 0;

    FreqMeasure2 _freqMeasure;
    float _shift = 0;
    bool _lastClock = false;
};

inline void ClockShifter4::setShift(float x) {
    _shift = x;
}

inline bool ClockShifter4::process(bool trigger, bool clock) {
    _freqMeasure.process(trigger, clock);
    if (!_freqMeasure.freqValid()) {
        return false;
    }

    bool ret = false;

    // if it's the edge of a new clock, re-sync
    if (clock && !_lastClock) {
        ret = true;
        _clocksSinceReset = 0;
    }
    _clocksSinceReset++;
    
    const float targetClockf = float(_freqMeasure.getPeriod()) * _shift;
    const int targetClock = int(targetClockf);
    if (targetClock <= _clocksSinceReset) {
        _clocksSinceReset = 0;
        ret = true;
    }
    return ret;
}

inline bool ClockShifter4::freqValid() const {
    return _freqMeasure.freqValid();
}