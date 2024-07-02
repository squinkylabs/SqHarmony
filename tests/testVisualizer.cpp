

#include "TestComposite.h"
#include "Visualizer.h"
#include "asserts.h"

using Comp = Visualizer<TestComposite>;

static void copyInCV(Comp& comp, int input, const std::vector<float>& cVin) {
  //  SQINFO("-- copy in CV");
     if (!cVin.empty()) {
        comp.inputs[input].channels = 1;  // connect the input.
        comp.inputs[input].setChannels(cVin.size());

        //  comp.inputs.push_back(Port());
        // const auto ips = comp.inputs.size();
        for (int i = 0; i < cVin.size(); ++i) {
         //   SQINFO("setting2 input %d to %f", i, cVin[i]);
            comp.inputs[input].setVoltage(cVin[i], i);
        }
       // SQINFO("for input %d set channels to %d", input, comp.inputs[input].getChannels());
    }
}

static void run(Comp& comp, const std::vector<float>& cVin, const std::vector<float>& gateVin = {} ) {
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
  //  SQINFO("------------ testCanClockInCMajor");
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

static void testWithGate() {
    Comp v;
    const float semi = 1.f / 12.f;
    const std::vector<float> cv = {0, 4 * semi, 7 * semi, 8 * semi};
    const std::vector<float> gate = { 10, 10, 10, 0 };
    run(v, cv, gate);
    assertEQ(v.params[Comp::TYPE_PARAM].value, float(ChordRecognizer::Type::MajorTriad));
}

void testVisualizer() {
    testCanCall();

    testCanClockInCMajor();
    testCanClockInEMinor();
    testWrongNumberNotRecognized();
    testWithGate();
}

#if 0
void testFirst() {
 //   testCanClockInCMajor();
    //  testCanClockInEMinor();
  //  testWithGate();
    testVisualizer();
}
#endif