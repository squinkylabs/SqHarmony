#pragma once

#include "asserts.h"
#include "FreqMeasure.h"
#include "GateTrigger.h"
#include "OneShot.h"
#include "ResettablePhaseAccumulator.h"

class ClockShifter1 {
public:
    ClockShifter1();
    float run(float input, float secondsElapsed);

    /**
     * @brief Set the shift phase.
     *
     * @param amount is between 0 and 1. 0 is no shift, 1 is a whole period
     */
    void setShift(float amount) {
        _shiftAmount = amount;
        assertGE(amount, 0);
        assertLE(amount, 1);
    }

private:
    GateTrigger _inputConditioning;
    FreqMeasure _freqMeasure;
    ResettablePhaseAccumulator _acc;
    OneShot _oneShot;
    float _shiftAmount = 0;
};

inline ClockShifter1::ClockShifter1() {
    _oneShot.setDelayMs(2);
}

inline float ClockShifter1::run(float input, float secondsElapsed) {
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

        const double newFreq = 1.0 / double(_freqMeasure.getPeriod());
        const double newPhase = 1 - (_shiftAmount + (newFreq / 2));  // rounding, so we aren't always "exactly on" the phase.
        _acc.reset(newPhase, newFreq);
    }
    shouldOutputTrigger = _acc.tick();
    if (shouldOutputTrigger) {
        _oneShot.set();
    } else {
        _oneShot.step(secondsElapsed);
    }

    const auto ret = _oneShot.hasFired() ? cGateOutLow : cGateOutHi;
    return ret;
}