#pragma once

#include "FreqMeasure.h"
#include "asserts.h"

/**
 * @brief not currently used?
 *
 */
class ClockShifter3 {
public:
    ClockShifter3();
    float run(float input);

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
    FreqMeasure _freqMeasure;
    float _shiftAmount = 0;
};

inline ClockShifter3::ClockShifter3() {
}

inline float ClockShifter3::run(float input) {
    return 0;
}