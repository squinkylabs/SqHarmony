#pragma once

#include "FreqMeasure.h"
#include "GateTrigger.h"
#include "ResettablePhaseAccumulator.h"

class ClockShifter {
public:
    float run(float input);

    /**
     * @brief Set the shift phase.
     *
     * @param amount is between 0 and 1. 0 is no shift, 1 is a whole period
     */
    void setShift(float amount) {
        _shiftAmount = amount;
    }

private:
    GateTrigger _inputConditioning;
    FreqMeasure _freqMeasure;
    ResettablePhaseAccumulator _acc;
    float _shiftAmount = 0;
};

inline float ClockShifter::run(float input) {
    // SQINFO("clock shifter run %f", input);
    _inputConditioning.go(input);
    bool gotTrigger = false;

    _freqMeasure.onSample(_inputConditioning.trigger());
    if (_inputConditioning.trigger()) {
        // SQINFO("got input trigger input");
        gotTrigger = _freqMeasure.freqValid();
    }

    bool shouldOutputTrigger = false;
    if (gotTrigger) {
        // SQINFO("now processing stable trigger");
        double newPhase = 1 - _shiftAmount;
        const double newFreq = .1;  // fake

        newPhase -= newFreq / 2;    // rounding, so we aren't always "exactly on" the phase.
        _acc.reset(newPhase, newFreq);

        // was here before
        //  shouldOutputTrigger = _acc.tick();
    }
    shouldOutputTrigger = _acc.tick();

    const auto ret = shouldOutputTrigger ? cGateOutHi : cGateOutLow;
    // SQINFO("sq will return %f from run", ret);
    return ret;
}