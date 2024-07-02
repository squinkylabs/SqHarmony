
#include "ChordRecognizer.h"
#include "asserts.h"

static void testCanCreate() {
    //  ChordRecognizer ch;
    const int chord[] = {1, 2, 3};
    ChordRecognizer::recognize(chord, 3);
}

static void testJunkNotRecognized() {
    ChordRecognizer ch;
    const int chord[] = {1, 2, 3};
    auto const result = ch.recognize(chord, 3);
    ChordRecognizer::Type t = std::get<0>(result);
    assert(t == ChordRecognizer::Type::Unrecognized);
}

static void testCMajorRecognized() {
    ChordRecognizer ch;
    const int chord[] = {MidiNote::C, MidiNote::E, MidiNote::G};
    auto const result = ch.recognize(chord, 3);

    assert(ChordRecognizer::typeFromInfo(result) == ChordRecognizer::Type::MajorTriad);
    assert(ChordRecognizer::pitchFromInfo(result) == MidiNote::C);
}

static void testCDiminishedSecondInversion() {
    const int chord[] = {MidiNote::C + 12, MidiNote::E - 1 + 12, MidiNote::G - 1};
    auto const result = ChordRecognizer::recognize(chord, 3);

    assert(ChordRecognizer::typeFromInfo(result) == ChordRecognizer::Type::DiminishedTriad);
    assert(ChordRecognizer::pitchFromInfo(result) == MidiNote::C);
    assert(ChordRecognizer::inversionFromInfo(result) == ChordRecognizer::Inversion::Second);
}

static void testCDiminished() {
    const int chord[] = {MidiNote::C, MidiNote::E - 1, MidiNote::G - 1};
    auto const result = ChordRecognizer::recognize(chord, 3);

    assert(ChordRecognizer::typeFromInfo(result) == ChordRecognizer::Type::DiminishedTriad);
    assert(ChordRecognizer::pitchFromInfo(result) == MidiNote::C);
    assert(ChordRecognizer::inversionFromInfo(result) == ChordRecognizer::Inversion::Root);
}

static void testRecognizer(
    const int* chord,
    unsigned length,
    ChordRecognizer::Type expectedType,
    ChordRecognizer::Inversion expectedInversion,
    int expectedRoot) {
    auto const result = ChordRecognizer::recognize(chord, length);
    assert(ChordRecognizer::typeFromInfo(result) == expectedType);
    assert(ChordRecognizer::inversionFromInfo(result) == expectedInversion);
    assertEQ(ChordRecognizer::pitchFromInfo(result), expectedRoot);
}

static void testCMajMinRecognized() {
    const int chord[] = {MidiNote::C, MidiNote::E, MidiNote::G, MidiNote::B - 1};
    testRecognizer(
        chord,
        4,
        ChordRecognizer::Type::MajMinSeventh,
        ChordRecognizer::Inversion::Root,
        MidiNote::C);
}

static void testCMajMin9Recognized() {
    const int chord[] = {MidiNote::C, MidiNote::E, MidiNote::G, MidiNote::B - 1, MidiNote::D + 12};
    testRecognizer(
        chord,
        5,
        ChordRecognizer::Type::MajMinNinth,
        ChordRecognizer::Inversion::Root,
        MidiNote::C);
}

static void testCMajMin95SRecognized() {
    const int chord[] = {MidiNote::C, MidiNote::E, MidiNote::B - 1, MidiNote::D + 12};
    testRecognizer(
        chord,
        4,
        ChordRecognizer::Type::MajMinNinth,
        ChordRecognizer::Inversion::Root,
        MidiNote::C);
}

static void testCMajMinFirstInversionRecognized() {
    const int chord[] = {MidiNote::C + 12, MidiNote::E, MidiNote::G, MidiNote::B - 1};
    testRecognizer(
        chord,
        4,
        ChordRecognizer::Type::MajMinSeventh,
        ChordRecognizer::Inversion::First,
        MidiNote::C);
}

static void testCMajMajFirstInversionRecognized() {
    const int chord[] = {MidiNote::C + 12, MidiNote::E, MidiNote::G, MidiNote::B};
    testRecognizer(
        chord,
        4,
        ChordRecognizer::Type::MajMajSeventh,
        ChordRecognizer::Inversion::First,
        MidiNote::C);
}

