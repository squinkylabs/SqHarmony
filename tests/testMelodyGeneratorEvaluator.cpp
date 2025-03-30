
#include "asserts.h"

#include "MelodyGenerator.h"

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


void testMelodyEvaluator() {
    testMelodyEvaluatorCanCall();
    testMelodyEvaluatorLeaps();
    testMelodyEvaluatorLeaps2();
    testMelodyEvaluatorUnison();
    testMelodyEvaluatorUnison2();
}

#if 0
void testFirst() {
    //foo();
   testMelodyEvaluator();
}
#endif