#pragma once

#include <cmath>

#include "assert.h"

class ShiftMath {
public:
    class ClockWithPhase {
    public:
        ClockWithPhase() {}
        ClockWithPhase(int clocks, float phase) : _clocks(clocks), _phase(phase) {
            assert(_clocks >= 0);
            assert(_phase >= 0);
            assert(_phase <= 1);
        }

        int _clocks = 0;
        float _phase = 0;
    };

    class ClockWithSamples {
    public:
        ClockWithSamples() {}
        ClockWithSamples(int clocks, int samples) : _clocks(clocks), _samples(samples) {}
        int _clocks = 0;
        int _samples = 0;
    };

    /**
     * @brief converts from clock/phase to clock/samples
     */
    inline static ClockWithSamples convert(const ClockWithPhase& input, int periodOfClock) {
        assert(periodOfClock > 0);
        const int clocks = input._clocks;
        const int samples = int(std::round(input._phase * double(periodOfClock)));
        assert(samples < periodOfClock);
        return ClockWithSamples(clocks, samples);
    }

    inline static ClockWithSamples addWithWrap(const ClockWithSamples& a, const ClockWithSamples& b, int periodOfClock) {
        return ClockWithSamples(a._clocks + b._clocks, a._samples + b._samples);
    }
};
