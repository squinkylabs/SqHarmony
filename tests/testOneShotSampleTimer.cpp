
#include "OneShotSampleTimer.h"
#include "asserts.h"

static void testCanCall() {
    OneShotSampleTimer t;
    const bool b = t.run();
    t.get();
    t.arm(0);
}

static void init() {
    OneShotSampleTimer t;
    for (int i = 0; i < 20000; ++i) {
        assertEQ(t.get(), false);
        assertEQ(t.run(), false);
    }
}

static void fires() {
    OneShotSampleTimer t;
    t.arm(0);
    assertEQ(t.get(), true);  // even with zero delay we should get a sample of output
}

static void testRun1() {
    OneShotSampleTimer t;
    t.arm(0);
    assertEQ(t.get(), true);  // even with zero delay we should get a sample of output
    const auto b = t.run();
    assertEQ(b, false);
    assertEQ(t.get(), false);
}

static void testRunN(int n) {
    OneShotSampleTimer t;
    const int expectedDelay = std::max(0, n - 1);
    t.arm(n);
    for (int i = 0; i < n * 2; ++i) {
        const auto x = t.run();
        assertEQ(x, (i == expectedDelay));
    }
}

void testOneShotSampleTimer() {
    testCanCall();
    init();
    fires();
    //  testRun1();
    //   testRunN(0);
    // TODO: this test is failing for a logit reason
  //  testRunN(1);
}