

#include "NoiseGen.h"
#include "asserts.h"

static void testCanCall() {
    NoiseGen n;
    n.get();
}

static void testDifferentValues() {
    NoiseGen n;
    float_4 x = n.get();
    float_4 y = n.get();
    for (int i=0; i<4; ++i) {
        assertNE(x[i], y[i]);
    }
} 

void testNoiseGen() {
    testCanCall();
    testDifferentValues();
}