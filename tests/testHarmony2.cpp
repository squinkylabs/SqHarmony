#include "Harmony2.h"
#include "TestComposite.h"
#include "asserts.h"

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

    comp->inputs[Comp::PITCH_INPUT].channels = 1;    // connect the input
    comp->outputs[Comp::PITCH_OUTPUT].channels = 1;  // and output

    // enable channels 1,4,5
    comp->params[Comp::XPOSE_ENABLE1_PARAM].value = 10;
    comp->params[Comp::XPOSE_ENABLE4_PARAM].value = 10;
    comp->params[Comp::XPOSE_ENABLE5_PARAM].value = 10;

    //   static int getSubSampleFactor() { return 32; }
    for (int i = 0; i < Comp::getSubSampleFactor(); ++i) {
        const auto args = TestComposite::ProcessArgs();
        comp->process(args);
    }
    assertEQ(int(comp->outputs[Comp::PITCH_OUTPUT].channels), 3);
}

static void testKeyCV(bool cvConnected) {
    auto comp = std::make_shared<Comp>();
    comp->inputs[Comp::KEY_INPUT].setVoltage(3.f / 12.f);  // 'E'
    comp->inputs[Comp::KEY_INPUT].channels = cvConnected ? 1 : 0;
    processBlock(comp);

    // Expect that changing the CV will change the key param
    const float expectedKey = cvConnected ? 3.f / 12.f : 0;
    assertEQ(comp->params[Comp::KEY_PARAM].value, expectedKey);
}

static void testChord() {
    auto comp = std::make_shared<Comp>();

    comp->inputs[Comp::PITCH_INPUT].channels = 1;    // connect the input
    comp->outputs[Comp::PITCH_OUTPUT].channels = 1;  // and output

    // enable channels 1,4,5
    comp->params[Comp::XPOSE_ENABLE1_PARAM].value = 10;
    comp->params[Comp::XPOSE_ENABLE4_PARAM].value = 10;
    comp->params[Comp::XPOSE_ENABLE5_PARAM].value = 10;

   
   
    comp->params[Comp::XPOSE_DEGREE1_PARAM].value = 0;           // root
    comp->params[Comp::XPOSE_DEGREE4_PARAM].value = 2;  // third
    comp->params[Comp::XPOSE_DEGREE5_PARAM].value = 4;  // fifthe

    SQINFO("test set CV to zero, now test");
    comp->inputs[Comp::PITCH_INPUT].setVoltage(0, 0);  // send middle C

    // run a block
    for (int i = 0; i < Comp::getSubSampleFactor(); ++i) {
        const auto args = TestComposite::ProcessArgs();
        comp->process(args);
    }

    SQINFO(" output chanels = %d", comp->outputs[Comp::PITCH_OUTPUT].channels);
    SQINFO(" cv0=%f 4=%f 5=%f",
           comp->outputs[Comp::PITCH_OUTPUT].getVoltage(0),
           comp->outputs[Comp::PITCH_OUTPUT].getVoltage(3),
           comp->outputs[Comp::PITCH_OUTPUT].getVoltage(4));
    // assertEQ(int(comp->outputs[Comp::PITCH_OUTPUT].channels), 3);

 //const float degree = 1.f / 8.f;s
   const float semi = 1.f / 12.f;
   assertClose(comp->outputs[Comp::PITCH_OUTPUT].getVoltage(0), 0, .00001);
   assertClose(comp->outputs[Comp::PITCH_OUTPUT].getVoltage(1), 4 * semi, .00001);      // major third
    assertClose(comp->outputs[Comp::PITCH_OUTPUT].getVoltage(2), 7 * semi, .00001);     // firth
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
     testChord();
}

#if 0
void testFirst() {
    // testKeyCV(true);
    testChord();
}
#endif