
#include "AdditivePitchLogic.h"
#include "asserts.h"

template <unsigned N>
inline  bool areAtHarmonics(const AdditivePitchLogic<N>& l) {
    const float base = l.getPitch(0);
    const float base2 = l.getPitch(1);
    // 2 is oct and 5th
    const float base4 = l.getPitch(3);


   
    if (base2 != (base +1)) {
        SQINFO("base 2 = %f 2base=%f", base2, base + 1);
        return false;
    }

    if (base4 != (base + 2)) {
        SQINFO("base 4 = %f 4base=%f", base4, base + 2);
        return false;
    }

    assert(false);

    return true;
}

static void testAdditivePitchLogic_initialPitch() {
    const static unsigned N = 16;
    AdditivePitchLogic<N> l;
    assertEQ(l.getPitch(0), 0.f);
    assert(areAtHarmonics(l));
}

static void testAdditivePitchLogic_cv(float cv) {
    assert(false);
}

static void testAdditivePitchLogic_cv() {
    testAdditivePitchLogic_cv(1);
    testAdditivePitchLogic_cv(2);
    testAdditivePitchLogic_cv(-3);
    testAdditivePitchLogic_cv(0.1f);
    testAdditivePitchLogic_cv(-0.1f);
}

void testAdditivePitchLogic() {
    testAdditivePitchLogic_initialPitch();
    testAdditivePitchLogic_cv();
}