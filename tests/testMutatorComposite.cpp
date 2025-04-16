#include "Mutator.h"
#include "TestComposite.h"
#include "asserts.h"

using Comp = Mutator<TestComposite>;

static void testCanCall() {
    Comp c;
    const auto args = TestComposite::ProcessArgs();
    c.process(args);
}

// tests that initial voltage doesn't come through immediately
static void testInitial1() {
    Comp c;
    c.inputs[Comp::INITIAL_VOLTAGE_INPUT].channels = 8;
    c.inputs[Comp::INITIAL_VOLTAGE_INPUT].setVoltage(1.f, 0);

    const float v = c.outputs[Comp::NOTES_OUTPUT].getVoltage(0);
    assertEQ(v, 0.f)
}

static void testInitial2() {
    Comp c;
    const auto args = TestComposite::ProcessArgs();

    c.inputs[Comp::INITIAL_VOLTAGE_INPUT].channels = 8;
    const float testV = 3.7f;
    c.inputs[Comp::INITIAL_VOLTAGE_INPUT].setVoltage(testV, 0);

    c.process(args);

    const float v = c.outputs[Comp::NOTES_OUTPUT].getVoltage(0);
    assertClose(v, testV, .04);     // TODO: is this "close" because of pitch quantization?
}

void testMutatorComposite() {
    testCanCall();
    testInitial1();
    testInitial2();
}

#if 1
void testFirst() {
    testMutatorComposite();
}
#endif
