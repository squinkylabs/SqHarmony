

#include "ChordRecognizer.h"
#include "NotationNote.h"
#include "Scale.h"
#include "ScorePitchUtils.h"
#include "SharpsFlatsPref.h"
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

static void testNoteEFlatInEFlatMinor() {
    SQINFO("---- testNoteEFlatInEFlatMinor ---");
    Scale sc;
    MidiNote mnEFlat3(MidiNote::MiddleC + MidiNote::E - 1);
    MidiNote mnEFlat(MidiNote::E - 1);

    // note that this is strange key! The key of E flat (D sharp) minor.
    // I think originally this test was supposed to be in C minor...
    // I think the test passes in E flat major!
    sc.set(mnEFlat, Scale::Scales::Minor);
    SQINFO("set keysig root to %d (+72 = %d)", mnEFlat.get(), mnEFlat.get() + 72);

    // In E flat (minor), E flat notated needs no accidental
    auto x = ScorePitchUtils::getNotationNote(sc, mnEFlat3, false);
    assert(x._accidental == NotationNote::Accidental::none);
    assertEQ(ScorePitchUtils::validate(x, sc), true);
}

static void testNoteEFlatInEFlatMajor() {
    SQINFO("---- testNoteEFlatInEFlatMajor ---");
    Scale sc;
    MidiNote mnEFlat3(MidiNote::MiddleC + MidiNote::E - 1);
    MidiNote mnEFlat(MidiNote::E - 1);

    sc.set(mnEFlat, Scale::Scales::Major);

    // In E flat (minor), E flat notated needs no accidental
    auto x = ScorePitchUtils::getNotationNote(sc, mnEFlat3, false);
    assert(x._accidental == NotationNote::Accidental::none);
    assertEQ(ScorePitchUtils::validate(x, sc), true);
}

static void testCMinor() {
    Scale sc;
    MidiNote mnEFlat(MidiNote::E - 1);
    MidiNote mnE(MidiNote::E);
    sc.set(mnEFlat, Scale::Scales::Minor);  // Is this a mistake that we did the test in this key?

    // In C minor, E flat notated needs no accidental
    auto x = ScorePitchUtils::getNotationNote(sc, mnEFlat, false);
    assert(x._accidental == NotationNote::Accidental::none);

    x = ScorePitchUtils::getNotationNote(sc, mnE, false);
    assert(x._accidental == NotationNote::Accidental::natural);
}

static void testReSpellCMajorCSharp() {
    Scale scale(MidiNote(MidiNote::C), Scale::Scales::Major);
    MidiNote mn(MidiNote::MiddleC + 1);
    NotationNote nn = NotationNote(mn, NotationNote::Accidental::sharp, -2, false);
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
    NotationNote nn = NotationNote(mn, NotationNote::Accidental::flat, -1, false);
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
    NotationNote nn = NotationNote(mn, NotationNote::Accidental::sharp, -2, false);

    bool b = ScorePitchUtils::reSpell(nn, false, scale);
    assertEQ(b, true);
    b = ScorePitchUtils::reSpell(nn, false, scale);
    assertEQ(b, false);
}

static void testCSharpVariations() {
    Scale scale(MidiNote(MidiNote::C), Scale::Scales::Major);
    MidiNote mn(MidiNote::MiddleC + 1);
    NotationNote nn = NotationNote(mn, NotationNote::Accidental::sharp, -2, false);
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
    NotationNote nn = NotationNote(mn, NotationNote::Accidental::none, 0, false);
    assertEQ(ScorePitchUtils::validate(nn, scale), true);

    // SQINFO("---- about to get variation in CMajor for %s", nn.toString().c_str());

    auto variations = ScorePitchUtils::getVariations(nn, scale);
    assertEQ(variations.numValid(), 2);
    assertEQ(ScorePitchUtils::validate(variations.getAt(0), scale), true);
    assertEQ(ScorePitchUtils::validate(variations.getAt(1), scale), true);

    assertEQ(variations.getAt(0)._midiNote.get(), MidiNote::MiddleC + MidiNote::E);
    assertEQ(variations.getAt(1)._midiNote.get(), MidiNote::MiddleC + MidiNote::E);
    assert(variations.getAt(0)._accidental == NotationNote::Accidental::none);
    assert(variations.getAt(1)._accidental == NotationNote::Accidental::flat);
    assertEQ(variations.getAt(0)._legerLine, 0);
    assertEQ(variations.getAt(1)._legerLine, 1);
}

