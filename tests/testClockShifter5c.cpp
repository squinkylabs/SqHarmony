
#include "ClockShifter5.h"
#include "testShifter5TestUtils.h"
#include "asserts.h"

/**
 * These are mostly tests around the external clock speed changing.
 * 
 */

static void testSpeedUp() {
    float shift = 0;
    auto shifter = makeAndPrime(10, shift);
    assertEQ(shifter->getPeriod(), 10);
    bool b = clockItLow(shifter, 7, shift);
    assert(!b);
    shifter->process(true, true, shift);
    assertEQ(shifter->getPeriod(), 8);
}

static void testSlowDown() {
    float shift = 0;
    auto shifter = makeAndPrime(8, shift);
    assertEQ(shifter->getPeriod(), 8);
    bool b = clockItLow(shifter, 19, shift);
    assert(!b);
    shifter->process(true, true, shift);
    assertEQ(shifter->getPeriod(), 20);
}

static void testSpeedUpOutputsClock() {
    float shift = 0;
    auto shifter = makeAndPrime(10, shift);
    assertEQ(shifter->getPeriod(), 10);
    bool b = clockItLow(shifter, 7, shift);
    assert(!b);
    b = shifter->process(true, true, shift);
    assert(b);
}

// This is not debugged yet.
static void testSpeedUpOutputsClockShifted() {
    float shift = .9;
    auto shifter = makeAndPrime(10, shift);
    assertEQ(shifter->getPeriod(), 10);
    bool b = clockItLow(shifter, 7, shift);
    assert(!b);
    // we sped up, so output one now
    b = shifter->process(true, true, shift);
    assert(b);

    // Now we have measured a shorter period.
    b = clockItLow(shifter, 6, shift);  // then no more for the remaining short time
    assert(!b);
    // next cycle, we are still not firing (?)
    b = shifter->process(true, true, shift);
    assert(b);
}

void testClockShifter5c() {
    testSpeedUp();
    testSlowDown();
    testSpeedUpOutputsClock();
  //  SQINFO("testSpeedUpOutputsClockShifted not working yet");
    testSpeedUpOutputsClockShifted();
}