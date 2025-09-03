
#include "EvaluationSummary.h"
#include "FloatNote.h"
#include "MelodyEvaluator.h"
#include "MelodyGenerator.h"
#include "NoteConvert.h"
#include "asserts.h"

///////////////////////////////////////////////

static Scale scaleCMaj() {
    Scale scale;
    MidiNote base(MidiNote::C);
    scale.set(base, Scale::Scales::Major);
    return scale;
}

static MelodyRow getRow(int notes) {
    MelodyRow r;
    MelodyMutateState state;
    MelodyMutateStyle style;
    Scale scale = scaleCMaj();
    r.init(notes, scale);
    return r;
}

//////////////////////////////////////////////////////////

static void testMelodyEvaluatorCanCall() {
    MelodyRow r;
    r.setSize(1);
    MelodyMutateStyle style;
    const float a = MelodyEvaluator::getPenalty(r, style);
    const float b = MelodyEvaluator::leapsPenalty(r, style);
    const float c = MelodyEvaluator::unisonsPenalty(r, style);
    const float d = MelodyEvaluator::pitchRangePenalty(r, style);
}

// static void testMelodyGeneratorEvaluator() {
//     testMelodyGeneratorEvaluatorCanCall();
// }

static void testMelodyEvaluatorLeaps() {
    MelodyRow r = getRow(1);
    MelodyMutateStyle style;
    assertEQ(MelodyEvaluator::leapsPenalty(r, style), 0);
}

static void testMelodyEvaluatorLeaps2() {
    MelodyRow r = getRow(2);
    MelodyMutateStyle style;

    assert(r.getNote(0).get() == 72);
    assert(r.getNote(1).get() == 72);
    assertEQ(MelodyEvaluator::leapsPenalty(r, style), 0);

    r.setNote(1, MidiNote(MidiNote::MiddleC + MidiNote::A));  // huge leap
    assertGT(MelodyEvaluator::leapsPenalty(r, style), 0);
}

static void testMelodyEvaluatorUnison() {
    MelodyRow r = getRow(2);
    MelodyMutateStyle style;
    r.setNote(1, MidiNote(MidiNote::MiddleC + MidiNote::A));  // huge leap
    assertEQ(MelodyEvaluator::unisonsPenalty(r, style), 0);
}

static void testMelodyEvaluatorUnison2() {
    MelodyRow r = getRow(4);
    MelodyMutateStyle style;
    assertGT(MelodyEvaluator::unisonsPenalty(r, style), 0);
}

static void testMelodyEvaluatorConsonantCMajor() {
    MelodyRow r = getRow(1);
    MelodyMutateStyle style;
    style.scale.set(MidiNote(MidiNote::C), Scale::Scales::Major);

    assertEQ(MelodyEvaluator::disonnantPenalty(r, style), 0);

    r.setNote(0, MidiNote(MidiNote::F));
    assertEQ(MelodyEvaluator::disonnantPenalty(r, style), 0);

    r.setNote(0, MidiNote(MidiNote::G));
    assertEQ(MelodyEvaluator::disonnantPenalty(r, style), 0);

    r.setNote(0, MidiNote(MidiNote::C + 1));
    assertGT(MelodyEvaluator::disonnantPenalty(r, style), 0);
    r.setNote(0, MidiNote(MidiNote::D));
    assertGT(MelodyEvaluator::disonnantPenalty(r, style), 0);
    r.setNote(0, MidiNote(MidiNote::D + 1));
    assertGT(MelodyEvaluator::disonnantPenalty(r, style), 0);
    r.setNote(0, MidiNote(MidiNote::E));
    assertGT(MelodyEvaluator::disonnantPenalty(r, style), 0);

    r.setNote(0, MidiNote(MidiNote::F + 1));
    assertGT(MelodyEvaluator::disonnantPenalty(r, style), 0);
}

