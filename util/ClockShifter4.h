#pragma once

#include <cmath>

#include "FreqMeasure2.h"
#include "OneShotSampleTimer.h"

class ClockShifter4 {
public:
    friend class TestX;
    void setShift(float);
    bool process(bool trigger, bool clock);
    bool freqValid() const;
    int getPeriod() const;
    float getNormalizedPosition() const;

private:
    enum class ShiftPossibilities {
        ShiftOverForward,
        ShiftOverBackward,
        ShiftOverNone
    };

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

    bool _forceGenerateClockNextSample = false;
    bool _suppressNextClockOutput = false;
    bool _haveOutputClockThisInputPeriod = false;
    //
    void _onShiftJumpsOverUsHigher();
    void _onShiftJumpsOverUsLower();
    void _requestSuppressTheNextClockOut();
    void _requestForceGenerateClockNextSample();
    ShiftPossibilities _calculateShiftOver(float newShiftValue) const;
};

inline ClockShifter4::ShiftPossibilities ClockShifter4::_calculateShiftOver(float newShiftValue) const {
    // First we want to "unwrap" everything.

    float currentPosition = getNormalizedPosition();
    bool didWrap = false;
    float before = 0, after = 0;
    const float deltaShift = std::abs(newShiftValue - _shift);
   //SQINFO("_calculateShiftOver new shift value = %f, cur pos=%f delta=%f", newShiftValue, currentPosition, deltaShift);

    if (deltaShift < .5) {
        // this is normal, no wrap
        before = _shift;
        after = newShiftValue;
    } else {
        didWrap = true;
        // wrap case
        if (newShiftValue > _shift) {
            // new shift is the one "before" the wrap. _shift is after
            before = _shift + 1;  // small one wraps
            after = newShiftValue;
        } else {
            before = _shift;
            after = newShiftValue + 1;
        }
    }

    if (didWrap && (currentPosition < .5)) {
        currentPosition += 1;
    }

    // SQINFO("at 77 before=%f, cur=%f after=%f a-c=%f", before, currentPosition, after, after - currentPosition);
    // SQINFO(" a-c=%f a==c: %d subd=%f", after - currentPosition, after == currentPosition, double(after) - double(currentPosition));

    if ((before < currentPosition) && (currentPosition < after)) {
        // SQINFO("fwd at bottom of calc, before=%f, currentPos=%f after=%f", before, currentPosition, after);
        return ShiftPossibilities::ShiftOverForward;
    }
    if ((before > currentPosition) && (currentPosition > after)) {
        // SQINFO("back at bottom of calc, before=%f, currentPos=%f after=%f", before, currentPosition, after);
        return ShiftPossibilities::ShiftOverBackward;
    }

    // SQINFO("none at bottom of calc, before=%f, currentPos=%f after=%f", before, currentPosition, after);
    return ShiftPossibilities::ShiftOverNone;
}

inline void ClockShifter4::setShift(float x) {
   
    const float newShift = (x - std::floor(x));
    // SQINFO("... Shifter::setShift(%f) ->%f acc=%d", x, newShift, _phaseAccumulator);
    if (_shift == newShift) {
        // SQINFO("no change");
        return;
    }
    if (!freqValid()) {
        _shift = newShift;
        // SQINFO("can't set, invalid");
        return;
    }

    const ShiftPossibilities shiftInfo = _calculateShiftOver(newShift);

    switch (shiftInfo) {
        case ShiftPossibilities::ShiftOverForward:
            //SQINFO("over forward");
            // If it's jumping over us, in the increasing direction.
            _onShiftJumpsOverUsHigher();
            break;
        case ShiftPossibilities::ShiftOverBackward:
           // SQINFO("over backward");
            _onShiftJumpsOverUsLower();
            break;
        default:
            assert(shiftInfo == ShiftPossibilities::ShiftOverNone);
            break;
    }
    _shift = newShift;
}

