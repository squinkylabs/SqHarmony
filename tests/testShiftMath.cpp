
#include "ShiftMath.h"
#include "asserts.h"

static void testConstruct() {
    ShiftMath::ClockWithPhase ck(12, .3);
    assertEQ(ck._clocks, 12);
    assertClose(ck._phase, .3, .00001);

    ShiftMath::ClockWithSamples x(14, 2345);
    assertEQ(x._clocks, 14);
    assertEQ(x._samples, 2345);
}

static void testConvert1() {
    ShiftMath::ClockWithPhase cph(0, .5);
    const auto csp = ShiftMath::convert(cph, 100);  // 100 cycle period

    assertEQ(csp._clocks, 0);
    assertEQ(csp._samples, 50);
}

static void testConvert2() {
    ShiftMath::ClockWithPhase cph(70, .9);
    const auto csp = ShiftMath::convert(cph, 1000);  // 100 cycle period

    assertEQ(csp._clocks, 70);
    assertEQ(csp._samples, 900);
}

static void testAdd1() {
    ShiftMath::ClockWithSamples x(0, 0);
    ShiftMath::ClockWithSamples y(0, 0);
    const auto z = ShiftMath::addWithWrap(x, y, 1);
    assertEQ(z._clocks, 0);
    assertEQ(z._samples, 0);
}

static void testAdd2() {
    ShiftMath::ClockWithSamples x(123, 456);
    ShiftMath::ClockWithSamples y(7, 11);
    const auto z = ShiftMath::addWithWrap(x, y, 1000);
    assertEQ(z._clocks, 123 + 7);
    assertEQ(z._samples, 456 + 11);
}

void testShiftMath() {
    testConstruct();
    testConvert1();
    testConvert2();
    testAdd1();
    testAdd2();
}