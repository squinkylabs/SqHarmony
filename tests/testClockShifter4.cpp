
#include "asserts.h"

#include "ClockShifter4.h"


static void testCanCall() {
    ClockShifter4 c;
    c.setShift(.5);
    c.run(2);
}

class ClockShifter4Test {
};

void testClockShifter4() {
    testCanCall();
    // ClockShifter4Test::testInit();
    // ClockShifter4Test::testCounters();
    // ClockShifter4Test::testSetShift();
    // ClockShifter4Test::testFreqMeasure();
    // ClockShifter4Test::testDelay();
    // ClockShifter4Test::testServiceDelay();
}