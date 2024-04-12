
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
        const float v = c.outputs[Comp::CK_OUTPUT].getVoltage(0);
        assertClose(v, expectedClock, .001);
    }
}

// Sends one high, numlow low
static void clockItHighLowLGC(Comp& c, int numLow) {
    clockItHigh(c, -1);
    clockItLow(c, numLow, -1);
}

// sends one high, then enough lows to round out the period
static void clockItHighLow(Comp& c, int totalPeriod) {
    const int numLow = totalPeriod - 1;
    clockItHigh(c, -1);
    clockItLow(c, numLow, -1);
}

static CompPtr factory() {
    CompPtr comp = std::make_shared<Comp>();
    comp->params[Comp::RIB_SPAN_PARAM].value = 1;  // 0 makes test fail.
    comp->inputs[Comp::CK_INPUT].channels = 1;     // connect the input clock
    comp->outputs[Comp::CK_OUTPUT].channels = 1;   // and connect one output

    comp->inputs[Comp::CK_INPUT].setVoltage(0);  // init clock to low
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
    // SQINFO("testSimpleInputNoShift");
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
    // SQINFO(" ---- start testWithShift(%f, %f)", shiftParam, rangeParam);
    const int period = 8;
    assertEQ(shiftParam, .5f);

    auto c = factory();
    // SQINFO("--- test back from factory, will process block");
    c->params[Comp::SHIFT_RANGE_PARAM].value = rangeParam;
    processBlock(c);  // so it can see the ins and outs

    int expectedDelay = period * shiftParam;
    if (rangeParam > .5) expectedDelay *= 10;
    if (rangeParam > 1.5) expectedDelay *= 10;
    // SQINFO("--- test done with initial process block. expected delay = %d", expectedDelay);
    float shift = .5;
    c->params[Comp::SHIFT_PARAM].value = shift;

    // Let's make input clock period == 8.
    // Send first clock to prime, should still have no output.
    clockItHighLow(*c, period);

    // SQINFO("--- test done with initial 'prime'");

    // first clock established the period, and is first clock.
    // clock 4 to get a 50% duty cycle.
    // with delay, expect zeros.
    for (int i = 0; i < period / 2; ++i) {
        clockItHigh(*c, 0);
    }

    // SQINFO("--- test clocked in the fat clock.");

    // will only have output clock in first period if delay rang small
    const float expectedClockInFirstPeriod = (rangeParam < .5) ? 10 : 0;

    clockItLow(*c, period / 2, expectedClockInFirstPeriod);

    // all done for short range
    if (rangeParam < .5) {
        return;
    }

    const int cyclesToOutput = (rangeParam < 1.5) ? 5 : 50;
    // We have already done one half period of fat clock and half period on low clock.
    int samplesToOutput = (cyclesToOutput * period) - period;
    // SQINFO("--- now will finish: samples to output = %d", samplesToOutput);

    while (samplesToOutput) {
        int samplesThisTime = std::min(samplesToOutput, period);
        // SQINFO("--- in loop, this time =%d", samplesThisTime);
        // clockItHigh(*c, 0);
        clockItLow(*c, samplesThisTime, 0);
        samplesToOutput -= samplesThisTime;
    }

    assertEQ(samplesToOutput, 0);

    // now, after the delay, we see out 50% period clock come out.
    // SQINFO("--- In test, now we expect the clock to come");
    clockItLow(*c, period / 2, 10);
    // SQINFO("--- after block of high, will look for all low");
    clockItLow(*c, period * 2, 0);
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

static void testRIBButtons(bool outputConnected) {
    auto c = factory();
    c->outputs[Comp::CK_OUTPUT].channels = outputConnected ? 1 : 0;

    c->params[Comp::RIB_DURATION_PARAM].value = .1;

    processBlock(c);          // so it can see the ins and outs
    clockItHighLow(*c, 100);  // prime
    clockItHighLow(*c, 100);

    assertLT(c->lights[Comp::RIB_POSITIVE_LIGHT].value, 5);
    c->params[Comp::RIB_POSITIVE_BUTTON_PARAM].value = 10;
    processBlock(c);
    processBlock(c);
    processBlock(c);
    assertGT(c->lights[Comp::RIB_POSITIVE_LIGHT].value, 5);
}

static void testRIBButtons() {
    testRIBButtons(true);
    testRIBButtons(false);
}

static void testShiftCV(int shift, float expectedShift) {
    auto c = factory();
    c->inputs[Comp::SHIFT_INPUT].channels = 1;
    c->inputs[Comp::SHIFT_INPUT].setVoltage(10, 0);
    c->params[Comp::SHIFT_RANGE_PARAM].value = shift;
    processBlock(c);  // so it can see the ins and outs
    const float f = c->params[Comp::COMBINED_SHIFT_INTERNAL_PARAM].value;
    assertClose(f, expectedShift, .001);
}

static void testShiftCV() {
    testShiftCV(0, 1);
    testShiftCV(1, 10);
    testShiftCV(2, 100);
}

static void testNegativeCV() {
    auto c = factory();
    c->inputs[Comp::SHIFT_INPUT].channels = 1;
    c->inputs[Comp::SHIFT_INPUT].setVoltage(-10, 0);
    processBlock(c);  //
    const float v = c->_getShift(0);
    assertGE(v, 0);
}

static void testIndex2Value() {
    assertEQ(Comp::indexToValueRibDuration(0), 1.f / 3.f);
    assertEQ(Comp::indexToValueRibDuration(1), 1.f / 2.f);
    assertEQ(Comp::indexToValueRibDuration(2), 1.f);
    assertEQ(Comp::indexToValueRibDuration(3), 2.f);
    assertEQ(Comp::indexToValueRibDuration(4), 3.f);

    assertEQ(Comp::valueToIndexRibDuration(1.f / 3.f), 0);
    assertEQ(Comp::valueToIndexRibDuration(1.f / 2.f), 1);
    assertEQ(Comp::valueToIndexRibDuration(1.f), 2);
    assertEQ(Comp::valueToIndexRibDuration(2.f), 3);
    assertEQ(Comp::valueToIndexRibDuration(3.f), 4);
}

void testPhasePatterns() {
    testOver1();
    testSimpleInputNoShift();
    testWithShift();
    testUIDurations();
    testRIBButtons();
    testShiftCV();
    testNegativeCV();
    testIndex2Value();
}

#if 0
void testFirst() {
   // testIndex2Value();
   // testNegativeCV();
   testRIBButtons();
}
#endif