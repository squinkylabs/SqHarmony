

#include "TestComposite.h"
#include "Visualizer.h"
#include "asserts.h"

using Comp = Visualizer<TestComposite>;

static void copyInCV(Comp& comp, int input, const std::vector<float>& cVin) {
    if (!cVin.empty()) {
        comp.inputs[input].channels = 1;  // connect the input.
        comp.inputs[input].setChannels(cVin.size());

        for (int i = 0; i < cVin.size(); ++i) {
            comp.inputs[input].setVoltage(cVin[i], i);
        }
    }
}

static void run(Comp& comp, const std::vector<float>& cVin, const std::vector<float>& gateVin = {}) {
    copyInCV(comp, Comp::CV_INPUT, cVin);
    copyInCV(comp, Comp::GATE_INPUT, gateVin);

    const auto args = TestComposite::ProcessArgs();
    for (int i = 0; i < Comp::getSubSampleFactor(); ++i) {
        comp.process(args);
    }
}

static void testCanCall() {
    Comp v;

    const std::vector<float> foo;
    run(v, foo);
}

static void testCanClockInCMajor() {
    Comp v;
    const float semi = 1.f / 12.f;
    const std::vector<float> foo = {0, 4 * semi, 7 * semi};
    run(v, foo);
    assertEQ(v.params[Comp::TYPE_PARAM].value, float(ChordRecognizer::Type::MajorTriad));
    assertEQ(v.params[Comp::INVERSION_PARAM].value, float(ChordRecognizer::Inversion::Root));
    assertEQ(v.params[Comp::ROOT_PARAM].value, float(MidiNote::C));
}

static void testCanClockInEMinor() {
    Comp v;
    const float semi = 1.f / 12.f;
    const std::vector<float> foo = {
        2 + semi * MidiNote::E,
        2 + semi * MidiNote::G,
        2 + semi * MidiNote::B};

    run(v, foo);
    assertEQ(v.params[Comp::TYPE_PARAM].value, float(ChordRecognizer::Type::MinorTriad));
    assertEQ(v.params[Comp::INVERSION_PARAM].value, float(ChordRecognizer::Inversion::Root));
    assertEQ(v.params[Comp::ROOT_PARAM].value, float(MidiNote::E));
}

static void testCanGetOutput() {
    Comp v;
    const float semi = 1.f / 12.f;
    const std::vector<float> foo = {
        semi * MidiNote::E,
        semi * MidiNote::G,
        semi * MidiNote::B};

    run(v, foo);
    // const auto x = v.getQuantizedPitchesAndChannels();
    // const unsigned size = std::get<1>(x);
    // const int* note = std::get<0>(x);

    const auto x = v.getQuantizedPitches();
    const unsigned size = x.numValid();
    const int* note = x.getDirectPtrAt(0);

    assertEQ(size, 3);
    const int n0 = note[0];
    const int n1 = note[1];
    const int n2 = note[2];
    assertEQ(note[0], MidiNote::MiddleC + MidiNote::E);
    assertEQ(note[1], MidiNote::MiddleC + MidiNote::G);
    assertEQ(note[2], MidiNote::MiddleC + MidiNote::B);
}

static void testWrongNumberNotRecognized() {
    Comp v;
    const float semi = 1.f / 12.f;
    const std::vector<float> foo = {0, 4 * semi, 7 * semi, 8 * semi};
    run(v, foo);
    assertEQ(v.params[Comp::TYPE_PARAM].value, float(ChordRecognizer::Type::Unrecognized));
}

static void testWithGate() {
    Comp v;
    const float semi = 1.f / 12.f;
    const std::vector<float> cv = {0, 4 * semi, 7 * semi, 8 * semi};
    const std::vector<float> gate = {10, 10, 10, 0};
    run(v, cv, gate);
    assertEQ(v.params[Comp::TYPE_PARAM].value, float(ChordRecognizer::Type::MajorTriad));
}

