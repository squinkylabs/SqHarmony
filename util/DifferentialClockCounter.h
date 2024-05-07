
#pragma once

#include "GateTrigger.h"
#include "SqLog.h"

class DifferentialClockCounter {
public:
    void process(float v1, float v2);

    int getDiff() const;
   // void dump() const;
private:
    GateTrigger _ref;
    GateTrigger _other;
    int _difference = 0;

    mutable int _lastDifference = -100;
    mutable int _lastMeasurement = 0;
    mutable int _count = 0;
};

inline void DifferentialClockCounter::process(float refInput, float otherInput) {
    _ref.go(refInput);
    _other.go(otherInput);
    ++_count;

    bool trigger = false;
    if (_ref.trigger() && !_other.trigger()) {
        _difference -= 1;
        trigger = true;
    } else if (!_ref.trigger() && _other.trigger()) {
        _difference += 1;
        trigger = true;
    } else if (_ref.trigger() && _other.trigger()) {
        _difference  = 0;
        trigger = true;
    }   
    const int diff = _count = _lastMeasurement;
    if (trigger) {
        SQINFO("diff: %d, delta=%d", _difference, diff);
        _lastMeasurement = _count;
    }
}

inline int DifferentialClockCounter::getDiff() const {
    return _difference;
}