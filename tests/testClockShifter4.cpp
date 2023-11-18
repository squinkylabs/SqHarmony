
#include "ClockShifter4.h"
#include "asserts.h"

using Comp = ClockShifter4;
using CompPtr = std::shared_ptr<Comp>;

// clocks high once, then low
static void clockIt(CompPtr shifter, int cyclesLow) {
    shifter->process(true, true);
    for (int i = 0; i < cyclesLow; ++i) {
        shifter->process(false, false);  // Note very low duty cycle.
    }
}

// Sends two clocks, with 7 non-clocks in-between.
static void prime(CompPtr shifter, int totalPeriod) {
    assert(totalPeriod > 1);
    clockIt(shifter, totalPeriod - 1);
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

static void testDelay() {
    testDelaySub(8, .5);
    testDelaySub(12, .5);
    testDelaySub(8, .25);
}

static void testDelayNeg() {
    testDelaySub(8, -.5);
    testDelaySub(8, -.25);
}

void testClockShifter4() {
    testCanCall();
    testStraightThrough();
    testStraightThrough2();
    testInputValid();
    testHalfCycleDelay();
    testHalfCycleDelay2();
    testDelay();
    testDelayNeg();
}