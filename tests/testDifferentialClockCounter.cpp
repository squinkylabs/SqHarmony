#include "DifferentialClockCounter.h"
#include "asserts.h"

static void testCanCall() {
    DifferentialClockCounter d;
    d.process(0, 0);
    const int x = d.getDiff();
}

static void testInitial() {
    DifferentialClockCounter d;
    assertEQ(0, d.getDiff());
}

static void testRefLead() {
    DifferentialClockCounter d;
    assertEQ(0, d.getDiff());
    d.process(0, 0);
    d.process(10, 0);
    assertEQ(-1, d.getDiff());
}

static void testOtherLead() {
    DifferentialClockCounter d;
    assertEQ(0, d.getDiff());
    d.process(0, 0);
    d.process(0, 10);
    assertEQ(1, d.getDiff());
}

void testDifferenticalClockCounter() {
    testCanCall();
    testInitial();
    testRefLead();
    testOtherLead();
}

#if 0
void testFirst() {
    testDifferenticalClockCounter();
}
#endif