
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

static void testCanCall() {
    ClockShifter4 c;
    c.setShift(.5);
    c.process(true, true);
    c.freqValid();
}

static void testInputValid() {
    CompPtr shifter = std::make_shared<Comp>();
    assertEQ(shifter->freqValid(), false);
    shifter = makeAndPrime(4);
    assertEQ(shifter->freqValid(), true);
}

static void testStraightThrough() {
    CompPtr shifter = makeAndPrime(8);
    bool b = shifter->process(false, false);
    assert(!b);
    b = shifter->process(false, false);
    assert(!b);
    b = shifter->process(true, true);
    assert(b);
}

static void testStraightThrough2() {
    CompPtr shifter = std::make_shared<Comp>();
    // Prime for period = 4, 75% duty cycle. no shift
    shifter->process(true, true);
    shifter->process(false, true);
    shifter->process(false, true);
    shifter->process(false, false);
    // first clock
    bool b = shifter->process(true, true);
    assertEQ(b, true);

    // second clock
    b = shifter->process(false, false);
    assertEQ(b, true);

    // third clock
    b = shifter->process(false, false);
    assertEQ(b, true);

    // fourth clock
    b = shifter->process(false, false);
    assertEQ(b, false);
}

static void testHalfCycleDelay() {
    // 8 periods, just at start
    CompPtr shifter = makeAndPrime(8);
    shifter->setShift(.5);

    // 1
    bool b = shifter->process(false, false);
    assertEQ(b, false);
    // 2
    b = shifter->process(false, false);
    assertEQ(b, false);
    // 3
    b = shifter->process(false, false);
    assertEQ(b, false);

    // 4
    b = shifter->process(false, false);
    assertEQ(b, true);
    // assertEQ(b, false);

    // 5
    b = shifter->process(false, false);
    assertEQ(b, false);
}

static void testHalfCycleDelay2() {
    // 8 periods, just at start
    CompPtr shifter = makeAndPrime(8);
    shifter->setShift(.5);

    // 1
    bool b = shifter->process(false, false);
    assertEQ(b, false);
    // 2
    b = shifter->process(false, false);
    assertEQ(b, false);
    // 3
    b = shifter->process(false, false);
    assertEQ(b, false);
    // 4
    b = shifter->process(false, false);
    assertEQ(b, true);
    // 5
    b = shifter->process(false, false);
    assertEQ(b, false);
    // 6
    b = shifter->process(false, false);
    assertEQ(b, false);
    // 7
    b = shifter->process(false, false);
    assertEQ(b, false);
    // 8
    b = shifter->process(true, true);
    assertEQ(b, false);
    // 9 (1)
    b = shifter->process(false, false);
    assertEQ(b, false);
    // 10 (2)
    b = shifter->process(false, false);
    assertEQ(b, false);
    // 11 (3)
    b = shifter->process(false, false);
    assertEQ(b, false);

    // 12 (4) all the way back to the next cycle
    b = shifter->process(false, false);
    assertEQ(b, true);

    // 13 (5)
    b = shifter->process(false, false);
    assertEQ(b, false);
}

static void testDelaySub(int period, float rawDelay) {
    // SQINFO("--- testDelaySub period=%d, rawDelay = %f", period, rawDelay);
    CompPtr shifter = makeAndPrime(period, rawDelay);
    bool sawAClock = false;

    // Now get the normalized delay from rawDelay.
    float delay = rawDelay;
    while (delay < 0) {
        delay += 1.0;
    }
    while (delay > 1) {
        delay -= 1.0;
    }
    int expectedClockTime = (period * delay) - 1;
    for (int i = 0; i < period * 4; ++i) {
        const int rem = i % period;
        const bool b = (rem == 0) && (i != 0);
        // SQINFO("in loop i=%d b=%d i+1=%d expected=%d", i, b, i+1, expectedClockTime);
        const bool b2 = shifter->process(b, b);
        assertEQ(b2, (i == expectedClockTime));
        if (b2) {
            if (expectedClockTime < period) {
                ++expectedClockTime;  // Possible off by one error in our test? Doesn't matter.
            }
            expectedClockTime += period;
            sawAClock = true;
        }
    }
    assertEQ(sawAClock, true);
}

