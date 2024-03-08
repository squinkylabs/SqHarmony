
// #include "TestComposite.h"  // need a Port definition.

// #include "PolyClockShifter.h"

#include "PhasePatterns.h"
#include "TestComposite.h"
#include "asserts.h"

using Comp = PhasePatterns<TestComposite>;
using CompPtr = std::shared_ptr<Comp>;

// was 10. 60 ok
static int testPeriod = 100;

//----------------- utilities -----------------------
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

static void processBlock(Comp& comp) {
    const auto args = TestComposite::ProcessArgs();
    // process enough times to do the ribs thing.
    for (int i = 0; i < Comp::getSubSampleFactor(); ++i) {
        comp.process(args);
    }
}

static CompPtr factory() {
    CompPtr comp = std::make_shared<Comp>();
    comp->params[Comp::RIB_SPAN_PARAM].value = 1;  // 0 makes test fail.
    comp->inputs[Comp::CK_INPUT].channels = 1;     // connect the input clock
    comp->inputs[Comp::CK_OUTPUT].channels = 1;    // connect the input clock
    return comp;
}

//---------------------------------------------------------------------

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
    comp.inputs[Comp::RIB_POSITIVE_INPUT].channels = ribsChannels;

    // init, should be connected but not set
    assertEQ(int(comp.outputs[Comp::CK_OUTPUT].channels), 1);
    comp.process(args);  // this also should not set up any channels (since block won't run).
    assertEQ(int(comp.outputs[Comp::CK_OUTPUT].channels), 1);
    for (int i = 0; i < Comp::getSubSampleFactor(); ++i) {
        comp.process(args);
    }
    assertEQ(int(comp.outputs[Comp::CK_OUTPUT].channels), expectedClockChannels);
}

// Verify that the output has the the expected number of channels for the given input
static void testChannels() {
    testChannelsSub(0, 0, 0, 1);
    testChannelsSub(1, 1, 1, 1);
    testChannelsSub(2, 1, 1, 2);
    testChannelsSub(1, 2, 1, 2);
    testChannelsSub(1, 1, 2, 2);
    testChannelsSub(16, 16, 16, 16);
}

/**
 * @brief tests multiple polyphonic clocks.
 * Verifies one to one between inputs clock and output clock channels.
 *
 * @param clockInChannels
 * @param channelToTest
 */
static void testCanClockPolySub(int clockInChannels, int channelToTest) {
    Comp comp;
    const auto args = TestComposite::ProcessArgs();

    comp.outputs[Comp::CK_OUTPUT].channels = 1;  // connect the output
    comp.inputs[Comp::CK_INPUT].channels = clockInChannels;

    prime(comp, channelToTest);
    // Even though we are "primed" we will not emit a clock for one more period,
    // if shift is zero.
    clockItHighLow(comp, 1, testPeriod - 1);

    comp.inputs[Comp::CK_INPUT].setVoltage(10, channelToTest);

    comp.process(args);
    for (int i = 0; i < 16; ++i) {
        const auto v = comp.outputs[Comp::CK_OUTPUT].getVoltage(i);
        const auto expect = (i == channelToTest) ? 10.f : 0.f;
        assertEQ(v, expect);
    }

    // One more clock and it should go down.
    comp.inputs[Comp::CK_INPUT].setVoltage(0, channelToTest);

    comp.process(args);
    assertEQ(comp.outputs[Comp::CK_OUTPUT].getVoltage(channelToTest), 0);
}


/** test that poly shift makes to to the right channels.
 * I see now there is already a unit test for this. Oh, well...
 */
static void testPolyShiftCV(int shiftChannels) {
    const float shiftAmount = .73;
    Comp comp;                                   // only poly input is shift amount
    comp.outputs[Comp::CK_OUTPUT].channels = 1;  // connect the output
    comp.inputs[Comp::SHIFT_INPUT].channels = shiftChannels;
    comp.inputs[Comp::CK_INPUT].channels = 1;

    for (int i = 0; i < shiftChannels; ++i) {
        comp.inputs[Comp::SHIFT_INPUT].setVoltage(10 * shiftAmount * i, i);
    }

    const auto args = TestComposite::ProcessArgs();
    prime(comp, 0);
    // Even though we are "primed" we will not emit a clock for one more period,
    // if shift is zero.
    clockItHighLow(comp, 0, testPeriod - 1);

    comp.process(args);
    const int actualShiftChannels = std::max(shiftChannels, 1);  // there is always one channel...
    for (int i = 0; i < 16; ++i) {
        const float expectedShift = (i < shiftChannels) ? shiftAmount * i : 0;
        assertClose(comp._getShift(i), expectedShift, .00001);
    }
}

static void testPolyShiftCV() {
    testPolyShiftCV(1);
    testPolyShiftCV(7);
    testPolyShiftCV(16);
}

// input: shift polyphonic, clock in mono
// checks that the shift input is controlling the active outputs
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

        const auto expect = (i < actualShiftChannels) ? 10.f : 0.f;
        assertEQ(v, expect);
    }

    // one more clock and it should go down.
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