static void testCMajMajSecondInversionRecognized() {
    const int chord[] = {MidiNote::C + 12, MidiNote::E + 12, MidiNote::G, MidiNote::B};
    testRecognizer(
        chord,
        4,
        ChordRecognizer::Type::MajMajSeventh,
        ChordRecognizer::Inversion::Second,
        MidiNote::C);
}

static void testCMajMajThirdInversionRecognized() {
    const int chord[] = {MidiNote::C + 12, MidiNote::E + 12, MidiNote::G + 12, MidiNote::B};
    testRecognizer(
        chord,
        4,
        ChordRecognizer::Type::MajMajSeventh,
        ChordRecognizer::Inversion::Third,
        MidiNote::C);
}
static void testCMajMinSecondInversionRecognized() {
    const int chord[] = {MidiNote::C + 12, MidiNote::E + 12, MidiNote::G, MidiNote::B - 1};
    testRecognizer(
        chord,
        4,
        ChordRecognizer::Type::MajMinSeventh,
        ChordRecognizer::Inversion::Second,
        MidiNote::C);
}

static void testCMinorFirstInversion() {
    const int chord[] = {MidiNote::C + 12, MidiNote::E - 1, MidiNote::G};
    testRecognizer(
        chord, 3,
        ChordRecognizer::Type::MinorTriad,
        ChordRecognizer::Inversion::First,
        MidiNote::C);
}

static void testCMajMajRecognized() {
    const int chord[] = {MidiNote::C, MidiNote::E, MidiNote::G, MidiNote::B};
    testRecognizer(
        chord,
        4,
        ChordRecognizer::Type::MajMajSeventh,
        ChordRecognizer::Inversion::Root,
        MidiNote::C);
}

static void testCMajMaj9Recognized() {
    const int chord[] = {MidiNote::C, MidiNote::E, MidiNote::G, MidiNote::B, MidiNote::D + 12};
    testRecognizer(
        chord,
        5,
        ChordRecognizer::Type::MajMajNinth,
        ChordRecognizer::Inversion::Root,
        MidiNote::C);
}

static void testCMinMinRecognized() {
    const int chord[] = {MidiNote::C, MidiNote::E - 1, MidiNote::G, MidiNote::B - 1};
    testRecognizer(
        chord,
        4,
        ChordRecognizer::Type::MinMinSeventh,
        ChordRecognizer::Inversion::Root,
        MidiNote::C);
}

static void testCMinMin9Recognized() {
    const int chord[] = {MidiNote::C, MidiNote::E - 1, MidiNote::G, MidiNote::B - 1, MidiNote::D + 12};
    testRecognizer(
        chord,
        5,
        ChordRecognizer::Type::MinMinNinth,
        ChordRecognizer::Inversion::Root,
        MidiNote::C);
}

static void testCMinMajRecognized() {
    const int chord[] = {MidiNote::C, MidiNote::E - 1, MidiNote::G, MidiNote::B};
    testRecognizer(
        chord,
        4,
        ChordRecognizer::Type::MinMajSeventh,
        ChordRecognizer::Inversion::Root,
        MidiNote::C);
}

static void testCMinMaj9Recognized() {
    const int chord[] = {MidiNote::C, MidiNote::E - 1, MidiNote::G, MidiNote::B, MidiNote::D + 12};
    testRecognizer(
        chord,
        5,
        ChordRecognizer::Type::MinMajNinth,
        ChordRecognizer::Inversion::Root,
        MidiNote::C);
}

static void testCAugRecognized() {
    ChordRecognizer ch;
    const int chord[] = {MidiNote::C, MidiNote::E, MidiNote::G + 1};
    auto const result = ch.recognize(chord, 3);

    assert(ChordRecognizer::typeFromInfo(result) == ChordRecognizer::Type::AugmentedTriad);
    assert(ChordRecognizer::pitchFromInfo(result) == MidiNote::C);
}

static void testCMinorRecognized() {
    ChordRecognizer ch;
    const int chord[] = {MidiNote::C, MidiNote::E - 1, MidiNote::G};
    auto const result = ch.recognize(chord, 3);
    ChordRecognizer::Type t = std::get<0>(result);
    assert(ChordRecognizer::typeFromInfo(result) == ChordRecognizer::Type::MinorTriad);
    assert(ChordRecognizer::pitchFromInfo(result) == MidiNote::C);
}

