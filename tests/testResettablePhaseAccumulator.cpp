


#include "asserts.h"
#include "ResettablePhaseAccumulator.h"

static void test0() {
    ResettablePhaseAccumulator r;
    r.reset(0, 0);
    r.getPhase();
    r.tick();
}

static void testZeroFreq() {
    ResettablePhaseAccumulator r;
    r.tick();
    assert(r.getPhase() == 0.0);
}

static void testSetPhase() {
    ResettablePhaseAccumulator r;
    r.reset(.1, .2);
    assert(r.getPhase() == .1);
}

static void testSimpleTick() {
    ResettablePhaseAccumulator r;
    r.reset(0, .2);
    r.tick();
    assert(r.getPhase() == .2);
}

void testResettablePhaseAccumulator() {
    test0();
    testZeroFreq();
    testSetPhase();
    testSimpleTick();
}