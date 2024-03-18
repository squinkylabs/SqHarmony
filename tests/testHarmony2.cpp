#include "Harmony2.h"
#include "TestComposite.h"
#include "testUtils.h"
#include "asserts.h"

using Comp = Harmony2<TestComposite>;
using CompPtr = std::shared_ptr<Comp>;

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
    processBlock(*comp);

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
    SQINFO("-- testModeCV cv=%f, expected mode = %d", cv, expectedParam);
    assert(expectedParam >= 0);
    assert(expectedParam < 14);
    auto comp = std::make_shared<Comp>();
    comp->inputs[Comp::MODE_INPUT].setVoltage(cv);
    comp->inputs[Comp::MODE_INPUT].channels = 1;
    comp->params[Comp::ONLY_USE_DIATONIC_PARAM].value = 1;
    processBlock(*comp);

    assertEQ(comp->params[Comp::MODE_PARAM].value, expectedParam);
}

static void testModeCV() {
    const int numDia = Scale::numDiatonicScales();
    const float degree = 1.f / 12.f;        // each mode is 1/12 volt from the prev
    testModeCV(0, 0);                       // Major 
    testModeCV(degree, 1);                  // Dorian

    // All the diatonic modes
    for (int i = 0; i <= numDia; ++i) {
        testModeCV(degree * i, i);
    }

    // All the diatonic modes wrapped
    for (int i = 0; i <= numDia; ++i) {
        for (int j = -2; j <= 2; ++j) {
            const float f = (degree * i) + j * numDia;
            testModeCV(f, i);
        }
    }

    assert(false); // need tests for non-diatonic, too
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

    testModeDetails();
}

#if 1
void testFirst() {
    testModeCV();
}
#endif