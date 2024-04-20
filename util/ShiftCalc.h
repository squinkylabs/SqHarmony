#pragma once

#include <assert.h>
#include "SqLog.h"

class ShiftCalc {
public:
    friend class TestX;
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
    bool busyPositive() const;
    bool busyNegative() const;
    bool busyEither() const;

    /**
     * @brief set up to generate a shift what will go from 0 to 1 in 8 periods.
     *  Triggers ignored if already triggered.
     *
     * @param periodOfClock
     * @param totalShiftAmount - for example, will be .5 if we want to slip half a clock.
     * @param totalShiftDurationInClocks - over what period of time will the shift occur?
     */
    void trigger(int periodOfClock, float totalShiftAmount, float totalShiftDurationInClocks);

    void reset();
private:
    double _acc = 0;  // Accumulates over one "shift session".
    double _delta = 0;
    double _masterAccumulator = 0;  // Accumulates all the sessions before the current one.
    double _shiftAmountLimit = 0;
};

inline void ShiftCalc::reset() {
    _acc = 0;
    _delta = 0;
    _masterAccumulator = 0;
    _shiftAmountLimit = 0;
}


inline float ShiftCalc::go() {
    if (_delta) {
        _acc += _delta;
        const bool positive = _shiftAmountLimit > 0;
        if ((positive && (_acc >= _shiftAmountLimit)) ||
            (!positive && (_acc <= _shiftAmountLimit)))
        {
            _acc = _shiftAmountLimit;
            _delta = 0;
        }
    }
    return float(_acc);
}

inline float ShiftCalc::get() const {
    return float(_masterAccumulator + _acc);
}

inline bool ShiftCalc::busyEither() const {
    return _delta != 0;
}

inline bool ShiftCalc::busyPositive() const {
    return _delta > 0;
}

inline bool ShiftCalc::busyNegative() const {
    return _delta < 0;
}

inline void ShiftCalc::trigger(int periodOfClock, float totalShiftAmount, float totalShiftDurationInClocks) {
   // SQINFO("ShiftCalc::trigger(%d, %f, %f)", periodOfClock, totalShiftAmount, totalShiftDurationInClocks);
    if (busyEither()) {
        return;  // ignore triggers while running
    }
    assert(periodOfClock > 0);
    _masterAccumulator += _acc;
    _acc = 0;
    _delta = totalShiftAmount / (double(periodOfClock) * totalShiftDurationInClocks);
    _shiftAmountLimit = totalShiftAmount;
}