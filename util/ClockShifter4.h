#pragma once

#include <cmath>

#include "FreqMeasure2.h"
#include "OneShotSampleTimer.h"

class ClockShifter4 {
public:
    friend class TestClockShifter4;
    void setShift(float);
    bool process(bool trigger, bool clock);
    bool freqValid() const;
    int getPeriod() const;

    float getNormalizedPosition() const;

private:
    /**
     * @brief reset on trigger, then counts up on each process count
     *
     */
    int _phaseAccumulator = 0;
    FreqMeasure2 _freqMeasure;
    OneShotSampleTimer _clockWidthGenerator;
    float _shift = 0;
    int _elapsedInputSamplesSinceLastOutput = 0;
    bool _firstClock = true;

 //   bool _forceGenerateClockNextSample = true;
    bool _suppressNextClockOutput = false;
    //
    void _onShiftJumpsOverUsHigher();
    void _onShiftJumpsOverUsLower();
    void _requestSuppressTheNextClockOut();
};

inline void ClockShifter4::setShift(float x) {
    const float newShift = (x - std::floor(x));
    if (_shift == newShift) {
        return;
    }
    if (!freqValid()) {
        _shift = newShift;
        return;
    }
    const float currentPosition = getNormalizedPosition();
    if ((newShift > currentPosition) && (_shift < currentPosition)) {
        // If it's jumping over us, in the increasing direction.
        _onShiftJumpsOverUsHigher();
    }
    if ((newShift < currentPosition) && (_shift > currentPosition)) {
        _onShiftJumpsOverUsLower();
    }
    // if (_shift)
    _shift = newShift;
}

inline void ClockShifter4::_requestSuppressTheNextClockOut() {
    _suppressNextClockOutput = true;
 //   assert(!_forceGenerateClockNextSample);
}

// Higher, meaning later, shift increases.
// We will need to suppress the first clock at the new location, because
// we already sent a clock for the old location.
inline void ClockShifter4::_onShiftJumpsOverUsHigher() {
    _requestSuppressTheNextClockOut();
}

inline void ClockShifter4::_onShiftJumpsOverUsLower() {
   // assert(false);
   SQINFO("_onShiftJumpsOverUsLower nimp");
}

inline bool ClockShifter4::freqValid() const {
    return _freqMeasure.freqValid();
}

inline int ClockShifter4::getPeriod() const {
    return _freqMeasure.getPeriod();
}

inline float ClockShifter4::getNormalizedPosition() const {
    assert(freqValid());
    return float(_phaseAccumulator) / float(_freqMeasure.getPeriod());
}

inline bool ClockShifter4::process(bool trigger, bool clock) {
    // If both of these were true, what would we do???
    assert(!_firstClock || !_suppressNextClockOutput);
    _freqMeasure.process(trigger, clock);
    if (!_freqMeasure.freqValid()) {
        return false;
    }
    // SQINFO("");
    // SQINFO("process, input freq stable");

    bool ret = false;

    // if it's the edge of a new clock, re-sync
    if (trigger) {
        // ret = true;
        _phaseAccumulator = 0;
    } else {
        _phaseAccumulator++;
    }

    // Now, see if enough samples of delay have gone by to generate a new output clock.
    // Fire the clock when phase acc crosses the shift point.
    const float targetClockf = float(_freqMeasure.getPeriod()) * _shift;
    const int targetClock = int(targetClockf);
    // SQINFO("trigger in = %d, targetClock=%d, phase acc=%d", trigger, targetClock, _phaseAccumulator);
    // SQINFO("firstClock = %d elapsed = %d period=%d", _firstClock, _elapsedInputSamplesSinceLastOutput, _freqMeasure.getPeriod());
    // SQINFO("target-phaseAcc=%d", std::abs(_phaseAccumulator - targetClock)) ;
    bool outputClock = false;
    if ((_phaseAccumulator >= targetClock) && (_phaseAccumulator < (targetClock + 1))) {
        // SQINFO("old path the always output clock");


        // It's time to output, so let's output. Unless we are really far frome where we expect (it this still current?)
        // But don't do that check for the first clock - we probably won't we settled down.
        // 
        // period / 2 is too aggressive. Should probably make it depend on delay time, but haxoring around...
        bool shouldFireClock = (_firstClock || (_elapsedInputSamplesSinceLastOutput >= (_freqMeasure.getPeriod() / 3)));
        if (_suppressNextClockOutput) {
            shouldFireClock = false;
            _suppressNextClockOutput = false;
        }
        if (_firstClock || shouldFireClock) {
            // SQINFO("sending a clock, elapsed = %d", _elapsedInputSamplesSinceLastOutput);
            ret = true;
            _clockWidthGenerator.arm(_freqMeasure.getHighDuration());
            _elapsedInputSamplesSinceLastOutput = 0;
            _firstClock = false;
            outputClock = true;
        } else {
            // SQINFO("suppressed clock");
        }
    }

    if (!outputClock) {
        // If we aren't seeing a new clock, the work on the width out the output.
        _clockWidthGenerator.run();
        ret = _clockWidthGenerator.isRunning();
        if (ret) {
            // SQINFO("clock extender is forcing clock return");
        }
        _elapsedInputSamplesSinceLastOutput++;
    }
    return ret;
}
