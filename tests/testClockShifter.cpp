#include "ClockShifter1.h"
#include "ClockShifter2.h"
#include "asserts.h"

template <class T>
static void test0() {
    T c;
    c.run(0, 0);
    c.setShift(0);
    (void)c;
}

template <class T>
static void clockIt(T& c, int numTimes, float expectedOutput, float sampleTime) {
    for (int i=0; i<numTimes; ++i) {
        const float x = c.run(false, sampleTime);
        if (expectedOutput >= 0) {
            assertEQ(x, expectedOutput);
        }
    }
}

template <class T>
static void testNoInput() {
    ClockShifter1 c;
    clockIt(c, 10000, 0, 1);
}

template <class T>
static void testJustOneClock() {
    // 10 fails, 5 passes
    const int iter = 10;
    T c;
    clockIt(c, iter, -1, 1);
    float x = c.run(5, 1);
    assertEQ(x, 0);
    clockIt(c, iter, 0, 1);
}

template <class T>
static void testSimpleInput() {
    const int iter = 10;
    T c;

    clockIt(c, iter, -1, 0);

    // then a single trigger input
    float x = c.run(5, 1);
    assertEQ(x, 0);
    clockIt(c, iter, 0, 0);

    // second input should do something
    x = c.run(5, .0001);
    assertGT(x, 1);
}

template <class T>
static void testShiftGeneral(float shiftAmount, int period) {
    // This test can only work if the expected delay is an integer
    float delayCyclesF = (float)period * shiftAmount;
    const int delayCyclesI = std::round(delayCyclesF);
    assertClose(float(delayCyclesI), delayCyclesF, .000001);
    assertGT(delayCyclesI, 0);  // this case not implemented

    T c;
    c.setShift(shiftAmount);

    // first a bunch of nothing
    clockIt(c, period, -1, 1);

    // then a single trigger input
    float x = c.run(5, 1);
    assertEQ(x, 0);
    // then enough zeros to make a full period
    clockIt(c, period-1, 0, 1);

    // second input should do something
    const int expectedOutputIndex = delayCyclesI - 1;
    assertGE(expectedOutputIndex, 0);
    assertLT(expectedOutputIndex, period - 1);

    // SQINFO("-- going into final test round");
    x = c.run(5, 1);
    assertEQ(x, 0);  // should be delayed
    // SQINFO("just did the first trigger, now the zero samples:");

    for (int i = 0; i < period - 1; ++i) {                                                                                                                                                                                                  x = c.run(0, 1);
       // SQINFO("i=%d output=%f", i, x);
        const float expectedValue = (i == expectedOutputIndex) ? 10 : 0;
        assertEQ(x, expectedValue);
    }
}

template <class T>
static void testOneShot() {
    T c;

    const float sampleTime = .1 / 1000;     // .1 milliseconds
    // 10 cycles of nothing
    clockIt(c, 10, -1, 0);

    // Then a single trigger input
    float x = c.run(5, 1);
    assertEQ(x, 0);
    clockIt(c, 100, 0, 0);

    // second input should do something
    // sample time === 1/10 millisecond
    x = c.run(5, sampleTime);
    assertGT(x, 1);
    //const int expectedOutputHigh = 2 /1000; // two milliseconds
    const int expectedClockHigh = 20;      // 2 ms one shot, .1 ms sample time
    clockIt(c, expectedClockHigh, 10, sampleTime);
    clockIt(c, 1, 0, sampleTime);
}

template <class T>
static void testShiftGeneral10() {
    testShiftGeneral<T>(.1f, 10);
    testShiftGeneral<T>(.2f, 10);
    testShiftGeneral<T>(.5f, 10);
    testShiftGeneral<T>(.9f, 10);
}

template <class T>
static void testShiftGeneral100() {
    testShiftGeneral<T>(.1f, 100);
    testShiftGeneral<T>(.5f, 100);
    testShiftGeneral<T>(.51f, 100);
}


// Even if we don't end up using ClockShifter1, 
// this is an easy way to use the same tests for 1 and 2.
template <class T>
static void testClockShifter() {
    test0<T>();
    testNoInput<T>();
    testJustOneClock<T>();
    testSimpleInput<T>();
     testShiftGeneral10<T>();
    testShiftGeneral100<T>();
    testOneShot<T>();
} 

void testClockShifter() {
   
    testClockShifter<ClockShifter1>();
    testClockShifter<ClockShifter2>();
    
   
    
   
}