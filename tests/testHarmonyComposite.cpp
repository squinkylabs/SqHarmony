
#include "Harmony.h"
#include "SqLog.h"
#include "TestComposite.h"
#include "asserts.h"

using Comp = Harmony<TestComposite>;

static void test0() {
    Comp h;

    // hook up the input
    h.inputs[Comp::CV_INPUT].channels = 1;
    // and the output
    h.outputs[Comp::xQUANTIZER_OUTPUT].channels = 1;
    h.inputs[Comp::CV_INPUT].setVoltage(0, 0);
    h.process(TestComposite::ProcessArgs());
    assertEQ(h.outputs[Comp::xQUANTIZER_OUTPUT].getVoltage(0), 0);
}

#if 0
static void testQuant1() {
    Comp h;
    h.inputs[Comp::CV_INPUT].channels = 1;
    h.inputs[Comp::CV_INPUT].setVoltage(1.f, 0);
    h.process(TestComposite::ProcessArgs());
    assertEQ(h.outputs[Comp::xQUANTIZER_OUTPUT].getVoltage(0), 1);
}
#endif

static void testRunABit() {
    Comp h;
    h.inputs[Comp::CV_INPUT].channels = 1;
    h.inputs[Comp::CV_INPUT].setVoltage(1.f, 0);
    h.process(TestComposite::ProcessArgs());
    h.inputs[Comp::CV_INPUT].setVoltage(1.5f, 0);
    h.process(TestComposite::ProcessArgs());
    h.inputs[Comp::CV_INPUT].setVoltage(1.2f, 0);
    h.process(TestComposite::ProcessArgs());
    h.inputs[Comp::CV_INPUT].setVoltage(1.4f, 0);
    h.process(TestComposite::ProcessArgs());
}

// Seeing C3 and then C4 used to cause asserts.
static void testOctave() {
    Comp h;
    h.inputs[Comp::CV_INPUT].channels = 1;
    h.inputs[Comp::CV_INPUT].setVoltage(1.f, 0);
    h.process(TestComposite::ProcessArgs());
    h.process(TestComposite::ProcessArgs());

    h.inputs[Comp::CV_INPUT].setVoltage(2.0f, 0);
    h.process(TestComposite::ProcessArgs());
    h.process(TestComposite::ProcessArgs());
}

static void testSemiUpDown() {
    Comp h;
    h.inputs[Comp::CV_INPUT].channels = 1;
    h.inputs[Comp::CV_INPUT].setVoltage(1.f, 0);
    h.process(TestComposite::ProcessArgs());
    h.process(TestComposite::ProcessArgs());
    const float root1 = h.outputs[Comp::BASS_OUTPUT].value;

    h.inputs[Comp::CV_INPUT].setVoltage(1.f + 1 / 12.f, 0);
    h.process(TestComposite::ProcessArgs());
    h.process(TestComposite::ProcessArgs());
    const float root2 = h.outputs[Comp::BASS_OUTPUT].value;

    h.inputs[Comp::CV_INPUT].setVoltage(1.f, 0);
    h.process(TestComposite::ProcessArgs());
    h.process(TestComposite::ProcessArgs());
    const float root3 = h.outputs[Comp::BASS_OUTPUT].value;

    assert(root2 != root1);
    assert(root2 != root3);
}

static void testMonoConnectVoiceCount() {
    Comp h;

    // patch all the outputs
    h.outputs[Comp::BASS_OUTPUT].channels = 1;
    h.outputs[Comp::TENOR_OUTPUT].channels = 1;
    h.outputs[Comp::ALTO_OUTPUT].channels = 1;
    h.outputs[Comp::SOPRANO_OUTPUT].channels = 1;

    // set to D so we can't get any zeros
    h.inputs[Comp::CV_INPUT].setVoltage(2.f / 12.f, 0);

    for (int i = 0; i < 50; ++i) {
        h.process(TestComposite::ProcessArgs());
    }

    const int x = h.outputs[Comp::BASS_OUTPUT].getChannels();
    assertEQ(h.outputs[Comp::BASS_OUTPUT].channels, 1);
    assertEQ(h.outputs[Comp::TENOR_OUTPUT].channels, 1);
    assertEQ(h.outputs[Comp::ALTO_OUTPUT].channels, 1);
    assertEQ(h.outputs[Comp::SOPRANO_OUTPUT].channels, 1);

    assertNE(h.outputs[Comp::BASS_OUTPUT].getVoltage(0), 0);
    assertNE(h.outputs[Comp::ALTO_OUTPUT].getVoltage(0), 0);
    assertNE(h.outputs[Comp::TENOR_OUTPUT].getVoltage(0), 0);
    assertNE(h.outputs[Comp::SOPRANO_OUTPUT].getVoltage(0), 0);
}