inline void ClockShifter4::_requestSuppressTheNextClockOut() {
    // SQINFO("_requestSuppressTheNextClockOut");
    assert(!_firstClock);
    assert(!_forceGenerateClockNextSample);
    // assert(!_suppressNextClockOutput);
    if (_suppressNextClockOutput) {
        SQWARN("potentially losing clock");
    }
    _suppressNextClockOutput = true;
}

inline void ClockShifter4::_requestForceGenerateClockNextSample() {
    // SQINFO("_requestForceGenerateClockNextSample, will set _force.");
    assert(!_forceGenerateClockNextSample);
    assert(!_suppressNextClockOutput);
    _forceGenerateClockNextSample = true;
}

// Higher, meaning later, shift increases.
// We will need to suppress the first clock at the new location, because
// we already sent a clock for the old location.
inline void ClockShifter4::_onShiftJumpsOverUsHigher() {
    // SQINFO("requesting sup from _onShiftJumpsOverUsHigher");
    _requestSuppressTheNextClockOut();
}

inline void ClockShifter4::_onShiftJumpsOverUsLower() {
    _requestForceGenerateClockNextSample();
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
   // SQINFO("***** ClockShifter4::process trig=%d, phase acc=%d shift=%f", trigger, _phaseAccumulator, _shift);
    // If both of these were true, what would we do???
    assert(!_firstClock || !_suppressNextClockOutput);
    const bool freqWasValid = _freqMeasure.freqValid();
    _freqMeasure.process(trigger, clock);
    if (!_freqMeasure.freqValid()) {
        //SQINFO("input freq not settled, not processing");
        return false;
    }

    bool ret = false;
    const float targetClockf = float(_freqMeasure.getPeriod()) * _shift;
    const int targetClock = int(targetClockf);

    // if it's the edge of a new clock, re-sync
    if (trigger) {
        if (freqWasValid) {
            // assert(_haveOutputClockThisInputPeriod);
            if (!_haveOutputClockThisInputPeriod) {
               // SQINFO("!!! one cycle with no output. pos=%f shift=%f", getNormalizedPosition(), this->_shift);
                if (this->_shift > .5) {
                   // SQINFO("!!!!");
                }
            }
        }
        _phaseAccumulator = 0;
        // SQINFO("zero pa on trigger");
        _haveOutputClockThisInputPeriod = false;
    } else {
        _phaseAccumulator++;
      //  SQINFO("inc pa to %d period=%d", _phaseAccumulator, _freqMeasure.getPeriod());
    }

    // Now, see if enough samples of delay have gone by to generate a new output clock.
    // Fire the clock when phase acc crosses the shift point.

    bool outputClock = false;
    bool isTimeToOutputClock = ((_phaseAccumulator >= targetClock) && (_phaseAccumulator < (targetClock + 1)));
    //SQINFO("it time to output = %d force = %d pa=%d tg=%d", isTimeToOutputClock, _forceGenerateClockNextSample, _phaseAccumulator, targetClock);
    
    if (_forceGenerateClockNextSample) {
        assert(!isTimeToOutputClock);
        // SQINFO("shifter4 131: attempting to execute force");
        isTimeToOutputClock = true;
        _forceGenerateClockNextSample = false;
    }
    if (isTimeToOutputClock) {
        // SQINFO("old path the always output clock");

        // It's time to output, so let's output. Unless we are really far from where we expect (it this still current?)
        // But don't do that check for the first clock - we probably won't we settled down.
        //
        // period / 2 is too aggressive. Should probably make it depend on delay time, but haxoring around...
        bool shouldFireClock = (_firstClock || (_elapsedInputSamplesSinceLastOutput >= (_freqMeasure.getPeriod() / 3)));
        // SQINFO("in shifter 142, should fire = %d, suppress = %d", shouldFireClock, _suppressNextClockOutput);
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
    if (ret) {
        _haveOutputClockThisInputPeriod = true;
    }
    return ret;
}
