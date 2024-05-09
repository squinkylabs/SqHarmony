
#include "DeGlitch.h"
#include "asserts.h"

static void testThrough() {
    DeGlitch d;
    bool b = d.process(true, 0);
    assertEQ(b, true);

    b = d.process(false, 0);
    assertEQ(b, false);
}

static void testInit() {
    DeGlitch d;
    // until something happens we are ready
    bool b = d.process(true, 10);
    assertEQ(b, true);
}

static void testSupress1() {
    DeGlitch d;
    bool b = d.process(true, 10);
    assertEQ(b, true);
     b = d.process(true, 10);
    assertEQ(b, false);
}

void testDeGlitch() {
    testThrough();
    testInit();
    testSupress1();
 
}

#if 0
void testFirst() {
    testSupress1();
}
#endif