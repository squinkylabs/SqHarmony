#pragma once

#include "GateTrigger.h"
#include "FreqMeasure.h"
#include "ResettablePhaseAccumulator.h"

class ClockShifter {
public:
    float run(float input);
private:
    GateTrigger _inputConditioning;
    FreqMeasure _freqMeasure;
    ResettablePhaseAccumulator _acc;
};

inline float ClockShifter::run(float input) {
    _inputConditioning.go(input);
    bool gotTrigger = false;

    if (_inputConditioning.trigger()) {
        SQINFO("got input trigger");
     //   gotTrigger = true;
        // If we saw a new trigger

        // We
        const auto measuredPeriod = _freqMeasure.getPeriod();
        SQINFO("measure period = %lld", measuredPeriod);
        gotTrigger = (measuredPeriod > 0);
    }

    bool shouldOutputTrigger = false;
    if (gotTrigger) {
        const double newPhase = 0;      // for now, no shift
        const double newFreq = .1;      //fake
        _acc.reset(newPhase, newFreq);
        shouldOutputTrigger = _acc.tick();
    }

    _freqMeasure.onSample(gotTrigger);

    const auto ret = shouldOutputTrigger ? cGateOutHi : cGateOutLow;
    SQINFO("sq will return %f from run", ret);
    return ret;
}