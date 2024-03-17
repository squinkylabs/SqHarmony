
#include "asserts.h"

#include "Harmony2.h"
#include "TestComposite.h"

using Comp = Harmony2<TestComposite>;
using CompPtr = std::shared_ptr<Comp>;

template <typename T>
void processBlock(T* composite) {
       const auto args = TestComposite::ProcessArgs();
    // process enough times to do the ribs thing.
    for (int i = 0; i < T::getSubSampleFactor(); ++i) {
        composite->process(args);
    }
}

static void test2(bool shouldPass, Comp* composite,  const std::vector<int>& expectedScale) {
    bool wouldFail = false;
    assert(!expectedScale.empty());
   
    processBlock(composite);

    const auto args = TestComposite::ProcessArgs();
    for (auto x : expectedScale) {
        const float cv = float(x) / 12.f;
        composite->inputs[Comp::PITCH_INPUT].setVoltage(cv, 0);
        composite->process(args);
        SQINFO("exp = %f actual=%f", cv, composite->outputs[Comp::PITCH_OUTPUT].getVoltage(0));
        if (shouldPass) {
            assertEQ(composite->outputs[Comp::PITCH_OUTPUT].getVoltage(0), cv);
        }
        // If any fail, they all fail
        const bool pass = composite->outputs[Comp::PITCH_OUTPUT].getVoltage(0) == cv;
        wouldFail = wouldFail || !pass;
    }
    assertEQ(wouldFail, !shouldPass);
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
static void testKeyCV2() {
    auto composite = std::make_shared<Comp>();
    // All scales allowed
    composite->params[Comp::ONLY_USE_DIATONIC_PARAM].value = 0;

    // The CV inputs we all care about are patched for mono input
    composite->inputs[Comp::PITCH_INPUT].channels = 1;
    composite->inputs[Comp::KEY_INPUT].channels = 1;
    composite->inputs[Comp::MODE_INPUT].channels = 1;

    // Key of C
    assertEQ(composite->inputs[Comp::KEY_INPUT].getChannels(), 1);
    composite->inputs[Comp::KEY_INPUT].setVoltage(0);
    SQINFO("in test root CV (KEY_INPUT) = %f", composite->inputs[Comp::KEY_INPUT].getVoltage(0));

    // Mode major
    composite->inputs[Comp::MODE_INPUT].setVoltage(0);
    SQINFO("in test, mode input (MODE_INPUT) = %f", composite->inputs[Comp::MODE_INPUT].getVoltage(0));

    composite->params[Comp::XPOSE_ENABLE1_PARAM].value = 10;  // enable the first transposer.
    composite->params[Comp::XPOSE_OCTAVE1_PARAM].value = 2;     // no octavbe offset
    test2(true, composite.get(), { 0, 2, 4, 5, 7, 9, 11 });
}


void testHarmony2B() {
    testKeyCV2();

    assert(false);
}

#if 1
void testFirst() {
    testKeyCV2();
   
}
#endif