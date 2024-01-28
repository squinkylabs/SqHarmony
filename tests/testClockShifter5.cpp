
#include "ClockShifter5.h"
#include "SqLog.h"
#include "asserts.h"
#include "testShifter5TestUtils.h"

static void testCanCall() {
    ClockShifter5 c;
    //  c.setShift(.5);
    c.process(true, true, .5);
    c.freqValid();
}

static void testInputValid() {
    auto shifter = std::make_shared<ClockShifter5>();
    assertEQ(shifter->freqValid(), false);
    shifter = makeAndPrime(4);
    assertEQ(shifter->freqValid(), true);
}

static void testStraightThrough() {
    auto primeResult = makeAndPrime2(8);
    auto shifter = primeResult.shifter;
    assertEQ(primeResult.clocked, true);        // Since no shift, should get a clock during prime.
    bool b = shifter->process(false, false, 0);
    assert(!b);
    b = shifter->process(false, false, 0);
    assert(!b);
    b = shifter->process(true, true, 0);
    assert(b);

    b = shifter->process(false, false, 0);
    assert(!b);
     b = shifter->process(false, false, 0);
    assert(!b);
}

#if 0
static void testStraightThrough3() {
    auto shifter = makeAndPrime(8);
    for (int i = 0; i < 7; ++i) {
        bool b = shifter->process(false, false);
        assert(!b);
    }

    const bool b = shifter->process(true, true);
    assert(b);
}

static void testStraightThrough2() {
    auto shifter = std::make_shared<ClockShifter4>();
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
    auto shifter = makeAndPrime(8);
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
    auto shifter = makeAndPrime(8);
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
    auto shifter = makeAndPrime(period, rawDelay);
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
    auto shifter = makeAndPrime(period);
    int x = clockIt(shifter, period);
    assertEQ(x, 1);
}

