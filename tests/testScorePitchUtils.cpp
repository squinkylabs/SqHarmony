

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
    Scale scale;
    MidiNote mn(MidiNote::MiddleC + 1);
    NotationNote nn = NotationNote(mn, NotationNote::Accidental::sharp, -2);
    assert(ScorePitchUtils::validate(nn, scale));
    bool b = ScorePitchUtils::reSpell(nn, false, scale);
    assertEQ(b, true);
    assert(ScorePitchUtils::validate(nn, scale));
    assertEQ(nn._midiNote.get(), MidiNote::MiddleC + 1);
    assert(nn._accidental == NotationNote::Accidental::flat);
    assertEQ(nn._legerLine, -1);
}

static void testReSpellCMajorDFlat() {
    Scale scale(MidiNote(MidiNote::C), Scale::Scales::Major);
    MidiNote mn(MidiNote::MiddleC + 1);
    NotationNote nn = NotationNote(mn, NotationNote::Accidental::flat, -1);
    assert(ScorePitchUtils::validate(nn, scale));
    bool b = ScorePitchUtils::reSpell(nn, true, scale);
    assertEQ(b, true);
    assert(ScorePitchUtils::validate(nn, scale));
    assertEQ(nn._midiNote.get(), MidiNote::MiddleC + 1);
    assert(nn._accidental == NotationNote::Accidental::sharp);
    assertEQ(nn._legerLine, -2);
}

static void testReSpellCMajorCSharpTwice() {
    Scale scale(MidiNote(MidiNote::C), Scale::Scales::Major);
    MidiNote mn(MidiNote::MiddleC + 1);
    NotationNote nn = NotationNote(mn, NotationNote::Accidental::sharp, -2);

    bool b = ScorePitchUtils::reSpell(nn, false, scale);
    assertEQ(b, true);
    b = ScorePitchUtils::reSpell(nn, false, scale);
    assertEQ(b, false);
}

static void testCSharpVariations() {
    Scale scale(MidiNote(MidiNote::C), Scale::Scales::Major);
    MidiNote mn(MidiNote::MiddleC + 1);
    NotationNote nn = NotationNote(mn, NotationNote::Accidental::sharp, -2);
    assertEQ(ScorePitchUtils::validate(nn, scale), true);

    auto variations = ScorePitchUtils::getVariations(nn, scale);
    assertEQ(variations.numValid(), 2);

    assertEQ(variations.getAt(0)._midiNote.get(), MidiNote::MiddleC + 1);
    assertEQ(variations.getAt(1)._midiNote.get(), MidiNote::MiddleC + 1);
    assert(variations.getAt(0)._accidental == NotationNote::Accidental::sharp);
    assert(variations.getAt(1)._accidental == NotationNote::Accidental::flat);

    //  assertEQ(variations.getAt(0).valid(), true);
    assertEQ(ScorePitchUtils::validate(variations.getAt(0), scale), true);
    assertEQ(ScorePitchUtils::validate(variations.getAt(1), scale), true);
}

static void testEVariations() {
    Scale scale(MidiNote(MidiNote::C), Scale::Scales::Major);
    MidiNote mn(MidiNote::MiddleC + MidiNote::E);
    NotationNote nn = NotationNote(mn, NotationNote::Accidental::none, 0);
    assertEQ(ScorePitchUtils::validate(nn, scale), true);

    auto variations = ScorePitchUtils::getVariations(nn, scale);
    assertEQ(variations.numValid(), 2);
    assertEQ(ScorePitchUtils::validate(variations.getAt(0), scale), true);
    assertEQ(ScorePitchUtils::validate(variations.getAt(1), scale), true);

    //  SQINFO("var0 = %s", variations.getAt(0).toString().c_str());
    //  SQINFO("var1 = %s", variations.getAt(1).toString().c_str());

    assertEQ(variations.getAt(0)._midiNote.get(), MidiNote::MiddleC + MidiNote::E);
    assertEQ(variations.getAt(1)._midiNote.get(), MidiNote::MiddleC + MidiNote::E);
    assert(variations.getAt(0)._accidental == NotationNote::Accidental::none);
    assert(variations.getAt(1)._accidental == NotationNote::Accidental::flat);
    assertEQ(variations.getAt(0)._legerLine, 0);
    assertEQ(variations.getAt(1)._legerLine, 1);
}

static void testValidate() {
    Scale scale(MidiNote(MidiNote::C), Scale::Scales::Major);
    MidiNote mn(MidiNote::MiddleC);
    NotationNote nn = NotationNote(mn, NotationNote::Accidental::none, -2);
    assertEQ(ScorePitchUtils::validate(nn, scale), true);
}

static void testValidate2() {
    Scale scale(MidiNote(MidiNote::C), Scale::Scales::Major);
    MidiNote mn(MidiNote::MiddleC + 4);
    NotationNote nn = NotationNote(mn, NotationNote::Accidental::none, -2);
    assertEQ(ScorePitchUtils::validate(nn, scale), false);
}

