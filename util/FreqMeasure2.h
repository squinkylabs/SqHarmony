#pragma once

#include <cstdint>

#include "SqLog.h"
#include "assert.h"

class FreqMeasure2 {
public:
    //  void onSample(bool trigger);
    void process(bool trigger, bool gate);
    bool freqValid() const;
    int getPeriod() const;
    int getHighDuration() const;

private:
    int _countSinceTrigger = 0;
    int _clockHighCountSinceTrigger = 0;
    int _period = 0;
    int _highCount = 0;
    int _clocksReceived = 0;
    bool _lastClock = false;
};

inline void FreqMeasure2::process(bool trigger, bool clock) {
    assert((trigger && clock) || !trigger);

    // if not a new clock, then keep accumulating
    if (!trigger) {
        ++_countSinceTrigger;
        if (clock) {
            ++_clockHighCountSinceTrigger;
        }
    } else {
        _clocksReceived++;
        if (_clocksReceived > 10) {
            _clocksReceived = 10;
        }
        _period = _countSinceTrigger;
        _highCount = _clockHighCountSinceTrigger; 
        _countSinceTrigger = 0;
        _clockHighCountSinceTrigger = 0;
    }
}

inline bool FreqMeasure2::freqValid() const {
    return _clocksReceived > 1;
}

inline int FreqMeasure2::getPeriod() const {
    assert(freqValid());
    return _period + 1;
}

inline int FreqMeasure2::getHighDuration() const {
    assert(freqValid());
    return _highCount+1;
}