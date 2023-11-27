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
};

inline void ClockShifter4::setShift(float x) {
    _shift = (x - std::floorf(x));
}

inline bool ClockShifter4::process(bool trigger, bool clock) {
    _freqMeasure.process(trigger, clock);
    if (!_freqMeasure.freqValid()) {
        return false;
    }
    SQINFO("");
    SQINFO("process, input freq stable");

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
    SQINFO("trigger in = %d, targetClock=%d, phase acc=%d", trigger, targetClock, _phaseAccumulator);
    SQINFO("firstClock = %d elapsed = %d period=%d", _firstClock, _elapsedInputSamplesSinceLastOutput, _freqMeasure.getPeriod());
    SQINFO("target-phaseAcc=%d", std::abs(_phaseAccumulator - targetClock)) ;
    bool outputClock = false;
    if ((_phaseAccumulator >= targetClock) && (_phaseAccumulator < (targetClock + 1))) {
        SQINFO("old path the always output clock");

        // period / 2 is too aggressive. Should probably make it depend on delay time, but haxoring around...
        if (_firstClock || (_elapsedInputSamplesSinceLastOutput >= (_freqMeasure.getPeriod() / 3))) {
            // TODO: suppress this if "too close" to last clocks.
            SQINFO("sending a clock, elapsed = %d", _elapsedInputSamplesSinceLastOutput);
            ret = true;
            _clockWidthGenerator.arm(_freqMeasure.getHighDuration());
            _elapsedInputSamplesSinceLastOutput = 0;
            _firstClock = false;
            outputClock = true;
        } else {
            SQINFO("suppressed clock");
        }
    } 
    
    if (!outputClock) {
        // If we aren't seeing a new clock, the work on the width out the output.
        _clockWidthGenerator.run();
        ret = _clockWidthGenerator.isRunning();
        if (ret) {
            SQINFO("clock extender is forcing clock return");
        }
        _elapsedInputSamplesSinceLastOutput++;
    }
    return ret;
}

inline bool ClockShifter4::freqValid() const {
    return _freqMeasure.freqValid();
}