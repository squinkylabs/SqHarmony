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

/**
 */
static void testKeyCV(bool cvConnected, float cv, int expectedParam) {
    auto comp = std::make_shared<Comp>();
    comp->inputs[Comp::KEY_INPUT].setVoltage(cv);
    comp->inputs[Comp::KEY_INPUT].channels = cvConnected ? 1 : 0;
    processBlock(comp);

    // Expect that changing the CV will change the key param
    const float expectedKey = cvConnected ? expectedParam : 0.f;
    assertEQ(comp->params[Comp::KEY_PARAM].value, expectedKey);
}

static void testKeyCV(bool connected) {
    testKeyCV(connected, 3.f / 12.f, 3.f);
}

static void testKeyCVWrap() {
    testKeyCV(true, 1, 0);               // 1V should wrap back down to C
    testKeyCV(true, 5 + 1.f / 12.f, 1);  // 5+1 should wrap down to +1
    testKeyCV(true, -8, 0);              // negative should wrap
    testKeyCV(true, -4 + 5.f / 12.f, 5);
}

/**
 */
static void testModeCV(float cv, int expectedParam) {
    auto comp = std::make_shared<Comp>();
    comp->inputs[Comp::MODE_INPUT].setVoltage(cv);
    comp->inputs[Comp::MODE_INPUT].channels = 1;
    processBlock(comp);

    assertEQ(comp->params[Comp::MODE_PARAM].value, expectedParam);
}

static void testModeCV() {
    const float degree = 1.f / 8.f;
    testModeCV(0, 0);
    testModeCV(degree, 1);
    testModeCV(degree * 7, 7);
    testModeCV(-1, 0);
    testModeCV(-5 + degree, 1);
}

static void testChord(
    const std::vector<float>& degreeParamValues,
    const std::vector<float>& octaveParamValues,
    const std::vector<float>& expectedCV) {
    assert(!degreeParamValues.empty());
    assertEQ(degreeParamValues.size(), octaveParamValues.size());
    assertEQ(expectedCV.size(), octaveParamValues.size());

    auto comp = std::make_shared<Comp>();

    comp->inputs[Comp::PITCH_INPUT].channels = 1;    // connect the input
    comp->outputs[Comp::PITCH_OUTPUT].channels = 1;  // and output

    for (int i = 0; i < degreeParamValues.size(); ++i) {
        // enable channels
        comp->params[Comp::XPOSE_ENABLE1_PARAM + i].value = 10;
        // transpose
        comp->params[Comp::XPOSE_DEGREE1_PARAM + i].value = degreeParamValues[i];
        // octaves
        comp->params[Comp::XPOSE_OCTAVE1_PARAM + i].value = octaveParamValues[i];  // root
    }
    // run a block
    for (int i = 0; i < Comp::getSubSampleFactor(); ++i) {
        const auto args = TestComposite::ProcessArgs();
        comp->process(args);
    }
    for (int i = 0; i < 16; ++i) {
        const float expectedOut = (i < degreeParamValues.size()) ? expectedCV[i] : 0;
        assertClose(comp->outputs[Comp::PITCH_OUTPUT].getVoltage(i), expectedOut, .00001);
    }
}

static void testChord2() {
    const float semi = 1.f / 12.f;
    testChord(
        {0, 2, 4},  // root, 3rd, 5th
        {2, 2, 2},  // no octave shift
        {0, 4 * semi, 7 * semi});
}

static void testChord3() {
    testChord(
        {0},      // root
        {2 + 1},  // one octave shift
        {1});
}

static void testKeyCV() {
    testKeyCV(true);
    testKeyCV(false);
}

class TestParams {
public:
    float keyCVIn = 0;
    bool onlyDiatonic = false;
    float modeCVIn = 0;
    std::vector<int> expectedScale;
    int transposeOctaves = 2;
    bool shouldPass = true;
};

static void testKeyCV2(const TestParams& tp) {
    bool wouldFail = false;
    assert(!tp.expectedScale.empty());
    auto composite = std::make_shared<Comp>();
    composite->params[Comp::ONLY_USE_DIATONIC_PARAM].value = tp.onlyDiatonic ? 1 : 0;

    composite->inputs[Comp::PITCH_INPUT].channels = 1;

    composite->inputs[Comp::KEY_INPUT].channels = 1;
    assertEQ(composite->inputs[Comp::KEY_INPUT].getChannels(), 1);
    composite->inputs[Comp::KEY_INPUT].setVoltage(tp.keyCVIn);

    composite->inputs[Comp::MODE_INPUT].channels = 1;
    composite->inputs[Comp::MODE_INPUT].setVoltage(tp.modeCVIn);

    composite->params[Comp::XPOSE_ENABLE1_PARAM].value = 10;  // enable the first transposer.
    composite->params[Comp::XPOSE_OCTAVE1_PARAM].value = tp.transposeOctaves;
    processBlock(composite);

    const auto args = TestComposite::ProcessArgs();
    for (auto x : tp.expectedScale) {
        const float cv = float(x) / 12.f;
        composite->inputs[Comp::PITCH_INPUT].setVoltage(cv, 0);
        composite->process(args);
        SQINFO("exp = %f actual=%f", cv, composite->outputs[Comp::PITCH_OUTPUT].getVoltage(0));
        if (tp.shouldPass) {
            assertEQ(composite->outputs[Comp::PITCH_OUTPUT].getVoltage(0), cv);
        }
        // If any fail, they all fail
        const bool pass = composite->outputs[Comp::PITCH_OUTPUT].getVoltage(0) == cv;
        wouldFail = wouldFail || !pass;
    }
    assertEQ(wouldFail, !tp.shouldPass);
}

