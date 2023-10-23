
#include "ShiftCalc.h"
#include "asserts.h"

static void testCanCall() {
    ShiftCalc s;
    const float f = s.go();
    s.setup(0);
    s.trigger(1);
}

static void testGeneratesNothing() {
    ShiftCalc s;
    s.setup(0);
    const float f = s.go();
    assertEQ(f, 0);
}

static void testGeneratesSomething() {
    ShiftCalc s;
    s.setup(0);
    s.trigger(10);
    const float f = s.go();
    assertGT(f, 0);
}

void testShiftCalc() {
    testCanCall();
    testGeneratesNothing();
    testGeneratesSomething();
}