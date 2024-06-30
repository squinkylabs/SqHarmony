
#include "PESConverter.h"
#include "asserts.h"

static void outputCMajor() {
    TestComposite comp;
    auto outputToTest = comp.outputs[2];
    outputToTest.channels = 1;  // connect it.

    PESConverter::PESOutput params;
    params.keyRoot = 0;  // C
    params.mode = Scale::Scales::Major;
    PESConverter::outputPES(outputToTest, params);
    assertEQ(outputToTest.getChannels(), 12);

    assertEQ(outputToTest.getVoltage(0), 10);  // C
    assertEQ(outputToTest.getVoltage(1), 0);   // C#
    assertEQ(outputToTest.getVoltage(2), 8);   // D

    assertEQ(outputToTest.getVoltage(0), 10);  // C
    assertEQ(outputToTest.getVoltage(0), 10);  // C
    assertEQ(outputToTest.getVoltage(0), 10);  // C
    assertEQ(outputToTest.getVoltage(0), 10);  // C
    assertEQ(outputToTest.getVoltage(0), 10);  // C
    assertEQ(outputToTest.getVoltage(0), 10);  // C
    assertEQ(outputToTest.getVoltage(0), 10);  // C
    assertEQ(outputToTest.getVoltage(0), 10);  // C
    assertEQ(outputToTest.getVoltage(0), 10);  // C

    //   const auto note = MidiNote(MidiNote::C + int(std::round(TBase::params[KEY_PARAM].value)));
    // const auto mode = Scale::Scales(int(std::round(TBase::params[MODE_PARAM].value)));
}

void testPESConverter() {
    outputCMajor();
}

#if 1
void testFirst() {
    outputCMajor();
}
#endif