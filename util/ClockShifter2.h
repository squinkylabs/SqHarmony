#pragma once

#include "FreqMeasure.h"
#include "GateTrigger.h"
#include "OneShot.h"
#include "OneShotSampleTimer.h"
#include "ResettablePhaseAccumulator.h"
#include "asserts.h"

/**
 * @brief not currently used?
 *
 */
class ClockShifter2 {
public:
    ClockShifter2();
    float run(float input, float secondsElapsed);

    /**
     * @brief Set the shift phase.
     *
     * @param amount is between 0 and 1. 0 is no shift, 1 is a whole period
     */
    void setShift(float amount) {
        //  _shiftAmount = amount;
        //  assertGE(amount, 0);
        //  assertLE(amount, 1);
    }

private:
    OneShotSampleTimer _timer;
    GateTrigger _inputConditioning;
    FreqMeasure _freqMeasure;
    float _shiftAmount = 0;
    OneShot _triggerOutputOneShot;
};

inline ClockShifter2::ClockShifter2() {
}

inline float ClockShifter2::run(float input, float secondsElapsed) {
    // SQINFO("clock shifter2 run %f", input);
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

      //  const double newFreq = 1.0 / double(_freqMeasure.getPeriod());
      //  const double newPhase = 1 - (_shiftAmount + (newFreq / 2));  // rounding, so we aren't always "exactly on" the phase.
       // _acc.reset(newPhase, newFreq);
       const double periodSamples = double(_freqMeasure.getPeriod());
       const double delaySamples = _shiftAmount * periodSamples;
       _timer.arm(delaySamples);
    }
    shouldOutputTrigger = _timer.run();
    if (shouldOutputTrigger) {
        _triggerOutputOneShot.set();
    } else {
        _triggerOutputOneShot.step(secondsElapsed);
    }

    const auto ret = _triggerOutputOneShot.hasFired() ? cGateOutLow : cGateOutHi;
    return ret;
}