static void testKeyCVCMajor() {
    TestParams tp;
    tp.keyCVIn = 0;
    tp.modeCVIn = 0;
    tp.onlyDiatonic = false;
    tp.expectedScale = {0, 2, 4, 5, 7, 9, 11};
    testKeyCV2(tp);
}

static void testKeyCVNotCMajor() {
    SQINFO("-------------- testKeyCVNotCMajor");
    TestParams tp;
    tp.keyCVIn = 2.f / 12.f;  // D major
    tp.modeCVIn = 0;
    tp.onlyDiatonic = false;
    tp.expectedScale = {0, 2, 4, 5, 7, 9, 11};  // all the white keys
    tp.shouldPass = false;
    testKeyCV2(tp);
}

static void testKeyCV2() {
    testKeyCVCMajor();
    testKeyCVNotCMajor();
}

static void testNotesInScale(Comp* composite) {
    composite->params[Comp::MODE_PARAM].value = int(Scale::Scales::Major);
    assertEQ(composite->numNotesInCurrentScale(), 7);
    composite->params[Comp::MODE_PARAM].value = int(Scale::Scales::Dorian);
    assertEQ(composite->numNotesInCurrentScale(), 7);
    composite->params[Comp::MODE_PARAM].value = int(Scale::Scales::Phrygian);
    assertEQ(composite->numNotesInCurrentScale(), 7);
    composite->params[Comp::MODE_PARAM].value = int(Scale::Scales::Lydian);
    assertEQ(composite->numNotesInCurrentScale(), 7);
    composite->params[Comp::MODE_PARAM].value = int(Scale::Scales::Mixolydian);
    assertEQ(composite->numNotesInCurrentScale(), 7);
    composite->params[Comp::MODE_PARAM].value = int(Scale::Scales::Minor);
    assertEQ(composite->numNotesInCurrentScale(), 7);
    composite->params[Comp::MODE_PARAM].value = int(Scale::Scales::Locrian);
    assertEQ(composite->numNotesInCurrentScale(), 7);

    composite->params[Comp::MODE_PARAM].value = int(Scale::Scales::MinorPentatonic);
    assertEQ(composite->numNotesInCurrentScale(), 5);
    composite->params[Comp::MODE_PARAM].value = int(Scale::Scales::Diminished);
    assertEQ(composite->numNotesInCurrentScale(), 8);
    composite->params[Comp::MODE_PARAM].value = int(Scale::Scales::DominantDiminished);
    assertEQ(composite->numNotesInCurrentScale(), 8);
    composite->params[Comp::MODE_PARAM].value = int(Scale::Scales::WholeStep);
    assertEQ(composite->numNotesInCurrentScale(), 6);
    composite->params[Comp::MODE_PARAM].value = int(Scale::Scales::Chromatic);
    assertEQ(composite->numNotesInCurrentScale(), 12);
}
static void testModeDetails() {
    /*
        static bool diatonicOnly() { return false; }
    static int numCurrentModes() { return 12; }
    static int numNotesInCurrentScale() { return 8; }
    */
    auto composite = std::make_shared<Comp>();
    composite->params[Comp::ONLY_USE_DIATONIC_PARAM].value = .3;

    // All the assertsions for diatonic only
    assertEQ(composite->diatonicOnly(), false);
    assertEQ(composite->numCurrentModes(), 13);

    //  all the assertions for all scales, not just diatonic
    composite->params[Comp::ONLY_USE_DIATONIC_PARAM].value = .7;
    assertEQ(composite->diatonicOnly(), true);
    assertEQ(composite->numCurrentModes(), 7);

    // things that don't really care about setting for diatonic
    composite->params[Comp::ONLY_USE_DIATONIC_PARAM].value = 0;
    testNotesInScale(composite.get());
    composite->params[Comp::ONLY_USE_DIATONIC_PARAM].value = 1;
    testNotesInScale(composite.get());
}

void testHarmony2() {
    testCanCall();
    testLabels();
    testPolyOutput();

    testKeyCV();
    testKeyCVWrap();
    testModeCV();

    testChord2();
    testChord3();

    testKeyCV2();
    testModeDetails();
}

#if 0
void testFirst() {
    // testKeyCV2();
    // testKeyCVNotCMajor();
    testModeDetails();
}
#endif