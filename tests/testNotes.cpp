
#include "FloatNote.h"
#include "MidiNote.h"
#include "NoteConvert.h"
#include "Scale.h"
#include "ScaleNote.h"
#include "asserts.h"

static void testMidiNoteCtor() {
    MidiNote m;
    assert(m.get() == 64);

    MidiNote m2(12);
    assert(m2.get() == 12);
}

static void testMidiNoteNorm() {
#if 0
    assertEQ(MidiNote(0).getNormalize().first, 0);
    assertEQ(MidiNote(0).getNormalize().second, 0);

    assertEQ(MidiNote(1).getNormalize().first, 0);
    assertEQ(MidiNote(1).getNormalize().second, 1);

    assertEQ(MidiNote(12).getNormalize().first, 1);
    assertEQ(MidiNote(12).getNormalize().second, 0);

    assertEQ(MidiNote(3 * 12 + 11).getNormalize().first, 3);
    assertEQ(MidiNote(3 * 12 + 11).getNormalize().second, 11);
#endif
}

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

static void testMidiStaffE() {
    MidiNote mn(MidiNote::MiddleC + 4);  // e above middle C
    const int ll = mn.getLedgerLine(false);
    assertEQ(ll, 0);
}

static void testMidiStaffG() {
    MidiNote mn(MidiNote::MiddleC + 7);
    const int ll = mn.getLedgerLine(false);
    assertEQ(ll, 2);
}

static void testMidiStaffC() {
    MidiNote mn(MidiNote::MiddleC);
    const int ll = mn.getLedgerLine(false);
    assertEQ(ll, -2);
}

static void testMidiStaffC2() {
    MidiNote mn(MidiNote::MiddleC + 12);
    const int ll = mn.getLedgerLine(false);
    assertEQ(ll, 5);
}

static void testMidiStaffC3() {
    MidiNote mn(MidiNote::MiddleC + 2 * 12);
    const int ll = mn.getLedgerLine(false);
    assertEQ(ll, 12);
}

static void testMidiStaffLowA() {
    MidiNote mn(MidiNote::MiddleC - 3);
    const int ll = mn.getLedgerLine(false);
    assertEQ(ll, -4);
}

static void testMidiBassStaffA() {
    MidiNote mn(MidiNote::MiddleC - 3);  // A below middle C. top of bass
    const int ll = mn.getLedgerLine(true);
    assertEQ(ll, 8);
}

static void testMidiStaffAll(bool bassStaff) {
    int lastLL = 0;
    for (int i = 0; i < 128; ++i) {
        MidiNote mn(i);
        const int ll = mn.getLedgerLine(bassStaff);
        if (i != 0) {
            assert((ll == lastLL) || (ll == (lastLL + 1)));
        }
        lastLL = ll;
    }
}

static void testMidiStaffX() {
    MidiNote mn(MidiNote::MiddleC);
    const int ll = mn.getLedgerLine(false);
    MidiNote mn2(MidiNote::MiddleC + 2);  // D
    const int ll2 = mn2.getLedgerLine(false);
    MidiNote mn3(MidiNote::MiddleC + 4);  // E
    const int ll3 = mn3.getLedgerLine(false);
}

static void testMidiIsBlackKey() {
    assertEQ(MidiNote(MidiNote::C).isBlackKey(), false);
    assertEQ(MidiNote(MidiNote::C + 1).isBlackKey(), true);
    assertEQ(MidiNote(MidiNote::D).isBlackKey(), false);
    assertEQ(MidiNote(MidiNote::D + 1).isBlackKey(), true);
    assertEQ(MidiNote(MidiNote::E).isBlackKey(), false);
    assertEQ(MidiNote(MidiNote::F).isBlackKey(), false);
    assertEQ(MidiNote(MidiNote::F + 1).isBlackKey(), true);
    assertEQ(MidiNote(MidiNote::G).isBlackKey(), false);
    assertEQ(MidiNote(MidiNote::G + 1).isBlackKey(), true);
    assertEQ(MidiNote(MidiNote::A).isBlackKey(), false);
    assertEQ(MidiNote(MidiNote::A + 1).isBlackKey(), true);
    assertEQ(MidiNote(MidiNote::B).isBlackKey(), false);

    assertEQ(MidiNote(MidiNote::C + 24).isBlackKey(), false);
    assertEQ(MidiNote(MidiNote::C + 1 + 48).isBlackKey(), true);
}

void testNotes() {
    testMidiNoteCtor();
    testMidiNoteNorm();
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

    testMidiStaffX();
    testMidiStaffE();
    testMidiStaffG();
    testMidiStaffC();
    testMidiStaffC2();
    testMidiStaffC3();
    testMidiStaffLowA();
    // testMidiBassStaffA();
    testMidiStaffAll(false);
    testMidiStaffAll(true);
    testMidiIsBlackKey();
}

#if 0
void testFirst() {
    //  testScorePitchUtils();
    testMidiIsBlackKey();
}
#endif