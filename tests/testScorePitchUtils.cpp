
#include "ScorePitchUtils.h"
#include "asserts.h"

static void test() {
    // ScorePitchUtils sp;
    Scale sc;
    MidiNote mn;
   // ScorePitchUtils::getNotationNote(sc, mn);
}

static void testCMajor() {
    Scale sc;
    MidiNote mnc(MidiNote::C);
    MidiNote mncSharp(MidiNote::C + 1);
    sc.set(mnc, Scale::Scales::Major);

    auto x = ScorePitchUtils::getNotationNote(sc, mnc, false);
    assert(x._accidental == NotationNote::Accidental::none);

    // This note is also D-. I guess this API uses sharps all the time
    x = ScorePitchUtils::getNotationNote(sc, mncSharp, false);
    assert(x._accidental == NotationNote::Accidental::sharp);
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
  assert(x._accidental == NotationNote::Accidental::none);

  x = ScorePitchUtils::getNotationNote(sc, mnE, false);
  assert(x._accidental == NotationNote::Accidental::natural);
    
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

#if 0
void testFirst() {
    //  testScorePitchUtils();
   // testCMinor();
  // testCompare();
}
#endif