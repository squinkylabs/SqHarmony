
#include "Harmony2.h"
#include "TestComposite.h"
#include "asserts.h"
#include "testUtils.h"

using Comp = Harmony2<TestComposite>;
using CompPtr = std::shared_ptr<Comp>;

static void test2(bool shouldPass, Comp& composite, const std::vector<int>& expectedScale) {
    //SQINFO("-- test2 should pass = %d", shouldPass);
    bool wouldFail = false;
    assert(!expectedScale.empty());

    processBlock(composite);

    const auto args = TestComposite::ProcessArgs();
    for (auto x : expectedScale) {
        const float cv = float(x) / 12.f;
        composite.inputs[Comp::PITCH_INPUT].setVoltage(cv, 0);
      //  SQINFO("running process for the test");
        composite.process(args);
     //   SQINFO("exp = %f actual=%f", cv, composite.outputs[Comp::PITCH_OUTPUT].getVoltage(0));
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

static void testKeyCVEMinorXp3(int modeWrap, bool limitToDiatonic) {
    //SQINFO("testKeyCVEMinorXp3(%d, %d)", modeWrap, limitToDiatonic);
    Comp composite;
    // All scales allowed
    composite.params[Comp::ONLY_USE_DIATONIC_PARAM].value = limitToDiatonic? 1 : 0;
    hookUpCVInputs(composite);

    const float semitone = 1.f / 12.f;
    composite.inputs[Comp::KEY_INPUT].setVoltage(4.f * semitone);

    const int numScales = Scale::numScales(limitToDiatonic);
    const float modeCV = float(Scale::Scales::Minor) * semitone + (semitone * numScales * modeWrap);
  //  modeCV += modeWrap * 
    composite.inputs[Comp::MODE_INPUT].setVoltage(modeCV);  // there are 13 scales);
    // SQINFO("for Minor, want mode %d, using CV %f den=%d",
    //        int(Scale::Scales::Minor),
    //        composite.inputs[Comp::MODE_INPUT].getVoltage(),
    //        12);
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
}

#if 0
void testFirst() {
    SQINFO("Test First");
   testKeyCVEMinorXp3();
  //  testKeyCVEMinorXp3(0, true);
}
#endif