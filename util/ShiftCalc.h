#pragma once

#include <assert.h>

class ShiftCalc {
public:
    /**
     * @brief clocks generator once, returns current output.
     * 
     * @return float 
     */
    float go();


    /**
     * @brief set up to generate a shift what will go from 0 to 1 in 8 periods.
     *  Triggers ignored if already triggered.
     * 
     * @param periodOfClock 
     */
    void trigger(int periodOfClock);
private:
    double _acc = 0;
    double _delta = 0;
};

inline float ShiftCalc::go() {
    if (_delta) {
        _acc += _delta;
        if (_acc >= 1) {
            _acc = 1;
            _delta = 0;
        }
    }
    return float(_acc);
}

// inline void ShiftCalc::setup(int samplesInMasterPeriod) {
// } 

inline void ShiftCalc::trigger(int periodOfClock) {
    assert(periodOfClock > 0);
    _delta = 1.0 / (8.0 *double(periodOfClock));
} 