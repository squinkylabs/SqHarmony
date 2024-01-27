
#pragma once

#include "ClockShifter5.h"
#include <memory>

static int clockItLow(std::shared_ptr<ClockShifter5> shifter, int count, float shift) {
    //SQINFO("clock it low %d", count);
    int clocksSeen = 0;
    for (int i = 0; i < count; ++i) {
        const bool b = shifter->process(false, false, shift);
        if (b) {
            clocksSeen++;
        }
    }
    return clocksSeen;
}

// Clocks high once, then low for period-1 cycles.
static int clockIt(std::shared_ptr<ClockShifter5> shifter, int period, float shift) {
    int clocksSeen = 0;
    if (shifter->process(true, true, shift)) {
        clocksSeen++;
    }
    return clocksSeen + clockItLow(shifter, period - 1, shift);
}

// Sends two clocks, with totalPeriod-1 non-clocks in-between.
// returns clock
static bool prime(std::shared_ptr<ClockShifter5> shifter, int totalPeriod, float shift) {
    assert(totalPeriod > 1);
    clockIt(shifter, totalPeriod, shift);
    const bool b = shifter->process(true, true, shift);
    assert(shifter->freqValid());
    return b;
}

static std::shared_ptr<ClockShifter5> makeAndPrime(int totalPeriod, float shift = 0) {
    std::shared_ptr<ClockShifter5> shifter = std::make_shared<ClockShifter5>();
  //  shifter->setShift(shift);
    prime(shifter, totalPeriod, shift);
    return shifter;
}

class PrimeResult {
public:
    std::shared_ptr<ClockShifter5> shifter;
    bool clocked = false;
};

static PrimeResult makeAndPrime2(int totalPeriod, float shift = 0) {
    std::shared_ptr<ClockShifter5> shifter = std::make_shared<ClockShifter5>();
  //  shifter->setShift(shift);
    const bool b = prime(shifter, totalPeriod, shift);
    PrimeResult result;
    result.shifter = shifter;
    result.clocked = b;
    return result;
}

