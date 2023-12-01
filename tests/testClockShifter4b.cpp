
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
    
    // Case 1. Current position < 1, shift move from higher to even higher. Shift not near the ends. 
    // (this needs work)
    static void testCase1() {
        auto shifter = makeAndPrime(testPeriod, .5);

        // We are will in front of the shift amount now
        bool b = clockItLow(shifter, 2);
        assert(!b);
        assertClose(shifter->getNormalizedPosition(), .2, .0001);

        // Move shift even father away - we don't care.
        shifter->setShift(.7);

        // Clock to the last clock before roll the new shift time.
        b = clockItLow(shifter, 4);
        assert(!b);
        assertClose(shifter->getNormalizedPosition(), .6, .0001);

        // Now when we hit the shift we should output a clock.
        b = clockItLow(shifter, 1);
        assert(b);

        // Now two more low to get the end of first cycle.
        b = clockItLow(shifter, 2);
        assert(!b);
        assertClose(shifter->getNormalizedPosition(), .9, .0001);

        // now time for an input clock (period 10)
        b = shifter->process(true, true);
        assert(!b);
        assertEQ(shifter->getNormalizedPosition(), 0);

        // now take us up to 6 (clock + 5 low in second period)
        b = clockItLow(shifter, 6);
        assertClose(shifter->getNormalizedPosition(), .6, .0001);
        assert(!b);

        // Now when we hit the shift we should output a clock.
        b = clockItLow(shifter, 1);
        assert(b);
    }

    // Case 2. Shift moves from < current position to > current position.
    // shift starts at .3 current position goes to .6, then shift goes to .7
    // input clocks x---------x---------x
    // shift move   ---| ->|--.---
    // position     ------x---.
    // shift value  --.3   .7-.----
    // exp out      ---x------.------x-----
    static void testCase2() {
        auto shifter = makeAndPrime(testPeriod, .3);
        bool b = clockItLow(shifter, 2);         // should take us up close, to 2
        assert(!b);
        assertClose(shifter->getNormalizedPosition(), .2, .0001);

        // next one to 3, where we should output a clock
        b = clockItLow(shifter, 1);
        assert(b);
        assertClose(shifter->getNormalizedPosition(), .3, .0001);

        // now to .6, where we will to the test.
        b = clockItLow(shifter, 3);
        assert(!b);
        assertClose(shifter->getNormalizedPosition(), .6, .0001);

        // now set shift to .7, on the other side of where we are
        shifter->setShift(.7);

        // now take us to the end. should have not clocks in here
        b = clockItLow(shifter, 3);
        assert(!b);

        assert(false);
    }
};

void testClockShifter4b() {
    TestClockShifter::testCase1();
    TestClockShifter::testCase2();
}