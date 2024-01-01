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
    SQINFO("_calculateShiftOver new shift value = %f, cur pos=%f", newShiftValue, currentPosition);
    bool didWrap = false;
    float before = 0, after = 0;
    const float deltaShift = std::abs(newShiftValue - _shift);
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

    if ((before < currentPosition) && (currentPosition < after)) {
        SQINFO("fwd at bottom of calc, before=%f, currentPos=%f after=%f", before, currentPosition, after);
        return ShiftPossibilities::ShiftOverForward;
    }
    if ((before > currentPosition) && (currentPosition > after)) {
        SQINFO("back at bottom of calc, before=%f, currentPos=%f after=%f", before, currentPosition, after);
        return ShiftPossibilities::ShiftOverBackward;
    }

    SQINFO("none at bottom of calc, before=%f, currentPos=%f after=%f", before, currentPosition, after);
    return ShiftPossibilities::ShiftOverNone;
}

inline void ClockShifter4::setShift(float x) {
    const float newShift = (x - std::floor(x));
    if (_shift == newShift) {
        return;
    }
    if (!freqValid()) {
        _shift = newShift;
        return;
    }

    const ShiftPossibilities shiftInfo = _calculateShiftOver(newShift);

    switch (shiftInfo) {
        case ShiftPossibilities::ShiftOverForward:
            // If it's jumping over us, in the increasing direction.
            _onShiftJumpsOverUsHigher();
            break;
        case ShiftPossibilities::ShiftOverBackward:
            _onShiftJumpsOverUsLower();
            break;
        default:
            assert(shiftInfo == ShiftPossibilities::ShiftOverNone);
            break;
    }
    _shift = newShift;
}

inline void ClockShifter4::_requestSuppressTheNextClockOut() {
    SQINFO("_requestSuppressTheNextClockOut");
    assert(!_firstClock);
    assert(!_forceGenerateClockNextSample);
    assert(!_suppressNextClockOutput);
    _suppressNextClockOutput = true;
}

inline void ClockShifter4::_requestForceGenerateClockNextSample() {
    SQINFO("_requestForceGenerateClockNextSample, will set _force.");
    assert(!_forceGenerateClockNextSample);
    assert(!_suppressNextClockOutput);
    _forceGenerateClockNextSample = true;
}

// Higher, meaning later, shift increases.
// We will need to suppress the first clock at the new location, because
// we already sent a clock for the old location.
inline void ClockShifter4::_onShiftJumpsOverUsHigher() {
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
    // If both of these were true, what would we do???
    assert(!_firstClock || !_suppressNextClockOutput);
    const bool freqWasValid = _freqMeasure.freqValid();
    _freqMeasure.process(trigger, clock);
    if (!_freqMeasure.freqValid()) {
        // SQINFO("input freq not settled, not processing");
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
                SQINFO("!!! one cycle with no output. pos=%f shift=%f",
                       getNormalizedPosition(),
                       this->_shift);
                if (this->_shift > .5) {
                    SQINFO("!!!!");
                }
            }
        }
        _phaseAccumulator = 0;
        _haveOutputClockThisInputPeriod = false;
    } else {
        _phaseAccumulator++;
    }

    // Now, see if enough samples of delay have gone by to generate a new output clock.
    // Fire the clock when phase acc crosses the shift point.

    bool outputClock = false;
    bool isTimeToOutputClock = ((_phaseAccumulator >= targetClock) && (_phaseAccumulator < (targetClock + 1)));
    if (_forceGenerateClockNextSample) {
        assert(!isTimeToOutputClock);
        // SQINFO("shifter4 131: attempting to execute force");
        isTimeToOutputClock = true;
        _forceGenerateClockNextSample = false;
    }
    if (isTimeToOutputClock) {
        // SQINFO("old path the always output clock");

        // It's time to output, so let's output. Unless we are really far frome where we expect (it this still current?)
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
