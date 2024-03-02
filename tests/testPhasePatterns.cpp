
#include <memory>

#include "PhasePatterns.h"
#include "TestComposite.h"
#include "asserts.h"

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

/** If expectedOutput < 0, will not check. Normal values are 0 and 10
 */
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

static void clockItHigh(Comp& c, float expectedClock) {
    const auto args = TestComposite::ProcessArgs();
    c.inputs[Comp::CK_INPUT].setVoltage(10);
    c.process(args);
    if (expectedClock >= 0) {
        //assert(false);
        const float v = c.outputs[Comp::CK_OUTPUT].getVoltage(0);
        assertClose(v, expectedClock, .001);
    }
}

static void clockItHighLow(Comp& c, int numLow) {
    clockItHigh(c, -1);
    clockItLow(c, numLow, -1);
}

static CompPtr factory() {
    CompPtr comp = std::make_shared<Comp>();
    comp->params[Comp::RIB_SPAN_PARAM].value = 1;  // 0 makes test fail.
    comp->inputs[Comp::CK_INPUT].channels = 1;     // connect the input clock
    comp->outputs[Comp::CK_OUTPUT].channels = 1;   // and connect one output
    return comp;
}

static void processBlock(CompPtr comp) {
    const auto args = TestComposite::ProcessArgs();
    // process enough times to do the ribs thing.
    for (int i = 0; i < Comp::getSubSampleFactor(); ++i) {
        comp->process(args);
    }
}

static void testSimpleInputNoShift() {
    //  Comp c;
    SQINFO("testSimpleInputNoShift");
    auto c = factory();
    processBlock(c);  // so it can see the ins and outs

    // Let's make input clock period == 10.
    // Send first two clocks to prime, should still have no output.
    c->params[Comp::SHIFT_PARAM].value = 0;
    clockItHighLow(*c, 9);
    clockItHighLow(*c, 9);
    assertEQ(c->outputs[Comp::CK_OUTPUT].getVoltage(), 0);

    // Then the third clock will actually do something. (low to high if no shift)
    clockItHigh(*c, -1);
    assertEQ(c->outputs[Comp::CK_OUTPUT].getVoltage(), 10);

    // clock staying high should continue to output high (total three periods high
    clockItHigh(*c, -1);
    assertEQ(c->outputs[Comp::CK_OUTPUT].getVoltage(), 10);
    clockItHigh(*c, -1);
    assertEQ(c->outputs[Comp::CK_OUTPUT].getVoltage(), 10);

    // Now, first low after the three high should force a low output (since no shift).
    clockItLow(*c, 1, -1);
    assertEQ(c->outputs[Comp::CK_OUTPUT].getVoltage(), 0);

    // clock  should stay low, too
    clockItLow(*c, 3, -1);
    assertEQ(c->outputs[Comp::CK_OUTPUT].getVoltage(), 0);
}

static void testWithShift(float shiftParam, float rangeParam) {
    SQINFO("start test");
    assertEQ(shiftParam, .5f);
    assertEQ(rangeParam, 0);
   // ClockShifter6::llv = 1;
    auto c = factory();
    processBlock(c);  // so it can see the ins and outs

    float shift = .5;
    c->params[Comp::SHIFT_PARAM].value = shift;

    // Let's make input clock period == 8.
    // Send first two clocks to prime, should still have no output.

    clockItHighLow(*c, 8);

    // first clock established the period, and is first clock.
    // clock 4 to get a 50% duty cycle.
    // with delay, expect zeros.
    clockItHigh(*c, 0);
    clockItHigh(*c, 0);
    clockItHigh(*c, 0);
    clockItHigh(*c, 0);

    // now clock low, but output will have delayed high
    clockItLow(*c, 1, 10);
    clockItLow(*c, 1, 10);
    clockItLow(*c, 1, 10);
    clockItLow(*c, 1, 10);


    

    // bool clockIn = true;
    // float clockV = (clockIn) ? 10 : 0;
    // // now raise the clock
    // c->inputs[Comp::CK_INPUT].setVoltage(clockV, 0);

}

static void testWithShift() {
    testWithShift(.5, 0);
    testWithShift(.5, 1);
    testWithShift(.5, 2);
}

static void testUIDurations() {
    assertEQ(Comp::getRibDurationLabels()[0], "1/3");
    assertEQ(Comp::getRibDurationLabels()[1], "1/2");
    assertEQ(Comp::getRibDurationLabels()[2], "1");
    assertEQ(Comp::getRibDurationLabels()[3], "2");
    assertEQ(Comp::getRibDurationLabels()[4], "3");

    assertEQ(Comp::getRibDurationFromIndex(0), 1.f / 3.f);
    assertEQ(Comp::getRibDurationFromIndex(1), 1.f / 2.f);
    assertEQ(Comp::getRibDurationFromIndex(2), 1.f);
    assertEQ(Comp::getRibDurationFromIndex(3), 2.f);
    assertEQ(Comp::getRibDurationFromIndex(4), 3.f);
}

void testPhasePatterns() {
    testOver1();
    testSimpleInputNoShift();
    testWithShift();
    testUIDurations();
}

#if 1
void testFirst() {
    // ClockShifter6::llv = 1;
    //  This is the case that is bad without the dodgy "fix"
    // testWithLFO(4, 16, 0.136364, 0.400000, 3);

    // testSlowDown(5, 3552, 0.0001407658, 7);
    testWithShift();
}
#endif