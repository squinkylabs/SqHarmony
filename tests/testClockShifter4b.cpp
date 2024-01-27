
#include "ClockShifter4.h"
#include "testShifter4TestUtils.h"
#include "asserts.h"

const int testPeriod = 10;

/**
 * These are mainly tests around the different cases of shift times moving around.
 * 
 */

// Case 1. Current position << 1, shift move from higher to even higher. Shift not near the ends.
// shift starts at .5 current position goes to .2, then shift goes to .7
// input clocks x---------x---------x
// shift move   --| ->|--.---
// position     --x------.
// shift value  --.5 .7-.----
// exp out      -------x--.-------x----
static void testCase1() {
    auto shifter = makeAndPrime(testPeriod, .5);

    // We are will in front of the shift amount now
    bool b = clockItLow(shifter, 2);
    assert(!b);
    assertClose(shifter->getNormalizedPosition(), .2, .0001);

    // Move shift even father away - we don't care.
    shifter->setShift(.7);

    // Clock to the last clock before roll the new shift time.
    b = clockItLow(shifter, 4);
    assert(!b);
    assertClose(shifter->getNormalizedPosition(), .6, .0001);

    // Now when we hit the shift we should output a clock.
    b = clockItLow(shifter, 1);
    assert(b);

    // Now two more low to get the end of first cycle.
    b = clockItLow(shifter, 2);
    assert(!b);
    assertClose(shifter->getNormalizedPosition(), .9, .0001);

    // now time for an input clock (period 10)
    b = shifter->process(true, true);
    assert(!b);
    assertEQ(shifter->getNormalizedPosition(), 0);

    // now take us up to 6 (clock + 5 low in second period)
    b = clockItLow(shifter, 6);
    assertClose(shifter->getNormalizedPosition(), .6, .0001);
    assert(!b);

    // Now when we hit the shift we should output a clock.
    b = clockItLow(shifter, 1);
    assert(b);
}

// Case 2. Shift moves from < current position to > current position.
// shift starts at .3 current position goes to .6, then shift goes to .7
// input clocks x---------x---------x
// shift move   ---| ->|--.---
// position     ------x---.
// shift value  --.3   .7-.----
// exp out      ---x------.------x-----
static void testCase2() {
    auto shifter = makeAndPrime(testPeriod, .3);
    bool b = clockItLow(shifter, 2);  // should take us up close, to 2
    assert(!b);
    assertClose(shifter->getNormalizedPosition(), .2, .0001);

    // next one to 3, where we should output a clock
    b = clockItLow(shifter, 1);
    assert(b);
    assertClose(shifter->getNormalizedPosition(), .3, .0001);

    // now to .6, where we will to the test.
    b = clockItLow(shifter, 3);
    assert(!b);
    assertClose(shifter->getNormalizedPosition(), .6, .0001);

    // Now set shift to .7, on the other side of where we are
    shifter->setShift(.7f);

    // Now take us to the end of this period. should have no clocks in here.
    // We might have one, but it should be suppressed.
    b = clockItLow(shifter, 3);
    assert(!b);

    // now ready for a high clock
    assertClose(shifter->getNormalizedPosition(), .9, .0001);
    // Give it the expected input clock
    b = shifter->process(true, true);
    assert(!b);

    // Now, since shift is .7, we should have 6 low.
    b = clockItLow(shifter, 6);
    assert(!b);

    // And then output.
    b = clockItLow(shifter, 1);
    assert(b);
}

// Case 3. Shift moves from > current position to < current position.
// shift starts at .8 current position goes to .6, then shift goes to .2
// input clocks x---------x---------x
// position     ------x---.
// shift value  --.8   .2-.----
// exp out      ------x---.-x---
// try to fix: instead of .8, use .7. instead of .2, use .5.
static void testCase3() {
    auto shifter = makeAndPrime(testPeriod, .7);
    bool b = clockItLow(shifter, 5);  // should take us up close, to 5
    assert(!b);
    assertClose(shifter->getNormalizedPosition(), .5, .0001);

    // now move the shift and clock low
    shifter->setShift(.4);

    // Now when we clock is should synthesize and output clock.
    // But this is only a synthetic clock, it shouldn't reset phase and stuff.
    b = clockItLow(shifter, 1);
    assert(b);
    assertClose(shifter->getNormalizedPosition(), .6, .0001);

    b = clockItLow(shifter, 3);
    assert(!b);
    assertClose(shifter->getNormalizedPosition(), .9, .0001);

    b = shifter->process(true, true);
    assert(!b);
    assertClose(shifter->getNormalizedPosition(), 0, .0001);

    // Final shift was .4, so let's clock 3 - should be no clock.
    b = clockItLow(shifter, 3);
    assert(!b);
    assertClose(shifter->getNormalizedPosition(), .3, .0001);

    // now the clock
    b = clockItLow(shifter, 1);
    assert(b);
}

// Case 5. Shift "wraps" around zero, but current pos is well away.
// shift starts at .9 current position goes to .5, then shift goes to .1
// input clocks x---------x---------x
// position     -----x----.
// shift value  --.9   .1-.----
// exp out      ----------.x----
static void testCase5() {
    auto shifter = makeAndPrime(testPeriod, .9);
    bool b = clockItLow(shifter, 5);  // Should take us to middle, to 5.
    assert(!b);
    assertClose(shifter->getNormalizedPosition(), .5, .0001);

    // Mow move the shift and clock low to near end. should not change things for us
    shifter->setShift(.1);
    b = clockItLow(shifter, 4);
    assert(!b);
    assertClose(shifter->getNormalizedPosition(), .9, .0001);

    // this clock won't tick, but will turn us over
    b = shifter->process(true, true);
    assert(!b);
    assertClose(shifter->getNormalizedPosition(), 0.f, .0001);

    // shift is .1, so one more sample should do it.
    b = clockItLow(shifter, 1);
    assert(b);
}

