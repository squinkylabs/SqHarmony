

#include "Ratchet.h"
#include "asserts.h"

static void testCanCall() {
    Ratchet r;

    r.setCount(4);
    const int i = r.run(true);
    r.trigger();
    r.onOutputClock();
    bool b = r.isStillRatcheting();
}

static void testNoTrigger() {
    Ratchet r;
    for (int i = 0; i < 10; ++i) {
        const int b = r.run(false);
        assertEQ(r.run(false), 1);
        const bool x = r.isStillRatcheting();
        assertEQ(r.isStillRatcheting(), false);
    }
}

static void testTrigger() {
    Ratchet r;
    r.setCount(2);
    r.trigger();
    int i = r.run(true);
    assertEQ(i, 2);
    assertEQ(r.isStillRatcheting(), true);
}

static void testEnds() {
    // This first part from the test above.
    Ratchet r;
    r.setCount(2);
    r.trigger();
    int i = r.run(true);
    assertEQ(i, 2);
    assertEQ(r.isStillRatcheting(), true);

    // now, new stuff.
    r.onOutputClock();  // it this the first one, which happens when run(true)? Probably...
    assertEQ(r.isStillRatcheting(), true);
    r.onOutputClock();
    assertEQ(r.isStillRatcheting(), false);
}

void testRatchet() {
    testCanCall();
    testNoTrigger();
    testTrigger();
    testEnds();
}