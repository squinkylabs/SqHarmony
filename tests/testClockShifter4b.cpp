
#include "ClockShifter4.h"
#include "asserts.h"

using Comp = ClockShifter4;
using CompPtr = std::shared_ptr<Comp>;

static int clockItLow(CompPtr shifter, int count) {
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
static int clockIt(CompPtr shifter, int period) {
    int clocksSeen = 0;
    if (shifter->process(true, true)) {
        clocksSeen++;
    }
    return clocksSeen + clockItLow(shifter, period - 1);
}

// Sends two clocks, with totalPeriod-1 non-clocks in-between.
static void prime(CompPtr shifter, int totalPeriod) {
    assert(totalPeriod > 1);
    clockIt(shifter, totalPeriod);
    shifter->process(true, true);
    assert(shifter->freqValid());
}

static CompPtr makeAndPrime(int totalPeriod, float shift = 0) {
    CompPtr shifter = std::make_shared<Comp>();
    shifter->setShift(shift);
    prime(shifter, totalPeriod);
    return shifter;
}

const int testPeriod = 10;
class TestClockShifter {
public:
    
    static void testCase1() {
        auto shifter = makeAndPrime(testPeriod, .8);
        assert(false);
    }

    static void testCase2() {
        auto shifter = makeAndPrime(testPeriod, .5);
        clockItLow(shifter, 4);         // should take us up close.
        shifter->getNormalizedPosition();
        assert(false);
    }
};

void testClockShifter4b() {
   // TestClockShifter::testCase1();
    TestClockShifter::testCase2();
}