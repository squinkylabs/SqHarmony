#include "ClockShifter6.h"
#include "asserts.h"

static void testCanCreate() {
    ClockShifter6 c;
    c.setMaxDelaySamples(1);
    c.process(0, false);
}

static void canReturnSizeError() {
    ClockShifter6 c;
    c.setMaxDelaySamples(1);

    ClockShifter6::Errors err;

    c.process(false, 100, &err);
    assert(err == ClockShifter6::Errors::ExceededDelaySize);

    c.process(false, 1, &err);
    assert(err == ClockShifter6::Errors::NoError);
}

static void canDelayZero() {
    ClockShifter6 c;
    c.setMaxDelaySamples(0);
    ClockShifter6::Errors err;
    bool x = c.process(true, 0, &err);
    assertEQ(x, true);

    x = c.process(false, 0, &err);
    assertEQ(x, false);
}

void testClockShifter6() {
    testCanCreate();
    // canReturnSizeError();
    // canDelayZero();
}