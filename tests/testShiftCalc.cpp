
#include "ShiftCalc.h"
#include "asserts.h"

static void testCanCall() {
    ShiftCalc s;
    float f = s.go();
    s.trigger(12, .1, .2);
    f = s.get();
}

static void testGeneratesNothing() {
    ShiftCalc s;
    const float f = s.go();
    assertEQ(f, 0);
    assertEQ(s.get(), 0);
}

static void testGeneratesSomething() {
    ShiftCalc s;
    s.trigger(10, 1, 8);
    const float f = s.go();
    assertGT(f, 0);
}

static void testRate1(int period) {
    ShiftCalc s;
    s.trigger(period, 1, 8);
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

static float run(ShiftCalc& s, int numTimes) {
    float x = 0;
    for (int i = 0; i < numTimes; ++i) {
        x = s.go();
    }
    return x;
}

static void testMulti() {
    ShiftCalc s;
    const int period = 10;
    s.trigger(period, 1, 8);

    run(s, period * 8);
    assertClose(s.get(), 1, .0001);

    // This second time should do nothing
    const float x = run(s, period * 8);
    assertClose(s.get(), 1, .0001);
    assertClose(x, 1, .0001);
}

static void testBusy() {
    ShiftCalc s;
    assertEQ(s.busy(), false);
    const int period = 10;
    s.trigger(period, 3, 4);
    assertEQ(s.busy(), true);
}

static void testTriggerSecondTime() {
    ShiftCalc s;
    assertEQ(s.busy(), false);
    assertEQ(s.get(), 0);
    const int period = 10;

    // trigger first time
    s.trigger(period, 1, 8);
    // TODO: why do we need to clock one "extra"?
    run(s, 1 + period * 8);
    assertEQ(s.get(), 1);
    assertEQ(s.busy(), false);

    s.trigger(period, 1, 8);
    run(s, 1);
    assertClose(s.get(), 1.0125, .0001);
}

static void testTriggerDurring() {
    ShiftCalc s;
    assertEQ(s.busy(), false);
    assertEQ(s.get(), 0);
    const int period = 10;

    // trigger first time
    s.trigger(period, 1, 8);

    run(s, 1 + period * 4);
    assertEQ(s.busy(), true);
    s.trigger(period, 1, 8);  // re-trigger.
    run(s, 10 + period * 4);
    assertEQ(s.busy(), false);
    assertEQ(s.get(), 1);
}

static void testLimitSub(float limit) {
    ShiftCalc s;
    const int period = 14;
  //  const float limit = .47;
    s.trigger(period, limit, 11);
    int ct = 0;
    for (bool done = false; !done;) {
        const auto value = s.get();
        const auto distance = (limit - value);
        ct++;
        assert(ct < 1000);
        s.go();
        if (distance < .0001) {
            done = true;
        }
    }

    for (int i = 0; i < 1000; ++i) {
        s.go();
    }

    assertClose(s.get(), limit, .0001);
}

static void testLimit() {
    testLimitSub(.47f);
    testLimitSub(1.3f);
    testLimitSub(-.31f);
    testLimitSub(-1.47f);
}
static void testRateSub(int period, float limit, float rampInClocks) {
    ShiftCalc s;
    s.trigger(period, limit, rampInClocks);
    int ct = 0;
    for (bool done = false; !done;) {
        const auto value = s.get();
        const auto distance = std::abs(limit - value);
        //  SQINFO("value = %f dist=%f", value, distance);
        ct++;
        assert(ct < 10000);
        s.go();
        if (distance < .0001) {
            done = true;
        }
    }
    const double x = period * rampInClocks;
    assertClosePct(ct, x, 1);
}

static void testRate() {
    testRateSub(100, 1.f / 3.f, 10);
    testRateSub(1000, 1.f / 3.f, 10);
    testRateSub(11, 1.1f, 17);
    testRateSub(11, -1.7f, 23);
}

void testShiftCalc() {
    testCanCall();
    testGeneratesNothing();
    testGeneratesSomething();
    testRate1(10);
    testRate1(25);

    testMulti();
    testBusy();

    testTriggerSecondTime();
    testTriggerDurring();
    testLimit();
    testRate();
}