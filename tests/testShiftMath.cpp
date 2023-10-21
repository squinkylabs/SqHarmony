
#include "asserts.h"
#include "ShiftMath.h"

static void testConstruct() {
    ClockWithPhase ck (12, .3);
    assertEQ(ck._clocks, 12);
    assertClose(ck._phase, .3, .00001);

    ClockWithSamples x(14, 2345);
    assertEQ(x._clocks, 14);
    assertEQ(x._samples, 2345);
}

static void testConvert1() {
    ClockWithPhase cph(0, .5);
    const auto csp = convert(cph, 100);     // 100 cycle period

    assertEQ(csp._clocks, 0);
    assertEQ(csp._samples, 50);
}

static void testConvert2() {
    ClockWithPhase cph(70, .9);
    const auto csp = convert(cph, 1000);     // 100 cycle period

    assertEQ(csp._clocks, 70);
    assertEQ(csp._samples, 900);
}

void testShiftMath() {
    testConstruct();
    testConvert1();
    testConvert2();
}