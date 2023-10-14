


#include "asserts.h"
#include "ResettablePhaseAccumulator.h"

static void test0() {
    ResettablePhaseAccumulator r;
    r.reset(0, 0);
    r.getPhaseAndDelta();
    r.tick();
}

static void testZeroFreq() {
    ResettablePhaseAccumulator r;
    r.tick();
    assertEQ(r.getPhaseAndDelta().first, 0.0);
}

static void testSetPhase() {
    ResettablePhaseAccumulator r;
    r.reset(.1, .2);
    assertEQ(r.getPhaseAndDelta().first, .1);
}

static void testSimpleTick() {
    ResettablePhaseAccumulator r;
    r.reset(0, .2);
    r.tick();
    assertEQ(r.getPhaseAndDelta().first, .2);
}

static void testWrap() {
    ResettablePhaseAccumulator r;
    r.reset(.9, .2);
    r.tick();
    assertClose(r.getPhaseAndDelta().first, .1, .00000001);
}

void testResettablePhaseAccumulator() {
    test0();
    testZeroFreq();
    testSetPhase();
    testSimpleTick();
    testWrap();

}