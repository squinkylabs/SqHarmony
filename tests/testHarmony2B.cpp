
#include "asserts.h"

#include "Harmony2.h"
#include "TestComposite.h"
#include "testUtils.h"

using Comp = Harmony2<TestComposite>;
using CompPtr = std::shared_ptr<Comp>;

static void test2(bool shouldPass, Comp& composite,  const std::vector<int>& expectedScale) {
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
    composite.params[Comp::XPOSE_OCTAVE1_PARAM].value = 2;     // no octave offset
}
/*
static void testKeyCVCMajor() {
    TestParams tp;
    tp.keyCVIn = 0;
    tp.modeCVIn = 0;
    tp.onlyDiatonic = false;
    tp.expectedScale = {0, 2, 4, 5, 7, 9, 11};
    testKeyCV2(tp);
}
*/
static void testCCommon(bool atC) {
   // auto composite = std::make_shared<Comp>();
    Comp composite;
    // All scales allowed
    composite.params[Comp::ONLY_USE_DIATONIC_PARAM].value = 0;

    hookUpCVInputs(composite);

    // Key of C
    composite.inputs[Comp::KEY_INPUT].setVoltage(0);
    // Mode major
    composite.inputs[Comp::MODE_INPUT].setVoltage(0);

    // Set v/oct input
    composite.inputs[Comp::PITCH_INPUT].setVoltage(atC ? 0 : 1.f / 12.f);

    enableOneTransposer(composite);
    test2(atC, composite, { 0, 2, 4, 5, 7, 9, 11 });
}

static void testKeyCVCMajor() {
     testCCommon(false);
}

static void testKeyCVNotCMajor() {
    testCCommon(false);
}


void testHarmony2B() {
    testKeyCVCMajor();
    testKeyCVNotCMajor();
    assert(false);
}

#if 1
void testFirst() {
    testKeyCVCMajor();
    testKeyCVNotCMajor();
   
}
#endif