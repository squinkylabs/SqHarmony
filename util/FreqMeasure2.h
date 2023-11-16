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
    int _count = 0;
    int _period = 0;
    int _clocksReceived = 0;
    bool _lastClock = false;
};

 inline void FreqMeasure2::process(bool trigger, bool clock) {
    assert((trigger && clock) || !trigger);

    // if not a new clock, then keep accumulating
    if (!trigger) {
        ++_count;
        return;
    } else {
          _clocksReceived++;
        if (_clocksReceived > 10) {
            _clocksReceived = 10;
        }
        _period = _count;
        _count = 0;
    }

 }

// inline void FreqMeasure2::onSample(bool clock) {

//     // only process transition, but being held still adds to period.
//     if (clock == _lastClock) {
//         ++_count;
//         return;
//     }
//     _lastClock = clock;

//     if (clock) {
//         _clocksReceived++;
//         if (_clocksReceived > 10) {
//             _clocksReceived = 10;
//         }
//         _period = _count;
//         _count = 0;
//     } else {
//         ++_count;
//     }
// }

inline bool FreqMeasure2::freqValid() const {
    return _clocksReceived > 1;
}

inline int FreqMeasure2::getPeriod() const {
    assert(freqValid());
    return _period + 1;
}
