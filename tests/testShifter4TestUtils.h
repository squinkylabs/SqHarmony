
#pragma once

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
static void prime(std::shared_ptr<ClockShifter4> shifter, int totalPeriod) {
    assert(totalPeriod > 1);
    clockIt(shifter, totalPeriod);
    shifter->process(true, true);
    assert(shifter->freqValid());
}

static std::shared_ptr<ClockShifter4> makeAndPrime(int totalPeriod, float shift = 0) {
    std::shared_ptr<ClockShifter4> shifter = std::make_shared<ClockShifter4>();
    shifter->setShift(shift);
    prime(shifter, totalPeriod);
    return shifter;
}

