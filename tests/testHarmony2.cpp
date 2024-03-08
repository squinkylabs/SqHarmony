#include "asserts.h"

#include "Harmony2.h"
#include "TestComposite.h"

using Comp = Harmony2<TestComposite>;
using CompPtr = std::shared_ptr<Comp>;


static void processBlock(CompPtr comp) {
    const auto args = TestComposite::ProcessArgs();
    // process enough times to do the ribs thing.
    for (int i = 0; i < Comp::getSubSampleFactor(); ++i) {
        comp->process(args);
    }
}

static void testCanCall() {
    CompPtr comp = std::make_shared<Comp>();
    Comp::getTransposeDegreeLabels();
    Comp::getTransposeOctaveLabels();
    Comp h;
}

static void testLabels(std::vector<std::string>& x) {  
    assertGT(x.size(), 4);
    int count = 0;
    for (auto s : x) {
        assertGT(s.size(), 0);
        ++count;   
    }
    assertEQ(count, x.size());
}

static void testLabels() {
    auto y = Comp::getTransposeOctaveLabels();
    testLabels(y);
    y = Comp::getTransposeDegreeLabels();
    testLabels(y);
}

static void testPolyOutput() {
    auto comp = std::make_shared<Comp>();

    comp->inputs[Comp::PITCH_INPUT].channels = 1;   // connect the input
    comp->outputs[Comp::PITCH_OUTPUT].channels = 1; // and output

    // enable channels 1,4,5
    comp->params[Comp::XPOSE_ENABLE1_PARAM].value = 10;
    comp->params[Comp::XPOSE_ENABLE4_PARAM].value = 10;
    comp->params[Comp::XPOSE_ENABLE5_PARAM].value = 10;

//   static int getSubSampleFactor() { return 32; }
    for (int i=0; i < Comp::getSubSampleFactor(); ++i) {
         const auto args = TestComposite::ProcessArgs();
        comp->process(args);
    }
    assertEQ(int(comp->outputs[Comp::PITCH_OUTPUT].channels), 3);
}

static void testKeyCV(bool cvConnected) {
     auto comp = std::make_shared<Comp>();
     comp->inputs[Comp::KEY_INPUT].setVoltage(3.f / 12.f);    // 'E'
     comp->inputs[Comp::KEY_INPUT].channels = cvConnected ? 1 : 0;
     processBlock(comp);
    
    // Expect that changing the CV will change the key param
     const float expectedKey = cvConnected ? 3.f / 12.f : 0;
    assertEQ( comp->params[Comp::KEY_PARAM].value, expectedKey);
}

static void testKeyCV() {
    testKeyCV(true);
    testKeyCV(false);
}

void testHarmony2() {
    testCanCall();
    testLabels();
    testPolyOutput();

    SQINFO("add back testKeyCV() and make it work");
    // testKeyCV();
}

#if 0
void testFirst() {
    testKeyCV(true);
}
#endif