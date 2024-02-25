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

void testClockShifter6() {
    testCanCreate();
    canReturnSizeError();
}