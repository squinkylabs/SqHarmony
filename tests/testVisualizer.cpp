

#include "TestComposite.h"
#include "Visualizer.h"
#include "asserts.h"

using Comp = Visualizer<TestComposite>;

void run(Comp& comp, const std::vector<float>& cVin) {
    if (!cVin.empty()) {
        comp.inputs[Comp::CV_INPUT].channels = 1;  // connect the input.
        comp.inputs[Comp::CV_INPUT].setChannels(cVin.size());

        //  comp.inputs.push_back(Port());
        // const auto ips = comp.inputs.size();
        for (int i = 0; i < cVin.size(); ++i) {
            SQINFO("setting2 input %d to %f", i, cVin[i]);
            comp.inputs[Comp::CV_INPUT].setVoltage(cVin[i], i);
        }
    }

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

 static void testWrongNumberNotRecognized() {
    Comp v;
    const float semi = 1.f / 12.f;
    const std::vector<float> foo = {0, 4 * semi, 7 * semi, 8 * semi};
    run(v, foo);
    assertEQ(v.params[Comp::TYPE_PARAM].value, float(ChordRecognizer::Type::Unrecognized));
 }

void testVisualizer() {
    testCanCall();

    testCanClockInCMajor();
    testCanClockInEMinor();
}

#if 1
void testFirst() {
  //  testCanClockInEMinor();
    testWrongNumberNotRecognized();
}
#endif