static void testFVariationsInG() {
    Scale scale(MidiNote(MidiNote::G), Scale::Scales::Major);
    NotationNote nn = NotationNote(MidiNote(MidiNote::MiddleC + MidiNote::F), NotationNote::Accidental::natural, 1, false);
    // should give E# and F
    auto variations = ScorePitchUtils::getVariations(nn, scale);
    assertEQ(variations.numValid(), 2);
}

static void testValidate() {
    // SQINFO("---- testValidate()");

    // All tests in C Major
    Scale scale(MidiNote(MidiNote::C), Scale::Scales::Major);

    // Valid middle C
    MidiNote mn(MidiNote::MiddleC);
    NotationNote nn = NotationNote(mn, NotationNote::Accidental::none, -2, false);
    assertEQ(ScorePitchUtils::validate(nn, scale), true);

    // This is invalid, becuase C does not need a natural in C
    nn = NotationNote(mn, NotationNote::Accidental::natural, -2, false);
    assertEQ(ScorePitchUtils::validate(nn, scale), false);

    MidiNote mn2(MidiNote::C);
    int octavesDown = MidiNote::MiddleC / 12;
    int ll = -2 - octavesDown * int(NotationNote::llInOctave);
    nn = NotationNote(mn2, NotationNote::Accidental::none, ll, false);

    assertEQ(ScorePitchUtils::validate(nn, scale), true);

    MidiNote mn3(MidiNote::C - 12);
    octavesDown = 1 + MidiNote::MiddleC / 12;
    ll = -2 - octavesDown * int(NotationNote::llInOctave);
    nn = NotationNote(mn3, NotationNote::Accidental::none, ll, false);
}

static void testValidate2() {
    Scale scale(MidiNote(MidiNote::C), Scale::Scales::Major);
    MidiNote mn(MidiNote::MiddleC + 4);
    NotationNote nn = NotationNote(mn, NotationNote::Accidental::none, -2, false);
    assertEQ(ScorePitchUtils::validate(nn, scale), false);
}

static void testValidateCMajor() {
    Scale scale(MidiNote(MidiNote::C), Scale::Scales::Major);
    MidiNote mnE(MidiNote::MiddleC + MidiNote::E);
    NotationNote nn = NotationNote(mnE, NotationNote::Accidental::none, 0, false);
    assertEQ(ScorePitchUtils::validate(nn, scale), true);

    MidiNote mnEFlat(MidiNote::MiddleC + MidiNote::E - 1);
    nn = NotationNote(mnEFlat, NotationNote::Accidental::flat, 0, false);
    assertEQ(ScorePitchUtils::validate(nn, scale), true);

    // F Flat should be a valid E
    nn = NotationNote(mnE, NotationNote::Accidental::flat, 1, false);
    assertEQ(ScorePitchUtils::validate(nn, scale), true);
}

static void testValidateCminor() {
    Scale scale(MidiNote(MidiNote::C), Scale::Scales::Minor);
    MidiNote mn(MidiNote::MiddleC + MidiNote::E);
    NotationNote nn = NotationNote(mn, NotationNote::Accidental::natural, 0, false);
    assertEQ(ScorePitchUtils::validate(nn, scale), true);

    nn = NotationNote(mn, NotationNote::Accidental::none, 0, false);
    assertEQ(ScorePitchUtils::validate(nn, scale), false);

    MidiNote mn2(MidiNote::MiddleC + MidiNote::E - 1);
    NotationNote nn2 = NotationNote(mn2, NotationNote::Accidental::none, 0, false);
    assertEQ(ScorePitchUtils::validate(nn2, scale), true);

    nn2 = NotationNote(mn2, NotationNote::Accidental::natural, 0, false);
    assertEQ(ScorePitchUtils::validate(nn2, scale), false);
}