static void testMelodyEvaluatorConsonantBMinor() {
    MelodyRow r = getRow(1);
    MelodyMutateStyle style;
    style.scale.set(MidiNote(MidiNote::B), Scale::Scales::Minor);

    r.setNote(0, MidiNote(MidiNote::B));
    assertEQ(MelodyEvaluator::disonnantPenalty(r, style), 0);

    r.setNote(0, MidiNote(MidiNote::E));
    assertEQ(MelodyEvaluator::disonnantPenalty(r, style), 0);

    r.setNote(0, MidiNote(MidiNote::F + 1));
    assertEQ(MelodyEvaluator::disonnantPenalty(r, style), 0);

    r.setNote(0, MidiNote(MidiNote::C));
    assertGT(MelodyEvaluator::disonnantPenalty(r, style), 0);
    r.setNote(0, MidiNote(MidiNote::C + 1));
    assertGT(MelodyEvaluator::disonnantPenalty(r, style), 0);
    r.setNote(0, MidiNote(MidiNote::D));
    assertGT(MelodyEvaluator::disonnantPenalty(r, style), 0);
    r.setNote(0, MidiNote(MidiNote::D + 1));
    assertGT(MelodyEvaluator::disonnantPenalty(r, style), 0);
    r.setNote(0, MidiNote(MidiNote::F));
    assertGT(MelodyEvaluator::disonnantPenalty(r, style), 0);

    r.setNote(0, MidiNote(MidiNote::G));
    assertGT(MelodyEvaluator::disonnantPenalty(r, style), 0);
    r.setNote(0, MidiNote(MidiNote::G + 1));
    assertGT(MelodyEvaluator::disonnantPenalty(r, style), 0);
    r.setNote(0, MidiNote(MidiNote::A));
    assertGT(MelodyEvaluator::disonnantPenalty(r, style), 0);
    r.setNote(0, MidiNote(MidiNote::A + 1));
    assertGT(MelodyEvaluator::disonnantPenalty(r, style), 0);
}

static void testMelodyEvaluatorCentered() {
    // test should be centered
    MelodyMutateStyle style;
    MelodyRow r = getRow(4);
    assertEQ(MelodyEvaluator::nonCenteredPenalty(r, style), 0);
}

static void testMelodyEvaluatorCentered2() {
    // test should be centered
    MelodyRow r = getRow(4);
    MelodyMutateStyle style;
    r.setNote(1, MidiNote(MidiNote::MiddleC + MidiNote::C + 1));  // tiny fluctuation
    assertGT(MelodyEvaluator::nonCenteredPenalty(r, style), 0);
}

static void testMelodyEvaluatorCentered3() {
    // test should be centered
    MelodyRow r = getRow(4);
    MelodyMutateStyle style;
    r.setNote(1, MidiNote(MidiNote::MiddleC + MidiNote::C - 1));  // tiny fluctuation
    assertGT(MelodyEvaluator::nonCenteredPenalty(r, style), 0);
}

static void testMelodyEvaluatorCentered4() {
    // SQINFO("enter testMelodyEvaluatorCentered4");
    const int midiPitch = 94;
    // test should be centered
    MelodyRow r = getRow(4);
    MelodyMutateStyle style;
    FloatNote floatNote;
    NoteConvert::m2f(floatNote, MidiNote(midiPitch));
    style.centerVoltage = floatNote.get();

    for (int i = 0; i < 4; ++i) {
        r.setNote(i, MidiNote(midiPitch));
    }
    // SQINFO("row after init = %s", r.print().c_str());
    assertEQ(MelodyEvaluator::nonCenteredPenalty(r, style), 0);
}

static void testMelodyEvaluatorCenteredWeight() {
    MelodyRow r = getRow(2);
    const auto note = r.getNote(0);
    const MidiNote note2(note.get() + 12);
    r.setNote(0, note2);
    // SQINFO("init row %s", r.toString().c_str());

    MelodyMutateStyle style;
    style.setStyles(Styles::OnlySeekCenter);

    assertGT(MelodyEvaluator::nonCenteredPenalty(r, style), 0);
    style.nonCenteredWeight = 0;
    assertEQ(MelodyEvaluator::nonCenteredPenalty(r, style), 0);
}

