#pragma once

#include "FreqMeasure.h"
#include "SqRingBuffer.h"
#include "ShiftMath.h"
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
        assertGE(amount, 0);
        // integral part is the number of clocks
        const int ck = std::floor(amount);
        const float phase = amount - ck;
        _requestedShift = ClockWithPhase(ck, phase);
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

        ClockWithSamples _timeStamp;
        EventType _type = EventType::highToLow;
    };

  //  float _requestedFractionShiftAmount = 0;
  //  int _requestedShiftAmountClocks = 0;
    ClockWithPhase _requestedShift;


    int _currentClockCount = 0;
    int _currentSampleCountSinceLastClock = 0;
    FreqMeasure _freqMeasure;
    bool _lastClock = false;
    SqRingBuffer<ClockEvent, 32, true> _clockDelayLine;

    /**
     * @return std::pair<bool, bool>, first true if effective clock is high, second it clock is valid
     */
    std::pair<bool, bool> processClockInput(float input);

    /**
     * @return std::pair<bool, bool>, first is true if the clock went low to high, 
     *      second is true on either transition
     */
    std::pair<bool, bool>  updateClockStates(bool ck);

    void serviceDelayLine();
    bool shouldHandleEvent(const ClockEvent& event);
};

inline ClockShifter3::ClockShifter3() : _clockDelayLine(true) {
}

inline std::pair<bool, bool> ClockShifter3::processClockInput(float input) {
    const bool ck = input > 5;  // TODO: use better (like previous clock shifters)

    const bool freqWasValid = _freqMeasure.freqValid();
    _freqMeasure.onSample(input);

    return std::make_pair(ck, freqWasValid);
}

inline  std::pair<bool, bool> ClockShifter3::updateClockStates(bool ck) {
    bool didTick = false;
    bool wasTransition = false;
    if (ck != _lastClock) {
        wasTransition = true;
        if (ck) {
            _currentClockCount++;
            _currentSampleCountSinceLastClock = 0;
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

    const ClockWithSamples& eventStartTime = ev._timeStamp;
  //  const ClockWithPhase currentSh
    ClockWithSamples targetTime = ev._timeStamp;

    SQINFO("finish me 108");
}

inline bool ClockShifter3::shouldHandleEvent(const ClockEvent& event) {
    SQINFO("finish me 112");
    return false;
}

inline float ClockShifter3::run(float input) {
    const auto proc = processClockInput(input);
    if (!proc.second) {
        return 0;
    }
    const auto clockTransitionStatus = updateClockStates(proc.first); 
    const bool wasTransition = clockTransitionStatus.second;
    const bool didTick = clockTransitionStatus.first;

    if (wasTransition) {
        ClockEvent ev;
      //  ev._samples = _currentSampleCountSinceLastClock;
     //   ev._clocks = _currentClockCount;
        ev._timeStamp = ClockWithSamples(_currentClockCount, _currentSampleCountSinceLastClock);
        ev._type = didTick ? EventType::lowToHigh : EventType::highToLow;
        _clockDelayLine.push(ev);
    }

    serviceDelayLine();

    if (!didTick) {
        ++_currentSampleCountSinceLastClock;
    }

    return 0;
}