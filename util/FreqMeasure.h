#pragma once

#include <cstdint>

#include "SqLog.h"
#include "assert.h"

// A little old - use FreqMeasure2 instead.
class FreqMeasure {
public:
    void onSample(bool trigger);
    bool freqValid() const;
    int getPeriod() const;

private:
    int _count = 0;
    int _period = 0;
    int _clocksReceived = 0;
    bool _lastClock = false;
};

inline void FreqMeasure::onSample(bool clock) {
    // only process transition, but being held still adds to period.
    if (clock == _lastClock) {
        ++_count;
        return;
    }
    _lastClock = clock;

    if (clock) {
        _clocksReceived++;
        if (_clocksReceived > 10) {
            _clocksReceived = 10;
        }
        _period = _count;
        _count = 0;
    } else {
        ++_count;
    }
}

inline bool FreqMeasure::freqValid() const {
    return _clocksReceived > 1;
}

inline int FreqMeasure::getPeriod() const {
    assert(freqValid());
    return _period + 1;
}