static void testValidateGMajor() {
    // Want F is E Sharp in G
    Scale scale(MidiNote(MidiNote::G), Scale::Scales::Major);
    MidiNote mn(MidiNote::MiddleC + MidiNote::F);

    // E sharp should be valid in G Major
    NotationNote nn = NotationNote(mn, NotationNote::Accidental::sharp, 0, false);
    assertEQ(ScorePitchUtils::validate(nn, scale), true);

    // F natural should be valid in G major
    nn = NotationNote(mn, NotationNote::Accidental::natural, 1, false);
    assertEQ(ScorePitchUtils::validate(nn, scale), true);

    // F (nothing def to sharp) should be valid
    MidiNote mnFS(MidiNote::MiddleC + MidiNote::F + 1);
    nn = NotationNote(mnFS, NotationNote::Accidental::none, 1, false);
    assertEQ(ScorePitchUtils::validate(nn, scale), true);
}

//    static void findSpelling( vlenArray<int, 16> inputPitch, vlenArray<NotationNote, 16> outputNotes, bool bassStaff);
static void testFindSpelling() {
    SqArray<int, 16> inputPitch;
    // SqArray<NotationNote, 16> outputNotes;

    Scale scale;
    scale.set(MidiNote(MidiNote::C), Scale::Scales::Major);

    inputPitch.putAt(0, MidiNote::MiddleC);
    inputPitch.putAt(1, MidiNote::MiddleC + 12);
    const auto results = ScorePitchUtils::findSpelling(scale, inputPitch, false, UIPrefSharpsFlats::DefaultPlusSharps);

    assertEQ(inputPitch.numValid(), 2);
    assertEQ(results.notes.numValid(), 2);
}

static void testFindSpelling(
    const SqArray<NotationNote, 16>& expectedOutputNotes,
    const Scale& scale,
    const SqArray<int, 16>& inputPitches,
    bool bassStaff,
    UIPrefSharpsFlats prefs = UIPrefSharpsFlats::DefaultPlusSharps) {
    assertGT(inputPitches.numValid(), 0);
    const auto result = ScorePitchUtils::findSpelling(scale, inputPitches, bassStaff, prefs);

    SQINFO("246 final score = %d", result.score);
#if 1
    if (inputPitches.numValid() == 4) {
        SQINFO("input notes = %d | %d | %d | %d",
               inputPitches.getAt(0),
               inputPitches.getAt(1),
               inputPitches.getAt(2),
               inputPitches.getAt(3));

        SQINFO("output notes = <%s> | <%s> | <%s> | <%s>",
               result.notes.getAt(0).toString().c_str(),
               result.notes.getAt(1).toString().c_str(),
               result.notes.getAt(2).toString().c_str(),
               result.notes.getAt(3).toString().c_str());
    } else {
        SQINFO("can't print input");
    }
#endif

    for (unsigned i = 0; i < inputPitches.numValid(); ++i) {
        // ChordRecognizer::_show("intput notes", inputPitches);
        // SQINFO("input[%d]=%d", i, inputPitches.getAt(i));
    }

    for (unsigned i = 0; i < result.notes.numValid(); ++i) {
        // SQINFO("output[%d]=%s", i, result.notes.getAt(i).toString().c_str());
    }

    for (unsigned i = 0; i < inputPitches.numValid(); ++i) {
        const NotationNote n = result.notes.getAt(i);
        const NotationNote expected = expectedOutputNotes.getAt(i);
        if (n != expected) {
            SQINFO("in eval, n=%s, expected=%s", n.toString().c_str(), expected.toString().c_str());
        }
        assert(n == expected);
    }
}

static void testFindSpellingCMajor() {
    // SQINFO("--- testFindSpellingCMajor");
    Scale scale(MidiNote(MidiNote::C), Scale::Scales::Major);

    // TODO: make this work
    // SqArray<int, 16> inputPitch = {MidiNote::MiddleC};
    SqArray<int, 16> inputPitches;
    inputPitches.putAt(0, MidiNote::MiddleC);
    inputPitches.putAt(1, MidiNote::MiddleC + MidiNote::E);
    inputPitches.putAt(2, MidiNote::MiddleC + MidiNote::G);

    SqArray<NotationNote, 16> expectedOutputNotes;

    expectedOutputNotes.putAt(0, NotationNote(MidiNote(MidiNote::MiddleC), NotationNote::Accidental::none, -2, false));
    expectedOutputNotes.putAt(1, NotationNote(MidiNote(MidiNote::MiddleC + MidiNote::E), NotationNote::Accidental::none, 0, false));
    expectedOutputNotes.putAt(2, NotationNote(MidiNote(MidiNote::MiddleC + MidiNote::G), NotationNote::Accidental::none, 2, false));

    testFindSpelling(expectedOutputNotes, scale, inputPitches, false);
}

