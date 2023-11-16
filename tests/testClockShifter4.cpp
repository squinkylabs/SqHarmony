
#include "ClockShifter4.h"
#include "asserts.h"

using Comp = ClockShifter4;
using CompPtr = std::shared_ptr<Comp>;

// clocks high once, then low
static void clockIt(CompPtr shifter, int cyclesLow) {
    shifter->run(true);
    for (int i = 0; i < cyclesLow; ++i) {
        shifter->run(false);
    }
}

// Sends two clocks, with 7 non-clocks in-between.
static void prime(CompPtr shifter) {
    clockIt(shifter, 7);
    shifter->run(true);
    assert(shifter->freqValid());
}

static CompPtr makeAndPrime() {
    CompPtr shifter = std::make_shared<Comp>();
    prime(shifter);
    return shifter;
}

static void testCanCall() {
    ClockShifter4 c;
    c.setShift(.5);
    c.run(2);
    c.freqValid();
}

static void testInputValid() {
    CompPtr shifter = std::make_shared<Comp>();
    assertEQ(shifter->freqValid(), false);
    shifter = makeAndPrime();
    assertEQ(shifter->freqValid(), true);
}

static void testStraightThrough() {
    CompPtr shifter = makeAndPrime();
    bool b = shifter->run(false);
    assert(!b);
    b = shifter->run(false);
    assert(!b);
    b = shifter->run(true);
    assert(b);
}

static void testHalfCycleDelay() {
    // 8 periods, just at start
    CompPtr shifter = makeAndPrime();
    shifter->setShift(.5);
    bool b = shifter->run(false);
    assertEQ(b, false);
    b = shifter->run(false);
    assertEQ(b, false);
    b = shifter->run(false);
    assertEQ(b, false);
    b = shifter->run(false);
    assertEQ(b, true);


}

void testClockShifter4() {
    testCanCall();

    // this test should be re-written
   // testStraightThrough();
    testInputValid();
    testHalfCycleDelay();
    // ClockShifter4Test::testInit();
    //  ClockShifter4Test::testCounters();
    //  ClockShifter4Test::testSetShift();
    //  ClockShifter4Test::testFreqMeasure();
    //  ClockShifter4Test::testDelay();
    //  ClockShifter4Test::testServiceDelay();
}