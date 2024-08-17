
#pragma once

#include <algorithm>

#include "GateTrigger.h"
#include "SqLog.h"

class DifferentialClockCounter {
public:
    enum class Mode {
        DifferentialClock,  // the default mode
        MeasureRefInput
    };
    void process(float v1, float v2);

    int getDiff() const;
    // void dump() const;
    Mode _mode = Mode::DifferentialClock;

private:
    GateTrigger _ref;
    GateTrigger _other;
    int _difference = 0;

    void _processDifferentialClock(float v1, float v2);
    void _processMeasureRefInput(float v1);

    mutable int _lastDifference = -100;
    mutable int _lastMeasurement = 0;
    mutable int _count = 0;
    mutable int _min = 100000;
    mutable int _max = -100;
};

inline void DifferentialClockCounter::process(float refInput, float otherInput) {
    switch (_mode) {
        case Mode::DifferentialClock:
            _processDifferentialClock(refInput, otherInput);
            return;
        case Mode::MeasureRefInput:
            _processMeasureRefInput(otherInput);
            return;
        default:
            assert(false);
    }
}

inline void DifferentialClockCounter::_processMeasureRefInput(float v) {
    ++_count;

    _other.go(v);
    if (!_other.trigger()) {
        return;
    }
    const int delta = _count - _lastMeasurement;
    SQINFO("ref trigger, period = %d min=%d max=%d", delta, _min, _max);
    _lastMeasurement = _count;
    _min = std::min(_min, delta);
    _max = std::max(_max, delta);
    // _count = 0;
}

inline void DifferentialClockCounter::_processDifferentialClock(float refInput, float otherInput) {
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
        _difference = 0;
        trigger = true;
    }
   // const int diff = _count - _lastMeasurement;
    if (trigger) {
   //     SQINFO("diff: %d, delta=%d", _difference, diff);
        _lastMeasurement = _count;
    }
}

inline int DifferentialClockCounter::getDiff() const {
    return _difference;
}

///////////////////////////////////////////////////////////

// detects min and max spacing of low to high transitions.
class ClockMonitor {
public:
    bool go(bool input);

    int _maxClocksSinceLH = 0;
    int _minClocksSinceLH = 10000000;
    int _delta = 0;

    bool _sawLH = false;
    bool _last = false;
    unsigned _count = 0;
    unsigned _countLH = 0;
};

inline bool ClockMonitor::go(bool input) {
    ++_count;
    if (!input || _last) {
        _last = input;
        return false;
    }
     _last = input;
    // here we know we saw a low to high.
    if (!_sawLH) {
        _sawLH = true;
        return false;
    }

    _delta = _count - _countLH;
    _countLH = _count;

    if (_delta < _minClocksSinceLH) {
        _minClocksSinceLH = _delta;
        return true;
    }

    if (_delta > _maxClocksSinceLH) {
        _maxClocksSinceLH = _delta;
        return true;
    }


    return true;
}