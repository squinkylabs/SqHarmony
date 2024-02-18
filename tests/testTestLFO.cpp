#include "TestLFO.h"
#include "asserts.h"

static void testLFO0() {
    TestLFO lfo;
    lfo.setFreq(.4);
    lfo.setAmp(1);
    lfo.setSin(true);
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

static void testLFOAmpl(float ampl) {
    TestLFO lfo;
    lfo.setFreq(.25f);
    lfo.setAmp(ampl);
    float x = lfo.process();
    assertEQ(x, ampl);
    x = lfo.process();
    x = lfo.process();
    assertEQ(x, -ampl);
}

static void testLFOAmpl() {
    testLFOAmpl(0);
    testLFOAmpl(2);
    testLFOAmpl(.01f);
}

static void testSetSineFalse() {
    TestLFO l;
    l.setAmp(1);
    l.setFreq(.1);
    l.setSin(false);
    // cosine of zero is 1
    assertClose(l.process(), 1, .00001);
}

static void testSetSineTrue() {
      TestLFO l;
    l.setAmp(1);
    l.setFreq(.1);
    l.setSin(true);
    // sine of zero is 0
    assertClose(l.process(), 0, .00001);
}

static void testSetSine() {
    testSetSineFalse();
    testSetSineTrue();
}

void testTestLFO() {
    testLFO0();
    testLFO1();
    testLFO2();
    testLFOAmpl();
    testSetSine();
}

static void showLFO() {
    TestLFO lfo;
    lfo.setFreq(.02f);
    lfo.setAmp(10);
    for (int i = 0; i < 50; ++i) {
        const float f = lfo.process();
        SQINFO("i=%d %f", i, f);
    }
}

#if 1
void testFirst() {
    testTestLFO();
    // showLFO();
}
#endif