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
    printf("-- testJustOneClock --\n");
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
    printf("-- testSimpleInput --\n");
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
    x = c.run(5);
    assertGT(x, 1);
}

void testClockShifter() {
    test0();
   // testNoInput();
    testJustOneClock();
  //  testSimpleInput();
}