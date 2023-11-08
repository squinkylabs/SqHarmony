
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
    assertAllOutputsX(l, 1);
}

// turn up all the adsr, and all the mods
static void testAdditiveModLogic_notZero() {
    const unsigned N = 12;

    AdditiveModLogic<12> l;
    assertAllOutputsX(l, 1);
    for (unsigned i = 0; i < N; ++i) {
        l.setADSRTrimValue(0, i, 1.f);
        l.setADSRTrimValue(1, i, 1.f);
        l.setADSRTrimValue(2, i, 1.f);
    }
    l.setADSRValue(0, 1.f, true);
    l.setADSRValue(1, 1.f, true);
    l.setADSRValue(2, 1.f, true);
    assertAllOutputsX(l, 1);
}

// don't connect the adsr
static void testAdditiveModLogic_notConnected() {
    // SQINFO("\n\n-------------- testAdditiveModLogic_notConnected");
    const unsigned N = 12;
    AdditiveModLogic<12> l;

    assertAllOutputsX(l, 1);
    for (unsigned i = 0; i < N; ++i) {
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

template <unsigned N>
static void assertOneHarmonic(AdditiveModLogic<N>& l, unsigned harmonic, float value) {
    //SQINFO("assertOneHarmonic called with harm = %d, value =%f", harmonic, value);
    for (unsigned i = 0; i < N; ++i) {
        const float expected = (i == harmonic) ? value : 0;
        const float actual = l.getCombinedADSR(i);
        assertEQ(expected, actual);
    }
}

template <unsigned N>
static void testAdditiveModLogic_singleADSRsingleHarmonic(AdditiveModLogic<N>& l, unsigned adsr, unsigned harmonic) {
    // SQINFO("\n\n--------------- testAdditiveModLogic_singleADSRsingleHarmonic adsr=%d ha=%d", adsr, harmonic);
    // connect only the passed asr, and make it 1. all others zero
    for (unsigned j = 0; j < l.numADSR; ++j) {
        if (j == adsr) {
            l.setADSRValue(j, 1, true);
        } else {
            l.setADSRValue(j, 0, false);
        }
    }

    for (unsigned i = 0; i < N; ++i) {
        for (unsigned j = 0; j < l.numADSR; ++j) {
            // put the trim up only on the passed harmonic
            l.setADSRTrimValue(j, i, (i == harmonic && j == adsr) ? 1.f : 0.f);
        }
    }
    assertOneHarmonic(l, harmonic, 1);
}

static void testAdditiveModLogic_singleADSRsingleHarmonic() {
    const unsigned N = 12;
    AdditiveModLogic<12> l;

    for (unsigned i = 0; i < N; ++i) {
        for (unsigned j = 0; j < l.numADSR; ++j) {
            testAdditiveModLogic_singleADSRsingleHarmonic(l, j, i);
        }
    }
}

void testAdditiveModLogic() {
    testAdditiveModLogic_initial();
    testAdditiveModLogic_notZero();
    testAdditiveModLogic_notConnected();
    testAdditiveModLogic_singleADSRsingleHarmonic();
}