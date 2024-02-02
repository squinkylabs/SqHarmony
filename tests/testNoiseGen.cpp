

#include "NoiseGen.h"
#include "asserts.h"

static void testCanCall() {
    NoiseGen n(0);
    n.get();
}

static void testDifferentValues() {
    NoiseGen n(0);
    float_4 x = n.get();
    float_4 y = n.get();
    for (int i=0; i<4; ++i) {
        assertNE(x[i], y[i]);
    }
} 

static void testChannelsDifferent() {
    NoiseGen n(12);
    float_4 x = n.get();
     for (int i=1; i<4; ++i) {
        assertNE(x[0], x[i]);
     }
}

static void testDifferentSeeds() {
    NoiseGen n(0);
    NoiseGen n2(1);
    assertNE(n.get()[0], n2.get()[0]);
}


void testNoiseGen() {
    testCanCall();
    testDifferentValues();
    testChannelsDifferent();
    testDifferentSeeds();

}