// Case 6a. Shift "wraps" around zero, crossing current pos. pos still in first half.
// shift starts at .8 current position goes to .9, then shift goes to .1
// input clocks x---------x---------x
// position     ---------x.
// shift value  --.9   .1-.----
// exp out      ----------.x----
static void testCase6a() {
    // Make the shifter with the desired delay, click until we almost expect a clock.
    auto shifter = makeAndPrime(testPeriod, .8);
    bool b = clockItLow(shifter, 7);
    assert(!b);
    assertClose(shifter->getNormalizedPosition(), .7, .0001);

    // Next sample will generate a clock as we cross the shift location.
    b = clockItLow(shifter, 1);
    assert(b);
    assertClose(shifter->getNormalizedPosition(), .8, .0001);

    // One more sample and we will be at the end of this cycle.
    b = clockItLow(shifter, 1);
    assert(!b);
    assertClose(shifter->getNormalizedPosition(), .9, .0001);

    // Now, for the test case, we move the shift from before cur to after.
    shifter->setShift(.1);

    // And the input clock. We generate an output for the shift passing over us.
    b = shifter->process(true, true);
    assert(!b);
    assertClose(shifter->getNormalizedPosition(), 0, .0001);

    // Again, run samples until almost the end. No reason to generate a clock here.
    b = clockItLow(shifter, 9);
    assert(!b);
    assertClose(shifter->getNormalizedPosition(), .9, .0001);

    // now the input clock. This time there is no "debt", so we won't generate.
    b = shifter->process(true, true);
    assert(!b);
    assertClose(shifter->getNormalizedPosition(), 0, .0001);

    // Now the sample takes us to shift point, so we should generate a normal delay clock.
    b = clockItLow(shifter, 1);
    assert(b);
    assertClose(shifter->getNormalizedPosition(), .1, .0001);

    b = clockItLow(shifter, 8);
    assert(!b);
    assertClose(shifter->getNormalizedPosition(), .9, .0001);
}

// Case 6b. Shift "wraps" around zero, crossing current pos. pos in second half.
// shift starts at .9 current position goes to .1, then shift goes to .2
// input clocks x---------x---------x
// position
// shift value
// exp out
static void testCase6b() {
    // Make the shifter with the desired delay, click until we almost expect a clock.
    auto shifter = makeAndPrime(testPeriod, .9);
    bool b = clockItLow(shifter, 8);
    assert(!b);
    assertClose(shifter->getNormalizedPosition(), .8, .0001);
    // Now another sample. This should be a normal shift delay.
    b = clockItLow(shifter, 1);
    assert(b);
    assertClose(shifter->getNormalizedPosition(), .9, .0001);

    // Now a normal input clock
    b = shifter->process(true, true);
    assert(!b);
    assertClose(shifter->getNormalizedPosition(), 0, .0001);

    // And one more to get where we want to be for the test case.
    b = clockItLow(shifter, 1);
    assert(!b);
    assertClose(shifter->getNormalizedPosition(), .1, .0001);

    // Move shift forward, over us.
    shifter->setShift(.2);

    // This sample takes pos to .2, at the shift, we don't owe any. Since
    // we just output one, this should be suppressed.
    b = clockItLow(shifter, 1);
    assert(!b);
    assertClose(shifter->getNormalizedPosition(), .2, .0001);

    // now clock out the rest of this cycle, and up to .1, right before where we expect one.
    b = clockItLow(shifter, 7);
    assert(!b);
    assertClose(shifter->getNormalizedPosition(), .9, .0001);

    // Now a normal input clock.
    b = shifter->process(true, true);
    assert(!b);
    assertClose(shifter->getNormalizedPosition(), 0, .0001);

    // and a sample to get us to .1
    b = clockItLow(shifter, 1);
    assert(!b);
    assertClose(shifter->getNormalizedPosition(), .1, .0001);

    // and we expect a clock here.
    b = clockItLow(shifter, 1);
    assert(b);
}

// Case 7a. Shift "wraps" backwards through zero, crossing current pos. pos in first half.
// shift starts at .1 current position goes to .9, then shift goes to .8
// input clocks x---------x---------x
// position
// shift value
// exp out
static void testCase7a() {
    // Make the shifter with the desired delay, click until we almost expect a clock.
    auto shifter = makeAndPrime(testPeriod, .1);

    // first sample take us to pos via normal case.
    bool b = clockItLow(shifter, 1);
    assert(b);
    assertClose(shifter->getNormalizedPosition(), .1, .0001);

    b = clockItLow(shifter, 8);
    assert(!b);
    assertClose(shifter->getNormalizedPosition(), .9, .0001);

    // now, move shifter to before us.
    shifter->setShift(.8);

    // input clock+sample here should kick us to .1, and should
    // generate a synthesized "make up" clock.
    b = shifter->process(true, true);
    assert(b);
    assertClose(shifter->getNormalizedPosition(), 0, .0001);

    b = clockItLow(shifter, 7);
    assert(!b);
    assertClose(shifter->getNormalizedPosition(), .7, .0001);

    b = clockItLow(shifter, 1);
    assert(b);
    assertClose(shifter->getNormalizedPosition(), .8, .0001);
}

void testClockShifter4b() {
    testCase1();
    testCase2();
    testCase3();
    testCase5();
    testCase6a();
    testCase6b();
    testCase7a();
    // TODO: testCase7b
}