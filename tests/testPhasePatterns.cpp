
#include <memory>

#include "PhasePatterns2.h"
#include "TestComposite.h"

#include "asserts.h"

using Comp = PhasePatterns2<TestComposite>;
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

static void clockItLow(Comp& c, int numTimes, float expectedOutput) {
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

static void clockItHighLow(Comp& c, int numLow) {
    clockItHigh(c);
    clockItLow(c, numLow, -1);
}

static void testSimpleInputNoShift() {
    Comp c;

    // Let's make input clock period == 10.
    // Send first two clocks to prime, should still have no output.
    c.params[Comp::SHIFT_PARAM].value = 0;
    clockItHighLow(c, 9);
    clockItHighLow(c, 9);
    assertEQ(c.outputs[Comp::CK_OUTPUT].getVoltage(), 0);

    // Then the third clock will actually do something. (low to high if no shift)
    clockItHigh(c);
    assertEQ(c.outputs[Comp::CK_OUTPUT].getVoltage(), 10);

    // clock staying high should continue to output high (total three periods high
    clockItHigh(c);
    assertEQ(c.outputs[Comp::CK_OUTPUT].getVoltage(), 10);
    clockItHigh(c);
    assertEQ(c.outputs[Comp::CK_OUTPUT].getVoltage(), 10);
   
    // Now, first low after the three high should force a low output (since no shift).
    clockItLow(c, 1, -1);
    assertEQ(c.outputs[Comp::CK_OUTPUT].getVoltage(), 0);


    
    // clock going low should be an event, too
    clockItLow(c, 3, -1);

    // second input real should do something
    //clockItHigh(c);
    // failing down here
    assertEQ(c.outputs[Comp::CK_OUTPUT].getVoltage(), 0);
}

// static void testSimpleInputNoShift() {
//     testSimpleInput(10, 0);
// }

// static void testSimpleInput() {
    
//     testSimpleInput(10, 1);
//     testSimpleInput(10, 2);
//     testSimpleInput(10, -1);
// }

void testPhasePatterns() {
    testOver1();
    SQINFO("fix testSimpleInputNoShift and put it back");
  //  testSimpleInputNoShift();
   // testSimpleInput();
}