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
        gotTrigger = true;
        // If we saw a new trigger
        const auto x = _freqMeasure.getPeriod();
        SQINFO("measure period = %lld", x);
     
        
        const double newPhase = 0;      // for now, no shift
        const double newFreq = .1;      //fake
        _acc.reset(newPhase, newFreq);
    }
    _acc.tick();
    _freqMeasure.onSample(gotTrigger);

  //  const auto ret = _acc.getPhase() < .5 ? cGateOutLow : cGateOutHi;
    const float ret = 0;
    SQFATAL("no ret value");
    SQINFO("sq will return %f from run", ret);
    return ret;
}