
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

void testAdditiveGainLogic() {
    testAdditiveGainLogic_initial();
    testAdditiveGainLogic_outOfRange();
}