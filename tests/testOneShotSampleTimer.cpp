
#include "OneShotSampleTimer.h"
#include "asserts.h"

static void testCanCall() {
    OneShotSampleTimer t;
    const bool b = t.run();
    t.isRunning();
    t.arm(0);
}

static void init() {
    OneShotSampleTimer t;
    for (int i = 0; i < 20000; ++i) {
        assertEQ(t.isRunning(), false);
        assertEQ(t.run(), false);
    }
}

static void fires() {
    OneShotSampleTimer t;
    t.arm(0);
    assertEQ(t.isRunning(), true);  // even with zero delay we should get a sample of output
}

static void testRun1() {
    OneShotSampleTimer t;
    t.arm(0);
    assertEQ(t.isRunning(), true);  // even with zero delay we should get a sample of output
    bool b = t.run();
    assertEQ(b, true);
    assertEQ(t.isRunning(), false);

    b = t.run();
    assertEQ(b, false);
}

static void testRunN(int n) {
    assert(n > 0);  // don't work at zero.
    OneShotSampleTimer t;
    const int expectedDelay = n-1;
    t.arm(n);
    bool didExpire = false;
    for (int i = 0; i < (n * 2 + 2); ++i) {
        const bool expired = t.run();
        // When timer first expires, tell when.
        if (expired) {
            didExpire = true;
            assertEQ(expired, (i== expectedDelay));
        }
    }
    assert(didExpire);
}

void testOneShotSampleTimer() {
    testCanCall();
    init();
    fires();
    testRun1();
    testRunN(1);
    testRunN(2);
    for (int i = 2; i < 20; ++i) {
        testRunN(i);
    }
}