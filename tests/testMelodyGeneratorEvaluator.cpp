
#include "asserts.h"

#include "MelodyGenerator.h"
#include "NoteConvert.h"

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
    const int a = MelodyEvaluator::getPenalty(r);
    const int b = MelodyEvaluator::leapsPenalty(r);
}

//static void testMelodyGeneratorEvaluator() {
//    testMelodyGeneratorEvaluatorCanCall();
//}

static void testMelodyEvaluatorLeaps() {
    MelodyRow r = getRow(1);
    assertEQ(MelodyEvaluator::leapsPenalty(r), 0);
}

static void testMelodyEvaluatorLeaps2() {
    MelodyRow r = getRow(2);
    assert(r.getNote(0).get() == 72);
    assert(r.getNote(1).get() == 72);
    assertEQ(MelodyEvaluator::leapsPenalty(r), 0);

    r.setNote(1, MidiNote(MidiNote::MiddleC + MidiNote::A));    // huge leap
    assertGT(MelodyEvaluator::leapsPenalty(r), 0);
}

static void testMelodyEvaluatorUnison() {
    MelodyRow r = getRow(2);
    r.setNote(1, MidiNote(MidiNote::MiddleC + MidiNote::A));    // huge leap
    assertEQ(MelodyEvaluator::unisonsPenalty(r), 0);
}

static void testMelodyEvaluatorUnison2() {
    MelodyRow r = getRow(4);
    assertGT(MelodyEvaluator::unisonsPenalty(r), 0);
}

static void testMelodyEvaluatorCentered() {
    // test should be centered
    MelodyRow r = getRow(4);
    assertEQ(MelodyEvaluator::nonCenteredPenalty(r), 0);
}

static void testMelodyEvaluatorCentered2() {
    // test should be centered
    MelodyRow r = getRow(4);
    r.setNote(1, MidiNote(MidiNote::MiddleC + MidiNote::C + 1));    // tiny fluctuation
    assertGT(MelodyEvaluator::nonCenteredPenalty(r), 0);
}


static void testMelodyEvaluatorCentered3() {
    // test should be centered
    MelodyRow r = getRow(4);
    r.setNote(1, MidiNote(MidiNote::MiddleC + MidiNote::C - 1));    // tiny fluctuation
    assertGT(MelodyEvaluator::nonCenteredPenalty(r), 0);
}

void testMelodyEvaluator() {
    testMelodyEvaluatorCanCall();
    testMelodyEvaluatorLeaps();
    testMelodyEvaluatorLeaps2();
    testMelodyEvaluatorUnison();
    testMelodyEvaluatorUnison2();
    testMelodyEvaluatorCentered();
    testMelodyEvaluatorCentered2();
    testMelodyEvaluatorCentered3();

}

// not a real test
static void runABit(int numTimes, int rowSize) {
    SQINFO("-- enter runABit(%d, %d) --", numTimes, rowSize);
    MelodyRow r;
    MelodyMutateState state;
    MelodyMutateStyle style;
    Scale scale = scaleCMaj();

   // const int size = 8;
    r.init(rowSize, scale);
    SQINFO("here is starting row");
    SQINFO(r.print().c_str());
    for (int i = 0; i < numTimes; ++i) {
        SQINFO("\n\n---------------- about to mutate %s at index %d", r.print().c_str(), state.nextToMutate);
        MelodyGenerator::mutate(r, scale, state, style);
        SQINFO("here is generated row %s penalty=%f", r.print().c_str(), MelodyEvaluator::getPenalty(r));
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

#if 1
void testFirst() {
   // runABit(50, 8);
   testMelodyEvaluator();
   //showBias();
}
#endif