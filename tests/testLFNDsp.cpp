

#include "LFNDsp.h"
#include "asserts.h"

static void testCanCall() {
    LFNDsp l(0);
    l.genStraightNoise(true);
    l.process();
}

static float_4 getOutput(bool straight) {
    LFNDsp l(0);
    l.genStraightNoise(straight);
    l.process();
    l.process();
    return l.process();
}

static void testMakesSound(bool straight) {
    float_4 x = getOutput(straight);
    for (int i=0;i<4;++i) {
        assertNE(x[i], 0);
    }
 }

static void testMakesSound() {
    testMakesSound(true);
    testMakesSound(false);
}

static void testStraight() {
    const auto x = getOutput(false);
    const auto y = getOutput(true);
    for (int i=0;i<4;++i) {
        assertNE(x[i], y[i]);
    }
}

void testLFNDsp() {


    testCanCall();
    testMakesSound();
 //   SQINFO("fix all these tests!!");
    testStraight();
}