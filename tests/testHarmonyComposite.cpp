
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
    assert(x > 100);  // plenty of chords, by default
    h.params[Comp::CENTER_PREFERENCE_PARAM].value = float(int(Style::Ranges::VOCAL_RANGE));

    h.process(TestComposite::ProcessArgs());

    int y = h._size();
    assertLT(y, x);
}

static void testLabels() {
    assertEQ(Comp::getHistoryLabels().size(), 4);
    assertEQ(Comp::getHistoryLabels()[0], "off");
    assertEQ(Comp::getHistoryLabels()[1], "4");
    assertEQ(Comp::getHistoryLabels()[2], "8");
    assertEQ(Comp::getHistoryLabels()[3], "13");
}
/*
 * test 0 = not clocks at all
 * test 1 = one clock, no delay
 */
static void testTrigger(int whichTest) {
    Comp h;

    const bool delay = (whichTest == 2);

    h.outputs[Comp::BASS_OUTPUT].channels = 1;
    h.inputs[Comp::TRIGGER_INPUT].channels = 1;
    h.inputs[Comp::TRIGGER_INPUT].setVoltage(0, 0);

    // process a D for a bit;
    h.inputs[Comp::CV_INPUT].setVoltage(2.f / 12.f, 0);
    h.params[Comp::TRIGGER_DELAY_PARAM].value = delay ? 1.f : 0.f;
    h.params[Comp::RETRIGGER_CV_AND_NOTE_PARAM].value = 0;

    for (int i = 0; i < 50; ++i) {
        h.process(TestComposite::ProcessArgs());
    }

    // X is the at rest output with no trigger
    const float x = h.outputs[Comp::BASS_OUTPUT].value;

    // maybe we send a trigger here
    if (whichTest > 0) {
        h.inputs[Comp::TRIGGER_INPUT].setVoltage(10, 0);
        h.process(TestComposite::ProcessArgs());
    }

    // after that first trigger we change the CV
    h.inputs[Comp::CV_INPUT].setVoltage(4.f / 12.f, 0);
    h.process(TestComposite::ProcessArgs());

    // y is after a cv change, but no trigger
    const float y = h.outputs[Comp::BASS_OUTPUT].value;
    for (int i = 0; i < 5; ++i) {
        h.process(TestComposite::ProcessArgs());
    }

    // Z is is possibly one sample after the trigger
    const float z = h.outputs[Comp::BASS_OUTPUT].value;

    switch (whichTest) {
        case 0:
            assertEQ(x, 0);
            assertEQ(y, 0);
            assertEQ(z, 0);
            break;
        case 1:  // no delay one trigger
            assertEQ(x, 0);
            assertNE(x, y);
            assertEQ(y, z);
            break;
        case 2:
            assertEQ(x, 0);
            assertEQ(y, 0);
            assertNE(y, z);
            break;
        default:
            assert(false);
    }
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

    //
    SQINFO("put back composite test num chords");
    // testNumChords();
    testLabels();

    testTrigger(0);
    testTrigger(1);
    testTrigger(2);
}