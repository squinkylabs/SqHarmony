
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

// This is not debugged yet.
static void testSpeedUpOutputsClockShifted() {
    auto shifter = makeAndPrime(10, .9);
    assertEQ(shifter->getPeriod(), 10);
    bool b = clockItLow(shifter, 7);
    assert(!b);
    // we sped up, so output one now
    b = shifter->process(true, true);
    assert(b);
    b = clockItLow(shifter, 7);
    assert(!b);
    // next cycle, we are still not firing (?)
    b = shifter->process(true, true);
    assert(b);
}

void testClockShifter4c() {
    testSpeedUp();
    testSlowDown();
    testSpeedUpOutputsClock();
    SQINFO("testSpeedUpOutputsClockShifted not working yet");
    //testSpeedUpOutputsClockShifted();
}