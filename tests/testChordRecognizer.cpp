
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

static void testCMinMinRecognized() {
    const int chord[] = {MidiNote::C, MidiNote::E - 1, MidiNote::G, MidiNote::B - 1};
    testRecognizer(
        chord,
        4,
        ChordRecognizer::Type::MinMinSeventh,
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
    testToString();
    testCMinorRecognized();
    testCSus4Recognized();
    testCSus2Recognized();
    testCDimRecognized();
    testCAugRecognized();

    testCMajMinRecognized();
    testCMajMajRecognized();
    testCMinMinRecognized();
    testCMinMajRecognized();

    testCMajMinFirstInversionRecognized();
    testCMajMinSecondInversionRecognized();

    testCMajMajFirstInversionRecognized();
    testCMajMajSecondInversionRecognized();

    // To add:

    // chord inverted
    // non-major triads
    // other chord types
    // chords larger than an octave (9th)
}

#if 0
void testFirst() {
    //  testCMajorRecognized();
    //  testCMajor4Voice();
    testCMajorFirstInversion2();
    //   testCMinorFirstInversion();
    // testCMajMinFirstInversionRecognized();
    //  testCMajMinSecondInversionRecognized();

    //  testCMajorSecondInversion();
    //  testToString();
}
#endif