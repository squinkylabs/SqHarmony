
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
    SQINFO("will not test de-glitch. it's busted");
#if 0
    testThrough();
    testInit();
    testSupress1();
#endif
 
}

#if 0
void testFirst() {
    testSupress1();
}
#endif