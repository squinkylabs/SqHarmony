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


static void testActulDCC() {
 testCanCall();
    testInitial();
    testRefLead();
    testOtherLead();
}

////////////////////

static void testClockMonitor1() {
    ClockMonitor cm;
    bool b = cm.go(false);
    assertEQ(b, false);
    b = cm.go(true);
    assertEQ(b, false);
    b = cm.go(false);
    assertEQ(b, false);
    b = cm.go(true);  
    assertEQ(b, true);  
}

static void testActualClockMonitor() {
    testClockMonitor1();
}

void testDifferenticalClockCounter() {
   testActulDCC();
   testActualClockMonitor();
}



#if 1
void testFirst() {
    testClockMonitor1();
}
#endif