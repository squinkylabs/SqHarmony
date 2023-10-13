#include "asserts.h"
#include "ClockShifter.h"

static void test0() {
    ClockShifter c;
    (void) c;
}

static void  testNoInput() {
     ClockShifter c;
     for (int i=0; i< 10000; ++i) {
        float x = c.run(false);
        assertEQ(x, 0);
     }
}

static void testSimpleInput() {
    const int iter = 10;
    ClockShifter c;
    for (int i = 0; i<iter; ++i) {
        c.run(false);
    }
    float x = c.run(true);
    assertEQ(x, 0);
     for (int i = 0; i<iter; ++i) {
        x = c.run(false);
        assertEQ(x, 0);
    }
    x = c.run(true);
    assertEQ(x, 1);

}

void testClockShifter() {
    test0();
    testNoInput();
    testSimpleInput();
}