static void testClockIt() {
    const int period = 23;
    CompPtr shifter = makeAndPrime(period);
    int x = clockIt(shifter, period);
    assertEQ(x, 1);
}

static void testSetDelayMidCycle() {
    const int period = 12;
    CompPtr shifter = makeAndPrime(period);
    // At this point we have put in trigger + 11 cycles no trigger + trigger.

    // We got one clock during the prime, right at the end.
    int clocksReceived = 1;
    int clocksSent = 0;
    shifter->setShift(.1);                     // Set for small delay.
    clocksReceived += clockItLow(shifter, 7);  // Clock a few more time;
    assertEQ(clocksReceived, 1);
}

static void testIncreaseDelayMidCycle() {
    // start with period 12, shift .5
    const int period = 12;
    CompPtr shifter = makeAndPrime(period, .5);
    // At this point we have put in trigger + 11 cycles no trigger + trigger
    int clocksGenerated = 0;
    clocksGenerated += clockItLow(shifter, 5);  // take almost to trigger.
    assertEQ(clocksGenerated, 0);
    clocksGenerated += shifter->process(false, false) ? 1 : 0;
    // now we should emit a clock from shift = .5;
    assertEQ(clocksGenerated, 1);

    clocksGenerated += clockItLow(shifter, 5);  // finish out this period
    assertEQ(clocksGenerated, 1);

    clocksGenerated += clockIt(shifter, 6);  // almost up to next clock
    assertEQ(clocksGenerated, 1);

    shifter->setShift(.5 + .1);  // Set for small additional delay.

    // With shift still at .5, we would expect a clock here,
    // but with the extra .1 delay, that should be postponed.
    clocksGenerated += shifter->process(false, false) ? 1 : 0;
    assertEQ(clocksGenerated, 1);

    // now should clock, delayed
    clocksGenerated += shifter->process(false, false) ? 1 : 0;
    assertEQ(clocksGenerated, 2);

    clocksGenerated += clockItLow(shifter, 5);  // finish out this period
    assertEQ(clocksGenerated, 2);               // and no more clocks.
}

static void
testDecreaseDelayMidCycle() {
    // start with period 17, shift .5
    const int period = 17;
    CompPtr shifter = makeAndPrime(period, .5);
    // At this point we have put in trigger + 11 cycles no trigger + trigger
    int clocksGenerated = 0;
    clocksGenerated += clockItLow(shifter, 7);  // take almost to trigger. 8 too high, 6 too low.
    assertEQ(clocksGenerated, 0);
    clocksGenerated += shifter->process(false, false) ? 1 : 0;
    assertEQ(clocksGenerated, 1);

    // b
    clocksGenerated += clockItLow(shifter, 7);  // finish out this period
    assertEQ(clocksGenerated, 1);

#if 0  // this is without shift, for ref
    SQINFO("now clock and then 8 samples");
    clocksGenerated += clockIt(shifter, 8);          // almost up to next clock. 

    //c
    // this is without changing shift
    clocksGenerated += shifter->process(false, false) ? 1 : 0;
    assertEQ(clocksGenerated, 2);
#else
    // SQINFO("now clock and then 8 samples");
    clocksGenerated += clockIt(shifter, 8);  // almost up to next clock. (above, was 8)
    assertEQ(clocksGenerated, 1);
    //  SQINFO("just set shift amount");
    shifter->setShift(.5 - .1);  // this may move to "before" where we currently are.
    // c
    //  this is without changing shift
    clocksGenerated += shifter->process(false, false) ? 1 : 0;

    // nomally we would go new, but shirt put us earlier.
    //  assertEQ(clocksGenerated, 2);

    clocksGenerated += clockItLow(shifter, 2);
    assertEQ(clocksGenerated, 2);

#endif
    assert(false);
}

