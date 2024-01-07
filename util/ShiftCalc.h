#pragma once

#include <assert.h>

class ShiftCalc {
public:
    /**
     * @brief clocks generator once, returns current output.
     * 
     * If you don't want to use the return value, it can always be retrieved with a call to get().
     * Must be called once on every sample.
     *
     * @return float
     */
    float go();

    float get() const;
    bool busy() const;

    /**
     * @brief set up to generate a shift what will go from 0 to 1 in 8 periods.
     *  Triggers ignored if already triggered.
     *
     * @param periodOfClock
     * @param totalShiftAmount - for example, will be .5 if we want to slip half a clock.
     * @param totalShiftDurationInClocks - over what period of time will the shift occur?
     */
    void trigger(int periodOfClock, float totalShiftAmount, float totalShiftDurationInClocks);

private:
    double _acc = 0;  // Accumulates over one "shift session".
    double _delta = 0;
    double _masterAccumulator = 0;  // Accumulates all the sessions before the current one.
    double _shiftAmountLimit = 0;
};

inline float ShiftCalc::go() {
    if (_delta) {
        _acc += _delta;
        if (_acc >= _shiftAmountLimit) {
            _acc = _shiftAmountLimit;
            _delta = 0;
        }
    }
    return float(_acc);
}

inline float ShiftCalc::get() const {
    return float(_masterAccumulator + _acc);
}

inline bool ShiftCalc::busy() const {
    return _delta > 0;
}

// inline void ShiftCalc::trigger(int periodOfClock) {
//     if (busy()) {
//         return;  // ignore triggers while running
//     }
//     assert(periodOfClock > 0);
//     _masterAccumulator += _acc;
//     _acc = 0;
//     _delta = 1.0 / (8.0 * double(periodOfClock));
// }

inline void ShiftCalc::trigger(int periodOfClock, float totalShiftAmount, float totalShiftDurationInClocks) {
    if (busy()) {
        return;  // ignore triggers while running
    }
    assert(periodOfClock > 0);
    _masterAccumulator += _acc;
    _acc = 0;
 //   _delta = 1.0 / (8.0 * double(periodOfClock));
    _delta = totalShiftAmount / (double(periodOfClock) * totalShiftDurationInClocks);
    _shiftAmountLimit = totalShiftAmount;
}