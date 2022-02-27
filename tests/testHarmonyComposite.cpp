
#include "Harmony.h"
#include "TestComposite.h"
#include "asserts.h"

using Comp = Harmony<TestComposite>;

static void test0() {
    Comp h;

    // hook up the input
    h.inputs[Comp::CV_INPUT].channels = 1;
    // and the output
    h.outputs[Comp::QUANTIZER_OUTPUT].channels = 1;
    h.inputs[Comp::CV_INPUT].setVoltage(0, 0);
    h.process(TestComposite::ProcessArgs());
    assertEQ(h.outputs[Comp::QUANTIZER_OUTPUT].getVoltage(0), 0);
}

static void testQuant1() {
    Comp h;
    h.inputs[Comp::CV_INPUT].channels = 1;
    h.inputs[Comp::CV_INPUT].setVoltage(1.f, 0);
    h.process(TestComposite::ProcessArgs());
    assertEQ(h.outputs[Comp::QUANTIZER_OUTPUT].getVoltage(0), 1);
}

static void testRunABit() {
    Comp h;
    h.inputs[Comp::CV_INPUT].channels = 1;
    h.inputs[Comp::CV_INPUT].setVoltage(1.f, 0);
    h.process(TestComposite::ProcessArgs());
    h.inputs[Comp::CV_INPUT].setVoltage(1.5f, 0);
    h.process(TestComposite::ProcessArgs());
    h.inputs[Comp::CV_INPUT].setVoltage(1.2f, 0);
    h.process(TestComposite::ProcessArgs());
    h.inputs[Comp::CV_INPUT].setVoltage(1.4f, 0);
    h.process(TestComposite::ProcessArgs());
}

// Seeing C3 and then C4 used to cause asserts.
static void testOctave() {
    Comp h;
    h.inputs[Comp::CV_INPUT].channels = 1;
    h.inputs[Comp::CV_INPUT].setVoltage(1.f, 0);
    h.process(TestComposite::ProcessArgs());
    h.process(TestComposite::ProcessArgs());

    h.inputs[Comp::CV_INPUT].setVoltage(2.0f, 0);
    h.process(TestComposite::ProcessArgs());
    h.process(TestComposite::ProcessArgs());
}

static void testSemiUpDown() {
    Comp h;
    h.inputs[Comp::CV_INPUT].channels = 1;
    h.inputs[Comp::CV_INPUT].setVoltage(1.f, 0);
    h.process(TestComposite::ProcessArgs());
    h.process(TestComposite::ProcessArgs());
    const float root1 = h.outputs[Comp::BASS_OUTPUT].value;

    h.inputs[Comp::CV_INPUT].setVoltage(1.f + 1/12.f, 0);
    h.process(TestComposite::ProcessArgs());
    h.process(TestComposite::ProcessArgs());
    const float root2 = h.outputs[Comp::BASS_OUTPUT].value;

     h.inputs[Comp::CV_INPUT].setVoltage(1.f, 0);
    h.process(TestComposite::ProcessArgs());
    h.process(TestComposite::ProcessArgs());
    const float root3 = h.outputs[Comp::BASS_OUTPUT].value;

    assert(root2 != root1);
    assert(root2 != root3);

}
void testHarmonyComposite() {
    test0();
    testQuant1();
    testRunABit();
    testOctave();
    testSemiUpDown();
}