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
    const float testV = 3.f + 7.f / 12.f;           // pick a quantized pitch
    c.inputs[Comp::INITIAL_VOLTAGE_INPUT].setVoltage(testV, 0);

    c.process(args);

    const float v = c.outputs[Comp::NOTES_OUTPUT].getVoltage(0);
    assertEQ(v, testV);
}

static void testInitialQuantize() {
    Comp c;
    const auto args = TestComposite::ProcessArgs();

    c.inputs[Comp::INITIAL_VOLTAGE_INPUT].channels = 8;
    const float testV = 3.f + 7.f / 12.f;           // pick a quantized pitch
    const float inputV = testV + .3f / 12.f;        // but send an un quantized one.
    c.inputs[Comp::INITIAL_VOLTAGE_INPUT].setVoltage(inputV, 0);

    c.process(args);

    const float v = c.outputs[Comp::NOTES_OUTPUT].getVoltage(0);
    assertEQ(v, testV);
}

static void testInitialOnlyOnce() {
    Comp c;
    const auto args = TestComposite::ProcessArgs();

    c.inputs[Comp::INITIAL_VOLTAGE_INPUT].channels = 8;
    const float testV = 3.f + 7.f / 12.f;           // pick a quantized pitch
    c.inputs[Comp::INITIAL_VOLTAGE_INPUT].setVoltage(testV, 0);

    // process once to pick up testV.
    c.process(args);
    // now send a different voltage
    c.inputs[Comp::INITIAL_VOLTAGE_INPUT].setVoltage(testV + 2.3f, 0);
    c.process(args);

    const float v = c.outputs[Comp::NOTES_OUTPUT].getVoltage(0);
}

void testMutatorComposite() {
    testCanCall();
    testInitial1();
    testInitial2();
    testInitialQuantize();
    testInitialOnlyOnce();
}

#if 1
void testFirst() {
    testMutatorComposite();
}
#endif
