
#pragma once

#include "ClockShifter4.h"
#include <memory>

static int clockItLow(std::shared_ptr<ClockShifter4> shifter, int count) {
    int clocksSeen = 0;
    for (int i = 0; i < count; ++i) {
        const bool b = shifter->process(false, false);
        if (b) {
            clocksSeen++;
        }
    }
    return clocksSeen;
}

// Clocks high once, then low for period-1 cycles.
static int clockIt(std::shared_ptr<ClockShifter4> shifter, int period) {
    int clocksSeen = 0;
    if (shifter->process(true, true)) {
        clocksSeen++;
    }
    return clocksSeen + clockItLow(shifter, period - 1);
}

// Sends two clocks, with totalPeriod-1 non-clocks in-between.
// returns clock
static bool prime(std::shared_ptr<ClockShifter4> shifter, int totalPeriod) {
    assert(totalPeriod > 1);
    clockIt(shifter, totalPeriod);
    const bool b = shifter->process(true, true);
    assert(shifter->freqValid());
    return b;
}

static std::shared_ptr<ClockShifter4> makeAndPrime(int totalPeriod, float shift = 0) {
    std::shared_ptr<ClockShifter4> shifter = std::make_shared<ClockShifter4>();
    shifter->setShift(shift);
    prime(shifter, totalPeriod);
    return shifter;
}

class PrimeResult {
public:
    std::shared_ptr<ClockShifter4> shifter;
    bool clocked = false;
};

static PrimeResult makeAndPrime2(int totalPeriod, float shift = 0) {
    std::shared_ptr<ClockShifter4> shifter = std::make_shared<ClockShifter4>();
    shifter->setShift(shift);
    const bool b = prime(shifter, totalPeriod);
    PrimeResult result;
    result.shifter = shifter;
    result.clocked = b;
    return result;
}

