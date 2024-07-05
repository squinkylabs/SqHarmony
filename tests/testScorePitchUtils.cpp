
#include "ScorePitchUtils.h"
#include "asserts.h"

static void test() {
    Scale sc;
    MidiNote mn(MidiNote::C);
    sc.set(mn, Scale::Scales::Major);
    ScorePitchUtils::getNotationNote(sc, mn, false);
    ScorePitchUtils::getNotationNoteForce(sc, mn, false, true);
}

static void testCMajorC() {
    Scale sc;
    MidiNote mnc(MidiNote::C3 + 12);
    sc.set(MidiNote(MidiNote::C), Scale::Scales::Major);

    // C should default to drawing as C natural
    auto x = ScorePitchUtils::getNotationNote(sc, mnc, false);
    assert(x._accidental == ScorePitchUtils::Accidental::none);
    assertEQ(x._legerLine, -2);
    assert(x._scaleNote.getAdjustment() == ScaleNote::RelativeAdjustment::none);

    //-----------------------------------
    // Even when we try to "force" C to use sharps or flats it should stay natural
    x = ScorePitchUtils::getNotationNoteForce(sc, mnc, false, true);
    assert(x._accidental == ScorePitchUtils::Accidental::none);
    assert(x._accidental == ScorePitchUtils::Accidental::none);
    assertEQ(x._legerLine, -2);
    assert(x._scaleNote.getAdjustment() == ScaleNote::RelativeAdjustment::none);

    x = ScorePitchUtils::getNotationNoteForce(sc, mnc, false, false);
    assert(x._accidental == ScorePitchUtils::Accidental::none);
    assert(x._accidental == ScorePitchUtils::Accidental::none);
    assertEQ(x._legerLine, -2);
    assert(x._scaleNote.getAdjustment() == ScaleNote::RelativeAdjustment::none);
}

static void testCMajorCSharp() {
    Scale sc;
    MidiNote mncSharp(MidiNote::C3 + 12 + 1);
    sc.set(MidiNote(MidiNote::C), Scale::Scales::Major);

    // This note is also D-. But C Major defaults to sharps
    auto x = ScorePitchUtils::getNotationNote(sc, mncSharp, false);
    assert(x._accidental == ScorePitchUtils::Accidental::sharp);
    assertEQ(x._legerLine, -2);
    assert(x._scaleNote.getAdjustment() == ScaleNote::RelativeAdjustment::sharp);

    x = ScorePitchUtils::getNotationNoteForce(sc, mncSharp, false, true);
    assert(x._accidental == ScorePitchUtils::Accidental::sharp);

    x = ScorePitchUtils::getNotationNoteForce(sc, mncSharp, false, false);
    assert(x._accidental == ScorePitchUtils::Accidental::flat);
    assertEQ(x._legerLine, -1);
    assert(x._scaleNote.getAdjustment() == ScaleNote::RelativeAdjustment::flat);
}

static void testCMinor() {
    Scale sc;
    MidiNote mnEFlat(MidiNote::E - 1);
    MidiNote mnE(MidiNote::E);
    sc.set(mnEFlat, Scale::Scales::Minor);

    // In C minor, E flat notated needs no accidental
    auto x = ScorePitchUtils::getNotationNote(sc, mnEFlat, false);
    assert(x._accidental == ScorePitchUtils::Accidental::none);

    x = ScorePitchUtils::getNotationNote(sc, mnE, false);
    assert(x._accidental == ScorePitchUtils::Accidental::natural);
}

void testScorePitchUtils() {
    test();
    testCMajorC();
    testCMajorCSharp();
    testCMinor();
}

#if 0
void testFirst() {
    testScorePitchUtils();
}
#endif