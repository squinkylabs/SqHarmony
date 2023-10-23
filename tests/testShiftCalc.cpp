
#include "ShiftCalc.h"
#include "asserts.h"

static void testCanCall() {
    ShiftCalc s;
    const float f = s.go();
    s.setup(0);
}

void testShiftCalc() {
    testCanCall();
}