static void testCDimRecognized() {
    ChordRecognizer ch;
    const int chord[] = {MidiNote::C, MidiNote::E - 1, MidiNote::G - 1};
    auto const result = ch.recognize(chord, 3);
    ChordRecognizer::Type t = std::get<0>(result);
    assert(ChordRecognizer::typeFromInfo(result) == ChordRecognizer::Type::DiminishedTriad);
    assert(ChordRecognizer::pitchFromInfo(result) == MidiNote::C);
}
static void testCSus4Recognized() {
    ChordRecognizer ch;
    const int chord[] = {MidiNote::C, MidiNote::E + 1, MidiNote::G};
    auto const result = ch.recognize(chord, 3);
    ChordRecognizer::Type t = std::get<0>(result);
    assert(ChordRecognizer::typeFromInfo(result) == ChordRecognizer::Type::Sus4Triad);
    assert(ChordRecognizer::pitchFromInfo(result) == MidiNote::C);
}

static void testCSus2Recognized() {
    ChordRecognizer ch;
    const int chord[] = {MidiNote::C, MidiNote::D, MidiNote::G};
    auto const result = ch.recognize(chord, 3);
    ChordRecognizer::Type t = std::get<0>(result);
    assert(ChordRecognizer::typeFromInfo(result) == ChordRecognizer::Type::Sus2Triad);
    assert(ChordRecognizer::pitchFromInfo(result) == MidiNote::C);
}

static void testDMajorRecognized() {
    ChordRecognizer ch;
    const int chord[] = {MidiNote::D, MidiNote::F + 1, MidiNote::A};
    auto const result = ch.recognize(chord, 3);

    assert(ChordRecognizer::typeFromInfo(result) == ChordRecognizer::Type::MajorTriad);
    assert(ChordRecognizer::pitchFromInfo(result) == MidiNote::D);
}

static void testCMajorOctaveRecognized() {
    ChordRecognizer ch;
    const int chord[] = {MidiNote::C + 12, MidiNote::E + 12, MidiNote::G + 12};
    auto const result = ch.recognize(chord, 3);
    ChordRecognizer::Type t = std::get<0>(result);
    assert(ChordRecognizer::typeFromInfo(result) == ChordRecognizer::Type::MajorTriad);
    assert(ChordRecognizer::pitchFromInfo(result) == MidiNote::C);
}

static void testCMajorOrder() {
    ChordRecognizer ch;
    const int chord[] = {MidiNote::C, MidiNote::G, MidiNote::E};
    auto const result = ch.recognize(chord, 3);
    ChordRecognizer::Type t = std::get<0>(result);
    assert(ChordRecognizer::typeFromInfo(result) == ChordRecognizer::Type::MajorTriad);
    assert(ChordRecognizer::pitchFromInfo(result) == MidiNote::C);
}

static void testCMajor4Voice() {
    //   ChordRecognizer ch;
    const int chord[] = {MidiNote::C, MidiNote::E, MidiNote::G, MidiNote::C + 12};
    auto const result = ChordRecognizer::recognize(chord, 4);
    ChordRecognizer::Type t = std::get<0>(result);
    assert(ChordRecognizer::typeFromInfo(result) == ChordRecognizer::Type::MajorTriad);
    assert(ChordRecognizer::pitchFromInfo(result) == MidiNote::C);
}

static void testGMajorRecognized() {
    ChordRecognizer ch;
    const int chord[] = {MidiNote::G, MidiNote::B, MidiNote::D + 12};
    auto const result = ch.recognize(chord, 3);

    assert(ChordRecognizer::typeFromInfo(result) == ChordRecognizer::Type::MajorTriad);
    assert(ChordRecognizer::pitchFromInfo(result) == MidiNote::G);
}

static void testCMajorOneNoteCrazyOctave() {
    ChordRecognizer ch;
    const int chord[] = {MidiNote::C, MidiNote::E, MidiNote::G + 4 * 12};
    auto const result = ch.recognize(chord, 3);
    assert(ChordRecognizer::typeFromInfo(result) == ChordRecognizer::Type::MajorTriad);
    assert(ChordRecognizer::pitchFromInfo(result) == MidiNote::C);
}

static void testCMajorFirstInversion() {
    ChordRecognizer ch;
    const int chord[] = {MidiNote::C + 12, MidiNote::E, MidiNote::G};
    auto const result = ch.recognize(chord, 3);
    assert(ChordRecognizer::typeFromInfo(result) == ChordRecognizer::Type::MajorTriad);
    assert(ChordRecognizer::inversionFromInfo(result) == ChordRecognizer::Inversion::First);
    assertEQ(ChordRecognizer::pitchFromInfo(result), MidiNote::C);
}

