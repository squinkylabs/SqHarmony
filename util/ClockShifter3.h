#pragma once

#include "FreqMeasure.h"
#include "ShiftMath.h"
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
    bool run(bool input);

    /**
     * @brief Set the shift phase.
     *
     * @param amount is between 0 and 1. 0 is no shift, 1 is a whole period
     */
    void setShift(float amount);

    const FreqMeasure& getFreqMeasure() const {
        return _freqMeasure;
    }

private:
    enum class EventType {
        invalid,
        lowToHigh,
        highToLow
    };
    class ClockEvent {
    public:
        ClockEvent(EventType type, int clocks, int samples) : _type(type), _timeStamp(clocks, samples) {}
        ClockEvent() {}

        EventType _type = EventType::highToLow;
        ShiftMath::ClockWithSamples _timeStamp;
    };

    ShiftMath::ClockWithPhase _requestedShift;
    ShiftMath::ClockWithSamples _currentTime;

    FreqMeasure _freqMeasure;
    bool _lastClock = false;
    bool _gateOut = false;
    SqRingBuffer<ClockEvent, 32, true> _clockDelayLine;

    /**
     * @return true if clock is valid
     */
    bool processClockInput(bool input);

    /**
     * @return std::pair<bool, bool>, first is true if the clock went low to high,
     *      second is true on either transition
     */
    std::pair<bool, bool> updateClockStates(bool ck);

    void serviceDelayLine();
    bool shouldHandleEvent(const ClockEvent& event);
};

inline ClockShifter3::ClockShifter3() : _clockDelayLine(true) {
}

inline void ClockShifter3::setShift(float amount) {
    assertGE(amount, 0);
    // integral part is the number of clocks
    _requestedShift = ShiftMath::ClockWithPhase(amount);
    // assert(false);  // use new ctor
    // const int ck = std::floor(amount);
    // const float phase = amount - ck;
    // _requestedShift = ShiftMath::ClockWithPhase(ck, phase);
}

inline bool ClockShifter3::processClockInput(bool ckIn) {
    const bool freqWasValid = _freqMeasure.freqValid();
    _freqMeasure.onSample(ckIn);
    return freqWasValid;
}

inline std::pair<bool, bool> ClockShifter3::updateClockStates(bool ck) {
    bool didTick = false;
    bool wasTransition = false;
    if (ck != _lastClock) {
        wasTransition = true;
        if (ck) {
            _currentTime.advanceClockAndWrap();
            didTick = true;
        }
    }
    _lastClock = ck;
    return std::make_pair(didTick, wasTransition);
}

inline void ClockShifter3::serviceDelayLine() {
    if (_clockDelayLine.empty()) {
        return;
    }
    const auto ev = _clockDelayLine.peek();
    if (!shouldHandleEvent(ev)) {
        return;
    }
    _clockDelayLine.pop();
    // SQINFO("process delay event %d", int(ev._type));

    switch (ev._type) {
        case EventType::lowToHigh:
            _gateOut = true;
            break;
        case EventType::highToLow:
            _gateOut = false;
            break;
        default:
            assert(false);
    }
}

inline bool ClockShifter3::shouldHandleEvent(const ClockEvent& event) {
    if (!_freqMeasure.freqValid()) {
        return false;
    }

    const int periodOfClock = _freqMeasure.getPeriod();
    const auto shiftAmountSamples = ShiftMath::ph2s(_requestedShift, periodOfClock);
    const auto targetTime = ShiftMath::addWithWrap(event._timeStamp, shiftAmountSamples, periodOfClock);
    const auto ret = ShiftMath::exceedsOrEquals(_currentTime, targetTime);
    return ret;
}

inline bool ClockShifter3::run(bool input) {
    const auto clockValid = processClockInput(input);
    if (!clockValid) {
        return false;
    }
    const auto clockTransitionStatus = updateClockStates(input);
    const bool wasTransition = clockTransitionStatus.second;
    const bool didTick = clockTransitionStatus.first;

    if (wasTransition) {
        ClockEvent ev;
        ev._timeStamp = _currentTime;
        ev._type = didTick ? EventType::lowToHigh : EventType::highToLow;
        _clockDelayLine.push(ev);
    }

    serviceDelayLine();

    if (!didTick) {
        _currentTime._samples++;
    }

    return _gateOut;
}