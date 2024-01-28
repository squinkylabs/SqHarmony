#pragma once

#include "FreqMeasure2.h"
#include "OneShotSampleTimer.h"

class ClockShifter5 {
public:
    friend class TestX;
    bool process(bool trigger, bool clock, float shift);
    bool freqValid() const;

private:
    FreqMeasure2 _freqMeasure;
    OneShotSampleTimer _clockWidthGenerator;

    /**
     * @brief reset on trigger, then counts up on each process count.
     */
    int _phaseAccumulator = 0;

    /**
     * @brief true if a clock has been output in the current cycle.
     */
    bool _haveClocked = false;
    float _shift;
};

inline bool ClockShifter5::process(bool trigger, bool clock, float shift) {
    if (shift != 0) {
        SQINFO("Ignoring shift");
    }
    if (trigger != false || (clock != false)) {
        SQINFO("Ignoring clock");
    }

    const bool freqWasValid = _freqMeasure.freqValid();
    _freqMeasure.process(trigger, clock);
    if (!_freqMeasure.freqValid()) {
        // SQINFO("input freq not settled, not processing");
        return false;
    }

    bool ret = false;
    if (trigger) {
        // This clock for processing on the first clock
        _phaseAccumulator = 0;
        _haveClocked = false;
    }
    const float targetClockf = float(_freqMeasure.getPeriod()) * _shift;
    const int targetClock = int(targetClockf);

    _phaseAccumulator++;
    if (!_haveClocked && (_phaseAccumulator >= targetClock)) {
        SQINFO("crossed target");
        ret = true;
        _haveClocked = true;
    }

    return ret;
}

inline bool ClockShifter5::freqValid() const {
    return _freqMeasure.freqValid();
    ;
}