// shift and clock are mono, but ribs can be poly
static void testCanClockMonoWithRibSub(int ribChannels) {
    CompPtr comp = factory();
    comp->inputs[Comp::RIB_POSITIVE_INPUT].channels = ribChannels;
    comp->inputs[Comp::SHIFT_INPUT].channels = 1;

    const auto args = TestComposite::ProcessArgs();
    prime(*comp, 0);
    // Even though we are "primed" we will not emit a clock for one more period,
    // if shift is zero.
    clockItHighLow(*comp, 0, testPeriod - 1);

    // Set the mono clock high, and run.
    comp->inputs[Comp::CK_INPUT].setVoltage(10, 0);
    // process enough times to do the ribs thing.
    for (int i = 0; i < Comp::getSubSampleFactor(); ++i) {
        comp->process(args);
    }
    const int actualRibChannels = std::max(ribChannels, 1);  // there is always one channel...
    for (int i = 0; i < 16; ++i) {
        const auto v = comp->outputs[Comp::CK_OUTPUT].getVoltage(i);
        const auto expect = (i < actualRibChannels) ? 10.f : 0.f;
        assertEQ(v, expect);
    }

    // one more clock and it should go down.
    comp->inputs[Comp::CK_INPUT].setVoltage(0, 0);
    comp->process(args);
    for (int i = 0; i < 16; ++i) {
        const auto v = comp->outputs[Comp::CK_OUTPUT].getVoltage(i);
        const auto expect = 0.f;
        assertEQ(v, expect);
    }
}

static void testCanClockMonoWithRib() {
    testCanClockMonoWithRibSub(1);
    testCanClockMonoWithRibSub(2);
    testCanClockMonoWithRibSub(5);
    testCanClockMonoWithRibSub(11);
    testCanClockMonoWithRibSub(16);
}

class TestX {
public:
    static void testMonoClockPolyTriggerRibsSub(int numRibs, int ribToTest) {
        assert(ribToTest < numRibs);
        CompPtr comp = factory();
        comp->inputs[Comp::RIB_POSITIVE_INPUT].channels = numRibs;

        const auto args = TestComposite::ProcessArgs();
        prime(*comp, 0);
        // Even though we are "primed" we will not emit a clock for one more period,
        // if shift is zero.
        clockItHighLow(*comp, 0, testPeriod - 1);

        // trigger the rib
        comp->inputs[Comp::RIB_POSITIVE_INPUT].setVoltage(10, ribToTest);
        // process enough times to do the ribs thing.
        for (int i = 0; i < Comp::getSubSampleFactor(); ++i) {
            comp->process(args);
        }

        for (int i = 0; i < 16; ++i) {
            const bool expectedRibState = (i == ribToTest);
            assertEQ(comp->_ribGenerator[i].busyEither(), expectedRibState);
        }
    }

    // polyphonic shift
    static void testPolyphonicShiftSub(int inputShiftChannels, int channelToTest) {
        assert(inputShiftChannels > 0);
        assert(channelToTest >= 0);
        assert(channelToTest < inputShiftChannels);

        Comp comp;

        const float shiftAmount = .3f;
        comp.outputs[Comp::CK_OUTPUT].channels = 1;  // connect the output
        comp.inputs[Comp::SHIFT_INPUT].channels = inputShiftChannels;
        comp.inputs[Comp::SHIFT_INPUT].setVoltage(shiftAmount, channelToTest);

        processBlock(comp);

        for (int i = 0; i < 16; ++i) {
            const float shift = comp._curShift[i];
            const float expectedShift = (i == channelToTest) ? shiftAmount * .1 : 0;
            assertEQ(shift, expectedShift);
        }
    }

    // poly clock, shift knob
    static void testShiftKnobPolyClock(int numClockChannels) {
        assert(numClockChannels > 0);

        Comp comp;
        comp.outputs[Comp::CK_OUTPUT].channels = 1;  // connect the output
        comp.inputs[Comp::CK_INPUT].channels = numClockChannels;
        comp.params[Comp::SHIFT_PARAM].value = .5;

        processBlock(comp);

        for (int i = 0; i < 16; ++i) {
            const float shift = comp._curShift[i];
            const float expectedShift = (i < numClockChannels) ? .5 : 0;  // knob should affect all channels
            assertEQ(shift, expectedShift);
        }
    }

    static void testPolyClockMonoShiftCV(int channelsToTest) {
        assert(channelsToTest >= 0);

        const float shiftValue = .7f;
        Comp comp;
        comp.outputs[Comp::CK_OUTPUT].channels = 1;  // connect the output
        comp.inputs[Comp::CK_INPUT].channels = channelsToTest;

        comp.inputs[Comp::SHIFT_INPUT].channels = 1;
        comp.inputs[Comp::SHIFT_INPUT].setVoltage(shiftValue, 0);
        processBlock(comp);

        for (int i = 0; i < 16; ++i) {
            const float shift = comp._curShift[i];
            const float expectedShift = (i < channelsToTest) ? shiftValue * .1 : 0;  // knob should affect all channels
            assertEQ(shift, expectedShift);
        }
    }

