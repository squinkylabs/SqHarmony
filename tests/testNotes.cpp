
#include "FloatNote.h"
#include "MidiNote.h"
#include "NoteConvert.h"
#include "Scale.h"
#include "ScaleNote.h"
#include "asserts.h"




static void testFloatNoteCtor() {
    FloatNote f;
    assert(f.get() == 0);

    FloatNote f2(1.34f);
    assert(f2.get() == 1.34f);
}

static void testMidiToFloat() {
    const MidiNote m(MidiNote::C3 + 12);  // in VCV 0 v = c4
    FloatNote f;
    NoteConvert::m2f(f, m);
    assertEQ(f.get(), 0);
}

static void testMidiToFloat2() {
    const MidiNote m(MidiNote::C3 + 2 * 12);  // C5
    FloatNote f;
    NoteConvert::m2f(f, m);
    assert(f.get() == 1);
}

static void testMidiToFloat3() {
    for (int i = 0; i <= 127; ++i) {
        const MidiNote m(i);
        const float expected = (i - (MidiNote::C3 + 12)) * 1.f / 12.f;
        FloatNote f;
        NoteConvert::m2f(f, m);
        assertClose(f.get(), expected, .0001f);
    }
}

static void testFloatToMidiC4() {
    const FloatNote f(0);
    MidiNote m;
    NoteConvert::f2m(m, f);

    assert(m.get() == (MidiNote::C3 + 12));
}

static void testFloatToMidiD4() {
    const FloatNote f(2.f / 12.f);
    MidiNote m;
    NoteConvert::f2m(m, f);

    assert(m.get() == (MidiNote::C3 + 12 + 2));
}

static void testFloatToMidiC5() {
    const FloatNote f(1);
    MidiNote m;
    NoteConvert::f2m(m, f);

    assert(m.get() == (MidiNote::C3 + 2 * 12));
}

static void testFloatMidiRoundTrip1() {
    MidiNote mn(0);
    FloatNote fn;
    NoteConvert::m2f(fn, mn);
    MidiNote mn2;
    NoteConvert::f2m(mn2, fn);
    assertEQ(mn2.get(), mn.get());
}

static void testFloatMidiRoundTrip2() {
    for (int i = -20; i < 150; ++i) {
        const MidiNote mn(i);
        FloatNote fn;
        NoteConvert::m2f(fn, mn);
        MidiNote mn2;
        NoteConvert::f2m(mn2, fn);
        assertEQ(mn2.get(), i);
    }
}

static void testScaleNoteCtor() {
    ScaleNote d;
    assertEQ(d.getDegree(), 0);
    assertEQ(d.getOctave(), 0);
    assertEQ(d.isAccidental(), false);
}

static void testScaleNoteCtor2() {
    ScaleNote d(1, 2, ScaleNote::RelativeAdjustment::sharp);
    assertEQ(d.getDegree(), 1);
    assertEQ(d.getOctave(), 2);
    assertEQ(d.isAccidental(), true);
}

static void testScaleNoteToMidiC4() {
    Scale scale;
    scale.set(MidiNote(MidiNote::C), Scale::Scales::Major);  // 4 = C
    ScaleNote sn(0, 4);                                      // middle C
    MidiNote m;
    NoteConvert::s2m(m, scale, sn);
    assertEQ(m.get(), MidiNote::C3 + 12);
}

static void testScaleNoteToMidiC5() {
    Scale scale;
    scale.set(MidiNote(MidiNote::C), Scale::Scales::Major);
    ScaleNote sn(0, 5);  // middle C up an octave
    MidiNote m;
    NoteConvert::s2m(m, scale, sn);
    assertEQ(m.get(), MidiNote::C3 + 2 * 12);
}

static void testScaleNoteToMidiCsharp5() {
    Scale scale;
    scale.set(MidiNote(MidiNote::C), Scale::Scales::Major);
    ScaleNote sn(0, 5, ScaleNote::RelativeAdjustment::sharp);  // C5 sharp
    MidiNote m;
    NoteConvert::s2m(m, scale, sn);
    // assertEQ(m.get(), 64 + 12 + 1);
    assertEQ(m.get(), MidiNote::C3 + (2 * 12) + 1);
}