static void testFindSpellingDFlatMajorInCMajor() {
    // SQINFO("--- testFindSpellingDFlat");
    Scale scale(MidiNote(MidiNote::C), Scale::Scales::Major);

    // C# major
    SqArray<int, 16> inputPitches = {
        MidiNote::MiddleC + 1,
        MidiNote::MiddleC + MidiNote::E + 1,
        MidiNote::MiddleC + MidiNote::G + 1};

    SqArray<NotationNote, 16> expectedOutputNotes;

    // want it spelled at D flat
    expectedOutputNotes.putAt(0, NotationNote(MidiNote(MidiNote::MiddleC + MidiNote::D - 1), NotationNote::Accidental::flat, -1, false));
    expectedOutputNotes.putAt(1, NotationNote(MidiNote(MidiNote::MiddleC + MidiNote::F), NotationNote::Accidental::none, 1, false));
    expectedOutputNotes.putAt(2, NotationNote(MidiNote(MidiNote::MiddleC + MidiNote::A - 1), NotationNote::Accidental::flat, 3, false));

    testFindSpelling(expectedOutputNotes, scale, inputPitches, false);
}

static void testFindSpellingEMajorInCMajor() {
    // SQINFO("--- testFindSpellingE");
    Scale scale(MidiNote(MidiNote::C), Scale::Scales::Major);

    // E major: E, G#, B
    SqArray<int, 16> inputPitches = {
        MidiNote::MiddleC + MidiNote::E,
        MidiNote::MiddleC + MidiNote::G + 1,
        MidiNote::MiddleC + MidiNote::B};

    SqArray<NotationNote, 16> expectedOutputNotes;

    // want it spelled at D flat
    expectedOutputNotes.putAt(0, NotationNote(MidiNote(MidiNote::MiddleC + MidiNote::E), NotationNote::Accidental::none, 0, false));
    expectedOutputNotes.putAt(1, NotationNote(MidiNote(MidiNote::MiddleC + MidiNote::G + 1), NotationNote::Accidental::sharp, 2, false));
    expectedOutputNotes.putAt(2, NotationNote(MidiNote(MidiNote::MiddleC + MidiNote::B), NotationNote::Accidental::none, 4, false));

    testFindSpelling(expectedOutputNotes, scale, inputPitches, false);
}

static void testFindSpellingCminorInCMajor() {
    // SQINFO("--- testFindSpellingCminorInCMajor");
    Scale scale(MidiNote(MidiNote::C), Scale::Scales::Major);

    // TODO: make this work
    // SqArray<int, 16> inputPitch = {MidiNote::MiddleC};
    SqArray<int, 16> inputPitches;
    inputPitches.putAt(0, MidiNote::MiddleC);
    inputPitches.putAt(1, MidiNote::MiddleC + MidiNote::E - 1);
    inputPitches.putAt(2, MidiNote::MiddleC + MidiNote::G);

    SqArray<NotationNote, 16> expectedOutputNotes;

    expectedOutputNotes.putAt(0, NotationNote(MidiNote(MidiNote::MiddleC), NotationNote::Accidental::none, -2, false));
    expectedOutputNotes.putAt(1, NotationNote(MidiNote(MidiNote::MiddleC + MidiNote::E - 1), NotationNote::Accidental::flat, 0, false));
    expectedOutputNotes.putAt(2, NotationNote(MidiNote(MidiNote::MiddleC + MidiNote::G), NotationNote::Accidental::none, 2, false));

    testFindSpelling(expectedOutputNotes, scale, inputPitches, false);
}

