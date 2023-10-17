
#include <memory>

#include "PhasePatterns.h"
#include "TestComposite.h"

using Comp = PhasePatterns<TestComposite>;
using CompPtr = std::shared_ptr<Comp>;

static void test0() {
    Comp p;
}

static void proc(Comp& comp, int iterations) {
    const auto args = TestComposite::ProcessArgs();
    for (int i = 0; i < iterations; ++i) {
        comp.process(args);
    }
}

const void testOver1() {
    Comp p;
    auto args = TestComposite::ProcessArgs();
    p.params[Comp::SHIFT_PARAM].value = 2.f;
    proc(p, 16);  // just want to be sure this doesn't assert or crash
}

static void clockIt(Comp& c, int numTimes, float expectedOutput) {
    const auto args = TestComposite::ProcessArgs();
    c.inputs[Comp::CK_INPUT].setVoltage(0);
    for (int i = 0; i < numTimes; ++i) {
        c.process(args);
        if (expectedOutput >= 0) {
            assertEQ(c.outputs[Comp::CK_OUTPUT].getVoltage(0), expectedOutput);
        }
    }
}

static void clockItHigh(Comp& c) {
    const auto args = TestComposite::ProcessArgs();
    c.inputs[Comp::CK_INPUT].setVoltage(10);
    c.process(args);
}

static void testSimpleInput(int iter, float shift) {
    Comp c;

    c.params[Comp::SHIFT_PARAM].value = shift;
    clockIt(c, iter, -1);

    // then a single trigger input
    clockItHigh(c);
    assertEQ(c.outputs[Comp::CK_OUTPUT].getVoltage(), 0);

    clockIt(c, iter, 0);

    // second input should do something
    clockItHigh(c);
    assertGT(c.outputs[Comp::CK_OUTPUT].getVoltage(), 1);
}

static void testSimpleInput() {
    testSimpleInput(10, 0);
    testSimpleInput(10, 1);
    testSimpleInput(10, 2);
    testSimpleInput(10, -1);
}

void testPhasePatterns() {
    SQINFO("-- testPhasePatterns --");
    testOver1();
    testSimpleInput();
}