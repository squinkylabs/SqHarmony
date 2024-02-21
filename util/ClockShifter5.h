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

    static int llv;  // log level
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
    mutable float _lastRawShift = -1;
    mutable float _lastProcessedShift = -1;

    bool arePastDelay(float candidateDelay) const;
    // float processShift(float rawShift) const;
    // ret[0] is the normalized shift
    // ret[1] is true if the shift got larger, wrapping through zero
    // 3 - we are speeding up - was 0 (1) before
    std::tuple<float, bool, bool> processShift(float rawShift) const;
};

inline bool ClockShifter5::arePastDelay(float candidateDelay) const {
    const float targetClockf = float(_freqMeasure.getPeriod()) * candidateDelay;
    const int targetClock = int(targetClockf);
    if (llv > 0) SQINFO("in cs5::arePastDelay, candidate=%f targetClock=%d acc=%d ret=%d (acc> targCk)", candidateDelay, targetClock, _phaseAccumulator, (_phaseAccumulator > targetClock));
    return _phaseAccumulator > targetClock;
}

inline std::tuple<float, bool, bool> ClockShifter5::processShift(float rawShift) const {
    if (llv > 0) SQINFO("enter processShift raw=%f", rawShift);
    if (rawShift == _lastRawShift) {
        if (llv > 0) SQINFO("no change, ret %f", _lastProcessedShift);
        return std::make_tuple(_lastProcessedShift, false, false);
    }

    const float newShift = (rawShift - std::floor(rawShift));
    if (llv > 0) SQINFO("after proc, new =%f raw=%f floor=%f", newShift, rawShift, std::floor(rawShift));

    // If we reduced the phase of the delay, such that it goes over the current position.
    //  const bool crossedBackwards = ((_lastProcessedShift > position) && (newShift < position));

    // If the phase increases from, say .95 to .05, it wraps in the increasing direction;
    const bool phaseWrappedIncreasing = ((_lastProcessedShift > .7) && (newShift < .3));

    // phase wrapping in the decreasing direction, from slightly positive to really big, must be negative
    const bool delayDecreasingFromZero = _lastProcessedShift < 0 ? false : (_lastProcessedShift < .3) && (newShift > .7);
    // this is usually (llv > 1)
    if (llv > 0) {
        SQINFO("a) process shift, wrap inc=%d, wrap dec=%d, newShift=%f", phaseWrappedIncreasing, delayDecreasingFromZero, newShift);
        SQINFO("b) last shift %f new shft %f", _lastProcessedShift, newShift);
    }

    _lastRawShift = rawShift;
    _lastProcessedShift = newShift;
    if (llv > 0) {
        SQINFO("78 just set _lastProcessedShift to %f", _lastProcessedShift);
    }

    return std::make_tuple(newShift, phaseWrappedIncreasing, delayDecreasingFromZero);
}

inline bool ClockShifter5::process(bool trigger, bool clock, float rawShift) {
    if (llv > 0) {
        const int period = _freqMeasure.freqValid() ? _freqMeasure.getPeriod() : -1;
        SQINFO("\n-- enter cs5::process (%d, %d, %f) acc=%d haveckd=%d period=%d", trigger, clock, rawShift, _phaseAccumulator, _haveClocked, period);
        if (trigger) SQINFO("\n*** clock in*");
    }
    //   const bool freqWasValid = _freqMeasure.freqValid();
    _freqMeasure.process(trigger, clock);
    if (!_freqMeasure.freqValid()) {
        if (llv > 0) SQINFO("leaving unstable");
        return false;
    }

    const auto processedShift = processShift(rawShift);
    float shift = std::get<0>(processedShift);
    if (llv > 0) SQINFO("processed shift = %f, %d %d", std::get<0>(processedShift), std::get<1>(processedShift), std::get<2>(processedShift));

    assert(shift >= 0);
    assert(shift <= 1);

    bool ret = false;
    if (trigger) {
        if (llv > 0) SQINFO("processing an external trigger");
        // If we get a trigger, but haven't finished the period, then  we either have a rounding error, or
        // the clock has sped up. But check phase acc to make sure we are running (not priming).
        if (!_haveClocked && (_phaseAccumulator > 0)) {
            if (llv > 0) SQINFO("we owe a trigger, to gen a clock out");
            ret = true;
        }
        // This clock for processing on the first clock
        _phaseAccumulator = 0;
        if (llv > 0) SQINFO("clear _haveClocked and add for new trigger");
        _haveClocked = false;
    }

    // If a change in shift has moved us to a later time, thought zero, then
    // we must suppress the clocks we would generate.
    if (std::get<1>(processedShift)) {
        if (llv > 0) SQINFO("suppressing on shift wrap around zero. set _haveClocked ");
        _haveClocked = true;
    }
    if (std::get<2>(processedShift)) {
        if (llv > 0) SQINFO("allowing clock on wrap around zero. clr _haveClocked. was %d", _haveClocked);

        // SQINFO("removed code for negative wrap");
        if (trigger) {
            if (llv > 0) SQINFO("bgf: got a wrap and a trigger on the same call.  generate clock out!!!");

            assert(!ret);  // if already forcing, this will do nothing.
            ret = true;
        }
        _haveClocked = false;
    }

    _phaseAccumulator++;
    const bool _arePastDelay = arePastDelay(shift);
    if (llv > 0) SQINFO("ret from _arePast = %d _haveClocked = %d", _arePastDelay, _haveClocked);
    if (!_haveClocked && _arePastDelay) {
        if (llv > 0) SQINFO("clocking from are past, set _haveClocked and output");
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
        if (llv > 0) SQINFO("process ret clock");
    } else {
        if (llv > 0) SQINFO("process no clock");
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