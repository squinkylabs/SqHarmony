#pragma once

#include "SqLog.h"

#include <cmath>
#include <string>
#include <iostream>
#include <sstream>

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


        ClockWithPhase(double clocksAndPhase) {
            const int ck = int(std::floor(clocksAndPhase));
            const float phase = float(clocksAndPhase - ck);
            *this = ClockWithPhase(ck, phase);
        }

        int _clocks = 0;
        float _phase = 0;
    };

    class ClockWithSamples {
    public:
        ClockWithSamples() {}
        ClockWithSamples(int clocks, int samples) : _clocks(clocks), _samples(samples) {}
        bool operator==(const ClockWithSamples& other) const {
            return _clocks == other._clocks && _samples == other._samples;
        }
        bool operator!=(const ClockWithSamples& other) const {
            return !(*this == other);
        }
        void advanceClockAndWrap() {
            ++_clocks;
            _samples = 0;
        }
        void reset() {
            _clocks = 0;
            _samples = 0;
        }
        std::string toString() const {
            std::stringstream ss;
            ss << "clocks=" << _clocks << ", samples=" << _samples;
            return ss.str();
        }

        /**
         * @brief adds to "this" a number of samples. Wraps if appropriate/
         * 
         * @param samples 
         */
        void addDelta(int samples, int samplesInMasterPeriod) {
            _samples += samples;
            while(_samples >= samplesInMasterPeriod) {
                _samples -= samplesInMasterPeriod;
                ++_clocks;
            }
        }


        int _clocks = 0;
        int _samples = 0;
    };

   
    /**
     * @brief converts from clock/phase to clock/samples
     */
    inline static ClockWithSamples ph2s(const ClockWithPhase& input, int periodOfClock) {
        assert(periodOfClock > 0);
        const int clocks = input._clocks;
        const int samples = int(std::round(input._phase * double(periodOfClock)));
        assert(samples < periodOfClock);
        return ClockWithSamples(clocks, samples);
    }

     inline static ClockWithPhase s2ph(const ClockWithSamples& input, int periodOfClock) {
        assert(periodOfClock > 0);
        assert(input._clocks >= 0);
        assert(input._samples >= 0);

        const int clocks = input._clocks;
        double phase = double(input._samples) / double(periodOfClock);

        return ClockWithPhase(clocks, float(phase));
     }

    inline static ClockWithSamples addWithWrap(const ClockWithSamples& a, const ClockWithSamples& b, int periodOfClock) {
        auto result =  ClockWithSamples(a._clocks + b._clocks, a._samples + b._samples);
        while (result._samples >= periodOfClock) {
            result._clocks++;
            result._samples -= periodOfClock;
        }
        return result;
    }

   /**
    * @brief compares two ClockWithSamples
    * @returns true if a >= b
    */
    inline static bool exceedsOrEquals(const ClockWithSamples& a, const ClockWithSamples& b) {
        return (a._clocks > b._clocks) ||
            ((a._clocks == b._clocks) && (a._samples >= b._samples));
    }
};
