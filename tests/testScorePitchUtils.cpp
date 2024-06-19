
#include "ScorePitchUtils.h"
#include "asserts.h"

static void test() {
    // ScorePitchUtils sp;
    Scale sc;
    MidiNote mn;
    ScorePitchUtils::getNotationNote(sc, mn);
}

static void testCMajor() {
    Scale sc;
    MidiNote mnc(MidiNote::C);
    MidiNote mncSharp(MidiNote::C + 1);
    sc.set(mnc, Scale::Scales::Major);

    auto x = ScorePitchUtils::getNotationNote(sc, mnc);
    assert(std::get<1>(x) == ScorePitchUtils::Accidental::none);

    // This note is also D-. I guess this API uses sharps all the time
    x = ScorePitchUtils::getNotationNote(sc, mncSharp);
    assert(std::get<1>(x) == ScorePitchUtils::Accidental::sharp);
}

static void testCMinor() {
    Scale sc;
    // MidiNote mnc(MidiNote::C);
    // MidiNote mncSharp(MidiNote::C + 1);
    MidiNote mnEFlat(MidiNote::E - 1);
    MidiNote mnE(MidiNote::E );
    sc.set(mnEFlat, Scale::Scales::Minor);

    // In C minor, E flat notated needs no accidental
  auto x = ScorePitchUtils::getNotationNote(sc, mnEFlat);
  assert(std::get<1>(x) == ScorePitchUtils::Accidental::none);

  x = ScorePitchUtils::getNotationNote(sc, mnE);
  assert(std::get<1>(x) == ScorePitchUtils::Accidental::natural);
    
  
  assert(false);
}

void testScorePitchUtils() {
    test();
    testCMajor();
    testCMinor();
}

#if 0
void testFirst() {
    //  testScorePitchUtils();
    testCMinor();
}
#endif