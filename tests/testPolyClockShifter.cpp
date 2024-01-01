
#include "TestComposite.h"  // need a Port definition.
#include "PolyClockShifter.h"
#include "asserts.h"

class PolyShiftComposite : public TestComposite {
public:
    PolyShiftComposite(int numClockChannels, int ribsChannels, int shiftChannels) {
        _clockOutput->channels = 1;  // fake connected.
        _clockInput->channels = 1;  // connect
        _clockInput->channels = numClockChannels;
        _ribsTrigger->channels =  ribsChannels;
        _shiftAmount->channels = shiftChannels;
    }

    PolyClockShifter psh;
    Port* const _clockOutput = &outputs[0];
    Port* const _clockInput = &inputs[0];
    Port* const _ribsTrigger = &inputs[1];
    Port* const _shiftAmount = &inputs[2];
    void runEverySample() {
        PolyClockShifter::PortInfo info;
        _fillInfo(info);
        psh.runEverySample(info);
    }
    void runEveryBlock() {
        PolyClockShifter::PortInfo info;
        _fillInfo(info);
        psh.runEveryBlock(info);
    }

    void _fillInfo(PolyClockShifter::PortInfo& info) {
        Port* const _clockOutput = &outputs[0];
        Port* const _clockInput = &inputs[0];
        Port* const _ribsTrigger = &inputs[1];
        Port* const _shiftAmount = &inputs[2];

        info.clockInput = _clockInput;
        info.clockOutput = _clockOutput;
        info.ribsTrigger = _ribsTrigger;
        info.shiftAmount = _shiftAmount;
    }
};

static void test0b() {
    PolyShiftComposite comp(1, 1, 1);
    comp.runEveryBlock();
    comp.runEverySample();
}

static void testChannelsSub(
    int clockInChannels,
    int shiftAmountChannels,
    int ribsChannels,
    int expectedClockChannels) {
    PolyShiftComposite comp(clockInChannels, ribsChannels, shiftAmountChannels);

    // init, should be connected but not set
    assertEQ(int(comp._clockOutput->channels), 1);
    comp.runEverySample();  // this also should not set up any channels.
    assertEQ(int(comp._clockOutput->channels), 1);
    comp.runEveryBlock();
    assertEQ(int(comp._clockOutput->channels), expectedClockChannels);
}

static void testChannels() {
    testChannelsSub(0, 0, 0, 1);
    testChannelsSub(1, 1, 1, 1);
    testChannelsSub(2, 1, 1, 2);
    testChannelsSub(1, 2, 1, 2);
    testChannelsSub(1, 1, 2, 2);
    testChannelsSub(16, 16, 16, 16);
}

static int testPeriod = 10;

static void clockItLow(PolyShiftComposite& c, int numTimes, int channel, float expectedOutput) {
    c._clockInput->setVoltage(0, channel);
    for (int i = 0; i < numTimes; ++i) {
        c.runEverySample();
        if (expectedOutput >= 0) {
             assertEQ(c._clockOutput->getVoltage(channel), expectedOutput);
        }
    }
}

static void clockItHigh(PolyShiftComposite& c, int channel) {
    c._clockInput->setVoltage(10, channel);
    c.runEverySample();
}

static void clockItHighLow(PolyShiftComposite& c, int channel, int numLow) {
    clockItHigh(c, channel);
    clockItLow(c, numLow, channel, -1);
}

// does high + low, but not final high.
static void prime(PolyShiftComposite& comp, int channel, int period = testPeriod) {
    comp._clockOutput->channels = 1;            // connect the clock output
    comp.runEveryBlock();
    clockItHighLow(comp, channel, period-1);
}

static void testCanBlockSub(int clockInChannels, int channelToTest) {
    SQINFO("------ testCanBlockSub ");
    PolyShiftComposite comp(clockInChannels, 1, 1);
    prime(comp, channelToTest);
    // Even though we are "primed" we will not emit a clock for one more period,
    // if shift is zero.
    clockItHighLow(comp, 1, testPeriod-1);

  
    comp._clockInput->setVoltage(10, channelToTest);

    comp.runEverySample();
    for (int i = 0; i < 16; ++i) {
        const auto  v = comp._clockOutput->getVoltage(i);
        const auto expect = (i == channelToTest) ? 10.f : 0.f;
        assertEQ(v, expect);
    }
}

static void testCanClock() {
    testCanBlockSub(1, 0);
}

void testPolyClockShifter() {
    test0b();
    testChannels();
    testCanClock();
}