static void testCSharpMajorFirstInversion() {
    ChordRecognizer ch;
    const int chord[] = {MidiNote::C + 12 + 1, MidiNote::E + 1, MidiNote::G + 1};
    auto const result = ch.recognize(chord, 3);
    assert(ChordRecognizer::typeFromInfo(result) == ChordRecognizer::Type::MajorTriad);
    assert(ChordRecognizer::inversionFromInfo(result) == ChordRecognizer::Inversion::First);
    assertEQ(ChordRecognizer::pitchFromInfo(result), MidiNote::C + 1);
}

static void testGSharpMajorFirstInversion() {
    ChordRecognizer ch;
    const int chord[] = {MidiNote::G + 1, MidiNote::C, MidiNote::D + 1};  // G#, C D# is a major chord
    auto const result = ch.recognize(chord, 3);
    assert(ChordRecognizer::typeFromInfo(result) == ChordRecognizer::Type::MajorTriad);
    assert(ChordRecognizer::inversionFromInfo(result) == ChordRecognizer::Inversion::First);
    assertEQ(ChordRecognizer::pitchFromInfo(result), MidiNote::G + 1);
}

static void testCMajorFirstInversion2() {
    ChordRecognizer ch;
    const int chord[] = {MidiNote::C + 12 + 48, MidiNote::E + 48, MidiNote::G + 48};
    auto const result = ch.recognize(chord, 3);
    assert(ChordRecognizer::typeFromInfo(result) == ChordRecognizer::Type::MajorTriad);
    assert(ChordRecognizer::inversionFromInfo(result) == ChordRecognizer::Inversion::First);
    assertEQ(ChordRecognizer::pitchFromInfo(result), MidiNote::C);
}

static void testCMajorSecondInversion() {
    ChordRecognizer ch;
    const int chord[] = {MidiNote::C + 12, MidiNote::E + 12, MidiNote::G};
    auto const result = ch.recognize(chord, 3);
    assert(ChordRecognizer::typeFromInfo(result) == ChordRecognizer::Type::MajorTriad);
    assert(ChordRecognizer::inversionFromInfo(result) == ChordRecognizer::Inversion::Second);
    assertEQ(ChordRecognizer::pitchFromInfo(result), MidiNote::C);
}

static void testToStringSub(const ChordRecognizer::ChordInfo& info) {
    const bool expectInversion = ChordRecognizer::inversionFromInfo(info) != ChordRecognizer::Inversion::Root;
    std::vector<std::string> v = ChordRecognizer::toString(info);
    assertEQ(v.size(), 2);
    assertGT(v[0].size(), 0);
    if (expectInversion) {
        assertGT(v[1].size(), 0);
    } else {
        assertEQ(v[1].size(), 0);
    }

    const int pitch = ChordRecognizer::pitchFromInfo(info);
    const std::string name = PitchKnowledge::nameOfShort(pitch);
    const auto x = v[0].find(name);
    assertNE(x, std::string::npos);
}

