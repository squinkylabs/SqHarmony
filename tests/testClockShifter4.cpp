
#include "ClockShifter4.h"
#include "asserts.h"

using Comp = ClockShifter4;
using CompPtr = std::shared_ptr<Comp>;

// clocks high once, then low for period-1 cycles.
static int clockIt(CompPtr shifter, int period) {
    int clocksSeen = 0;
    if (shifter->process(true, true)) {
        clocksSeen++;
    }
    period--;  // just did one
    for (int i = 0; i < period; ++i) {
        const bool b = shifter->process(false, false);
        if (b) {
            clocksSeen++;
        }
    }
    return clocksSeen;
}

// Sends two clocks, with totalPeriod-1 non-clocks in-between.
static void prime(CompPtr shifter, int totalPeriod) {
    assert(totalPeriod > 1);
    clockIt(shifter, totalPeriod);
    shifter->process(true, true);
    assert(shifter->freqValid());
}

static CompPtr makeAndPrime(int totalPeriod) {
    CompPtr shifter = std::make_shared<Comp>();
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
    SQINFO("----- testHalfCycleDelay");
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
    CompPtr shifter = makeAndPrime(period);
    bool sawAClock = false;
    shifter->setShift(rawDelay);
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

static void testPosThenNeg() {
    const int period = 12;
    CompPtr shifter = makeAndPrime(period);
    int clocksReceived = 0;
    int clocksSent = 0;
    shifter->setShift(.1);  // set for small delay

    assert(false);      //finish me
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
        SQINFO("----- testHalfCycleDelay");
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
};

void testClockShifter4() {
    testCanCall();
    TestClockShifter4::testPeriod();
    TestClockShifter4::testMakeAndPrime();
    testStraightThrough();
    testStraightThrough2();
    testInputValid();
    testHalfCycleDelay();
    // testHalfCycleDelay2();
    // testDelay();
    testDelayNeg();
    testClockIt();
    testPosThenNeg();
}