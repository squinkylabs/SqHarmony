

#include "asserts.h"
#include "testShifter5TestUtils.h"

// More special case test files

/**
 * Tests the case where a clock and a delay crossover happen at the same time at zero
 * 
 */
void testWrapAtZero()  {
    float shift = .1;
    auto primeResult = makeAndPrime2(10, shift);

    assertEQ(primeResult.clocked, false);
  //  SQINFO("+++++ ok, primed");
    auto shifter = primeResult.shifter;

    //now, since there is a small shift, when we skip up pretty far, we will clock.
    int clocks = clockItLow(shifter, 9, shift);
    assertEQ(clocks, 1);
 //   SQINFO("end of test period a");

    // now it's ready to pop, let's lock it.
    bool b = shifter->process(true, true, .999);
  //  SQINFO("clocked on last = %d (should be true)", b);
    assertEQ(b, true);
    b = shifter->process(false, false, .999);
  //  SQINFO("clocked on last = %d", b);
    b = shifter->process(false, false, .999);
  //  SQINFO("clocked on last = %d", b);

}

static void  testSlowDownBug() {
    assert(false);
 }

void testClockShifter5e() {
    testWrapAtZero();
    testSlowDownBug();
}

#if 1
void testFirst() {
    // testSpeedUp();

    // max bigger than 5300 is bad
    // These are the exact numbers picked of from a bigger run.
    SQINFO("---- testFirst ----");
   // testWrapAtZero();
   testSlowDownBug();
}
#endif