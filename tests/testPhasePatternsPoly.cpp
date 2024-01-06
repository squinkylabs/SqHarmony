
// #include "TestComposite.h"  // need a Port definition.

// #include "PolyClockShifter.h"

#include "PhasePatterns.h"
#include "TestComposite.h"
#include "asserts.h"

using Comp = PhasePatterns<TestComposite>;

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
    // TODO: what is this input on channel 1??
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

static void testCanClockMonoWithRibSub(int ribChannels) {
    SQINFO("---- testCanClockMonoWithRibSub %d", ribChannels);
    Comp comp;                                   // only poly input is shift amount
    comp.outputs[Comp::CK_OUTPUT].channels = 1;  // connect the output
    comp.inputs[Comp::SHIFT_INPUT].channels = 1;
    comp.inputs[Comp::CK_INPUT].channels = 1;
    comp.inputs[Comp::RIB_INPUT].channels = ribChannels;

    const auto args = TestComposite::ProcessArgs();
    prime(comp, 0);
    // Even though we are "primed" we will not emit a clock for one more period,
    // if shift is zero.
    clockItHighLow(comp, 0, testPeriod - 1);

    // Set the mono clock high, and run.

    comp.inputs[Comp::CK_INPUT].setVoltage(10, 0);

    // comp.process(args);

    // process enough times to do the ribs thing.
    for (int i = 0; i < Comp::getSubSampleFactor(); ++i) {
        comp.process(args);
    }
    const int actualRibChannels = std::max(ribChannels, 1);  // there is always one channel...
    for (int i = 0; i < 16; ++i) {
        const auto v = comp.outputs[Comp::CK_OUTPUT].getVoltage(i);

        // this less than is problematic... It's not right...
        const auto expect = (i < actualRibChannels) ? 10.f : 0.f;
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

static void testCanClockMonoWithRib() {
    testCanClockMonoWithRibSub(1);
}

class TestX {
public:
    static void testTriggerRibsSub(int numRibs, int ribToTest) {
        SQINFO("------- testTriggerRibsSub %d %d", numRibs, ribToTest);
        assert(ribToTest < numRibs);
        Comp comp;                                   // only poly input is shift amount
        comp.outputs[Comp::CK_OUTPUT].channels = 1;  // connect the output
        comp.inputs[Comp::SHIFT_INPUT].channels = 1;
        comp.inputs[Comp::CK_INPUT].channels = 1;
        comp.inputs[Comp::RIB_INPUT].channels = numRibs;

        const auto args = TestComposite::ProcessArgs();
        prime(comp, 0);
        // Even though we are "primed" we will not emit a clock for one more period,
        // if shift is zero.
        clockItHighLow(comp, 0, testPeriod - 1);

        // trigger the rib
        SQINFO("setting rib input %d to 10", ribToTest);
        comp.inputs[Comp::RIB_INPUT].setVoltage(10, ribToTest);
        // process enough times to do the ribs thing.
        for (int i = 0; i < Comp::getSubSampleFactor(); ++i) {
            comp.process(args);
        }

        for (int i = 0; i < 16; ++i) {
            const bool expectedRibState = (i == ribToTest);
            assertEQ(comp._shiftCalculator[i].busy(), expectedRibState);
        }
    }

    static void testShiftSub(int inputShiftChannels, int channelToTest) {
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
            const float shift = comp._clockShifter[i]._shift;
            const float expectedShift = (i == channelToTest) ? shiftAmount : 0;
            SQINFO("channel %d shift %f", i, shift);
            assertEQ(shift, expectedShift);
        }
    }

    static void testShiftKnob(int numClockChannels) {
        assert(numClockChannels > 0);

        Comp comp;
        comp.outputs[Comp::CK_OUTPUT].channels = 1;  // connect the output
        comp.inputs[Comp::CK_INPUT].channels = numClockChannels;
       //comp.inputs[Comp::SHIFT_INPUT].setVoltage(1, channelToTest);
        comp.params[Comp::SHIFT_PARAM].value = .5;

         processBlock(comp);

         for (int i = 0; i < 16; ++i) {
            const float shift = comp._clockShifter[i]._shift;
            const float expectedShift = (i < numClockChannels) ? .5 : 0;     // knob should affect all channels
            assertEQ(shift, expectedShift);
           //SQINFO("channel %d shift %f", i, shift);
        }
    }

  
};

static void
testTriggersRib() {
    TestX::testTriggerRibsSub(1, 0);
    TestX::testTriggerRibsSub(2, 0);
    TestX::testTriggerRibsSub(2, 1);
}

static void
testShiftPropagation() {
    TestX::testShiftSub(1, 0);
    TestX::testShiftSub(2, 0);
    TestX::testShiftSub(3, 0);
    TestX::testShiftSub(16, 0);
    TestX::testShiftSub(16, 1);
    TestX::testShiftSub(16, 7);
    TestX::testShiftSub(16, 15);
}

static void testShiftKnob() {
    TestX::testShiftKnob(1);
    TestX::testShiftKnob(2);
    TestX::testShiftKnob(16);
}

void testPhasePatternsPoly() {
    testChannels();
    testCanClock();
    testCanClockMono();
    // testTriggersRib();
    //  testCanClockMonoWithRib();
    SQINFO("put back RIB tests and make them work");
    testShiftKnob();
    testShiftPropagation();
    
}