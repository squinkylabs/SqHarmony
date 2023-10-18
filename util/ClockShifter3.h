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

    // private:
    enum class ClockStates {
        invalid,
        high,
        low,
        unknown
    };
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

    int _clockCount = 0;
    int _sampleCountSinceLastClock = 0;
    FreqMeasure _freqMeasure;
    float _shiftAmount = 0;
    ClockStates _lastClock = ClockStates::unknown;
    SqRingBuffer<ClockEvent, 32, true> _clockDelayLine;
};

inline ClockShifter3::ClockShifter3() : _clockDelayLine(true) {
}

inline float ClockShifter3::run(float input) {
    const bool ck = input > 5;  // TODO: use better
    const auto ckState = ck ? ClockStates::high : ClockStates::low;
    bool didTick = false;
    bool wasTransition = false;
    if ((ckState != _lastClock) && (_lastClock != ClockStates::unknown)) {
        wasTransition = true;
        if (ckState == ClockStates::high) {
            _clockCount++;
            _sampleCountSinceLastClock = 0;
            didTick = true;
        }
    }
    _lastClock = ckState;

    if (wasTransition) {
        ClockEvent ev;
        ev._samples = _sampleCountSinceLastClock;
        ev._clocks = _clockCount;
        ev._type = didTick ? EventType::lowToHigh : EventType::highToLow;
        _clockDelayLine.push(ev);
    }

    if (!didTick) {
        ++_sampleCountSinceLastClock;
    }

    return 0;
}