static void testWithGate2() {
    Comp v;
    const float semi = 1.f / 12.f;
    const std::vector<float> cv = {0, 4 * semi, 7 * semi, 8 * semi};
    std::vector<float> gate = {0, 0, 0, 0};
    run(v, cv, gate);
    assertEQ(v.params[Comp::TYPE_PARAM].value, float(ChordRecognizer::Type::Unrecognized));
    // std::tuple<const int*, unsigned> getQuantizedPitchesAndChannels() const {
    auto x = v.getQuantizedPitches();
    assertEQ(x.numValid(), 0);

    gate[0] = 10;
    run(v, cv, gate);
    assertEQ(v.params[Comp::TYPE_PARAM].value, float(ChordRecognizer::Type::Unrecognized));
    x = v.getQuantizedPitches();
    assertEQ(x.numValid(), 1);
}

static void testRootOut() {
    Comp v;
    const float semi = 1.f / 12.f;
    const float octaveOffset = 2.f;
    const std::vector<float> cv = {
        octaveOffset + 6 * semi,
        octaveOffset + 2 * semi,
        octaveOffset + 6 * semi,
        octaveOffset + 6 * semi,
        octaveOffset + 9 * semi};  // D Major, out of order with doubling

    run(v, cv);

    assertEQ(v.params[Comp::TYPE_PARAM].value, float(ChordRecognizer::Type::MajorTriad));
    assertEQ(v.params[Comp::ROOT_PARAM].value, float(MidiNote::D));
    assertEQ(v.outputs[Comp::ROOT_OUTPUT].value, octaveOffset + 2 * semi);
}

static std::shared_ptr<Comp> testRecognizedOut(bool bTestCase) {
    //  Comp v;
    std::shared_ptr<Comp> comp = std::make_shared<Comp>();
    const float semi = 1.f / 12.f;
    const std::vector<float> cmajor = {0, 4 * semi, 7 * semi};
    const std::vector<float> other = {0, 1 * semi, 7 * semi};
    run(*comp, bTestCase ? cmajor : other);
    const bool bRecognized = comp->outputs[Comp::RECOGNIZED_OUTPUT].value > 5;
    assertEQ(bRecognized, bTestCase);
    return comp;
}

static void testRecognizedOut() {
    testRecognizedOut(true);
    testRecognizedOut(false);
}

static void testRecognizedOutSpeed(bool fast) {
    auto comp = testRecognizedOut(true);
    assert(comp->params[Comp::TYPE_PARAM].value == float(ChordRecognizer::Type::MajorTriad));

    const bool shouldConnectOutput = fast;
    const int channels = shouldConnectOutput ? 1 : 0;

    comp->outputs[Comp::RECOGNIZED_OUTPUT].channels = channels;  // disconnect recognizer outputs.
    comp->outputs[Comp::ROOT_OUTPUT].channels = channels;

    // Process once so we are not on an even boundary
    const auto args = TestComposite::ProcessArgs();
    comp->process(args);

    // Now change CV 0 so it's not a C Major anymore
    //   comp.inputs[input].setVoltage(cVin[i], i);
    comp->inputs[Comp::CV_INPUT].setVoltage(.5f, 0);

    // Process just once to see if we see that change in input CV.
    comp->process(args);

    float type = comp->params[Comp::TYPE_PARAM].value;
    const float expectedType = float(
        fast ? ChordRecognizer::Type::Unrecognized : ChordRecognizer::Type::MajorTriad);
    assertEQ(type, expectedType);
}

static void testRecognizedOutSpeed() {
    testRecognizedOutSpeed(false);
    testRecognizedOutSpeed(true);
}

void testVisualizer() {
    assertEQ(SqLog::errorCount, 0);
    testCanCall();

    testCanClockInCMajor();
    testCanClockInEMinor();
    testWrongNumberNotRecognized();
    testWithGate();
    testCanGetOutput();
    testRootOut();
    testRecognizedOut();
    testRecognizedOutSpeed();
    assertEQ(SqLog::errorCount, 0);
}

#if 0
void testFirst() {
    testRecognizedOutSpeed();
}
#endif