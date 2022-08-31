
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
    for (unsigned i=1; i<N; ++i) {
      //  SQINFO("in test loop on %d level=%f", i, l.getLevel(i));
        assertLT(l.getLevel(i), 1);
        assertLT(l.getLevel(i), last);
        last = l.getLevel(i);
    }
}

void testAdditiveGainLogic() {
    testAdditiveGainLogic_initial();
    testAdditiveGainLogic_outOfRange();
    testAdditiveGainLogic_harmonics();
    testAdditiveGainLogic_slope();
}