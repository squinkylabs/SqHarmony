

#include "NoiseGen.h"

#include "SimdBlocks.h"

#include "asserts.h"

static void testCanCall() {
    NoiseGen n(0);
    n.get();
    n.reset(0);
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


class RMS {
public:
    void accept(float x) {
        ++n;
        sumSquares += (x * x);
    }
    float get() const {
        float x = sumSquares / float(n);
        return std::sqrt(x);
    }
private:
    int n = 0;
    float sumSquares = 0;
};

static void testRMS() {
    NoiseGen n(0);
    RMS x;
    for (int i=0; i< 1000; ++i) {
        x.accept(n.get()[0]);
    }

    float f = x.get();
    SQINFO("rms = %f", f);
    assertLT(f, 2);
    assertGT(f, .5);
}

static void testReset(int i) {
    NoiseGen n(0);
    const auto xInit = n.get();
    n.get();
    n.get();
    n.get();
    n.get();

    const auto mask = SimdBlocks::maskTrue1(i);
    n.reset(mask);
    const auto x2 = n.get();

    for (int i = 1; i < 4; ++i) {
        assertNE(xInit[0], x2[i]);
    }

}

static void testReset() {
    for (int i = 0; i < 4; ++i) {
        testReset(i);
    }
}

static void testMaskTrue1(int channel) {
     const auto a = SimdBlocks::maskTrue1(channel);
     for (int i=0; i<4; ++i) {
        if (i == channel) {
            assertEQ(SimdBlocks::isChannelTrue(i, a), true);
        } else {
             assertEQ(SimdBlocks::isChannelTrue(i, a), false);
        }
     }
}

static void testMaskTrue1() {
  for (int i = 0; i < 4; ++i) {
        testMaskTrue1(i);
    }
}

void testNoiseGen() {
    testCanCall();
    testDifferentValues();
    testChannelsDifferent();
    testDifferentSeeds();
    testRMS();
    testMaskTrue1();
    testReset();

}