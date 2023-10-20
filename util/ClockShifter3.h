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

    /**
     * @brief
     *
     * @param input
     * @return std::pair<bool, bool>, first true if effective clock is high, second it clock is valid
     */
    std::pair<bool, bool> processClockInput(float input);

    /**
     * @brief 
     * 
     * @param ck 
     * @return std::pair<bool, bool>, first is true if the clock went low to high, 
     *      second is true on either transition
     */
    std::pair<bool, bool>  updateClockStates(bool ck);
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

inline float ClockShifter3::run(float input) {
    const auto proc = processClockInput(input);
    if (!proc.second) {
        return 0;
    }
  //  const auto ck = proc.first;

    const auto clockTransitionStatus = updateClockStates(proc.first); 
    const bool wasTransition = clockTransitionStatus.second;
    const bool didTick = clockTransitionStatus.first;

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