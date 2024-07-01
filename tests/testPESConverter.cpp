
#include "TestComposite.h"
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

    assertEQ(outputToTest.getVoltage(3), 0);  // D#
    assertEQ(outputToTest.getVoltage(4), 8);  // E
    assertEQ(outputToTest.getVoltage(5), 8);  // F
    assertEQ(outputToTest.getVoltage(6), 0);  // F#
    assertEQ(outputToTest.getVoltage(7), 8);  // G
    assertEQ(outputToTest.getVoltage(8), 0);  // G#
    assertEQ(outputToTest.getVoltage(9), 8);  // A
    assertEQ(outputToTest.getVoltage(10), 0);  // A#
    assertEQ(outputToTest.getVoltage(11), 8);  // B

    //   const auto note = MidiNote(MidiNote::C + int(std::round(TBase::params[KEY_PARAM].value)));
    // const auto mode = Scale::Scales(int(std::round(TBase::params[MODE_PARAM].value)));
}

static void inputCMajor() {
    TestComposite comp;
    auto inputToTest = comp.inputs[4];
    inputToTest.channels = 12;  // connect it.

    inputToTest.setVoltage(10, 0);  // C
    inputToTest.setVoltage(0, 1);  // C#
    inputToTest.setVoltage(8, 2);  // D
    inputToTest.setVoltage(0, 3);  // D#
    inputToTest.setVoltage(8, 4);  // E
    inputToTest.setVoltage(8, 5);  // F
    inputToTest.setVoltage(0, 6);  // F#
    inputToTest.setVoltage(8, 7);  // G
    inputToTest.setVoltage(0, 8);  // G#
    inputToTest.setVoltage(8, 9);  // A
    inputToTest.setVoltage(0, 10);  // A#
    inputToTest.setVoltage(8, 11);  // B

    const auto result = PESConverter::convertToPES(inputToTest, false);

    assertEQ(result.valid, true);
    assertEQ(result.keyRoot, MidiNote::C);
    assert(result.mode == Scale::Scales::Major);

}

static void nonDiatonic(bool acceptNonDiatonic) {
    TestComposite comp;
    auto inputToTest = comp.inputs[4];
    inputToTest.channels = 12;  // connect it.

    inputToTest.setVoltage(10, 0);  // C
    inputToTest.setVoltage(8, 1);  // C#
    inputToTest.setVoltage(8, 2);  // D
    inputToTest.setVoltage(8, 3);  // D#
    inputToTest.setVoltage(8, 4);  // E
    inputToTest.setVoltage(8, 5);  // F
    inputToTest.setVoltage(8, 6);  // F#
    inputToTest.setVoltage(8, 7);  // G
    inputToTest.setVoltage(8, 8);  // G#
    inputToTest.setVoltage(8, 9);  // A
    inputToTest.setVoltage(8, 10);  // A#
    inputToTest.setVoltage(8, 11);  // B

    const auto result = PESConverter::convertToPES(inputToTest, acceptNonDiatonic);

    assertEQ(result.valid, acceptNonDiatonic);
    if (acceptNonDiatonic) {
        assertEQ(result.keyRoot, MidiNote::C);
        assert(result.mode == Scale::Scales::Chromatic);
    }
}

static void nonDiatonic() {
    nonDiatonic(true);
    nonDiatonic(false);
}

void testPESConverter() {
    outputCMajor();
    inputCMajor();
}

#if 1
void testFirst() {
    //outputCMajor();
   //  inputCMajor();
     nonDiatonic();
}
#endif