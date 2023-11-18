#pragma once

#include "FreqMeasure2.h"
#include "OneShotSampleTimer.h"

class ClockShifter4 {
public:
    void setShift(float);
    bool process(bool trigger, bool clock);
    bool freqValid() const;

private:
    /**
     * @brief reset on trigger, then counts up on each process count
     *
     */
    int _phaseAccumulator = 0;
    FreqMeasure2 _freqMeasure;
    OneShotSampleTimer _clockWidthGenerator;
    float _shift = 0;
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
    if (trigger) {
        // ret = true;
        _phaseAccumulator = 0;
    } else {
        _phaseAccumulator++;
    }

    // Fire the clock when phase acc crosses the shift point.
    const float targetClockf = float(_freqMeasure.getPeriod()) * _shift;
    const int targetClock = int(targetClockf);
    if ((_phaseAccumulator >= targetClock) && (_phaseAccumulator < (targetClock + 1))) {
        ret = true;
        _clockWidthGenerator.arm(_freqMeasure.getHighDuration());
    } else {
        _clockWidthGenerator.run();
        ret = _clockWidthGenerator.get();
    }
    return ret;
}

inline bool ClockShifter4::freqValid() const {
    return _freqMeasure.freqValid();
}