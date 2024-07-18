

#include "ChordRecognizer.h"
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

static void testNoteEFlatInEFlatMinor() {
    SQINFO("---- testNoteEFlatInEFlatMinor ---");
    Scale sc;
    MidiNote mnEFlat3(MidiNote::MiddleC + MidiNote::E - 1);
    MidiNote mnEFlat(MidiNote::E - 1);

    // note that this is strange key! The key of E flat (D sharp) minor.
    // I think originally this test was supposed to be in C minor...
    // I think the test passes in E flat major!
    sc.set(mnEFlat, Scale::Scales::Minor);     
    SQINFO("set keysig root to %d (+72 = %d)", mnEFlat.get(), mnEFlat.get() + 72 );

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
    sc.set(mnEFlat, Scale::Scales::Minor);          // Is this a mistake that we did the test in this key?

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
    // SQINFO("---- testValidate()");
    Scale scale(MidiNote(MidiNote::C), Scale::Scales::Major);
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

//    static void findSpelling( vlenArray<int, 16> inputPitch, vlenArray<NotationNote, 16> outputNotes, bool bassStaff);
static void testFindSpelling() {
    SqArray<int, 16> inputPitch;
    //SqArray<NotationNote, 16> outputNotes;

    Scale scale;
    scale.set(MidiNote(MidiNote::C), Scale::Scales::Major);

    inputPitch.putAt(0, MidiNote::MiddleC);
    inputPitch.putAt(1, MidiNote::MiddleC + 12);
    const auto results = ScorePitchUtils::findSpelling(scale, inputPitch, false);

    assertEQ(inputPitch.numValid(), 2);
    assertEQ(results.notes.numValid(), 2);
}

static void testFindSpelling(const SqArray<NotationNote, 16>& expectedOutputNotes, const Scale& scale, const SqArray<int, 16>& inputPitches, bool bassStaff) {
    // assertEQ(expectedOutputNotes.numValid(), inputPitches.numValid());
    assertGT(inputPitches.numValid(), 0);
    const auto result = ScorePitchUtils::findSpelling(scale, inputPitches, bassStaff);

   // SQINFO("final score = %d", result.score);
#if 0
    if (inputPitches.numValid() == 3) {
        SQINFO("input notes = %d | %d | %d",
               inputPitches.getAt(0),
               inputPitches.getAt(1),
               inputPitches.getAt(2));

        SQINFO("output notes = <%s> | <%s> | <%s>",
               outputNotes.getAt(0).toString().c_str(),
               outputNotes.getAt(1).toString().c_str(),
               outputNotes.getAt(2).toString().c_str());
    } else {
        SQINFO("can't print input");
    }
#endif

    for (unsigned i = 0; i < inputPitches.numValid(); ++i) {
        // ChordRecognizer::_show("intput notes", inputPitches);
        //SQINFO("input[%d]=%d", i, inputPitches.getAt(i));
    }

    for (unsigned i = 0; i < result.notes.numValid(); ++i) {
        //SQINFO("output[%d]=%s", i, result.notes.getAt(i).toString().c_str());
    }

    for (unsigned i = 0; i < inputPitches.numValid(); ++i) {
        const NotationNote n = result.notes.getAt(i);
        const NotationNote expected = expectedOutputNotes.getAt(i);
        assert(n == expected);
    }
}

static void testFindSpellingCMajor() {
   //SQINFO("--- testFindSpellingCMajor");
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
    //SQINFO("--- testFindSpellingDFlat");
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
    //SQINFO("--- testFindSpellingE");
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
    assertEQ(ScorePitchUtils::pitchFromLeger(false, 0, NotationNote::Accidental::none, scale), MidiNote::MiddleC + MidiNote::E - 1);
   // assert(false);
}


static void testGetAjustmentForLeger() {
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
        // C Minor, root is white key, but some accidentals.
        // Will be affected by current pref
        assert(AccidentalResolver::getPref() == ResolvedSharpsFlatsPref::Sharps);
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
    assert(false);  // need e flat major, maybe an easier one, too.
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

void testScorePitchUtils() {
    test();
    testGetAjustmentForLeger();
    testPitchFromLegerTrebleCMajorWhiteKeys();
    testPitchFromLegerTrebleCMajorAccidentals();
    testPitchFromLegerTrebleCMajorNatural();
    testPitchFromLegerCminor();
    testPitchFromLegerEFlatMinor();

    testMakeCanonical();

    testCMajor();
    testCMinor();

    testValidate();
    testValidate2();
    testValidateCMajor();
    testValidateCminor();
     testNoteEFlatInEFlatMajor();
    testNoteEFlatInEFlatMinor();

    testReSpellCMajorCSharp();
    testReSpellCMajorDFlat();
    testReSpellCMajorCSharpTwice();
    testCSharpVariations();
    testEVariations();

    testFindSpelling();
    testFindSpellingCMajor();
    testFindSpellingCminorInCMajor();
    testFindSpellingCminorFirstInversionInCMajor();
    testFindSpellingCminorFirstInversionInCMajorDupesEtc();
    testFindSpellingDFlatMajorInCMajor();
    testFindSpellingEMajorInCMajor();
}

#if 1
void testFirst() {
    testGetAjustmentForLeger();
    testPitchFromLegerEFlatMinor();
    //testNoteEFlatInEFlatMinor();

    //  testScorePitchUtils();
  
}

#endif