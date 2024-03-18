
#include "Harmony2.h"
#include "TestComposite.h"
#include "asserts.h"
#include "testUtils.h"

using Comp = Harmony2<TestComposite>;
using CompPtr = std::shared_ptr<Comp>;

static void test2(bool shouldPass, Comp& composite, const std::vector<int>& expectedScale) {
    SQINFO("-- test2 should pass = %d", shouldPass);
    bool wouldFail = false;
    assert(!expectedScale.empty());

    processBlock(composite);

    const auto args = TestComposite::ProcessArgs();
    for (auto x : expectedScale) {
        const float cv = float(x) / 12.f;
        composite.inputs[Comp::PITCH_INPUT].setVoltage(cv, 0);
        composite.process(args);
        SQINFO("exp = %f actual=%f", cv, composite.outputs[Comp::PITCH_OUTPUT].getVoltage(0));
        if (shouldPass) {
            assertEQ(composite.outputs[Comp::PITCH_OUTPUT].getVoltage(0), cv);
        }
        // If any fail, they all fail
        const bool pass = composite.outputs[Comp::PITCH_OUTPUT].getVoltage(0) == cv;
        wouldFail = wouldFail || !pass;
    }
    assertEQ(wouldFail, !shouldPass);
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

static void testKeyCVEMinorXp3(int modeWrap) {
    Comp composite;
    // All scales allowed
    composite.params[Comp::ONLY_USE_DIATONIC_PARAM].value = 0;
    hookUpCVInputs(composite);

    // Key of CE
    composite.inputs[Comp::KEY_INPUT].setVoltage(4.f / 12.f);
    //   Minor is +6
    const int modeWrapAmount = modeWrap * Scale::numScales();
    const float modeCV = float(Scale::Scales::Minor) / float(Scale::numScales());
    composite.inputs[Comp::MODE_INPUT].setVoltage(float(Scale::Scales::Minor) / float(Scale::numScales() + 1));  // there are 13 scales);
    SQINFO("for Minor, want mode %d, using CV %f den=%d",
           int(Scale::Scales::Minor),
           composite.inputs[Comp::MODE_INPUT].getVoltage(),
           Scale::numScales() + 1);
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
    testKeyCVEMinorXp3(0);
    testKeyCVEMinorXp3(1);
    testKeyCVEMinorXp3(-1);
 }

static void testCCommon(bool atC) {
    SQINFO("---- testCCommon %d", atC);
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
}

#if 1
void testFirst() {
    SQINFO("Test First");
    testKeyCVEMinorXp3();
}
#endif