static void testFindSpellingCminorFirstInversionInCMajor() {
    //  SQINFO("--- testFindSpellingCminorFirstInversionInCMajor");
    Scale scale(MidiNote(MidiNote::C), Scale::Scales::Major);

    const int lowEFlat = MidiNote::MiddleC + MidiNote::E - 1 - 12;
    const int expectedLegerLineLowEFlat = 0 - NotationNote::llInOctave;

    SqArray<int, 16> inputPitches;
    inputPitches.putAt(0, MidiNote::MiddleC);
    inputPitches.putAt(1, lowEFlat);
    inputPitches.putAt(2, MidiNote::MiddleC + MidiNote::G);
    // for (unsigned i = 0; i < inputPitches.numValid(); ++i) {
    //     SQINFO("input note[%d] = %d", i, inputPitches.getAt(i));
    // }

    SqArray<NotationNote, 16> expectedOutputNotes;
    expectedOutputNotes.putAt(0, NotationNote(MidiNote(MidiNote::MiddleC), NotationNote::Accidental::none, -2, false));
    expectedOutputNotes.putAt(1, NotationNote(MidiNote(lowEFlat), NotationNote::Accidental::flat, expectedLegerLineLowEFlat, false));
    expectedOutputNotes.putAt(2, NotationNote(MidiNote(MidiNote::MiddleC + MidiNote::G), NotationNote::Accidental::none, 2, false));

    testFindSpelling(expectedOutputNotes, scale, inputPitches, false);
}

static void testFindSpellingCminorFirstInversionInCMajorDupesEtc() {
    // SQINFO("--- testFindSpellingCminorFirstInversionInCMajor");
    Scale scale(MidiNote(MidiNote::C), Scale::Scales::Major);

    const int lowEFlat = MidiNote::MiddleC + MidiNote::E - 1 - 12;
    const int expectedLegerLineLowEFlat = 0 - NotationNote::llInOctave;

    SqArray<int, 16> inputPitches;
    inputPitches.putAt(0, MidiNote::MiddleC);
    inputPitches.putAt(1, MidiNote::MiddleC);
    inputPitches.putAt(2, MidiNote::MiddleC);

    inputPitches.putAt(3, MidiNote::MiddleC + MidiNote::G);
    inputPitches.putAt(4, lowEFlat);
    inputPitches.putAt(5, MidiNote::MiddleC);

    // for (unsigned i = 0; i < inputPitches.numValid(); ++i) {
    //     SQINFO("input note[%d] = %d", i, inputPitches.getAt(i));
    // }

    SqArray<NotationNote, 16> expectedOutputNotes;
    expectedOutputNotes.putAt(0, NotationNote(MidiNote(MidiNote::MiddleC), NotationNote::Accidental::none, -2, false));
    expectedOutputNotes.putAt(1, NotationNote(MidiNote(MidiNote::MiddleC), NotationNote::Accidental::none, -2, false));
    expectedOutputNotes.putAt(2, NotationNote(MidiNote(MidiNote::MiddleC), NotationNote::Accidental::none, -2, false));
    expectedOutputNotes.putAt(3, NotationNote(MidiNote(MidiNote::MiddleC + MidiNote::G), NotationNote::Accidental::none, 2, false));
    expectedOutputNotes.putAt(4, NotationNote(MidiNote(lowEFlat), NotationNote::Accidental::flat, expectedLegerLineLowEFlat, false));
    expectedOutputNotes.putAt(5, NotationNote(MidiNote(MidiNote::MiddleC), NotationNote::Accidental::none, -2, false));

    testFindSpelling(expectedOutputNotes, scale, inputPitches, false);
}

static void testFindSpelling9thNo5() {
    Scale scale(MidiNote(MidiNote::C), Scale::Scales::Major);

    SqArray<int, 16> inputPitches;
    inputPitches.putAt(0, MidiNote::MiddleC);
    inputPitches.putAt(1, MidiNote::MiddleC + MidiNote::E);
    inputPitches.putAt(2, MidiNote::MiddleC + MidiNote::B - 1);
    inputPitches.putAt(3, MidiNote::MiddleC + MidiNote::D);

    SqArray<NotationNote, 16> expectedOutputNotes;
    expectedOutputNotes.putAt(0, NotationNote(MidiNote(MidiNote::MiddleC), NotationNote::Accidental::none, -2, false));
    expectedOutputNotes.putAt(1, NotationNote(MidiNote(MidiNote::MiddleC + MidiNote::E), NotationNote::Accidental::none, 0, false));
    expectedOutputNotes.putAt(2, NotationNote(MidiNote(MidiNote::MiddleC + MidiNote::B - 1), NotationNote::Accidental::flat, 4, false));
    expectedOutputNotes.putAt(3, NotationNote(MidiNote(MidiNote::MiddleC + MidiNote::D), NotationNote::Accidental::none, 6, false));

    testFindSpelling(expectedOutputNotes, scale, inputPitches, false);
}

