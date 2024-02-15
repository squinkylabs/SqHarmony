#include "TestLFO.h"
#include "asserts.h"

static void testLFO0() {
    TestLFO lfo;
    lfo.setFreq(.4);
    lfo.setAmp(1);
    const float x = lfo.process();
}

static void testLFO1() {
    TestLFO lfo;
    lfo.setFreq(.04f);
    lfo.setAmp(1);
    float x = lfo.process();

    // cosine starts at 1;
    assertEQ(x, 1);
    x = lfo.process();
    assertLT(x, 1);
    assertGT(x, .9);
}

static void testLFO2() {
    TestLFO lfo;
    lfo.setFreq(.25f);
    lfo.setAmp(1);

    float x = lfo.process();
    assertEQ(x, 1);
    x = lfo.process();
    assertClose(x, 0, .000001);
    x = lfo.process();
    assertClose(x, -1, .000001);
    x = lfo.process();
    assertClose(x, 0, .000001);
}

void testTestLFO() {
    testLFO0();
    testLFO1();
    testLFO2();
    assert(false);
}

#if 1
void testFirst() {
    testTestLFO();
}
#endif