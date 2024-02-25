#pragma once


#include <assert.h>

//#include <tuple>

// #include "FreqMeasure2.h"
// #include "OneShotSampleTimer.h"

#include <cstdint>
//#include <vector>

#include "SqLog.h"
#include "BitDelay.h"

class ClockShifter6 {
public:
    friend class TestX;
    enum class Errors {
        NoError,
        ExceededDelaySize,
        LostClocks
    };
    bool process(bool clock, float delay, Errors* error = nullptr);
    void setMaxDelaySamples(unsigned samples);

private:
    BitDelay _bitDelay;  
};