static void testSetDelayMidCycle() {
    const int period = 12;
    auto shifter = makeAndPrime(period);
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
    auto shifter = makeAndPrime(period, .5);
    // At this point we have put in trigger + 11 cycles no trigger + trigger
    int clocksGenerated = 0;
    clocksGenerated += clockItLow(shifter, 5);  // take almost to trigger.
    assertEQ(clocksGenerated, 0);
    clocksGenerated += shifter->process(false, false) ? 1 : 0;
    // now we should emit a clock from shift = .5;
    assertEQ(clocksGenerated, 1);

    clocksGenerated += clockItLow(shifter, 5);  // finish out this period
    assertEQ(clocksGenerated, 1);

    // period = 12, have sent 11?
    assertClose(shifter->getNormalizedPosition(), 11.f / 12.f, .0001);
    // now 1 clock gets to 0, next 5 get to 5/12
    clocksGenerated += clockIt(shifter, 6);  // almost up to next clock
    assertEQ(clocksGenerated, 1);
    assertClose(shifter->getNormalizedPosition(), 5.f / 12.f, .0001);

  //  SQINFO("now set shift to .6 pos is %f", 5.f / 12.f);
    shifter->setShift(.5 + .1);  // Set for small additional delay.
  //  SQINFO("back from set shift");

    // With shift still at .5, we would expect a clock here,
    // but with the extra .1 delay, that should be postponed.
    clocksGenerated += shifter->process(false, false) ? 1 : 0;
    assertEQ(clocksGenerated, 1);

 //   SQINFO("at 242");
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
    auto shifter = makeAndPrime(period, .5);
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

#endif

class TestX {
public:
#if 0
    static void testCalculateShiftOver1() {
        ClockShifter4::ShiftPossibilities x;
        const int testPeriod = 100;
        // Simple case that was giving errors.
        auto shifter = makeAndPrime(testPeriod, .5);
        assertClose(shifter->getNormalizedPosition(), 0, .0001);
        // .46 is less than half, so that triggers this error.
        clockItLow(shifter, 46);
        assertClose(shifter->getNormalizedPosition(), .46, .0001);

        // shift .5 to .6, will not cross .46
        x = shifter->_calculateShiftOver(.6);
        assert(x == ClockShifter4::ShiftPossibilities::ShiftOverNone);

        // .5 to .45, will cross .46
        x = shifter->_calculateShiftOver(.45);
        assert(x == ClockShifter4::ShiftPossibilities::ShiftOverBackward);

        // --------------------- now shift .5
        // now some simple cases with shift .5
        shifter = makeAndPrime(testPeriod, .5);
        assertClose(shifter->getNormalizedPosition(), 0, .0001);
        clockItLow(shifter, testPeriod / 2);
        assertClose(shifter->getNormalizedPosition(), .5, .0001);

        // Simple case - slightly later not over.
        x = shifter->_calculateShiftOver(.6);
        assert(x == ClockShifter4::ShiftPossibilities::ShiftOverNone);

        // Simple case - slightly earlier not over.
        x = shifter->_calculateShiftOver(.47);
        assert(x == ClockShifter4::ShiftPossibilities::ShiftOverNone);
    }
#endif

#if 0
    static void testCalculateShiftOver2() {
        const int testPeriod = 100;
        auto shifter = makeAndPrime(testPeriod, .5);
        assertClose(shifter->getNormalizedPosition(), 0, .0001);
        clockItLow(shifter, testPeriod / 2);
        assertClose(shifter->getNormalizedPosition(), .5, .0001);
        ClockShifter4::ShiftPossibilities x;

        // Shift from after to before
        shifter->_shift = .6;
        x = shifter->_calculateShiftOver(.4);
        assert(x == ClockShifter4::ShiftPossibilities::ShiftOverBackward);

        // Shift from before to after
        shifter->_shift = .4;
        x = shifter->_calculateShiftOver(.6);
        assert(x == ClockShifter4::ShiftPossibilities::ShiftOverForward);

        // Change is well downstream from us.
        shifter->_shift = .7;
        x = shifter->_calculateShiftOver(.8);
        assert(x == ClockShifter4::ShiftPossibilities::ShiftOverNone);

        // Shift from
        // Wrap, but way past position
        shifter->_shift = .9;  // Set the shift to after current pos.
        x = shifter->_calculateShiftOver(.1);
        assert(x == ClockShifter4::ShiftPossibilities::ShiftOverNone);

        // Wrap, but way past position
        shifter->_shift = .1;  // Set the shift to after current pos.
        x = shifter->_calculateShiftOver(.9);
        assert(x == ClockShifter4::ShiftPossibilities::ShiftOverNone);

        // Case from another test that isn't working. pos = .5, shift from .8 to .2.
        // OK, that was (correctly) wrapping, so let's make test not wrap.
        // .74 to .26 with pos = .5. That won't wrap.
        assertClose(shifter->getNormalizedPosition(), .5, .0001);
        shifter->_shift = .74;
        x = shifter->_calculateShiftOver(.26);
        assert(x == ClockShifter4::ShiftPossibilities::ShiftOverBackward);
    }
#endif

#if 0
    static void testCalculateShiftOver3() {
        const int testPeriod = 100;
        bool b;
        auto shifter = makeAndPrime(testPeriod, .5);
        assertClose(shifter->getNormalizedPosition(), 0, .0001);
        clockItLow(shifter, .95f * testPeriod);
        assertClose(shifter->getNormalizedPosition(), .95, .0001);
        ClockShifter4::ShiftPossibilities x;

        // Shift from .9 to 1.1, cur pos .95. Forward
        shifter->_shift = .9;
        x = shifter->_calculateShiftOver(.1);
        assert(x == ClockShifter4::ShiftPossibilities::ShiftOverForward);
        // ... then back.
        shifter->_shift = .1;
        x = shifter->_calculateShiftOver(.9);
        assert(x == ClockShifter4::ShiftPossibilities::ShiftOverBackward);


        // Shift from .9 to 1.1, cur pos 1.05
        // first to .99
        const int clocksRemaining = (.99 - .95) * testPeriod;
        b = clockItLow(shifter, clocksRemaining);
        assertClose(shifter->getNormalizedPosition(), .99, .0001);
        // now to zero.
        shifter->process(true, true);
        assertClose(shifter->getNormalizedPosition(), 0, .0001);
        // Now cur pos = .05
        b = clockItLow(shifter, .05 * testPeriod);
        assertClose(shifter->getNormalizedPosition(), .05, .0001);

      //  SQINFO("==== OK, here's the big wrap test ===");
        // Shift from .9 to 1.1, cur pos .05. Forward
        shifter->_shift = .9;
        x = shifter->_calculateShiftOver(.1);
        assert(x == ClockShifter4::ShiftPossibilities::ShiftOverForward);

        shifter->_shift = .1;
        x = shifter->_calculateShiftOver(.9);
        assert(x == ClockShifter4::ShiftPossibilities::ShiftOverBackward);
    }
#endif

    static void testPeriod() {
        auto shifter = std::make_shared<ClockShifter5>();
        bool b = shifter->process(false, false, 0);
        assertEQ(shifter->_freqMeasure.freqValid(), false);
        assert(!b);
        b = shifter->process(false, false, 0);
        assertEQ(shifter->_freqMeasure.freqValid(), false);

        // first clock
        b = shifter->process(true, true, 0);
        assertEQ(shifter->_freqMeasure.freqValid(), false);

        // four low clocks.
        for (int i = 0; i < 4; ++i) {
            bool b = shifter->process(false, false, 0);
            assertEQ(shifter->_freqMeasure.freqValid(), false);
        }
        // now this clock should make us stable.
        b = shifter->process(true, true, 0);
        assertEQ(shifter->_freqMeasure.freqValid(), true);
        assertEQ(shifter->_freqMeasure.getPeriod(), 5);
    }

    static void testMakeAndPrime() {
        const int period = 13;
        auto shifter = makeAndPrime(period);
        assertEQ(shifter->_freqMeasure.freqValid(), true);
        assertEQ(shifter->_freqMeasure.getPeriod(), 13);
    }

     static void testMakeAndPrime2() {
        const int period = 13;
        auto primeResult = makeAndPrime2(period);
        assertEQ(primeResult.clocked, true);
        auto shifter = primeResult.shifter;
        assertEQ(shifter->_freqMeasure.freqValid(), true);
        assertEQ(shifter->_freqMeasure.getPeriod(), 13);
    }

#if 0
    static void testHalfCycleDelay() {
        // 8 periods, just at start
        auto shifter = makeAndPrime(8);
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
#endif
};

void testClockShifter5() {
    testCanCall();
    SQINFO("---- implement all the following, from test clock shifter 4");
    TestX::testPeriod();
    TestX::testMakeAndPrime();
    TestX::testMakeAndPrime2();
    //     TestX::testGetNormalizedPosition();
    //     TestX::testCalculateShiftOver1();
    //     TestX::testCalculateShiftOver2();
    //     Tes::testCalculateShiftOver3();

    testStraightThrough();
    // testStraightThrough2();
    // testStraightThrough3();
    // testInputValid();

    // testHalfCycleDelay();
    // testHalfCycleDelay2();
    // testClockIt();
    // testDelay();
    // testDelayNeg();

    // testSetDelayMidCycle();
    // testIncreaseDelayMidCycle();
}