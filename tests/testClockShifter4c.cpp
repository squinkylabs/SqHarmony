
#include "ClockShifter4.h"
#include "testShifter4TestUtils.h"
#include "asserts.h"

/**
 * These are mostly tests around the external clock speed changing.
 * 
 */

static void testSpeedUp() {
    auto shifter = makeAndPrime(10, 0);
    assertEQ(shifter->getPeriod(), 10);
    bool b = clockItLow(shifter, 7);
    assert(!b);
    shifter->process(true, true);
    assertEQ(shifter->getPeriod(), 8);
}

static void testSlowDown() {
    auto shifter = makeAndPrime(8, 0);
    assertEQ(shifter->getPeriod(), 8);
    bool b = clockItLow(shifter, 19);
    assert(!b);
    shifter->process(true, true);
    assertEQ(shifter->getPeriod(), 20);
}

static void testSpeedUpOutputsClock() {
    auto shifter = makeAndPrime(10, 0);
    assertEQ(shifter->getPeriod(), 10);
    bool b = clockItLow(shifter, 7);
    assert(!b);
    b = shifter->process(true, true);
    assert(b);
}

void testClockShifter4c() {
    testSpeedUp();
    testSlowDown();
    testSpeedUpOutputsClock();
}