
#include "ShiftMath.h"
#include "asserts.h"

static void testConstruct1() {
    ShiftMath::ClockWithPhase ck(12, .3);
    assertEQ(ck._clocks, 12);
    assertClose(ck._phase, .3, .00001);

    ShiftMath::ClockWithSamples x(14, 2345);
    assertEQ(x._clocks, 14);
    assertEQ(x._samples, 2345);
}

static void testConstruct2() {
    ShiftMath::ClockWithPhase ck(12.3);
    assertEQ(ck._clocks, 12);
    assertClose(ck._phase, .3, .00001);
}

static void testConstruct3() {
    ShiftMath::ClockWithPhase ck;
    assertEQ(ck._clocks, 0);
    assertEQ(ck._phase, 0);
}

static void testConvertPhaseToSamples1() {
    ShiftMath::ClockWithPhase cph(0, .5);
    const auto csp = ShiftMath::convert(cph, 100);  // 100 cycle period

    assertEQ(csp._clocks, 0);
    assertEQ(csp._samples, 50);
}

static void testConvertSamplesToPhase1() {
    const int periodForTest = 40000;
    const double fraction = .25;
    const int fractionalPeriod = periodForTest * fraction;

    ShiftMath::ClockWithSamples samp(0, fractionalPeriod);
    const auto cphase = ShiftMath::convert(samp, periodForTest);  // 100 cycle period

    assertEQ(cphase._clocks, 0);
    assertEQ(cphase._phase, fraction);
}

static void testConvertPhaseToSamples2() {
    ShiftMath::ClockWithPhase cph(70, .9);
    const auto csp = ShiftMath::convert(cph, 1000);  // 100 cycle period

    assertEQ(csp._clocks, 70);
    assertEQ(csp._samples, 900);
}

static void testConvertSamplesToPhase2() {
    const int periodForTest = 12345;
    const double fraction = .3;
    const int fractionalPeriod = periodForTest * fraction;

    ShiftMath::ClockWithSamples samp(771, fractionalPeriod);
    const auto cphase = ShiftMath::convert(samp, periodForTest);  // 100 cycle period

    assertEQ(cphase._clocks, 771);
    assertClose(cphase._phase, fraction, .0001);
}

static void testRoundTripConvert(int clocks, float phase, int periodForTest) {
    ShiftMath::ClockWithPhase cph(clocks, phase);
    ShiftMath::ClockWithSamples csamp = ShiftMath::convert(cph, periodForTest);
    ShiftMath::ClockWithPhase cph2 = ShiftMath::convert(csamp, periodForTest);
    assertEQ(cph2._clocks, clocks);
    assertClose(cph2._phase, phase, .001);
}

static void testRoundTripConvert() {
    testRoundTripConvert(0, 0, 1);
    testRoundTripConvert(12, .5, 1234);
    testRoundTripConvert(311, .2, 12345);
    testRoundTripConvert(13, .97, 987);

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

static void testAddWrap1() {
    ShiftMath::ClockWithSamples x(0, 20);
    ShiftMath::ClockWithSamples y(0, 30);
    const auto z = ShiftMath::addWithWrap(x, y, 40);
    assertEQ(z._clocks, 1);
    assertEQ(z._samples, 10);
}

static void testAddWrap2() {
    ShiftMath::ClockWithSamples x(0, 2 * 40 + 20);
    ShiftMath::ClockWithSamples y(0, 3 * 40 + 30);
    const auto z = ShiftMath::addWithWrap(x, y, 40);
    assertEQ(z._clocks, 3 + 2 + 1);
    assertEQ(z._samples, 10);
}

static void testAddWrap3() {
    ShiftMath::ClockWithSamples x(3, 4 + 10);
    ShiftMath::ClockWithSamples y(5, 2 + 10);
    const auto z = ShiftMath::addWithWrap(x, y, 20);
    assertEQ(z._clocks, 3 + 5 + 1);
    assertEQ(z._samples, 4 + 2);
}

static void testExceeds1() {
    ShiftMath::ClockWithSamples x(5, 6);
    ShiftMath::ClockWithSamples y(5, 3);
    const bool b = ShiftMath::exceedsOrEquals(x, y);
    assert(b);
}

static void testExceeds2() {
    ShiftMath::ClockWithSamples x(12, 3);
    ShiftMath::ClockWithSamples y(12, 3);
    const bool b = ShiftMath::exceedsOrEquals(x, y);
    assert(b);
}

static void testExceeds3() {
    ShiftMath::ClockWithSamples x(11, 3);
    ShiftMath::ClockWithSamples y(11, 4);
    const bool b = ShiftMath::exceedsOrEquals(x, y);
    assert(!b);
}

static void testExceeds4() {
    ShiftMath::ClockWithSamples x(8, 3);
    ShiftMath::ClockWithSamples y(7, 4);
    const bool b = ShiftMath::exceedsOrEquals(x, y);
    assert(b);
}

static void testExceeds5() {
    ShiftMath::ClockWithSamples x(8, 9);
    ShiftMath::ClockWithSamples y(7, 4);
    const bool b = ShiftMath::exceedsOrEquals(x, y);
    assert(b);
}

static void testExceeds6() {
    ShiftMath::ClockWithSamples x(7, 3);
    ShiftMath::ClockWithSamples y(8, 4);
    const bool b = ShiftMath::exceedsOrEquals(x, y);
    assert(!b);
}

static void testExceeds7() {
    ShiftMath::ClockWithSamples x(7, 9);
    ShiftMath::ClockWithSamples y(8, 4);
    const bool b = ShiftMath::exceedsOrEquals(x, y);
    assert(!b);
}

static void testCanReset() {
    ShiftMath::ClockWithSamples x(7, 9);
    assertEQ(x._clocks, 7);
    assertEQ(x._samples, 9);
    x.reset();
    assertEQ(x._clocks, 0);
    assertEQ(x._samples, 0);
}

static void testEquality() {
    ShiftMath::ClockWithSamples x(7, 9);
    ShiftMath::ClockWithSamples y(7, 10);
    ShiftMath::ClockWithSamples z(6, 9);
    bool b = (x == x);
    assertEQ(b, true);
}

static void testToString() {
    ShiftMath::ClockWithSamples y(75, 103);
    const std::string s = y.toString();
    const std::string expected = "clocks=75, samples=103";
    assertEQ(s, expected);
}

void testShiftMath() {
    testConstruct1();
    testConstruct2();
    testConstruct3();
    testConvertPhaseToSamples1();
    testConvertPhaseToSamples2();
    testConvertSamplesToPhase1();
    testConvertSamplesToPhase2();
    testRoundTripConvert();
    testAdd1();
    testAdd2();
    testAddWrap1();
    testAddWrap2();
    testAddWrap3();
    testExceeds1();
    testExceeds2();
    testExceeds3();
    testExceeds4();
    testExceeds5();
    testExceeds6();
    testExceeds7();
    testCanReset();
    testEquality();
    testToString();
}