static void testToString() {
    std::vector<std::string> v = ChordRecognizer::toString(std::make_tuple(ChordRecognizer::Type::Unrecognized, ChordRecognizer::Inversion::Root, 0));
    assertEQ(v.size(), 2);
    assertEQ(v[0].size(), 0);
    assertEQ(v[1].size(), 0);

    testToStringSub(std::make_tuple(ChordRecognizer::Type::MajorTriad, ChordRecognizer::Inversion::Root, 0));
    testToStringSub(std::make_tuple(ChordRecognizer::Type::MajorTriad, ChordRecognizer::Inversion::Root, MidiNote::D));

    testToStringSub(std::make_tuple(ChordRecognizer::Type::MajorTriad, ChordRecognizer::Inversion::First, 0));
    testToStringSub(std::make_tuple(ChordRecognizer::Type::MajorTriad, ChordRecognizer::Inversion::Second, 0));
    testToStringSub(std::make_tuple(ChordRecognizer::Type::MinorTriad, ChordRecognizer::Inversion::Root, 0));

    testToStringSub(std::make_tuple(ChordRecognizer::Type::Sus2Triad, ChordRecognizer::Inversion::Root, 0));
    testToStringSub(std::make_tuple(ChordRecognizer::Type::Sus4Triad, ChordRecognizer::Inversion::Root, 0));
    testToStringSub(std::make_tuple(ChordRecognizer::Type::AugmentedTriad, ChordRecognizer::Inversion::Root, 0));
    testToStringSub(std::make_tuple(ChordRecognizer::Type::DiminishedTriad, ChordRecognizer::Inversion::Root, 0));

    testToStringSub(std::make_tuple(ChordRecognizer::Type::MajMajSeventh, ChordRecognizer::Inversion::Root, 0));
    testToStringSub(std::make_tuple(ChordRecognizer::Type::MajMinSeventh, ChordRecognizer::Inversion::Root, 0));
    testToStringSub(std::make_tuple(ChordRecognizer::Type::MinMinSeventh, ChordRecognizer::Inversion::Root, 0));
    testToStringSub(std::make_tuple(ChordRecognizer::Type::MinMajSeventh, ChordRecognizer::Inversion::Root, 0));

    testToStringSub(std::make_tuple(ChordRecognizer::Type::MajMajSeventh, ChordRecognizer::Inversion::First, 0));
    testToStringSub(std::make_tuple(ChordRecognizer::Type::MajMajSeventh, ChordRecognizer::Inversion::Second, 0));
    testToStringSub(std::make_tuple(ChordRecognizer::Type::MajMajSeventh, ChordRecognizer::Inversion::Third, 0));

    testToStringSub(std::make_tuple(ChordRecognizer::Type::MajMinNinth, ChordRecognizer::Inversion::Root, 0));
    testToStringSub(std::make_tuple(ChordRecognizer::Type::MajMajNinth, ChordRecognizer::Inversion::Root, 0));
    testToStringSub(std::make_tuple(ChordRecognizer::Type::MinMinNinth, ChordRecognizer::Inversion::Root, 0));
    testToStringSub(std::make_tuple(ChordRecognizer::Type::MinMajNinth, ChordRecognizer::Inversion::Root, 0));
}

static void testNotesInChord() {
    for (int i = 0; i <= int(ChordRecognizer::Type::MinMajSeventh); ++i) {
        const int notes = ChordRecognizer::notesInChord(ChordRecognizer::Type(i));
        if (notes != 0) {
            assertGE(notes, 3);
            assertLE(notes, 4);
        }
    }
}

static void test007() {
    ChordRecognizer ch;
    const int chord[] = {72, 72, 79};
    auto const result = ch.recognize(chord, 3);
    assert(ChordRecognizer::typeFromInfo(result) == ChordRecognizer::Type::Unrecognized);
    // assert(ChordRecognizer::inversionFromInfo(result) == ChordRecognizer::Inversion::Second);
    // assertEQ(ChordRecognizer::pitchFromInfo(result), MidiNote::C);
}

void testChordRecognizer() {
    //  testCanCreate();
    testJunkNotRecognized();
    testCMajorRecognized();
    testDMajorRecognized();
    testCMajorOctaveRecognized();
    testCMajorOrder();
    testCMajor4Voice();
    testGMajorRecognized();
    testCMajorOneNoteCrazyOctave();
    testCMajorFirstInversion();
    testCMajorFirstInversion2();
    testCMinorFirstInversion();
    testCMajorSecondInversion();
    testCSharpMajorFirstInversion();

    testCMinorRecognized();
    testCSus4Recognized();
    testCSus2Recognized();
    testCDimRecognized();
    testCAugRecognized();

    testCMajMinRecognized();
    testCMajMin9Recognized();
    testCMajMin95SRecognized();
    testCMajMajRecognized();
    testCMajMaj9Recognized();
    testCMinMinRecognized();
    testCMinMin9Recognized();
    testCMinMajRecognized();
    testCMinMaj9Recognized();

    testCMajMinFirstInversionRecognized();
    testCMajMinSecondInversionRecognized();

    testCMajMajFirstInversionRecognized();
    testCMajMajSecondInversionRecognized();
    testCMajMajThirdInversionRecognized();
    testGSharpMajorFirstInversion();

    testCDiminished();
    testCDiminishedSecondInversion();

    testToString();
    testNotesInChord();
    test007();
}

#if 0
void testFirst() {
    // This one is a problem
    // testGSharpMajorFirstInversion();
    // testCMajorFirstInversion();
      testChordRecognizer();
    //
    //  testCDiminishedSecondInversion();
    //  testCDiminished();
    //   test007();
    // testCMajMajThirdInversionRecognized();
   // testChordRecognizer();
    //   testCMajMin9Recognized();
   testCMajMin95SRecognized();
}
#endif