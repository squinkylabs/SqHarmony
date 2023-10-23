
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
    for (int i=0; i< numTimes; ++i) {
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

    const float x = run(s, period * 8);
    assertClose(s.get(), 2, .0001);
    assertClose(x, 1, .0001);
}

static void testBusy() {
    ShiftCalc s;
    assertEQ(s.busy(), false);
    const int period = 10;
    s.trigger(period);
     assertEQ(s.busy(), true);
}

void testShiftCalc() {
    testCanCall();
    testGeneratesNothing();
    testGeneratesSomething();
    testRate1(10);
    testRate1(25);
    testMulti();
    testBusy();
}