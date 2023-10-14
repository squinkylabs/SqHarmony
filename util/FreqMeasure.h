#pragma once

#include <cstdint>
#include "SqLog.h"
#include "assert.h"

class FreqMeasure {
public:
    void onSample(bool trigger);
    bool freqValid() const;
    int64_t getPeriod() const;
private:
    int64_t _count = 0;
    int64_t _period = -1;
};

inline void  FreqMeasure::onSample(bool trigger) {
    SQINFO("fm, onSamle %d", trigger);
    if (trigger) {
        _period = _count;
        _count = 0;
        SQINFO("fm accepting %lld", _period);
    } else {
        ++_count;
    }
}

inline bool  FreqMeasure::freqValid() const {
    return _period > 0;
}

inline int64_t FreqMeasure::getPeriod() const {
    assert(freqValid());
    return _period + 1;
}
