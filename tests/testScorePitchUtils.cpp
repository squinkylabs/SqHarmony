

#include "NotationNote.h"
#include "Scale.h"
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
    MidiNote mnE(MidiNote::E);
    sc.set(mnEFlat, Scale::Scales::Minor);

    // In C minor, E flat notated needs no accidental
    auto x = ScorePitchUtils::getNotationNote(sc, mnEFlat, false);
    assert(x._accidental == NotationNote::Accidental::none);

    x = ScorePitchUtils::getNotationNote(sc, mnE, false);
    assert(x._accidental == NotationNote::Accidental::natural);
}

static void testReSpellCMajorCSharp() {
    MidiNote mn(MidiNote::MiddleC + 1);
    NotationNote nn = NotationNote(mn, NotationNote::Accidental::sharp, -2);
    assert(ScorePitchUtils::validate(nn));
    bool b = ScorePitchUtils::reSpell(nn, false);
    assertEQ(b, true);
    assert(ScorePitchUtils::validate(nn));
    assertEQ(nn._midiNote.get(), MidiNote::MiddleC + 1);
    assert(nn._accidental == NotationNote::Accidental::flat);
    assertEQ(nn._legerLine, -1);
}

static void testReSpellCMajorDFlat() {
    MidiNote mn(MidiNote::MiddleC + 1);
    NotationNote nn = NotationNote(mn, NotationNote::Accidental::flat, -1);
    assert(ScorePitchUtils::validate(nn));
    bool b = ScorePitchUtils::reSpell(nn, true);
    assertEQ(b, true);
    assert(ScorePitchUtils::validate(nn));
    assertEQ(nn._midiNote.get(), MidiNote::MiddleC + 1);
    assert(nn._accidental == NotationNote::Accidental::sharp);
    assertEQ(nn._legerLine, -2);
}

static void testReSpellCMajorCSharpTwice() {
    MidiNote mn(MidiNote::MiddleC + 1);
    NotationNote nn = NotationNote(mn, NotationNote::Accidental::sharp, -2);
  
    bool b = ScorePitchUtils::reSpell(nn, false);
    assertEQ(b, true);
    b = ScorePitchUtils::reSpell(nn, false);
    assertEQ(b, false);  
}

static void testCSharpVariations() {
    MidiNote mn(MidiNote::MiddleC + 1);
    NotationNote nn = NotationNote(mn, NotationNote::Accidental::sharp, -2);
    assertEQ(ScorePitchUtils::validate(nn), true);

    auto variations = ScorePitchUtils::getVariations(nn);
    assertEQ(variations.size(), 2);

    assertEQ(variations[0]._midiNote.get(), MidiNote::MiddleC + 1);
    assertEQ(variations[1]._midiNote.get(), MidiNote::MiddleC + 1);
    assert(variations[0]._accidental == NotationNote::Accidental::sharp);
    assert(variations[1]._accidental == NotationNote::Accidental::flat);

}

static void testValidate() {
    MidiNote mn(MidiNote::MiddleC);
    NotationNote nn = NotationNote(mn, NotationNote::Accidental::none, -2);
    assertEQ(ScorePitchUtils::validate(nn), true);
}

static void testValidate2() {
    MidiNote mn(MidiNote::MiddleC + 4);
    NotationNote nn = NotationNote(mn, NotationNote::Accidental::none, -2);
    assertEQ(ScorePitchUtils::validate(nn), false);
}

//    static void findSpelling( vlenArray<int, 16> inputPitch, vlenArray<NotationNote, 16> outputNotes, bool bassStaff);
static void testFindSpelling() {
    ScorePitchUtils::vlenArray<int, 16> inputPitch;
    ScorePitchUtils::vlenArray<NotationNote, 16> outputNotes;

    Scale scale;
    scale.set(MidiNote(MidiNote::C), Scale::Scales::Major);

    inputPitch._push( MidiNote::MiddleC);
    inputPitch._push( MidiNote::MiddleC + 12);
    ScorePitchUtils::findSpelling(scale, inputPitch, outputNotes, false);

    assertEQ(inputPitch.size(), 2);
    assertEQ(outputNotes.size(), 2);
}

void testScorePitchUtils() {
    test();
    testCMajor();
    testCMinor();

    testValidate();
    testValidate2();

    testReSpellCMajorCSharp();
    testReSpellCMajorDFlat();
    testReSpellCMajorCSharpTwice();
    testCSharpVariations();
}

#if 0
void testFirst() {
   //  testScorePitchUtils();
   // testReSpellCMajorCSharpTwice();
 //  testCSharpVariations();
    testFindSpelling();
}
#endif