static void testPosThenNeg() {
    // test not written yet.
    assert(false);
}

static void testDelay() {
    testDelaySub(8, .5);
    testDelaySub(12, .5);
    testDelaySub(8, .25);
}

static void testDelayNeg() {
    testDelaySub(8, -.5);
    testDelaySub(8, -.25);
}

class TestClockShifter4 {
public:
    static void testCalculateShiftOver() {
        CompPtr shifter = makeAndPrime(100, .5);
        assertClose(shifter->getNormalizedPosition(), 0, .0001);
        ClockShifter4::ShiftPossibilities x;

        // Simple case - slightly later not over.
        x = shifter->_calculateShiftOver(.6);
        assert(x == ClockShifter4::ShiftPossibilities::ShiftOverNone);

        // Simple case - slightly earlier not over.
        x = shifter->_calculateShiftOver(.4);
        assert(x == ClockShifter4::ShiftPossibilities::ShiftOverNone);

        assert(false);
    }

    static void testPeriod() {
        CompPtr shifter = std::make_shared<Comp>();
        bool b = shifter->process(false, false);
        assertEQ(shifter->_freqMeasure.freqValid(), false);
        assert(!b);
        b = shifter->process(false, false);
        assertEQ(shifter->_freqMeasure.freqValid(), false);

        // first clock
        b = shifter->process(true, true);
        assertEQ(shifter->_freqMeasure.freqValid(), false);

        // four low clocks.
        for (int i = 0; i < 4; ++i) {
            bool b = shifter->process(false, false);
            assertEQ(shifter->_freqMeasure.freqValid(), false);
        }
        // now this clock should make us stable.
        b = shifter->process(true, true);
        assertEQ(shifter->_freqMeasure.freqValid(), true);
        assertEQ(shifter->_freqMeasure.getPeriod(), 5);
    }

    static void testMakeAndPrime() {
        const int period = 13;
        CompPtr shifter = makeAndPrime(period);
        assertEQ(shifter->_freqMeasure.freqValid(), true);
        assertEQ(shifter->_freqMeasure.getPeriod(), 13);
    }

    static void testHalfCycleDelay() {
        // 8 periods, just at start
        CompPtr shifter = makeAndPrime(8);
        shifter->setShift(.5);

        // 1
        bool b = shifter->process(false, false);
        assertEQ(b, false);
        // 2
        b = shifter->process(false, false);
        assertEQ(b, false);
        // 3
        b = shifter->process(false, false);
        assertEQ(b, false);

        // 4
        b = shifter->process(false, false);
        assertEQ(b, true);

        // 5
        b = shifter->process(false, false);
        assertEQ(b, false);
    }

    static void testGetNormalizedPosition() {
        auto shifter = makeAndPrime(10, 0);
        auto acc = shifter->_phaseAccumulator;
        assertEQ(acc, 0);
        assertEQ(shifter->getNormalizedPosition(), 0);
        clockItLow(shifter, 5);
        acc = shifter->_phaseAccumulator;
        assertEQ(acc, 5);
        assertEQ(shifter->getNormalizedPosition(), .5);
        clockItLow(shifter, 2);
        acc = shifter->_phaseAccumulator;
        assertEQ(acc, 7);
        assertClose(shifter->getNormalizedPosition(), .7, .0001);
    }
};

void testClockShifter4() {
    testCanCall();
    TestClockShifter4::testPeriod();
    TestClockShifter4::testMakeAndPrime();
    TestClockShifter4::testGetNormalizedPosition();
    TestClockShifter4::testCalculateShiftOver();

    testStraightThrough();
    testStraightThrough2();
    testInputValid();

    testHalfCycleDelay();
    testHalfCycleDelay2();
    testClockIt();
    testDelay();
    testDelayNeg();

    testSetDelayMidCycle();
    testIncreaseDelayMidCycle();
}