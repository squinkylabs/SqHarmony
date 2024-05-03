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
    ClockShifter6();
    /**
     * @brief
     *
     * @param clock is the clock we are delaying - the input clock. Leading edge is trigger.
     * @param delay is normalized to 1 == one clocking input period.
     * @param error is where errors are returned to the caller. nullptr is legal.
     * @return the delayed clock.
     */
    bool process(bool clock, float delay, Errors* error = nullptr);
    void setMaxDelaySamples(unsigned samples);

    bool freqValid() const;
    unsigned getPeriod() const;

private:
    BD _bitDelay;

    // was FreqMeasure2, when we had trigger input
    // I think we need FreqMesaure3.
    FreqMeasure3 _freqMeasure;
    //    OneShotSampleTimer _clockWidthGenerator;
    int _debug_counter = 0;
};

inline ClockShifter6::ClockShifter6() {
    _bitDelay.setMaxDelaySamples(100000);
}

inline bool ClockShifter6::freqValid() const {
    return _freqMeasure.freqValid();
}

inline unsigned ClockShifter6::getPeriod() const {
    return _freqMeasure.getPeriod();
}

inline bool ClockShifter6::process(bool clock, float delay, Errors* error) {
    if (logLevel > 0) {
        SQINFO("ClockShifter6::process(%d, %f, %p)", clock, delay, error);
        SQINFO("counter = %d", _debug_counter);
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
        SQINFO("sc6 derived delay samp =%d from period= %d, delay=%f prod=%f",
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

    //  if (ret) {
    //     _clockWidthGenerator.arm(_freqMeasure.getHighDuration());
    // } else {
    //     _clockWidthGenerator.run();
    //     ret = _clockWidthGenerator.isRunning();
    // }

    if (ret) {
        if (logLevel > 0) SQINFO("process output clock\n");
    } else {
        if (logLevel > 0) SQINFO("process no clock");
    }
    return ret;
}

inline void ClockShifter6::setMaxDelaySamples(unsigned samples) {
    _bitDelay.setMaxDelaySamples(samples);
}
