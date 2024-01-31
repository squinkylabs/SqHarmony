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
    int getPeriod() const;

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
    // ret[1] is true if the shift got larger, wrapping through zero
    std::tuple<float, bool> processShift(float rawShift) const;
};

inline bool ClockShifter5::arePastDelay(float candidateDelay) const {
    const float targetClockf = float(_freqMeasure.getPeriod()) * candidateDelay;
    const int targetClock = int(targetClockf);
    SQINFO("int are past delay, cand=%f tc=%d acc=%d ret=%d", candidateDelay, targetClock, _phaseAccumulator, (_phaseAccumulator > targetClock));
    return _phaseAccumulator > targetClock;
}

inline std::tuple<float, bool> ClockShifter5::processShift(float rawShift) const {
    if (rawShift == _lastRawShift) {
        // return c;
        SQINFO("no change, ret %f", _lastProcessedShift);
        return std::make_tuple(_lastProcessedShift, false);
    }

    const float newShift = (rawShift - std::floor(rawShift));
    const float position = getNormalizedPosition();

    // If we reduced the phase of the delay, such that it goes over the current position.
  //  const bool crossedBackwards = ((_lastProcessedShift > position) && (newShift < position));

    // If the phase increases from, say .95 to .05, it wraps in the increasing direction/
    const bool phaseWrappedIncreasing = ((_lastProcessedShift > .75) && (newShift < .25));

    _lastRawShift = rawShift;
    _lastProcessedShift = newShift;

    return std::make_tuple(newShift, phaseWrappedIncreasing);
}

inline bool ClockShifter5::process(bool trigger, bool clock, float rawShift) {
    SQINFO("\n-- cs5::process (%d, %d, %f) acc=%d hvck=%d", trigger, clock, rawShift, _phaseAccumulator, _haveClocked);
    if (trigger) SQINFO("\n*** clock in*");
    const bool freqWasValid = _freqMeasure.freqValid();
    _freqMeasure.process(trigger, clock);
    if (!_freqMeasure.freqValid()) {
        SQINFO("leaving unstable");
        return false;
    }

    const auto processedShift = processShift(rawShift);
    float shift = std::get<0>(processedShift);
    SQINFO("processed shift = %f, %d", std::get<0>(processedShift), std::get<1>(processedShift));

    assert(shift >= 0);
    assert(shift <= 1);

    bool ret = false;
    if (trigger) {
        // If we get a trigger, but haven't finished the period, then  we either have a rounding error, or
        // the clock has sped up. But check phase acc to make sure we are running (not priming).
        if (!_haveClocked && (_phaseAccumulator > 0)) {
            SQINFO("we owe a trigger");
            ret = true;
        }
        // This clock for processing on the first clock
        _phaseAccumulator = 0;
        SQINFO("clear hc");
        _haveClocked = false;
    }

    // If a change in shift has moved us to a later time, thought zero, then
    // we must suppress the clocks we would generate.
    if (std::get<1>(processedShift)) {
        SQINFO("suppressing on shift wrap around zero. set hc");
        _haveClocked = true;
    }

    _phaseAccumulator++;
    const bool _arePastDelay = arePastDelay(shift);
    SQINFO("ret from _arePast = %d _haveClocked = %d", _arePastDelay, _haveClocked);
    if (!_haveClocked && _arePastDelay) {
        SQINFO("clocking from are past, set hc");
        ret = true;
        _haveClocked = true;
    }

    if (ret) {
        _clockWidthGenerator.arm(_freqMeasure.getHighDuration());
    } else {
        _clockWidthGenerator.run();
        ret = _clockWidthGenerator.isRunning();
    }

    if (ret) {
        SQINFO("process ret clock: %d", ret);
        SQINFO("\n*** Clock Out");
    } else {
        SQINFO("process no clock");
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

inline int ClockShifter5::getPeriod() const {
    return _freqMeasure.getPeriod();
}