static void testValidateCMajor() {
    Scale scale(MidiNote(MidiNote::C), Scale::Scales::Major);
    MidiNote mn(MidiNote::MiddleC + MidiNote::E);
    NotationNote nn = NotationNote(mn, NotationNote::Accidental::none, 0);
    assertEQ(ScorePitchUtils::validate(nn, scale), true);

    MidiNote mn2(MidiNote::MiddleC + MidiNote::E - 1);
    NotationNote nn2 = NotationNote(mn2, NotationNote::Accidental::flat, 0);
    assertEQ(ScorePitchUtils::validate(nn2, scale), true);
}

static void testValidateCminor() {
    Scale scale(MidiNote(MidiNote::C), Scale::Scales::Minor);
    MidiNote mn(MidiNote::MiddleC + MidiNote::E);
    NotationNote nn = NotationNote(mn, NotationNote::Accidental::natural, 0);
    assertEQ(ScorePitchUtils::validate(nn, scale), true);

    MidiNote mn2(MidiNote::MiddleC + MidiNote::E - 1);
    NotationNote nn2 = NotationNote(mn2, NotationNote::Accidental::none, 0);
    assertEQ(ScorePitchUtils::validate(nn2, scale), true);
}

//    static void findSpelling( vlenArray<int, 16> inputPitch, vlenArray<NotationNote, 16> outputNotes, bool bassStaff);
static void testFindSpelling() {
    SqArray<int, 16> inputPitch;
    SqArray<NotationNote, 16> outputNotes;

    Scale scale;
    scale.set(MidiNote(MidiNote::C), Scale::Scales::Major);

    inputPitch.putAt(0, MidiNote::MiddleC);
    inputPitch.putAt(1, MidiNote::MiddleC + 12);
    ScorePitchUtils::findSpelling(scale, inputPitch, outputNotes, false);

    assertEQ(inputPitch.numValid(), 2);
    assertEQ(outputNotes.numValid(), 2);
}

static void testFindSpellingCMajor() {
    SqArray<int, 16> inputPitch;
    SqArray<NotationNote, 16> outputNotes;

    Scale scale;
    scale.set(MidiNote(MidiNote::C), Scale::Scales::Major);

    inputPitch.putAt(0, MidiNote::MiddleC);
    inputPitch.putAt(1, MidiNote::MiddleC + MidiNote::E);
    inputPitch.putAt(2, MidiNote::MiddleC + MidiNote::G);

    SQINFO("input 0 = %d", inputPitch.getAt(0));
    SQINFO("input 1 = %d", inputPitch.getAt(1));
    SQINFO("input 2 = %d", inputPitch.getAt(2));
    ScorePitchUtils::findSpelling(scale, inputPitch, outputNotes, false);

    assertEQ(inputPitch.numValid(), 3);
    assertEQ(outputNotes.numValid(), 3);

    SQINFO("0 = %s", outputNotes.getAt(0).toString().c_str());
    SQINFO("1 = %s", outputNotes.getAt(1).toString().c_str());
    SQINFO("2 = %s", outputNotes.getAt(2).toString().c_str());

    NotationNote n = outputNotes.getAt(0);
    assertEQ(n._midiNote.get(), MidiNote::MiddleC);
    assert(n._accidental == NotationNote::Accidental::none);

    n = outputNotes.getAt(1);
    assertEQ(n._midiNote.get(), MidiNote::MiddleC + MidiNote::E);
    assert(n._accidental == NotationNote::Accidental::none);

    n = outputNotes.getAt(2);
    assertEQ(n._midiNote.get(), MidiNote::MiddleC + MidiNote::G);
    assert(n._accidental == NotationNote::Accidental::none);
}

static void testPitchFromLegerTrebleCMajorWhiteKeys() {
    Scale scale(MidiNote(MidiNote::C), Scale::Scales::Major);
    assertEQ(ScorePitchUtils::pitchFromLeger(false, -2, NotationNote::Accidental::none, scale), MidiNote::MiddleC);
    assertEQ(ScorePitchUtils::pitchFromLeger(false, -1, NotationNote::Accidental::none, scale), MidiNote::MiddleC + MidiNote::D);
    assertEQ(ScorePitchUtils::pitchFromLeger(false, 0, NotationNote::Accidental::none, scale), MidiNote::MiddleC + MidiNote::E);
    assertEQ(ScorePitchUtils::pitchFromLeger(false, 1, NotationNote::Accidental::none, scale), MidiNote::MiddleC + MidiNote::F);
    assertEQ(ScorePitchUtils::pitchFromLeger(false, 2, NotationNote::Accidental::none, scale), MidiNote::MiddleC + MidiNote::G);
    assertEQ(ScorePitchUtils::pitchFromLeger(false, 3, NotationNote::Accidental::none, scale), MidiNote::MiddleC + MidiNote::A);
    assertEQ(ScorePitchUtils::pitchFromLeger(false, 4, NotationNote::Accidental::none, scale), MidiNote::MiddleC + MidiNote::B);
    assertEQ(ScorePitchUtils::pitchFromLeger(false, 5, NotationNote::Accidental::none, scale), MidiNote::MiddleC + 12);
}

