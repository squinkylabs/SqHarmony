

#include "NoiseGen.h"
#include "asserts.h"

static void testCanCall() {
    NoiseGen n;
    n.get();
}

void testNoiseGen() {
    testCanCall();
}