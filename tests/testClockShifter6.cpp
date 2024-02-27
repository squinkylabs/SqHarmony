

#include "ClockShifter6.h"
#include "asserts.h"
#include "testShifter6TestUtils.h"

int ClockShifter6::llv = 0;

static void testCanCreate() {
    ClockShifter6 c;
    c.setMaxDelaySamples(1);
    c.process(false, .2f, 10, nullptr);
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

    c.process(false, false, 1, &err);
    assert(err == ClockShifter6::Errors::ExceededDelaySize);

    // clock period 100, can delay 1/100 of that
    c.process(false, .01f, 100, &err);
    assert(err == ClockShifter6::Errors::NoError);
}

static void canDelayZero() {
    // ClockShifter6 c;
    // c.setMaxDelaySamples(0);
    ClockShifter6::Errors err;
    // prime();
    auto shifter = makeAndPrime(10, 0);
    bool x = shifter->process(true, true, 0, &err);
    assertEQ(x, true);

    x = shifter->process(false, 0, 1000, &err);
    assertEQ(x, false);
}

static void testClockWidthZero() {
    auto shifter = makeAndPrime(10, 0);
    // sample zero - own feel clock with 3 high duty cycle.
    shifter->process(true, true, 0);
    shifter->process(false, true, 0);
    shifter->process(false, true, 0);

    // and 7 more low
    bool x = clockItLow(shifter, 7, 0);
    assert(!x);

    // three high out now
    x = shifter->process(true, true, 0);
    assert(x);
    x = shifter->process(false, false, 0);
    assert(x);
    x = shifter->process(false, false, 0);
    assert(x);
    x = shifter->process(false, false, 0);
    assert(!x);
}

void testClockShifter6() {
    testCanCreate();
    testInitNotStable();
    testInitStable();
    SQINFO("make size error work again");
    // canReturnSizeError();
    canDelayZero();
    testClockWidthZero();
}