static void testSoprano4VoiceCount() {
    SQINFO("--- testSoprano4VoiceCount");
    Comp h;

    // patch only soprano
    h.outputs[Comp::SOPRANO_OUTPUT].channels = 1;
      // set to D so we can't get any zeros
    h.inputs[Comp::CV_INPUT].setVoltage(2.f / 12.f, 0);

    for (int i = 0; i < 50; ++i) {
        h.process(TestComposite::ProcessArgs());
    }

    const int x = h.outputs[Comp::BASS_OUTPUT].getChannels();
    assertEQ(h.outputs[Comp::BASS_OUTPUT].getChannels(), 0);
    assertEQ(h.outputs[Comp::TENOR_OUTPUT].getChannels(), 0);
    assertEQ(h.outputs[Comp::ALTO_OUTPUT].getChannels(), 0);
    assertEQ(h.outputs[Comp::SOPRANO_OUTPUT].getChannels(), 4);


    assertNE(h.outputs[Comp::SOPRANO_OUTPUT].getVoltage(0), 0);
    assertNE(h.outputs[Comp::SOPRANO_OUTPUT].getVoltage(1), 0);
    assertNE(h.outputs[Comp::SOPRANO_OUTPUT].getVoltage(2), 0);
    assertNE(h.outputs[Comp::SOPRANO_OUTPUT].getVoltage(3), 0);
}

static void testBassAndSopranoVoiceCount() {
    SQINFO("-------------- testBassAndSopranoVoiceCount ");
    Comp h;

    h.outputs[Comp::SOPRANO_OUTPUT].channels = 1;
    h.outputs[Comp::BASS_OUTPUT].channels = 1;
      // set to D so we can't get any zeros
    h.inputs[Comp::CV_INPUT].setVoltage(2.f / 12.f, 0);

    for (int i = 0; i < 50; ++i) {
        h.process(TestComposite::ProcessArgs());
    }

    assertEQ(h.outputs[Comp::BASS_OUTPUT].getChannels(), 1);
    assertEQ(h.outputs[Comp::TENOR_OUTPUT].getChannels(), 0);
    assertEQ(h.outputs[Comp::ALTO_OUTPUT].getChannels(), 0);
    assertEQ(h.outputs[Comp::SOPRANO_OUTPUT].getChannels(), 3);

    assertNE(h.outputs[Comp::BASS_OUTPUT].getVoltage(0), 0);
    assertNE(h.outputs[Comp::SOPRANO_OUTPUT].getVoltage(0), 0);
    assertNE(h.outputs[Comp::SOPRANO_OUTPUT].getVoltage(1), 0);
    assertNE(h.outputs[Comp::SOPRANO_OUTPUT].getVoltage(2), 0);
}

static void test2and2VoiceCount() {
    SQINFO("-------------- test2and2VoiceCount ");
    
    
    Comp h;
    h.outputs[Comp::TENOR_OUTPUT].channels = 1;
    h.outputs[Comp::SOPRANO_OUTPUT].channels = 1;
      // set to D so we can't get any zeros
    h.inputs[Comp::CV_INPUT].setVoltage(2.f / 12.f, 0);

    for (int i = 0; i < 50; ++i) {
        h.process(TestComposite::ProcessArgs());
    }

    assertEQ(h.outputs[Comp::BASS_OUTPUT].getChannels(), 0);
    assertEQ(h.outputs[Comp::TENOR_OUTPUT].getChannels(), 2);
    assertEQ(h.outputs[Comp::ALTO_OUTPUT].getChannels(), 0);
    assertEQ(h.outputs[Comp::SOPRANO_OUTPUT].getChannels(), 2);

    assertNE(h.outputs[Comp::TENOR_OUTPUT].getVoltage(0), 0);
    assertNE(h.outputs[Comp::TENOR_OUTPUT].getVoltage(1), 0);
    assertNE(h.outputs[Comp::SOPRANO_OUTPUT].getVoltage(0), 0);
    assertNE(h.outputs[Comp::SOPRANO_OUTPUT].getVoltage(1), 0);
}

static void testNumChords() {
    Comp h;
    int x = h._size();
    assert(x > 100);            // plenty of chords, by default
    SQINFO("test setting to enum narrow %d", int(Style::Ranges::NARROW_RANGE));
    h.params[Comp::CENTER_PREFERENCE_PARAM].value = float(int(Style::Ranges::NARROW_RANGE));
   // for (int i=0; i < 32; ++i)
   h.process(TestComposite::ProcessArgs());

    int y = h._size();
    assertLT(y, x);

}

static void testLabels() {
    // I this test fails may need to re-code how composite
    // interprets history size
    assertEQ(Comp::getHistoryLabels().size(), 4);
    assertEQ(Comp::getHistoryLabels()[0], "off");
    assertEQ(Comp::getHistoryLabels()[1], "4");
    assertEQ(Comp::getHistoryLabels()[2], "8");
    assertEQ(Comp::getHistoryLabels()[3], "13");
}

void testHarmonyComposite() {
    test0();
  //  testQuant1();
    testRunABit();
    testOctave();
    testSemiUpDown();

    testMonoConnectVoiceCount();
    testSoprano4VoiceCount();
    testBassAndSopranoVoiceCount();
    test2and2VoiceCount();
    testNumChords();
    testLabels();
}