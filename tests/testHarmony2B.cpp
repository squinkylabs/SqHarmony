
#include "Harmony2.h"
#include "TestComposite.h"
#include "asserts.h"
#include "testUtils.h"

using Comp = Harmony2<TestComposite>;
using CompPtr = std::shared_ptr<Comp>;

static bool testCV(Comp& composite, bool shouldPass, float cv, int channel, int expectedOutput) {

    const auto args = TestComposite::ProcessArgs();
    composite.inputs[Comp::PITCH_INPUT].setVoltage(cv, channel);
    //  SQINFO("running process for the test");
    composite.process(args);
    //   SQINFO("exp = %f actual=%f", cv, composite.outputs[Comp::PITCH_OUTPUT].getVoltage(0));
    const auto vout = composite.outputs[Comp::PITCH_OUTPUT].getVoltage(channel);
    const float expected = float(expectedOutput/ 12.f);
    if (shouldPass) {
        assertEQ(vout, expected);
    }
    // If any fail, they all fail
  //  const bool pass = vout == expected;
  //  wouldFail = wouldFail || !pass;
  return vout == expected;
}


static void test4(bool shouldPass,
                  Comp& composite,
                  const std::vector<int>& input,
                  const std::vector<int>& expectedOutput,
                  const std::vector<int>& expectedOutput2,
                  int secondChannel) {
    bool wouldFail = false;
    assert(!input.empty());
    assertEQ(input.size(), expectedOutput.size());
    const int expectedSize2 = secondChannel < 0 ? 0 : input.size();
    assertEQ(expectedOutput2.size(), expectedSize2);

    processBlock(composite);

    for (int i = 0; i < input.size(); ++i) {
        //  for (auto x : input) {
        const int x = input[i];
        const float cv = float(x) / 12.f;
        bool pass = testCV(composite, shouldPass, cv, 0, expectedOutput[i]);
        wouldFail = wouldFail || !pass;
        if (secondChannel >= 0) {
            pass = testCV(composite, shouldPass, cv, secondChannel, expectedOutput2[i]);
            wouldFail = wouldFail || !pass;
        }

     //   channel = secondChannel;
    }
    assertEQ(wouldFail, !shouldPass);
}

static void test3(bool shouldPass, Comp& composite, const std::vector<int>& input, const std::vector<int>& expectedOutput) {
    test4(shouldPass, composite, input, expectedOutput, {}, -1);
}

static void test2(bool shouldPass, Comp& composite, const std::vector<int>& expectedScale) {
    test3(shouldPass, composite, expectedScale, expectedScale);
}

static void hookUpCVInputs(Comp& composite) {
    // The CV inputs we all care about are patched for mono input
    composite.inputs[Comp::PITCH_INPUT].channels = 1;
    composite.inputs[Comp::KEY_INPUT].channels = 1;
    composite.inputs[Comp::MODE_INPUT].channels = 1;
    composite.inputs[Comp::XPOSE_INPUT].channels = 1;
}

static void enableOneTransposer(Comp& composite) {
    composite.params[Comp::XPOSE_ENABLE1_PARAM].value = 10;  // enable the first transposer.
    composite.params[Comp::XPOSE_OCTAVE1_PARAM].value = 2;   // no octave offset
}

static void testKeyOfCUp6Steps() {
    Comp composite;
    hookUpCVInputs(composite);
    enableOneTransposer(composite);

    // up six steps
    const float transposeCV = 6.f / 12.f;
    composite.inputs[Comp::XPOSE_INPUT].setVoltage(transposeCV);
    test3(true, composite,
          {MidiNote::C, MidiNote::D, MidiNote::E, MidiNote::F, MidiNote::G, MidiNote::A, MidiNote::B},
          {MidiNote::B, MidiNote::C + 12, MidiNote::D + 12, MidiNote::E + 12, MidiNote::F + 12, MidiNote::G + 12, MidiNote::A + 12});
}

