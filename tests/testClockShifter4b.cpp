
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
    // Case 1. Current position << 1, shift move from higher to even higher. Shift not near the ends.
    // shift starts at .5 current position goes to .2, then shift goes to .7
    // input clocks x---------x---------x
    // shift move   --| ->|--.---
    // position     --x------.
    // shift value  --.5 .7-.----
    // exp out      -------x--.-------x----
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
        bool b = clockItLow(shifter, 2);  // should take us up close, to 2
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

        // Now set shift to .7, on the other side of where we are
        shifter->setShift(.7);

        // Now take us to the end of this period. should have not clocks in here.
        // We might have one, but it should be suppressed.
        b = clockItLow(shifter, 3);
        assert(!b);

        // now ready for a high clock
        assertClose(shifter->getNormalizedPosition(), .9, .0001);
        // Give it the expected input clock
        b = shifter->process(true, true);
        assert(!b);

        // Now, since shift is .7, we should have 6 low.
        b = clockItLow(shifter, 6);
        assert(!b);

        // And then output.
        b = clockItLow(shifter, 1);
        assert(b);
    }

    // Case 3. Shift moves from > current position to < current position.
    // shift starts at .8 current position goes to .6, then shift goes to .2
    // input clocks x---------x---------x
    // position     ------x---.
    // shift value  --.8   .2-.----
    // exp out      ------x---.-x---
    // try to fix: instead of .8, use .7. instead of .2, use .5.
    static void testCase3() {
        auto shifter = makeAndPrime(testPeriod, .7);
        bool b = clockItLow(shifter, 5);  // should take us up close, to 5
        assert(!b);
        assertClose(shifter->getNormalizedPosition(), .5, .0001);

        // now move the shift and clock low
        shifter->setShift(.4);

        // Now when we clock is should synthesize and output clock.
        // But this is only a synthetic clock, it shouldn't reset phase and stuff.
        b = clockItLow(shifter, 1);
        assert(b);
        assertClose(shifter->getNormalizedPosition(), .6, .0001);

        b = clockItLow(shifter, 3);
        assert(!b);
        assertClose(shifter->getNormalizedPosition(), .9, .0001);

        b = shifter->process(true, true);
        assert(!b);
        assertClose(shifter->getNormalizedPosition(), 0, .0001);

        // Final shift was .4, so let's clock 3 - should be no clock.
        b = clockItLow(shifter, 3);
        assert(!b);
        assertClose(shifter->getNormalizedPosition(), .3, .0001);

        // now the clock
        b = clockItLow(shifter, 1);
        assert(b);
    }

    // Case 5. Shift "wraps" around zero, but current pos is well away.
    // shift starts at .9 current position goes to .5, then shift goes to .1
    // input clocks x---------x---------x
    // position     -----x----.
    // shift value  --.9   .1-.----
    // exp out      ----------.x----
    static void testCase5() {
        SQINFO("---- testCase5 -----");
        auto shifter = makeAndPrime(testPeriod, .9);
        bool b = clockItLow(shifter, 5);  // Should take us to middle, to 5.
        assert(!b);
        assertClose(shifter->getNormalizedPosition(), .5, .0001);

        // Mow move the shift and clock low to near end. should not change things for us
        SQINFO("--- will set shift ---");
        shifter->setShift(.1);
        SQINFO("--- will clock after set shift ---");
        b = clockItLow(shifter, 4);
        assert(!b);
        assertClose(shifter->getNormalizedPosition(), .9, .0001);

        // this clock won't tick, but will turn us over
        b = shifter->process(true, true);
        assert(!b);
        assertClose(shifter->getNormalizedPosition(), 0.f, .0001);

        // shift is .1, so one more sample should do it.
        b = clockItLow(shifter, 1);
        assert(b);
    }
};

void testClockShifter4b() {
    TestClockShifter::testCase1();
    TestClockShifter::testCase2();
    TestClockShifter::testCase3();
    TestClockShifter::testCase5();
}