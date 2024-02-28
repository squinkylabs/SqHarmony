#pragma once

#include <cstdint>

#include "SqLog.h"
#include "assert.h"

/**
 * @brief takes a standard "gate" input (like square wave LFO), converts to period.
 * Does not have the duty cycle measurement that FreqMesure2 has
 *
 */
class FreqMeasure3 {
public:
    // Has built-in edge detect, so can process raw gate.
    void process(bool gate);
    bool freqValid() const;
    int getPeriod() const;
private:
    int _countSinceTrigger = 0;
    int _period = 0;
    int _clocksReceived = 0;
    bool _lastClock = false;
};

inline void FreqMeasure3::process(bool clock) {
    if ((clock != _lastClock) && clock) {
        _clocksReceived++;
        if (_clocksReceived > 10) {
            _clocksReceived = 10;
        }
        _period = _countSinceTrigger;
        _countSinceTrigger = 0;
    } else {
        _countSinceTrigger++;
    }
    _lastClock = clock;
}

inline bool FreqMeasure3::freqValid() const {
    return _clocksReceived > 1;
}

inline int FreqMeasure3::getPeriod() const {
    assert(freqValid());
    return _period + 1;
}
