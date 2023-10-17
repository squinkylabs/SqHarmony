#include "ClockShifter.h"
#include "asserts.h"

static void test0() {
    ClockShifter c;
    (void)c;
}

static void clockIt(ClockShifter& c, int numTimes, float expectedOutput, float sampleTime) {
    for (int i=0; i<numTimes; ++i) {
        const float x = c.run(false, sampleTime);
        if (expectedOutput >= 0) {
            assertEQ(x, expectedOutput);
        }
    }
}

static void testNoInput() {
    ClockShifter c;
    clockIt(c, 10000, 0, 1);
}

static void testJustOneClock() {
    // 10 fails, 5 passes
    const int iter = 10;
    ClockShifter c;
    clockIt(c, iter, -1, 1);
    float x = c.run(5, 1);
    assertEQ(x, 0);
    clockIt(c, iter, 0, 1);
}

static void testSimpleInput() {
    const int iter = 10;
    ClockShifter c;

    clockIt(c, iter, -1, 0);

    // then a single trigger input
    float x = c.run(5, 1);
    assertEQ(x, 0);
    clockIt(c, iter, 0, 0);

    // second input should do something
    x = c.run(5, .0001);
    assertGT(x, 1);
}

static void testShiftGeneral(float shiftAmount, int period) {
    // SQINFO("---- testShift25 ---");

    // This test can only work if the expected delay is an integer
    float delayCyclesF = (float)period * shiftAmount;
    const int delayCyclesI = std::round(delayCyclesF);
    assertClose(float(delayCyclesI), delayCyclesF, .000001);
    assertGT(delayCyclesI, 0);  // this case not implemented
    // SQINFO("shift=%f, period=%d delaycycles=%d", shiftAmount, period, delayCyclesI);

    ClockShifter c;
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

    for (int i = 0; i < period - 1; ++i) {
                                                                                                                                                                                                           x = c.run(0, 1);
        SQINFO("i=%d output=%f", i, x);
        const float expectedValue = (i == expectedOutputIndex) ? 10 : 0;
        assertEQ(x, expectedValue);
    }
}

static void testOneShot() {
  //  const int iter = 10;
    ClockShifter c;

    const float sampleTime = .1 / 1000;     // .1 millseconds
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

static void testShiftGeneral10() {
    testShiftGeneral(.1f, 10);
    testShiftGeneral(.2f, 10);
    testShiftGeneral(.5f, 10);
    testShiftGeneral(.9f, 10);
}
static void testShiftGeneral100() {
    testShiftGeneral(.1f, 100);
    testShiftGeneral(.5f, 100);
    testShiftGeneral(.51f, 100);
}

void testClockShifter() {
    test0();
    testNoInput();
    testJustOneClock();
    testSimpleInput();
    testShiftGeneral10();
    testShiftGeneral100();
    testOneShot();
}