
#include "AdditiveModLogic.h"
#include "asserts.h"

template <unsigned N>
static void assertAllOutputsZero(const AdditiveModLogic<N>& l) {
    for (unsigned i = 0; i < N; ++i) {
        const float x = l.getCombinedADSR(i);
        assertEQ(x, 0);
    }
}

static void testAdditiveModLogic_initial() {
    AdditiveModLogic<16> l;
    assertAllOutputsZero(l);
}

void testAdditiveModLogic() {
    testAdditiveModLogic_initial();
}