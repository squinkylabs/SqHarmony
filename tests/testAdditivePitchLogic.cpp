
#include "asserts.h"
#include "AdditivePitchLogic.h"

static void testAdditivePitchLogic_initialPitch() {
    const static unsigned N = 16;
    AdditivePitchLogic<N> l;
    for (auto i = 0; i < N; ++i) {
        assertEQ(l.getPitch(i), 0.f);
    }
}


void testAdditivePitchLogic() {
    testAdditivePitchLogic_initialPitch();
}