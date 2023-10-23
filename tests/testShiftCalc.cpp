
#include "ShiftCalc.h"
#include "asserts.h"

static void testCanCall() {
    ShiftCalc s;
    const float f = s.go();
    s.trigger(1);
}

static void testGeneratesNothing() {
    ShiftCalc s;
    const float f = s.go();
    assertEQ(f, 0);
}

static void testGeneratesSomething() {
    ShiftCalc s;
    s.trigger(10);
    const float f = s.go();
    assertGT(f, 0);
}

static void testRate1(int period) {
    ShiftCalc s;
    s.trigger(period);
    float last = -1;
    const int durationEntireEvent = period * 8;
    for (int i = 0; i < durationEntireEvent; ++i) {
        const float f = s.go();
        const float expected = double(i + 1) / double(durationEntireEvent);
        assertGT(f, last);
        last = f;
        assertClose(f, expected, .00001);
    }
}

void testShiftCalc() {
    testCanCall();
    testGeneratesNothing();
    testGeneratesSomething();
    testRate1(10);
    testRate1(25);
}