    static void testPolyRibButton() {
        CompPtr comp = factory();
        assertEQ(comp->inputs[Comp::CK_INPUT].channels, 1);
        comp->outputs[Comp::CK_OUTPUT].channels = 1;          // connect the output
        comp->inputs[Comp::RIB_POSITIVE_INPUT].channels = 7;  // make the ribs poly

        processBlock(*comp);
        prime(*comp, 0);
        clockItHigh(*comp, 0);
        assert(comp->_clockShifter[0].freqValid());
        assert(comp->_numRibsGenerators > 1);

        comp->params[Comp::RIB_NEGATIVE_BUTTON_PARAM].value = 10;  // press the rib button
        processBlock(*comp);
        for (int i = 0; i < 16; ++i) {
            const bool busy = comp->_ribGenerator[i].busyEither();
            const bool expectedBusy = (i < 7);
            assertEQ(busy, expectedBusy);
        }
    }

    static void testPolyRibMonoClock(int numRibChannels, int channelToTest) {
        // SQINFO("----- testPolyRibMonoClock %d,%d", numRibChannels, channelToTest);
        assert(numRibChannels > 0);
        assert(channelToTest >= 0);
        assert(channelToTest < numRibChannels);

        CompPtr comp = factory();
        assertEQ(comp->inputs[Comp::CK_INPUT].channels, 1);
        comp->outputs[Comp::CK_OUTPUT].channels = 1;                       // connect the output
        comp->inputs[Comp::RIB_POSITIVE_INPUT].channels = numRibChannels;  // connect the poly rib
        processBlock(*comp);
        assertEQ(comp->_numInputClocks, 1);

        const auto args = TestComposite::ProcessArgs();
        prime(*comp, 0);
        // Even though we are "primed" we will not emit a clock for one more period,
        // if shift is zero.
        clockItHigh(*comp, 0);
        comp->process(args);  // why is this here?
        assert(comp->_clockShifter[channelToTest].freqValid());

        comp->inputs[Comp::RIB_POSITIVE_INPUT].setVoltage(0, channelToTest);
        processBlock(*comp);
        // trigger the rib
        comp->inputs[Comp::RIB_POSITIVE_INPUT].setVoltage(10, channelToTest);
        processBlock(*comp);

        for (int i = 0; i < 16; ++i) {
            const bool busy = comp->_ribGenerator[i].busyEither();
            const bool expectedBusy = (i == channelToTest);
            assertEQ(busy, expectedBusy);
        }
    }
};

static void
testMonoClockPolyTriggerRibs() {
    TestX::testMonoClockPolyTriggerRibsSub(1, 0);
    TestX::testMonoClockPolyTriggerRibsSub(2, 0);
    TestX::testMonoClockPolyTriggerRibsSub(2, 1);
}

// Tests poly shift CV
static void
testPolyphonicShift() {
    TestX::testPolyphonicShiftSub(1, 0);
    TestX::testPolyphonicShiftSub(2, 0);
    TestX::testPolyphonicShiftSub(3, 0);
    TestX::testPolyphonicShiftSub(16, 0);
    TestX::testPolyphonicShiftSub(16, 1);
    TestX::testPolyphonicShiftSub(16, 7);
    TestX::testPolyphonicShiftSub(16, 15);
}

static void testShiftKnobPolyClock() {
    TestX::testShiftKnobPolyClock(1);
    TestX::testShiftKnobPolyClock(2);
    TestX::testShiftKnobPolyClock(16);
}

static void testPolyClockMonoShiftCV() {
    TestX::testPolyClockMonoShiftCV(1);
    TestX::testPolyClockMonoShiftCV(2);
    TestX::testPolyClockMonoShiftCV(11);
    TestX::testPolyClockMonoShiftCV(15);
}

static void testPolyRibMonoClock() {
    TestX::testPolyRibMonoClock(1, 0);
    TestX::testPolyRibMonoClock(2, 0);
    TestX::testPolyRibMonoClock(2, 1);
    TestX::testPolyRibMonoClock(16, 0);
    TestX::testPolyRibMonoClock(16, 1);
    TestX::testPolyRibMonoClock(16, 5);
    TestX::testPolyRibMonoClock(16, 15);
}

void testPhasePatternsPoly() {
    testChannels();
    testCanClock();
    testCanClockMono();
    testMonoClockPolyTriggerRibs();
    testShiftKnobPolyClock();
    testPolyphonicShift();
    testPolyClockMonoShiftCV();
    testPolyRibMonoClock();
    TestX::testPolyRibButton();
    testPolyShiftCV();
}

#if 0
void testFirst() {
    // TestX::testPolyRibButton();
    // testCanClockMonoSub(2);
   // testPolyShiftCV(4);
    // TestX::testPolyphonicShiftSub(1, 0);
    TestX::testPolyClockMonoShiftCV(1);
}
#endif