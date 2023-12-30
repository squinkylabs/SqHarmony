
#include "TestComposite.h"  // need a Port definition.
#include "PolyClockShifter.h"
#include "asserts.h"

class PolyShiftComposite : public TestComposite {
public:
    PolyShiftComposite(int numClockChannels, int ribsChannels, int shiftChannels) {
       // assertEQ(numClockChannels, 1);
        _clockOutput->channels = 1;  // fake connected.
        _clockInput->channels = 1;  // connect
        _clockInput->channels = numClockChannels;
        assertEQ(ribsChannels, 1);
        assertEQ(shiftChannels, 1);
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
    // testChannelsSub(0, 0, 0, 1);
    //  testChannelsSub(1, 1, 1, 1);

    testChannelsSub(2, 1, 1, 2);
}

void testPolyClockShifter() {
    test0b();
    testChannels();
}