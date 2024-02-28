

#include "ClockShifter6.h"
#include "asserts.h"
#include "testShifter6TestUtils.h"

int ClockShifter6::llv = 0;

static void testCanCreate() {
    ClockShifter6 c;
    c.setMaxDelaySamples(1);
    c.process(false, .2f, nullptr);
}

static void testInitNotStable() {
    ClockShifter6 c;
    c.setMaxDelaySamples(1);
    assertEQ(c.freqValid(), false);
}

static void testInitStable() {
    const auto ret = makeAndPrime2(10, 0);
    assertEQ(ret.shifter->freqValid(), true);
}

static void canReturnSizeError() {
    ClockShifter6 c;
    c.setMaxDelaySamples(1);

    ClockShifter6::Errors err;

    c.process(false, 1, &err);
    assert(err == ClockShifter6::Errors::ExceededDelaySize);

    c.process(false, .01f, &err);
    assert(err == ClockShifter6::Errors::NoError);
}

static void canDelayZero() {
    // ClockShifter6 c;
    // c.setMaxDelaySamples(0);
    ClockShifter6::Errors err;
    // prime();
    auto shifter = makeAndPrime(10, 0);
    bool x = shifter->process(true, 0, &err);
    assertEQ(x, true);

    x = shifter->process(false, 0, &err);
    assertEQ(x, false);
}

static void testClockWidthZeroDelayFiniteWidth() {
    const unsigned period = 8;
    const unsigned numHigh = 3;
    const unsigned numLow = 5;
    assertEQ(period, (numLow + numHigh));
    auto shifter = makeAndPrime(period, 0);
    SQINFO("done with prime");
    // above already put out one clock (end of prime)
    // here is #2
    shifter->process(true, 0, 0);

    // and here is #3
    shifter->process(true, 0, 0);
   // shifter->process(false, true, 0);

    // and 5 more low
    bool x = clockItLow(shifter, numLow, 0);
    assert(!x);

    // three high out now
    x = shifter->process(true, 0, 0);
    assert(x);
    x = shifter->process(true, 0, 0);
    assert(x);
    x = shifter->process(true, 0, 0);
    assert(x);
    x = shifter->process(false, 0, 0);
    assert(!x);
}

void testClockShifter6() {
    testCanCreate();
    testInitNotStable();
    testInitStable();
    SQINFO("make size error work again");
    // canReturnSizeError();
    canDelayZero();
    testClockWidthZeroDelayFiniteWidth();
}

#if 0
void testFirst() {
    ClockShifter6::llv = 1;
    //  This is the case that is bad without the dodgy "fix"
    // testWithLFO(4, 16, 0.136364, 0.400000, 3);

    // testSlowDown(5, 3552, 0.0001407658, 7);
    testClockWidthZeroDelayFiniteWidth();
}
#endif