static void testPitchFromLegerTrebleCMajorAccidentals() {
    Scale scale(MidiNote(MidiNote::C), Scale::Scales::Major);
    assertEQ(ScorePitchUtils::pitchFromLeger(false, -2, NotationNote::Accidental::none, scale), MidiNote::MiddleC);
    assertEQ(ScorePitchUtils::pitchFromLeger(false, -2, NotationNote::Accidental::sharp, scale), MidiNote::MiddleC + 1);
    assertEQ(ScorePitchUtils::pitchFromLeger(false, -2, NotationNote::Accidental::flat, scale), MidiNote::MiddleC - 1);
}

static void testPitchFromLegerTrebleCMajorNatural() {
    Scale scale(MidiNote(MidiNote::C), Scale::Scales::Major);
    // This case is a little silly/illegal, but whatever...
    assertEQ(ScorePitchUtils::pitchFromLeger(false, -2, NotationNote::Accidental::none, scale),
             ScorePitchUtils::pitchFromLeger(false, -2, NotationNote::Accidental::natural, scale));
}

static void testPitchFromLegerCminor() {
    Scale scale(MidiNote(MidiNote::C), Scale::Scales::Minor);
    assertEQ(ScorePitchUtils::pitchFromLeger(false, -2, NotationNote::Accidental::none, scale), MidiNote::MiddleC);
    // E flat is in the scale
    assertEQ(ScorePitchUtils::pitchFromLeger(false, 0, NotationNote::Accidental::none, scale), MidiNote::MiddleC + MidiNote::E - 1);
    // E natural is not in the scale
    assertEQ(ScorePitchUtils::pitchFromLeger(false, 0, NotationNote::Accidental::natural, scale), MidiNote::MiddleC + MidiNote::E);
}

static void testGetAjustmentForLeger() {
    {
        Scale scale(MidiNote(MidiNote::C), Scale::Scales::Major);
        const int x = ScorePitchUtils::_getAjustmentForLeger(scale, false, -2);
        assertEQ(x, 0);
        assertEQ(ScorePitchUtils::_getAjustmentForLeger(scale, false, -1), 0);
        assertEQ(ScorePitchUtils::_getAjustmentForLeger(scale, false, 0), 0);
        assertEQ(ScorePitchUtils::_getAjustmentForLeger(scale, false, 1), 0);
        assertEQ(ScorePitchUtils::_getAjustmentForLeger(scale, false, 2), 0);
        assertEQ(ScorePitchUtils::_getAjustmentForLeger(scale, false, 3), 0);
        assertEQ(ScorePitchUtils::_getAjustmentForLeger(scale, false, 4), 0);
        assertEQ(ScorePitchUtils::_getAjustmentForLeger(scale, false, 5), 0);
        assertEQ(ScorePitchUtils::_getAjustmentForLeger(scale, false, 6), 0);
        assertEQ(ScorePitchUtils::_getAjustmentForLeger(scale, false, 7), 0);
        assertEQ(ScorePitchUtils::_getAjustmentForLeger(scale, false, 8), 0);
    }
    {
        Scale scale(MidiNote(MidiNote::C), Scale::Scales::Minor);
        assertEQ(ScorePitchUtils::_getAjustmentForLeger(scale, false, -2), 0);
        assertEQ(ScorePitchUtils::_getAjustmentForLeger(scale, false, -1), 0);
        assertEQ(ScorePitchUtils::_getAjustmentForLeger(scale, false, 0), -1);
        assertEQ(ScorePitchUtils::_getAjustmentForLeger(scale, false, 1), 0);
        assertEQ(ScorePitchUtils::_getAjustmentForLeger(scale, false, 2), 0);
        assertEQ(ScorePitchUtils::_getAjustmentForLeger(scale, false, 3), -1);
        assertEQ(ScorePitchUtils::_getAjustmentForLeger(scale, false, 4), -1);
        assertEQ(ScorePitchUtils::_getAjustmentForLeger(scale, false, 5), 0);
        assertEQ(ScorePitchUtils::_getAjustmentForLeger(scale, false, 6), 0);
        assertEQ(ScorePitchUtils::_getAjustmentForLeger(scale, false, 7), 0);
        assertEQ(ScorePitchUtils::_getAjustmentForLeger(scale, false, 8), 0);
    }
}

void testScorePitchUtils() {
    test();
    testGetAjustmentForLeger();
    testPitchFromLegerTrebleCMajorWhiteKeys();
    testPitchFromLegerTrebleCMajorAccidentals();
    testPitchFromLegerTrebleCMajorNatural();
    testPitchFromLegerCminor();

    testCMajor();
    testCMinor();

    testValidate();
    testValidate2();
    testValidateCMajor();
    testValidateCminor();

    testReSpellCMajorCSharp();
    testReSpellCMajorDFlat();
    testReSpellCMajorCSharpTwice();
    testCSharpVariations();
    testEVariations();

    testFindSpelling();
}

#if 1
void testFirst() {
    testScorePitchUtils();
    // testPitchFromLegerCminor();
}
#endif