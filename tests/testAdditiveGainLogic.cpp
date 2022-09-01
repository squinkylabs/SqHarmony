
#include "AdditiveGainLogic.h"
#include "asserts.h"

static void testAdditiveGainLogic_initial() {
    const static unsigned N = 16;
    AdditiveGainLogic<N> l;
    for (unsigned i = 0; i < N; ++i) {
        assertEQ(l.getLevel(i), l.defaultLevel);
    }
}

static void testAdditiveGainLogic_outOfRange() {
    const static unsigned N = 9;
    AdditiveGainLogic<N> l;
    assertEQ(l.getLevel(N), 0);
    assertEQ(l.getLevel(N + 1), 0);
    assertNE(l.getLevel(N - 1), 0);

    /*
        l.setCV(1.3f);
        assertEQ(l.getLevel(N), 0);
        assertEQ(l.getLevel(N + 1), 0);
        assertNE(l.getLevel(N - 1), 0);
        */
}

static void testAdditiveGainLogic_harmonics() {
    const static unsigned N = 16;
    AdditiveGainLogic<N> l;
    l.setHarmonic(0, 0);
    assertEQ(l.getLevel(0), 0);
    assertEQ(l.getLevel(1), l.defaultLevel);
}

static void testAdditiveGainLogic_slope() {
    const static unsigned N = 16;
    AdditiveGainLogic<N> l;
    l.setSlope(0);

    // l.dump("after slope 0");
    float last = l.defaultLevel;
    assertEQ(l.getLevel(0), l.defaultLevel);
    for (unsigned i = 1; i < N; ++i) {
        //  SQINFO("in test loop on %d level=%f", i, l.getLevel(i));
        assertLT(l.getLevel(i), 1);
        assertLT(l.getLevel(i), last);
        last = l.getLevel(i);
    }
}

static void testAdditiveGainLogic_isEven() {
    const static unsigned N = 16;
    assertEQ(AdditiveGainLogic<N>::isEven(0), false);
    assertEQ(AdditiveGainLogic<N>::isEven(1), true);
    assertEQ(AdditiveGainLogic<N>::isEven(2), false);

    assertEQ(AdditiveGainLogic<N>::isOdd(0), false);
    assertEQ(AdditiveGainLogic<N>::isOdd(1), false);
    assertEQ(AdditiveGainLogic<N>::isOdd(2), true);
}

static void testAdditiveGainLogic_Even() {
    SQINFO("--- testAdditiveGainLogic_Even");
    const static unsigned N = 16;
    AdditiveGainLogic<N> l;
    l.setEven(0);
    for (int i = 0; i < N; ++i) {
        const auto x = l.getLevel(i);
        const auto isEven = AdditiveGainLogic<N>::isEven(i);
        if (isEven) {
            SQINFO("%d: even, %f", i, x);
            assertEQ(x, 0);
        } else {
            SQINFO("%d: odd, %f", i, x);
            assertEQ(x, l.defaultLevel);
        }
    }
}

static void testAdditiveGainLogic_Odd() {
    const static unsigned N = 16;
    AdditiveGainLogic<N> l;
    l.setOdd(0);
    for (int i = 0; i < N; ++i) {
        const auto x = l.getLevel(i);
        const auto isOdd = AdditiveGainLogic<N>::isOdd(i);
        if (isOdd) {
            assertEQ(x, 0);
        } else {
            assertEQ(x, l.defaultLevel);
        }
    }
}

void testAdditiveGainLogic() {
    testAdditiveGainLogic_initial();
    testAdditiveGainLogic_outOfRange();
    testAdditiveGainLogic_harmonics();
    testAdditiveGainLogic_slope();
    testAdditiveGainLogic_isEven();
    testAdditiveGainLogic_Even();
    testAdditiveGainLogic_Odd();
}