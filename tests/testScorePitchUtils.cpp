
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

    x = ScorePitchUtils::getNotationNote(sc, mncSharp);
  //  auto y = std::get<1>(x);
    assert(std::get<1>(x) == ScorePitchUtils::Accidental::sharp);
}

void testScorePitchUtils() {
    test();
    testCMajor();
  
}

#if 1
void testFirst() {
  //  testScorePitchUtils();
  testCMajor();
}
#endif