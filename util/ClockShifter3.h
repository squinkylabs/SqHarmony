#pragma once

#include "FreqMeasure.h"
#include "SqRingBuffer.h"
#include "asserts.h"

/**
 * @brief not currently used?
 *
 */
class ClockShifter3 {
public:
    friend class ClockShifter3Test;
    ClockShifter3();
    float run(float input);

    /**
     * @brief Set the shift phase.
     *
     * @param amount is between 0 and 1. 0 is no shift, 1 is a whole period
     */
    void setShift(float amount) {
        //  _shiftAmount = amount;
        //  assertGE(amount, 0);
        //  assertLE(amount, 1);
    }

private:
    // enum class ClockStates {
    //     invalid,
    //     high,
    //     low,
    //     unknown
    // };
    enum class EventType {
        invalid,
        lowToHigh,
        highToLow
    };
    class ClockEvent {
    public:
        int _samples = 0;
        int _clocks = 0;
        EventType _type = EventType::highToLow;
    };

    int _currentClockCount = 0;
    int _currentSampleCountSinceLastClock = 0;
    FreqMeasure _freqMeasure;
    float _shiftAmount = 0;
    bool _lastClock = false;
    SqRingBuffer<ClockEvent, 32, true> _clockDelayLine;
};

inline ClockShifter3::ClockShifter3() : _clockDelayLine(true) {
}

inline float ClockShifter3::run(float input) {
    const bool ck = input > 5;  // TODO: use better (like previous clock shifters)
     _freqMeasure.onSample(input);
     if (!_freqMeasure.freqValid()) {
        return 0;       // exit early if we don't have clock length
     }
    const auto ckState = ck;
    bool didTick = false;
    bool wasTransition = false;
    if (ckState != _lastClock) {
        wasTransition = true;
        if (ckState) {
            _currentClockCount++;
            _currentSampleCountSinceLastClock = 0;
            didTick = true;
        }
    }
    _lastClock = ckState;

    if (wasTransition) {
        ClockEvent ev;
        ev._samples = _currentSampleCountSinceLastClock;
        ev._clocks = _currentClockCount;
        ev._type = didTick ? EventType::lowToHigh : EventType::highToLow;
        _clockDelayLine.push(ev);
    }

    if (!didTick) {
        ++_currentSampleCountSinceLastClock;
    }

    return 0;
}