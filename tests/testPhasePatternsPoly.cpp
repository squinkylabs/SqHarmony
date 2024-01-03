
// #include "TestComposite.h"  // need a Port definition.

// #include "PolyClockShifter.h"

#include "PhasePatterns.h"
#include "TestComposite.h"
#include "asserts.h"

using Comp = PhasePatterns<TestComposite>;

static int testPeriod = 10;

static void clockItLow(Comp& c, int numTimes, int channel, float expectedOutput) {
    const auto args = TestComposite::ProcessArgs();
    c.inputs[Comp::CK_INPUT].setVoltage(0, channel);
    for (int i = 0; i < numTimes; ++i) {
        c.process(args);
        if (expectedOutput >= 0) {
            assertEQ(c.outputs[Comp::CK_OUTPUT].getVoltage(channel), expectedOutput);
        }
    }
}

static void clockItHigh(Comp& c, int channel) {
    // c.outputs[]
    const auto args = TestComposite::ProcessArgs();
    c.inputs[Comp::CK_INPUT].setVoltage(10, channel);
    c.process(args);
}

static void clockItHighLow(Comp& c, int channel, int numLow) {
    clockItHigh(c, channel);
    clockItLow(c, numLow, channel, -1);
}

// does high + low, but not final high.
static void prime(Comp& comp, int channel, int period = testPeriod) {
    const auto args = TestComposite::ProcessArgs();
    comp.outputs[Comp::CK_OUTPUT].channels = 1;  // connect the clock output
    comp.process(args);
    clockItHighLow(comp, channel, period - 1);
}

static void testChannelsSub(
    int clockInChannels,
    int shiftAmountChannels,
    int ribsChannels,
    int expectedClockChannels) {

    Comp comp;
    const auto args = TestComposite::ProcessArgs();

    comp.process(args);                          // process once so that the first time we don't to the block.
    comp.outputs[Comp::CK_OUTPUT].channels = 1;  // connect the output
    comp.inputs[Comp::CK_INPUT].channels = clockInChannels;
    comp.inputs[Comp::SHIFT_INPUT].channels = shiftAmountChannels;
    comp.inputs[Comp::RIB_INPUT].channels = ribsChannels;

    // init, should be connected but not set
    assertEQ(int(comp.outputs[Comp::CK_OUTPUT].channels), 1);
    comp.process(args);  // this also should not set up any channels (since block won't run).
    assertEQ(int(comp.outputs[Comp::CK_OUTPUT].channels), 1);
    for (int i = 0; i < Comp::getSubSampleFactor(); ++i) {
        comp.process(args);
    }
    assertEQ(int(comp.outputs[Comp::CK_OUTPUT].channels), expectedClockChannels);
}

static void testChannels() {
    testChannelsSub(0, 0, 0, 1);
    testChannelsSub(1, 1, 1, 1);
    testChannelsSub(2, 1, 1, 2);
    testChannelsSub(1, 2, 1, 2);
    testChannelsSub(1, 1, 2, 2);
    testChannelsSub(16, 16, 16, 16);
}

static void testCanClockPolySub(int clockInChannels, int channelToTest) {
    Comp comp;
    const auto args = TestComposite::ProcessArgs();

    comp.outputs[Comp::CK_OUTPUT].channels = 1;  // connect the output
    comp.inputs[Comp::CK_INPUT].channels = clockInChannels;

    prime(comp, channelToTest);
    // Even though we are "primed" we will not emit a clock for one more period,
    // if shift is zero.
    // TODO: what is this input on channel 1??
    clockItHighLow(comp, 1, testPeriod - 1);

    comp.inputs[Comp::CK_INPUT].setVoltage(10, channelToTest);

    comp.process(args);
    for (int i = 0; i < 16; ++i) {
        const auto v = comp.outputs[Comp::CK_OUTPUT].getVoltage(i);
        const auto expect = (i == channelToTest) ? 10.f : 0.f;
        assertEQ(v, expect);
    }

    // one more clock and it should go down.
    comp.inputs[Comp::CK_INPUT].setVoltage(0, channelToTest);

    comp.process(args);
    assertEQ(comp.outputs[Comp::CK_OUTPUT].getVoltage(channelToTest), 0);
}

static void testCanClockMonoSub(int shiftChannels) {
    Comp comp;                                   // only poly input is shift amount
    comp.outputs[Comp::CK_OUTPUT].channels = 1;  // connect the output
    comp.inputs[Comp::SHIFT_INPUT].channels = shiftChannels;
    comp.inputs[Comp::CK_INPUT].channels = 1;

    const auto args = TestComposite::ProcessArgs();
    prime(comp, 0);
    // Even though we are "primed" we will not emit a clock for one more period,
    // if shift is zero.
    clockItHighLow(comp, 0, testPeriod - 1);

    // Set the mono clock high, and run.

    comp.inputs[Comp::CK_INPUT].setVoltage(10, 0);

    comp.process(args);
    const int actualShiftChannels = std::max(shiftChannels, 1);  // there is always one channel...
    for (int i = 0; i < 16; ++i) {
        const auto v = comp.outputs[Comp::CK_OUTPUT].getVoltage(i);

        // this less than is problematic... It's not right...
        const auto expect = (i < actualShiftChannels) ? 10.f : 0.f;
        assertEQ(v, expect);
    }

    // one more clock and it should go down.
    // comp._clockInput->setVoltage(0, 0);
    comp.inputs[Comp::CK_INPUT].setVoltage(0, 0);

    comp.process(args);
    for (int i = 0; i < 16; ++i) {
        const auto v = comp.outputs[Comp::CK_OUTPUT].getVoltage(i);
        const auto expect = 0.f;
        assertEQ(v, expect);
    }
}

static void testCanClockMono() {
    testCanClockMonoSub(12);
    testCanClockMonoSub(2);
    testCanClockMonoSub(1);
    testCanClockMonoSub(0);
    testCanClockMonoSub(16);
}

static void testCanClock() {
    testCanClockPolySub(1, 0);  // this one ok
    testCanClockPolySub(2, 0);
    testCanClockPolySub(2, 1);
}

void testPhasePatternsPoly() {
    testChannels();
    testCanClock();
    testCanClockMono();
}