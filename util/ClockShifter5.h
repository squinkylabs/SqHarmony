#pragma once

#include <cmath>
#include <tuple>

#include "FreqMeasure2.h"
#include "OneShotSampleTimer.h"

class ClockShifter5 {
public:
    friend class TestX;
    bool process(bool trigger, bool clock, float shift);
    bool freqValid() const;

    // This used to be used internally, now just for test
    float getNormalizedPosition() const;
    

private:
    FreqMeasure2 _freqMeasure;
    OneShotSampleTimer _clockWidthGenerator;

    /**
     * @brief reset on trigger, then counts up on each process count.
     */
    int _phaseAccumulator = 0;

    /**
     * @brief true if a clock has been output in the current cycle.
     */
    bool _haveClocked = false;
    mutable float _lastRawShift = 0;
    mutable float _lastProcessedShift = 0;

    bool arePastDelay(float candidateDelay) const;
   // float processShift(float rawShift) const;
    // ret[0] is the normalized shift
    // ret[1] is true if the shift crossed over the phase backwards
   std::tuple<float, bool> processShift(float rawShift) const;
};

inline bool ClockShifter5::arePastDelay(float candidateDelay) const {
    const float targetClockf = float(_freqMeasure.getPeriod()) * candidateDelay;
    const int targetClock = int(targetClockf);
    return _phaseAccumulator > targetClock;
}

inline std::tuple<float, bool>  ClockShifter5::processShift(float rawShift) const {
    if (rawShift == _lastRawShift) {
       // return c;
       SQINFO("no change, ret %f", _lastProcessedShift );
        return std::make_tuple(_lastProcessedShift, false);
    }
    const float newShift = (rawShift - std::floor(rawShift));
    const float position = getNormalizedPosition();
    const bool crossedBackwards =  ((_lastProcessedShift < position) && (newShift > position));

    _lastRawShift = rawShift;
    _lastProcessedShift = newShift;
    return std::make_tuple(newShift, crossedBackwards);
} 

inline bool ClockShifter5::process(bool trigger, bool clock, float rawShift) {
    const bool freqWasValid = _freqMeasure.freqValid();
    _freqMeasure.process(trigger, clock);
    if (!_freqMeasure.freqValid()) {
        return false;
    }

    const auto t = processShift(rawShift);
    float shift = std::get<0>(t);

    assert(shift >= 0);
    assert(shift <= 1);

    bool ret = false;
    if (trigger) {
        // This clock for processing on the first clock
        _phaseAccumulator = 0;
        _haveClocked = false;
    }

    _phaseAccumulator++;
    const bool _arePastDelay = arePastDelay(shift);
    if (!_haveClocked && _arePastDelay) {
        ret = true;
        _haveClocked = true;
    }

    if (ret) {
        _clockWidthGenerator.arm(_freqMeasure.getHighDuration());
    } else {
        _clockWidthGenerator.run();
        ret = _clockWidthGenerator.isRunning();
    }

    return ret;
}

inline bool ClockShifter5::freqValid() const {
    return _freqMeasure.freqValid();
    ;
}

inline float ClockShifter5::getNormalizedPosition() const {
    assert(freqValid());
    return float(_phaseAccumulator) / float(_freqMeasure.getPeriod());
}