static void testScaleNoteTrans() {
    ScaleNote sn(3, 5);
    assertEQ(sn.getDegree(), 3);
    sn.transposeDegree(1, Scale::numStepsInDiatonicScale);
    assertEQ(sn.getDegree(), 4);
}

static void testScaleNoteTrans2() {
    ScaleNote sn(6, 5);
    sn.transposeDegree(1, Scale::numStepsInDiatonicScale);
    assertEQ(sn.getDegree(), 0);
    assertEQ(sn.getOctave(), 6);
}

static void testScaleNoteTrans3() {
    ScaleNote sn(0, 5);
    sn.transposeDegree(-6, Scale::numStepsInDiatonicScale);
    assertEQ(sn.getDegree(), 1);
    assertEQ(sn.getOctave(), 4);
}

static void testScaleNoteTrans4() {
    Scale scale;
    scale.set(MidiNote(MidiNote::C), Scale::Scales::Major);
    ScaleNote sn(0, 4);  // start at middle C
    MidiNote mn;
    NoteConvert::s2m(mn, scale, sn);
    assertEQ(mn.get(), MidiNote::MiddleC);

    // second degree of CMaj is D
    sn.transposeDegree(1, Scale::numStepsInDiatonicScale);
    NoteConvert::s2m(mn, scale, sn);
    assertEQ(mn.get(), MidiNote::MiddleC + 2);

    // third degree of CMaj is E
    sn.transposeDegree(1, Scale::numStepsInDiatonicScale);
    NoteConvert::s2m(mn, scale, sn);
    assertEQ(mn.get(), MidiNote::MiddleC + 2 + 2);

    // fourth degree of CMaj is F
    sn.transposeDegree(1, Scale::numStepsInDiatonicScale);
    NoteConvert::s2m(mn, scale, sn);
    assertEQ(mn.get(), MidiNote::MiddleC + 5);

    // fifth degree of CMaj is G
    sn.transposeDegree(1, Scale::numStepsInDiatonicScale);
    NoteConvert::s2m(mn, scale, sn);
    assertEQ(mn.get(), MidiNote::MiddleC + 7);

    // sixth degree of CMaj is A
    sn.transposeDegree(1, Scale::numStepsInDiatonicScale);
    NoteConvert::s2m(mn, scale, sn);
    assertEQ(mn.get(), MidiNote::MiddleC + 9);

    // seventy degree of CMaj is B
    sn.transposeDegree(1, Scale::numStepsInDiatonicScale);
    NoteConvert::s2m(mn, scale, sn);
    assertEQ(mn.get(), MidiNote::MiddleC + 11);

    // 8th degree of CMaj is C up an octave
    sn.transposeDegree(1, Scale::numStepsInDiatonicScale);
    NoteConvert::s2m(mn, scale, sn);
    assertEQ(mn.get(), MidiNote::MiddleC + 12);
}

static void testScaleNoteToMidiD4() {
    Scale scale;
    scale.set(MidiNote(MidiNote::C), Scale::Scales::Major);

    ScaleNote d(1, 4);  // degree 1 in cmaj, d above middle c
    MidiNote m;
    NoteConvert::s2m(m, scale, d);
    assertEQ(m.get(), MidiNote::C3 + 2 + 12);
}

void testNotes() {
    testFloatNoteCtor();
    testMidiToFloat();
    testFloatToMidiC4();
    testFloatToMidiD4();

    testMidiToFloat();
    testMidiToFloat3();
    // testFloatToMidi2();
    testFloatToMidiC5();
    testFloatMidiRoundTrip1();
    testFloatMidiRoundTrip2();

    testScaleNoteCtor();
    testScaleNoteCtor2();
    testScaleNoteToMidiC4();
    testScaleNoteToMidiC5();
    testScaleNoteToMidiD4();
    testScaleNoteToMidiCsharp5();

    testScaleNoteTrans();
    testScaleNoteTrans2();
    testScaleNoteTrans3();
    testScaleNoteTrans4();

   
}

#if 0
void testFirst() {
    //  testScorePitchUtils();
    // testMidiIsBlackKey();
    // testMidiStaffE();
    //  testMidiStaffEFlat();
    testNotes();
}
#endif