static void testKeyOfCUpPoly(bool noErrors) {
    Comp composite;
    hookUpCVInputs(composite);
   // enableOneTransposer(composite);

    composite.params[Comp::XPOSE_ENABLE1_PARAM].value = 10;  // enable the first transposer.
    composite.params[Comp::XPOSE_OCTAVE1_PARAM].value = 2;   // no octave offset
    composite.params[Comp::XPOSE_ENABLE5_PARAM].value = 10;  // enable the fifth transposer.
    composite.params[Comp::XPOSE_OCTAVE5_PARAM].value = 2;   // no octave offset

    const float transposeCV1 = 2.f / 12.f;
    const float transposeCV2 = 4.f / 12.f;
  //  const int secondChannel = 11;
    const int numChannels = noErrors ? 2 : 1;
    composite.inputs[Comp::XPOSE_INPUT].setChannels(numChannels);
    const auto xx = composite.inputs[Comp::XPOSE_INPUT];
    assertEQ(int(composite.inputs[Comp::XPOSE_INPUT].channels), numChannels);
    composite.inputs[Comp::XPOSE_INPUT].setVoltage(transposeCV1, 0);
    composite.inputs[Comp::XPOSE_INPUT].setVoltage(transposeCV2, 1);
    //SQINFO("set xpose to %f, %f", transposeCV1, transposeCV2);
    test4(noErrors, composite,
          {MidiNote::C, MidiNote::D, MidiNote::E, MidiNote::F, MidiNote::G, MidiNote::A, MidiNote::B},
          {MidiNote::E, MidiNote::F, MidiNote::G, MidiNote::A, MidiNote::B, MidiNote::C + 12, MidiNote::D + 12},
          {MidiNote::G, MidiNote::A, MidiNote::B, MidiNote::C + 12, MidiNote::D + 12, MidiNote::E + 12, MidiNote::F + 12},
          1);
}

static void testKeyOfCUpPoly() {   
   // testKeyOfCUpPoly(true);
    testKeyOfCUpPoly(false);
}

static void testKeyCVEMinorXp3(int modeWrap, bool limitCVToDiatonic) {
    Comp composite;
    // All scales allowed
    composite.params[Comp::ONLY_USE_DIATONIC_FOR_CV_PARAM].value = limitCVToDiatonic ? 1 : 0;
    hookUpCVInputs(composite);

    const float semitone = 1.f / 12.f;
    composite.inputs[Comp::KEY_INPUT].setVoltage(4.f * semitone);

    const int numScales = Scale::numScales(limitCVToDiatonic);
    const float modeCV = float(Scale::Scales::Minor) * semitone + (semitone * numScales * modeWrap);

    composite.inputs[Comp::MODE_INPUT].setVoltage(modeCV);  // there are 13 scales);
    enableOneTransposer(composite);

    test2(true, composite,
          {MidiNote::E,
           MidiNote::F + 1,
           MidiNote::G,
           MidiNote::A,
           MidiNote::B,
           MidiNote::C,
           MidiNote::D});
}

static void testKeyCVEMinorXp3() {
    for (int i = -1; i <= 1; ++i) {
        // SQINFO("modeWrap = %d", i);

        testKeyCVEMinorXp3(i, false);
        testKeyCVEMinorXp3(i, true);
    }
}

static void testCCommon(bool atC) {
    //  SQINFO("---- testCCommon %d", atC);
    // auto composite = std::make_shared<Comp>();
    Comp composite;
    // All scales allowed
    composite.params[Comp::ONLY_USE_DIATONIC_FOR_CV_PARAM].value = 0;

    hookUpCVInputs(composite);

    // Key of C or c#, depending
    composite.inputs[Comp::KEY_INPUT].setVoltage(atC ? 0 : 1.f / 12.f);
    // Mode major
    composite.inputs[Comp::MODE_INPUT].setVoltage(0);
    enableOneTransposer(composite);
    test2(atC, composite, {0, 2, 4, 5, 7, 9, 11});
}

static void testKeyCVCMajor() {
    testCCommon(true);
}

static void testKeyCVNotCMajor() {
    testCCommon(false);
}

void testHarmony2B() {
    testKeyCVCMajor();
    testKeyCVNotCMajor();
    testKeyCVEMinorXp3();
    testKeyOfCUp6Steps();
    testKeyOfCUpPoly();
}

#if 0
void testFirst() {
    SQINFO("Test First");
    //  testHarmony2B();
   //  testKeyCVCMajor();
    // testKeyCVEMinorXp3();
    //  testKeyCVEMinorXp3(0, true);
   //  testKeyOfCUp6Steps();
    testKeyOfCUpPoly();
}
#endif