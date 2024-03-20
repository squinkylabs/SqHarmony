
#include "Harmony2.h"
#include "TestComposite.h"
#include "asserts.h"
#include "testUtils.h"

using Comp = Harmony2<TestComposite>;
using CompPtr = std::shared_ptr<Comp>;

static void test4(bool shouldPass,
                  Comp& composite,
                  const std::vector<int>& input,
                  const std::vector<int>& expectedOutput,
                  const std::vector<int>& expectedOutput2,
                  int secondChannel) {
    // SQINFO("-- test2 should pass = %d", shouldPass);
    assert(secondChannel < 0);  //nimp
    assert(expectedOutput2.empty());    // nimp
    bool wouldFail = false;
    assert(!input.empty());
    assertEQ(input.size(), expectedOutput.size());

    processBlock(composite);

    const auto args = TestComposite::ProcessArgs();
    for (int i = 0; i < input.size(); ++i) {
        //  for (auto x : input) {
        const int x = input[i];
        const float cv = float(x) / 12.f;
        //  SQINFO("transpose cv = %f")
        composite.inputs[Comp::PITCH_INPUT].setVoltage(cv, 0);
        //  SQINFO("running process for the test");
        composite.process(args);
        //   SQINFO("exp = %f actual=%f", cv, composite.outputs[Comp::PITCH_OUTPUT].getVoltage(0));
        const auto vout = composite.outputs[Comp::PITCH_OUTPUT].getVoltage(0);
        const float expected = float(expectedOutput[i] / 12.f);
        if (shouldPass) {
            assertEQ(vout, expected);
        }
        // If any fail, they all fail
        const bool pass = vout == expected;
        wouldFail = wouldFail || !pass;
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
}

static void enableOneTransposer(Comp& composite) {
    composite.params[Comp::XPOSE_ENABLE1_PARAM].value = 10;  // enable the first transposer.
    composite.params[Comp::XPOSE_OCTAVE1_PARAM].value = 2;   // no octave offset
}

static void testKeyOfCUp6Steps() {
    Comp composite;
    hookUpCVInputs(composite);
    enableOneTransposer(composite);

    SQINFO("testKeyOfCUp6Steps");
    // up six semitones
    const float transposeCV = 6.f / 12.f;
    SQINFO("test transpose CV = %f", transposeCV);
    composite.inputs[Comp::XPOSE_INPUT].setVoltage(transposeCV);
    test3(true, composite,
          {MidiNote::C, MidiNote::D, MidiNote::E, MidiNote::F, MidiNote::G, MidiNote::A, MidiNote::B},
          {MidiNote::B, MidiNote::C + 12, MidiNote::D + 12, MidiNote::E + 12, MidiNote::F + 12, MidiNote::G + 12, MidiNote::A + 12});
}

static void testKeyOfCUpPoly() {
    assert(false);
}
static void testKeyCVEMinorXp3(int modeWrap, bool limitToDiatonic) {
    Comp composite;
    // All scales allowed
    composite.params[Comp::ONLY_USE_DIATONIC_PARAM].value = limitToDiatonic ? 1 : 0;
    hookUpCVInputs(composite);

    const float semitone = 1.f / 12.f;
    composite.inputs[Comp::KEY_INPUT].setVoltage(4.f * semitone);

    const int numScales = Scale::numScales(limitToDiatonic);
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
    composite.params[Comp::ONLY_USE_DIATONIC_PARAM].value = 0;

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
    // testKeyCVEMinorXp3();
    //  testKeyCVEMinorXp3(0, true);
    testKeyOfCUp6Steps();
    // testKeyOfCUpPoly();
}
#endif