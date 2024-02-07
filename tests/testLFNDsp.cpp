

#include "asserts.h"

#include "LFNDsp.h"

static void testCanCall() {
    LFNDsp l(0);
    l.genStraightNoise(true);
    l.process();
}

void testLFNDsp() {
    testCanCall();
}