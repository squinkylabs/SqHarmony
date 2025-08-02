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

// tests that initial voltage comes through after process
static void testInitial2() {
    Comp c;
    const auto args = TestComposite::ProcessArgs();

    c.inputs[Comp::INITIAL_VOLTAGE_INPUT].channels = 8;
    const float testV = 3.f + 7.f / 12.f;  // pick a quantized pitch
    c.inputs[Comp::INITIAL_VOLTAGE_INPUT].setVoltage(testV, 0);

    c.process(args);

    const float v = c.outputs[Comp::NOTES_OUTPUT].getVoltage(0);
    assertEQ(v, testV);
}

static void testInitialQuantize() {
    Comp c;
    const auto args = TestComposite::ProcessArgs();

    c.inputs[Comp::INITIAL_VOLTAGE_INPUT].channels = 8;
    const float testV = 3.f + 7.f / 12.f;     // pick a quantized pitch
    const float inputV = testV + .3f / 12.f;  // but send an un quantized one.
    c.inputs[Comp::INITIAL_VOLTAGE_INPUT].setVoltage(inputV, 0);

    c.process(args);

    const float v = c.outputs[Comp::NOTES_OUTPUT].getVoltage(0);
    assertEQ(v, testV);
}

static void testInitialOnlyOnce() {
    Comp c;
    const auto args = TestComposite::ProcessArgs();

    c.inputs[Comp::INITIAL_VOLTAGE_INPUT].channels = 8;
    const float testV = 3.f + 7.f / 12.f;  // pick a quantized pitch
    c.inputs[Comp::INITIAL_VOLTAGE_INPUT].setVoltage(testV, 0);

    // process once to pick up testV.
    c.process(args);
    // now send a different voltage
    c.inputs[Comp::INITIAL_VOLTAGE_INPUT].setVoltage(testV + 2.3f, 0);
    c.process(args);

    const float v = c.outputs[Comp::NOTES_OUTPUT].getVoltage(0);
}

static void init(Comp& c) {
    // hook up io
    c.outputs[Comp::NOTES_OUTPUT].channels = 1;

    // init the params
    c.params[Comp::KEY_PARAM].value = 0;
    c.params[Comp::MODE_PARAM].value = 0;
    c.params[Comp::ROW_LENGTH_PARAM].value = 8;
    c.params[Comp::NON_CENTERED_WEIGHT_STYLE_PARAM].value = .5;
    c.params[Comp::PITCH_RANGE_WEIGHT_STYLE_PARAM].value = .5;
    c.params[Comp::PITCH_RANGE_STYLE_PARAM].value = 24;
    c.params[Comp::LEAPS_WEIGHT_STYLE_PARAM].value = .5;
    c.params[Comp::UNISON_WEIGHT_STYLE_PARAM].value = .5;
    c.params[Comp::SLOTS_TO_CHANGE_PARAM].value = 1;
    c.params[Comp::ADJACENT_SLOTS_PARAM].value = 1;
}

static void clockIt(Comp& c, unsigned int count) {
    assertGT(count, 0);
    // Comp c;
    const auto args = TestComposite::ProcessArgs();

    // make sure it's zero
    c.inputs[Comp::MUTATE_INPUT].setVoltage(0, 0);
    c.process(args);

    // clock it
    for (unsigned int i = 0; i < count; ++i) {
        c.inputs[Comp::MUTATE_INPUT].setVoltage(10, 0);
        c.process(args);
        c.inputs[Comp::MUTATE_INPUT].setVoltage(0, 0);
        c.process(args);
    }
}

static void testInitialSteps() {
    Comp c;
    init(c);
    clockIt(c, 1);
    const int x = c.outputs[Comp::NOTES_OUTPUT].channels;
    assertEQ(x, 8);
}

static void testStepsX(int steps) {
    Comp c;
    init(c);
    c.params[Comp::ROW_LENGTH_PARAM].value = steps;
    clockIt(c, 1);
    const int x = c.outputs[Comp::NOTES_OUTPUT].channels;
    assertEQ(x, steps);
}

static void testSteps() {
    for (int i=1; i<=16; ++i) {
        testStepsX(i);
    }
}

void testMutatorComposite() {
    testCanCall();
    // Tests for setting initial voltage
    testInitial1();
    testInitial2();
    testInitialQuantize();
    testInitialOnlyOnce();

    testInitialSteps();
    testSteps();
}

#if 1
void testFirst() {
    testMutatorComposite();
   // testInitial2();
   // testSteps();
}
#endif
