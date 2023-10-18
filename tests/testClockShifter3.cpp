
#include "asserts.h"
#include "ClockShifter3.h"

static void testCanCall() {
    ClockShifter3 c;
    c.setShift(.5);
    c.run(2);
}

void testClockShifter3() {
    testCanCall();
}