static void testFindSpellingOneNoteCSharpInCMajor(bool sharp) {
    Scale scale(MidiNote(MidiNote::C), Scale::Scales::Major);

    SqArray<int, 16> inputPitches;
    const int cSharp = MidiNote::MiddleC + 1;
    inputPitches.putAt(0, cSharp);

    SqArray<NotationNote, 16> expectedOutputNotes;
    UIPrefSharpsFlats prefs;
    if (sharp) {
        expectedOutputNotes.putAt(0, NotationNote(MidiNote(cSharp), NotationNote::Accidental::sharp, -2, false));
        prefs = UIPrefSharpsFlats::Sharps;
    } else {
        expectedOutputNotes.putAt(0, NotationNote(MidiNote(cSharp), NotationNote::Accidental::flat, -1, false));
        prefs = UIPrefSharpsFlats::Flats;
    }
    SQINFO("will call find with prefs");
    testFindSpelling(expectedOutputNotes, scale, inputPitches, false, prefs);
}

static void testFindSpellingOneNoteCSharpInCMajor() {
    testFindSpellingOneNoteCSharpInCMajor(true);
}

static void testFindSpellingOneNoteDFlatSharpInCMajor() {
    testFindSpellingOneNoteCSharpInCMajor(false);
}

#if 0
static void testFindSpellingCMajor() {
    SQINFO("--- testFindSpellingCMajor");
    SqArray<int, 16> inputPitch;
    SqArray<NotationNote, 16> outputNotes;

    Scale scale;
    scale.set(MidiNote(MidiNote::C), Scale::Scales::Major);

    inputPitch.putAt(0, MidiNote::MiddleC);
    inputPitch.putAt(1, MidiNote::MiddleC + MidiNote::E);
    inputPitch.putAt(2, MidiNote::MiddleC + MidiNote::G);

    ScorePitchUtils::findSpelling(scale, inputPitch, outputNotes, false);

    assertEQ(inputPitch.numValid(), 3);
    assertEQ(outputNotes.numValid(), 3);

    SQINFO("input notes = %d | %d | %d",
           inputPitch.getAt(0),
           inputPitch.getAt(1),
           inputPitch.getAt(2));

    SQINFO("output notes = %s | %s | %s",
           outputNotes.getAt(0).toString().c_str(),
           outputNotes.getAt(1).toString().c_str(),
           outputNotes.getAt(2).toString().c_str());

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
#endif

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

    assertEQ(ScorePitchUtils::pitchFromLeger(false, 5 + NotationNote::llInOctave, NotationNote::Accidental::none, scale), MidiNote::MiddleC + 2 * 12);
    assertEQ(ScorePitchUtils::pitchFromLeger(false, 5 - 2 * NotationNote::llInOctave, NotationNote::Accidental::none, scale), MidiNote::MiddleC - 12);
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

static void testPitchFromLegerEFlatMinor() {
    Scale scale(MidiNote(MidiNote::E - 1), Scale::Scales::Minor);

    // It's really going to be D# minor with our current preferences
    //  assert(scale.getSharpsFlatsPrefResolved() == ResolvedSharpsFlatsPref::Sharps);
    assertEQ(ScorePitchUtils::pitchFromLeger(false, 0, NotationNote::Accidental::none, scale), MidiNote::MiddleC + MidiNote::E + 1);
}

static void testGetAjustmentForLeger() {
#if 1
    {
        // CMajor - all white keys.
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
        // C Minor, root is white key, but some accidentals. (flats)
        // Will be affected by current pref
        //    assert(AccidentalResolver::getPref() == ResolvedSharpsFlatsPref::Sharps);
        Scale scale(MidiNote(MidiNote::C), Scale::Scales::Minor);
        //   assert(scale.getSharpsFlatsPrefResolved() == ResolvedSharpsFlatsPref::Flats);
        assertEQ(ScorePitchUtils::_getAjustmentForLeger(scale, false, -2), 0);
        assertEQ(ScorePitchUtils::_getAjustmentForLeger(scale, false, -1), 0);
        assertEQ(ScorePitchUtils::_getAjustmentForLeger(scale, false, 0), -1);
        assertEQ(ScorePitchUtils::_getAjustmentForLeger(scale, false, 1), 0);
        assertEQ(ScorePitchUtils::_getAjustmentForLeger(scale, false, 2), 0);
        assertEQ(ScorePitchUtils::_getAjustmentForLeger(scale, false, 3), -1);
        assertEQ(ScorePitchUtils::_getAjustmentForLeger(scale, false, 4), -1);
        assertEQ(ScorePitchUtils::_getAjustmentForLeger(scale, false, 5), 0);
        assertEQ(ScorePitchUtils::_getAjustmentForLeger(scale, false, 6), 0);
        assertEQ(ScorePitchUtils::_getAjustmentForLeger(scale, false, 7), -1);
        assertEQ(ScorePitchUtils::_getAjustmentForLeger(scale, false, 8), 0);
    }
#endif
    {
        // D Major, some sharps
        Scale scale(MidiNote(MidiNote::D), Scale::Scales::Major);
        // assert(scale.getSharpsFlatsPrefResolved() == ResolvedSharpsFlatsPref::Sharps);
        assertEQ(ScorePitchUtils::_getAjustmentForLeger(scale, false, -2), 1);  // C#
        assertEQ(ScorePitchUtils::_getAjustmentForLeger(scale, false, -1), 0);
        assertEQ(ScorePitchUtils::_getAjustmentForLeger(scale, false, 0), 0);
        assertEQ(ScorePitchUtils::_getAjustmentForLeger(scale, false, 1), 1);  // F#
        assertEQ(ScorePitchUtils::_getAjustmentForLeger(scale, false, 2), 0);
        assertEQ(ScorePitchUtils::_getAjustmentForLeger(scale, false, 3), 0);
        assertEQ(ScorePitchUtils::_getAjustmentForLeger(scale, false, 4), 0);  // B
        assertEQ(ScorePitchUtils::_getAjustmentForLeger(scale, false, 5), 1);  // C#
        assertEQ(ScorePitchUtils::_getAjustmentForLeger(scale, false, 6), 0);
        assertEQ(ScorePitchUtils::_getAjustmentForLeger(scale, false, 7), 0);
        assertEQ(ScorePitchUtils::_getAjustmentForLeger(scale, false, 8), 1);  // F#
    }
    // assert(false);  // need e flat major, maybe an easier one, too.
    SQINFO("need more unit tests for get adjustment");
}

static void testMakeCanonical() {
    Scale scale(MidiNote(MidiNote::C), Scale::Scales::Major);

    const MidiNote mnC(MidiNote::MiddleC);
    const NotationNote c(mnC, NotationNote::Accidental::none, -2, false);
    auto x = ScorePitchUtils::makeCanonical(c);
    assert(x == c);

    // make E spelled as F flat
    const MidiNote mnFf(MidiNote::MiddleC + MidiNote::E);
    const NotationNote fFlat(mnFf, NotationNote::Accidental::flat, 1, false);
    x = ScorePitchUtils::makeCanonical(fFlat);
    assert(x._accidental == NotationNote::Accidental::none);
    assertEQ(x._legerLine, 0);

    // make C spelled as B sharp
    const NotationNote bSharp(mnC, NotationNote::Accidental::sharp, -3, false);
    x = ScorePitchUtils::makeCanonical(bSharp);
    assert(x._accidental == NotationNote::Accidental::none);
    assertEQ(x._legerLine, -2);

    // G Sharp
    const MidiNote mnGs(MidiNote::MiddleC + MidiNote::G + 1);
    const NotationNote gSharp(mnGs, NotationNote::Accidental::sharp, 2, false);
    x = ScorePitchUtils::makeCanonical(gSharp);
    assert(x == gSharp);

    // A flat
    const NotationNote aFlat(mnGs, NotationNote::Accidental::flat, 3, false);
    x = ScorePitchUtils::makeCanonical(aFlat);
    assert(x == aFlat);
}

static void testFindSpellingNotSameLine() {
    Scale scale(MidiNote(MidiNote::C), Scale::Scales::Major);

    SqArray<int, 16> inputPitches = {MidiNote::MiddleC, MidiNote::MiddleC + 1};
    SqArray<NotationNote, 16> expectedOutputNotes;

    // don't want to see C and C#, want C and D flat
    expectedOutputNotes.putAt(0, NotationNote(MidiNote(MidiNote::MiddleC), NotationNote::Accidental::none, -2, false));
    expectedOutputNotes.putAt(1, NotationNote(MidiNote(MidiNote::MiddleC + 1), NotationNote::Accidental::flat, -1, false));

    testFindSpelling(expectedOutputNotes, scale, inputPitches, false);
}

static void testFindSpellingFFSharpInG() {
    Scale scale(MidiNote(MidiNote::G), Scale::Scales::Major);

    SqArray<int, 16> inputPitches{
        MidiNote::MiddleC + MidiNote::F,       // F
        MidiNote::MiddleC + MidiNote::F + 1};  // F sharp

    SqArray<NotationNote, 16> expectedOutputNotes{
        // E sharp
        NotationNote(MidiNote(MidiNote::MiddleC + MidiNote::F), NotationNote::Accidental::sharp, 0, false),
        // F sharp (F is naturally sharp in G)
        NotationNote(MidiNote(MidiNote::MiddleC + MidiNote::F + 1), NotationNote::Accidental::none, 1, false),
    };

    testFindSpelling(expectedOutputNotes, scale, inputPitches, false);
}

static void testLegerLineTracker() {
    ScorePitchUtils::LegerLineTracker llt;
    assertEQ(llt.getSawMulti(), 0);

    llt.sawLine(0);
    llt.sawLine(0);
    assertEQ(llt.getSawMulti(), 1);

    llt.sawLine(4);
    llt.sawLine(4);
    assertEQ(llt.getSawMulti(), 2);
}

static void testLegerLineTracker2() {
    ScorePitchUtils::LegerLineTracker llt;
    llt.sawLine(128);
    llt.sawLine(-128);

    assertEQ(llt.getSawMulti(), 0);
    llt.sawLine(128);
    assertEQ(llt.getSawMulti(), 1);
     llt.sawLine(-128);
    assertEQ(llt.getSawMulti(), 2);
}

void testScorePitchUtils() {
    test();
    testGetAjustmentForLeger();
    testPitchFromLegerTrebleCMajorWhiteKeys();
    testPitchFromLegerTrebleCMajorAccidentals();
    testPitchFromLegerTrebleCMajorNatural();
    testPitchFromLegerCminor();
    testPitchFromLegerEFlatMinor();

    testMakeCanonical();

    testValidate();
    testValidate2();
    testValidateCMajor();
    testValidateCminor();
    testValidateGMajor();
    testNoteEFlatInEFlatMajor();
    testNoteEFlatInEFlatMinor();

    testCMajor();
    testCMinor();

    testReSpellCMajorCSharp();
    testReSpellCMajorDFlat();
    testReSpellCMajorCSharpTwice();
    testCSharpVariations();
    testEVariations();
    testFVariationsInG();

    testFindSpelling();
    testFindSpellingCMajor();
    testFindSpellingCminorInCMajor();
    testFindSpellingCminorFirstInversionInCMajor();
    testFindSpellingCminorFirstInversionInCMajorDupesEtc();

    SQINFO("make testFindSpellingDFlatMajorInCMajor work");
    // testFindSpellingDFlatMajorInCMajor();
    testFindSpellingEMajorInCMajor();

    testFindSpellingOneNoteCSharpInCMajor();
    testFindSpellingOneNoteDFlatSharpInCMajor();

    SQINFO("make testFindSpelling9thNo5 work");
    // testFindSpelling9thNo5();

    testLegerLineTracker();
    testLegerLineTracker2();
    testFindSpellingNotSameLine();
    testFindSpellingFFSharpInG();
}

#if 0
void testFirst() {
   // testLegerLineTracker2();
  //  testFindSpellingFFSharpInG();

     testScorePitchUtils();

    // SQINFO("------ first test");
    //testValidateGMajor();

    // SQINFO("------ second test");
    // testFVariationsInG();
}

#endif