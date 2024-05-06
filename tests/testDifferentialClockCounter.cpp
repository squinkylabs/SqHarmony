#include "DifferentialClockCounter.h"
#include "asserts.h"

static void testCanCall() {
    DifferentialClockCounter d;
    d.process(0, 0);
    const int x = d.getDiff();
}

static void test2() {
    assert(false);
}

void testDifferenticalClockCounter() {
    testCanCall();
    test2();
}


#if 1
void testFirst() {
    test2();
}
#endif