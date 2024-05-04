

#include "ClockShifter6.h"
#include "asserts.h"
#include "testShifter6TestUtils.h"

static void testCanCreate() {
    ClockShifter6 c;
   // c.setMaxDelaySamples(1);
    c.process(false, .2f, nullptr);
}

static void testInitNotStable() {
    ClockShifter6 c;
   // c.setMaxDelaySamples(1);
    assertEQ(c.freqValid(), false);
}

static void testInitStable() {
    const auto ret = makeAndPrime2(10, 0);
    assertEQ(ret.shifter->freqValid(), true);
}

// static void canReturnSizeError() {
//     auto c = makeAndPrime(4, 0);
//     c->setMaxDelaySamples(1);


//     ClockShifter6::Errors err;

//     // Need a huge delay here to get the error, since bit delay
//     // has a lot of padding in it.
//     c->process(false, 10, &err);
//     assert(err == ClockShifter6::Errors::ExceededDelaySize);

//     c->process(false, .01f, &err);
//     assert(err == ClockShifter6::Errors::NoError);
// }

static void canDelayZero() {
    // ClockShifter6 c;
    // c.setMaxDelaySamples(0);
    ClockShifter6::Errors err;
    // prime();
    auto shifter = makeAndPrime(10, 0);
    bool x = shifter->process(true, 0, &err);
    assertEQ(x, true);

    x = shifter->process(false, 0, &err);
    assertEQ(x, false);
}

static void testClockWidthZeroDelayFiniteWidth() {
    const unsigned period = 8;
    const unsigned numHigh = 3;
    const unsigned numLow = 5;
    assertEQ(period, (numLow + numHigh));
    auto shifter = makeAndPrime(period, 0);
    //SQINFO("done with prime");
    // above already put out one clock (end of prime)
    // here is #2
    shifter->process(true, 0, 0);

    // and here is #3
    shifter->process(true, 0, 0);
    // shifter->process(false, true, 0);

    // and 5 more low
    bool x = clockItLow(shifter, numLow, 0);
    assert(!x);

    // three high out now
    x = shifter->process(true, 0, 0);
    assert(x);
    x = shifter->process(true, 0, 0);
    assert(x);
    x = shifter->process(true, 0, 0);
    assert(x);
    x = shifter->process(false, 0, 0);
    assert(!x);
}

/**
 * For this test, withe period 8, delay5, put in 50% clock
 *
 * Prime followed by 3 extra clocks in, for total 4
 *  ck in: x-------xxxx--------------------
 * ck out: ------------xxxx--------
 */
static void testClockWithDelay() {
    const unsigned period = 8;
    const float delay = .5;

    auto shifter = makeAndPrime(period, delay);
    //SQINFO("--- done with prime");

    // Now a high was clocked in, should see it after delay 4.
    // That previous one was the first delay.

    // Clock 3 more so we clock so delay 3. 
    bool b = shifter->process(false, .5);
    assertEQ(b, false);
    b = shifter->process(false, .5);
    assertEQ(b, false);
    b = shifter->process(false, .5);
    assertEQ(b, false);

    // Now we have clocked 4 times, expect to see the delayed clock pulse.
    b = shifter->process(false, .5);
    assertEQ(b, true);
    
    // And then nothing after that...
    for (int i = 0; i < 20; ++i) {
        b = shifter->process(false, .5);
        assertEQ(b, false);
    }
}

void testClockShifter6() {
    testCanCreate();
    testInitNotStable();
    testInitStable();

    SQINFO("implement size error or remove?");
   // canReturnSizeError();
    canDelayZero();
    testClockWidthZeroDelayFiniteWidth();
    testClockWithDelay();
}

#if 0
void testFirst() {
   // ClockShifter6::llv = 1;
    //  This is the case that is bad without the dodgy "fix"
    // testWithLFO(4, 16, 0.136364, 0.400000, 3);

    // testSlowDown(5, 3552, 0.0001407658, 7);
    // testClockWidthZeroDelayFiniteWidth();
   // testClockWithDelay();
   // canReturnSizeError();
}
#endif