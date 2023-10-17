#pragma once

#include "asserts.h"
#include "FreqMeasure.h"
#include "GateTrigger.h"
#include "OneShot.h"
#include "ResettablePhaseAccumulator.h"

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
};

inline ClockShifter2::ClockShifter2() {
}

inline float ClockShifter2::run(float input, float secondsElapsed) {
    return false;
}