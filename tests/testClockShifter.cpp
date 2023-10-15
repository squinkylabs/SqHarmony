#include "ClockShifter.h"
#include "asserts.h"

static void test0() {
    ClockShifter c;
    (void)c;
}

static void testNoInput() {
    ClockShifter c;
    for (int i = 0; i < 10000; ++i) {
        float x = c.run(false);
        assertEQ(x, 0);
    }
}

static void testJustOneClock() {
    // 10 fails, 5 passes
    const int iter = 10;
    ClockShifter c;
    for (int i = 0; i < iter; ++i) {
        c.run(0);
    }
    float x = c.run(5);
    assertEQ(x, 0);
    for (int i = 0; i < iter; ++i) {
        x = c.run(0);
        assertEQ(x, 0);
    }
}

static void testSimpleInput() {
    const int iter = 10;
    ClockShifter c;

    // first a bunch of nothing
    for (int i = 0; i < iter; ++i) {
        c.run(0);
    }

    // then a single trigger input
    float x = c.run(5);
    assertEQ(x, 0);
    for (int i = 0; i < iter; ++i) {
        x = c.run(0);
        assertEQ(x, 0);
    }

    // second input should do something
    x = c.run(5);
    assertGT(x, 1);
}

static void testShiftGeneral(float shiftAmount, int period) {
    // SQINFO("---- testShift25 ---");

    // Ths test can only work if the expected delay is an integer
    float delayCyclesF = (float)period * shiftAmount;
    const int delayCyclesI = std::round(delayCyclesF);
    assertClose(float(delayCyclesI), delayCyclesF, .000001);
    assertGT(delayCyclesI, 0);  // this case not implemented
    // SQINFO("shift=%f, period=%d delaycycles=%d", shiftAmount, period, delayCyclesI);

    ClockShifter c;
    c.setShift(shiftAmount);

    // first a bunch of nothing
    for (int i = 0; i < period; ++i) {
        c.run(0);
    }

    // then a single trigger input
    float x = c.run(5);
    assertEQ(x, 0);
    // then enough zeros to make a full period
    for (int i = 0; i < period - 1; ++i) {
        x = c.run(0);
        assertEQ(x, 0);
    }
    // second input should do something
    const int expectedOutputIndex = delayCyclesI - 1;
    assertGE(expectedOutputIndex, 0);
    assertLT(expectedOutputIndex, period - 1);

    // SQINFO("-- going into final test round");
    x = c.run(5);
    assertEQ(x, 0);  // should be delayed
    // SQINFO("just did the first trigger, now the zero samples:");

    for (int i = 0; i < period - 1; ++i) {
        x = c.run(0);
        // SQINFO("i=%d output=%f", i, x);
        const float expectedValue = (i == expectedOutputIndex) ? 10 : 0;
        assertEQ(x, expectedValue);
    }
}

static void testShiftGeneral10() {
    testShiftGeneral(.1f, 10);
    testShiftGeneral(.2f, 10);
    testShiftGeneral(.5f, 10);
    testShiftGeneral(.9f, 10);
}
static void testShiftGeneral100() {
    testShiftGeneral(.1f, 100);
}

void testClockShifter() {
    test0();
    testNoInput();
    testJustOneClock();
    testSimpleInput();
    testShiftGeneral10();
    testShiftGeneral100();
}