
#include "AdditivePitchLogic.h"
#include "asserts.h"

template <unsigned N>
inline void assertAreAtHarmonics(const AdditivePitchLogic<N>& l) {
    const float base = l.getPitch(0);
    const float base2 = l.getPitch(1);

    assertEQ(base2, (base + 1));
    assertGT(l.getPitch(2), base2);

    const float base4 = l.getPitch(3);
    assertEQ(base4, (base + 2));

    assertLT(l.getPitch(2), base4);
    assertGT(l.getPitch(4), base4);

    if (N >= 7) {
        const float base8 = l.getPitch(7);
        assertEQ(base8, (base + 3));
        assertLT(l.getPitch(4), base8);
    }

    if (N >= 15) {
        const float base16 = l.getPitch(15);
        assertEQ(base16, (base + 4));
    }

    for (unsigned i = 1; i < N; ++i) {
        assertGT(l.getPitch(i), l.getPitch(i - 1));
    }
}

static void testAdditivePitchLogic_initialPitch() {
    const static unsigned N = 16;
    AdditivePitchLogic<N> l;
    assertEQ(l.getPitch(0), 0.f);
    assertAreAtHarmonics(l);
}

static void testAdditivePitchLogic_cv(float cv) {
    const static unsigned N = 6;
    AdditivePitchLogic<N> l;
    l.setCV(cv);
    assertEQ(l.getPitch(0), cv);
    assertAreAtHarmonics(l);
}

static void testAdditivePitchLogic_cv() {
    testAdditivePitchLogic_cv(1);
    testAdditivePitchLogic_cv(2);
    testAdditivePitchLogic_cv(-3);
    testAdditivePitchLogic_cv(0.1f);
    testAdditivePitchLogic_cv(-0.1f);
}

static void testAdditivePitchLogic_outOfRange() {
    const static unsigned N = 9;
    AdditivePitchLogic<N> l;
    assertEQ(l.getPitch(N), 0);
    assertEQ(l.getPitch(N+1), 0);
    assertNE(l.getPitch(N-1), 0);

    l.setCV(1.3f);
    assertEQ(l.getPitch(N), 0);
    assertEQ(l.getPitch(N+1), 0);
    assertNE(l.getPitch(N-1), 0);
}

void testAdditivePitchLogic() {
    testAdditivePitchLogic_initialPitch();
    testAdditivePitchLogic_cv();
    testAdditivePitchLogic_outOfRange();
}