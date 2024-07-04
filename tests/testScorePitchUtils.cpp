
#include "ScorePitchUtils.h"
#include "asserts.h"

static void test() {
    Scale sc;
    MidiNote mn(MidiNote::C);
    sc.set(mn, Scale::Scales::Major);
    //     static NotationNote getNotationNote(const Scale&, const MidiNote&, bool bassStaff);
  //  static NotationNote getNotationNoteForce(const Scale&, const MidiNote&, bool bassStaff, bool useSharps);
   ScorePitchUtils::getNotationNote(sc, mn, false);
   ScorePitchUtils::getNotationNoteForce(sc, mn, false, true);
}

static void testCMajor() {
    Scale sc;
    MidiNote mnc(MidiNote::C);
    MidiNote mncSharp(MidiNote::C + 1);
    sc.set(mnc, Scale::Scales::Major);

    auto x = ScorePitchUtils::getNotationNote(sc, mnc, false);
    assert(x._accidental == ScorePitchUtils::Accidental::none);

    // Even when we try to "force" C to use sharps or flats it should.
    x = ScorePitchUtils::getNotationNoteForce(sc, mnc, false, true);
    assert(x._accidental == ScorePitchUtils::Accidental::none);
    x = ScorePitchUtils::getNotationNoteForce(sc, mnc, false, false);
    assert(x._accidental == ScorePitchUtils::Accidental::none);

    // This note is also D-. But C Major defaults to sharps
    x = ScorePitchUtils::getNotationNote(sc, mncSharp, false);
    assert(x._accidental == ScorePitchUtils::Accidental::sharp);

     x = ScorePitchUtils::getNotationNoteForce(sc, mncSharp, false, true);
    assert(x._accidental == ScorePitchUtils::Accidental::sharp);
    x = ScorePitchUtils::getNotationNoteForce(sc, mncSharp, false, false);
    assert(x._accidental == ScorePitchUtils::Accidental::flat);
}

static void testCMinor() {
    Scale sc;
    // MidiNote mnc(MidiNote::C);
    // MidiNote mncSharp(MidiNote::C + 1);
    MidiNote mnEFlat(MidiNote::E - 1);
    MidiNote mnE(MidiNote::E );
    sc.set(mnEFlat, Scale::Scales::Minor);

    // In C minor, E flat notated needs no accidental
  auto x = ScorePitchUtils::getNotationNote(sc, mnEFlat, false);
  assert(x._accidental == ScorePitchUtils::Accidental::none);

  x = ScorePitchUtils::getNotationNote(sc, mnE, false);
  assert(x._accidental == ScorePitchUtils::Accidental::natural);
    
}

#if 0
static void  testCompare() {
    // First the equal cases
    bool b = ScorePitchUtils::compareAccidentals(ScorePitchUtils::Accidental::flat, ScorePitchUtils::Accidental::flat);
    assertEQ(b, true);
     b = ScorePitchUtils::compareAccidentals(ScorePitchUtils::Accidental::sharp, ScorePitchUtils::Accidental::sharp);
    assertEQ(b, true);
     b = ScorePitchUtils::compareAccidentals(ScorePitchUtils::Accidental::natural, ScorePitchUtils::Accidental::natural);
    assertEQ(b, true);
     b = ScorePitchUtils::compareAccidentals(ScorePitchUtils::Accidental::none, ScorePitchUtils::Accidental::none);
    assertEQ(b, true);

    assert(false);

}
#endif
void testScorePitchUtils() {
    test();
    testCMajor();
    testCMinor();
    // testCompare();
}

#if 1
void testFirst() {
    testScorePitchUtils();
   // testCMinor();
  // testCompare();
}
#endif