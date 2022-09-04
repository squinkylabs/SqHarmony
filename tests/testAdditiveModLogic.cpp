
#include "AdditiveModLogic.h"
#include "asserts.h"

template <unsigned N>
static void assertAllOutputsX(const AdditiveModLogic<N>& l, float x) {
    for (unsigned i = 0; i < N; ++i) {
        const float y = l.getCombinedADSR(i);
        assertEQ(y, x);
    }
}

static void testAdditiveModLogic_initial() {
    AdditiveModLogic<16> l;

    // todo: this fails
    // assertAllOutputsX(l, 0);
}


static void testAdditiveModLogic_notZero() {
    const unsigned N = 12;

    AdditiveModLogic<12> l;
    // todo: this fails
    // assertAllOutputsZero(l);
    for (unsigned i = 0; i < N; ++i) {
        //  l.setHarmonicBaseLevel(i, 1);
        //  void setADSRValue(unsigned adsr, float value, bool connected);
        // void setADSRTrimValue(unsigned adsr, unsigned harmonic, float value);
        l.setADSRTrimValue(0, i, 1.f);
        l.setADSRTrimValue(1, i, 1.f);
        l.setADSRTrimValue(2, i, 1.f);
    }
    l.setADSRValue(0, 1.f, true);
    l.setADSRValue(1, 1.f, true);
    l.setADSRValue(2, 1.f, true);

    for (unsigned i = 0; i < N; ++i) {
        const float x = l.getCombinedADSR(i);
        assertEQ(x, 1);
    }
}

static void testAdditiveModLogic_notConnected() {
    SQINFO("\n\n-------------- testAdditiveModLogic_notConnected");
    const unsigned N = 12;
    AdditiveModLogic<12> l;
    // todo: this fails
    // assertAllOutputsZero(l);
    for (unsigned i = 0; i < N; ++i) {
        SQINFO("in set loop %d", i);
        //  l.setHarmonicBaseLevel(i, 1);
        //  void setADSRValue(unsigned adsr, float value, bool connected);
        // void setADSRTrimValue(unsigned adsr, unsigned harmonic, float value);
        l.setADSRTrimValue(0, i, 1.f);
        l.setADSRTrimValue(1, i, 1.f);
        l.setADSRTrimValue(2, i, 1.f);
    }

    l.setADSRValue(0, 0, false);
    l.setADSRValue(1, 0, false);
    l.setADSRValue(2, 0, false);

    for (unsigned i = 0; i < N; ++i) {
        const float x = l.getCombinedADSR(i);
        assertEQ(x, 1);
    }
}

void testAdditiveModLogic() {
    testAdditiveModLogic_initial();
    testAdditiveModLogic_notZero();
    testAdditiveModLogic_notConnected();
}