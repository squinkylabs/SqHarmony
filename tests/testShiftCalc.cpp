
#include "ShiftCalc.h"
#include "asserts.h"

static void testCanCall() {
    ShiftCalc s;
    float f = s.go();
    s.trigger(1);
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
    s.trigger(period);

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
    s.trigger(period);
    assertEQ(s.busy(), true);
}

static void testTriggerSecondTime() {
    ShiftCalc s;
    assertEQ(s.busy(), false);
    assertEQ(s.get(), 0);
    const int period = 10;

    // trigger first time
    s.trigger(period);
    // TODO: why do we need to clock one "extra"?
    run(s, 1 + period * 8);
    assertEQ(s.get(), 1);

    assertEQ(s.busy(), false);

    s.trigger(period);
    run(s, 1);

    SQINFO("output now %f", s.get());
    assertClose(s.get(), 1.0125, .0001);
}

static void testTriggerDurring() {
    ShiftCalc s;
    assertEQ(s.busy(), false);
    assertEQ(s.get(), 0);
    const int period = 10;

    // trigger first time
    s.trigger(period);

    run(s, 1 + period * 4);
    assertEQ(s.busy(), true);
    s.trigger(period);          // re-trigger.
    run(s, 10 + period * 4);
    assertEQ(s.busy(), false);
    assertEQ(s.get(), 1);
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
}