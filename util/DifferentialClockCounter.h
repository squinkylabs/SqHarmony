
#pragma once

#include "GateTrigger.h"

class DifferentialClockCounter {
public:
    void process(float v1, float v2);

    int getDiff() const;
private:
    GateTrigger _ref;
    GateTrigger _other;
    int _difference = 0;
};

inline void DifferentialClockCounter::process(float refInput, float otherInput) {
    _ref.go(refInput);
    _other.go(otherInput);
    if (_ref.trigger() && !_other.trigger()) {
        _difference -= 1;
    } else if (!_ref.trigger() && _other.trigger()) {
        _difference += 1;
    }
}

inline int DifferentialClockCounter::getDiff() const {
    return _difference;
}