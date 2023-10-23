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
    void setup(int samplesInMasterPeriod);
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

inline void ShiftCalc::setup(int samplesInMasterPeriod) {
} 

inline void ShiftCalc::trigger(int periodOfClock) {
    assert(periodOfClock > 0);
    _delta = 1.0 / double(periodOfClock);
} 