static void testMelodyEvaluatorConsonantWeight() {
    MelodyRow r = getRow(1);

    r.setNote(0, MidiNote(MidiNote::D));        // D is dissonant in C Maj
    MelodyMutateStyle style;
    style.setStyles(Styles::OnlyDissonant);

    assertGT(MelodyEvaluator::disonnantPenalty(r, style), 0);
    style.dissonantWeight = 0;
    assertEQ(MelodyEvaluator::disonnantPenalty(r, style), 0);
}

static void testMelodyEvaluatorPitchRange() {
    const size_t size = 8;
    MelodyRow r = getRow(size);
    MelodyMutateStyle style;

    const float f = MelodyEvaluator::pitchRangePenalty(r, style);
    assertEQ(f, 2 * style.pitchRangeWeight);
}

static void testMelodyEvaluatorPitchRange2() {
    const size_t size = 3;
    MelodyRow r = getRow(size);
    MelodyMutateStyle style;

    // make the range two octaves (should we use style?)
    r.setNote(0, MidiNote(r.getNote(0).get() + 24));
    const float f = MelodyEvaluator::pitchRangePenalty(r, style);
    assertEQ(f, 0);
}

void testMelodyEvaluator() {
    testMelodyEvaluatorCanCall();
    testMelodyEvaluatorLeaps();
    testMelodyEvaluatorLeaps2();
    testMelodyEvaluatorUnison();
    testMelodyEvaluatorUnison2();
    // testMelodyEvaluatorUnison3();
    testMelodyEvaluatorConsonantCMajor();
    testMelodyEvaluatorConsonantBMinor();
    testMelodyEvaluatorCentered();
    testMelodyEvaluatorCentered2();
    testMelodyEvaluatorCentered3();
    testMelodyEvaluatorCentered4();
    testMelodyEvaluatorCenteredWeight();
    testMelodyEvaluatorConsonantWeight();

    testMelodyEvaluatorPitchRange();
    testMelodyEvaluatorPitchRange2();
}

// not a real test
static void runABit(int numTimes, int rowSize) {
    SQINFO("-- enter runABit(%d, %d) --", numTimes, rowSize);
    MelodyRow r;
    MelodyMutateState state;
    MelodyMutateStyle style;
    style.scale = scaleCMaj();

    // const int size = 8;
    r.init(rowSize, style.scale);
    SQINFO("here is starting row");
    SQINFO(r.toString().c_str());
    for (int i = 0; i < numTimes; ++i) {
        SQINFO("\n\n---------------- about to mutate %s at index %d", r.toString().c_str(), state.nextToMutate);
        MelodyGenerator::mutate(r, state, style);
        SQINFO("here is generated row %s penalty=%f", r.toString().c_str(), MelodyEvaluator::getPenalty(r, style));
    }

    SQINFO("-- exit foo --");
}

#if 0
static void showBias() {
    Scale scale;

    // error = 3 for major
    // -1 for minor
    // -1 for minor penta
    // - 11 for major penta
    // 2 for diminished
    // -2 for dominant diminished.
    scale.set(MidiNote(MidiNote::C), Scale::Scales::DominantDiminished);
    int totalError = 0;
    for (int semi=0; semi < 12; ++semi) {
        const int x = scale.quantize(semi);      
        ScaleNote scaleNote(x, 0);
        MidiNote quantizedMidiNote;
        NoteConvert::s2m(quantizedMidiNote, scale, scaleNote);

        const int finalPitch = quantizedMidiNote.get() - 24;
        const int error = finalPitch - semi;
        SQINFO("quantize(%d) = %d back to midi = %d error = %d", semi, x, finalPitch, error);
        totalError += error;
    }
    SQINFO("at end, total error = %d", totalError);
}
#endif

#if 0
void testFirst() {
    // runABit(50, 8);
    testMelodyEvaluator();
    // showBias();
    // testMelodyEvaluatorCentered4();
    // testMelodyEvaluatorPitchRange2();
    // testMelodyEvaluator();
    // testMelodyEvaluatorCenteredWeight();
    //  testMelodyEvaluatorConsonantWeight();
}
#endif