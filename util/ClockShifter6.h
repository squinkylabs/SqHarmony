#pragma once

#include <assert.h>

#include <cstdint>

#include "BitDelay2.h"
#include "FreqMeasure3.h"
#include "OneShotSampleTimer.h"
#include "SqLog.h"

extern int logLevel;

using BD = BitDelay2;

class ClockShifter6 {
public:
    friend class TestX;
    enum class Errors {
        NoError = static_cast<int>(BD::Errors::NoError),
        ExceededDelaySize = static_cast<int>(BD::Errors::ExceededDelaySize),
        LostClocks = static_cast<int>(BD::Errors::LostClocks)
    };

    /**
     * @brief
     *
     * @param clock is the clock we are delaying - the input clock. Leading edge is trigger.
     * @param delay is normalized to 1 == one clocking input period.
     * @param error is where errors are returned to the caller. nullptr is legal.
     * @return the delayed clock.
     */
    bool process(bool clock, float delay, Errors* error = nullptr);

    bool freqValid() const;
    unsigned getPeriod() const;

private:
    BD _bitDelay;

    // was FreqMeasure2, when we had trigger input
    // I think we need FreqMesaure3.
    FreqMeasure3 _freqMeasure;
    //    OneShotSampleTimer _clockWidthGenerator;
    int _debug_counter = 0;

    float _lastDelay = -1;

    bool _lastClock = false;
};

inline bool ClockShifter6::freqValid() const {
    return _freqMeasure.freqValid();
}

inline unsigned ClockShifter6::getPeriod() const {
    return _freqMeasure.getPeriod();
}

inline bool ClockShifter6::process(bool clock, float delay, Errors* error) {
    if (logLevel > 0) {
        SQINFO("cs6::process(%d, %f)", clock, delay);
        SQINFO("counter = %d", _debug_counter);
        if (clock && !_lastClock) {
            SQINFO("++++ cs6 seeing new clock");
        }
        _lastClock = clock;
    }
    if (delay != _lastDelay) {
        //SQINFO("delay = %f", delay);
        _lastDelay = delay;
    }

    if (error) {
        *error = Errors::NoError;
    }
    _freqMeasure.process(clock);

    if (!_freqMeasure.freqValid()) {
        if (logLevel > 0) SQINFO("leaving unstable");
        return false;
    }
    
    const unsigned delayAbsolute = unsigned(float(_freqMeasure.getPeriod()) * delay);
    if (logLevel) {
        SQINFO("cs6 derived delay samp =%d from period= %d, delay=%f prod=%f",
               delayAbsolute,
               _freqMeasure.getPeriod(),
               delay,
               float(_freqMeasure.getPeriod()) * delay);
    }
    ++_debug_counter;
    BD::Errors bderr;
    float ret = _bitDelay.process(clock, delayAbsolute, &bderr);
    if (error) {
        *error = Errors(bderr);
    }

    if (ret) {
        if (logLevel > 0) SQINFO("cs6: process output clock");
    } else {
        if (logLevel > 0) SQINFO("cs6: process no clock");
    }
    return ret;
}

//inline void ClockShifter6::setMaxDelaySamples(unsigned samples) {
//    _bitDelay.setMaxDelaySamples(samples);
//}
