
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

static void testCMinorRecognized() {
    ChordRecognizer ch;
    const int chord[] = {MidiNote::C, MidiNote::E - 1, MidiNote::G};
    auto const result = ch.recognize(chord, 3);
    ChordRecognizer::Type t = std::get<0>(result);
    assert(ChordRecognizer::typeFromInfo(result) == ChordRecognizer::Type::MinorTriad);
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

static void testCMajorSecondInversion() {
    ChordRecognizer ch;
    const int chord[] = {MidiNote::C + 12, MidiNote::E + 12, MidiNote::G};
    auto const result = ch.recognize(chord, 3);
    assert(ChordRecognizer::typeFromInfo(result) == ChordRecognizer::Type::MajorTriad);
    assert(ChordRecognizer::inversionFromInfo(result) == ChordRecognizer::Inversion::Second);
    assertEQ(ChordRecognizer::pitchFromInfo(result), MidiNote::C);
}

static void testToString() {
    std::vector<std::string> v = ChordRecognizer::toString(std::make_tuple(ChordRecognizer::Type::Unrecognized, ChordRecognizer::Inversion::Root, 0));
    assertEQ(v.size(), 2);
    assertEQ(v[0].size(), 0);
    assertEQ(v[1].size(), 0)

    v = ChordRecognizer::toString(std::make_tuple(ChordRecognizer::Type::MajorTriad, ChordRecognizer::Inversion::Root, 0));
    assertEQ(v.size(), 2);
    assertGT(v[0].size(), 0);
    assertEQ(v[1].size(), 0)

    v = ChordRecognizer::toString(std::make_tuple(ChordRecognizer::Type::MajorTriad, ChordRecognizer::Inversion::First, 0));
    assertEQ(v.size(), 2);
    assertGT(v[0].size(), 0);
    assertGT(v[1].size(), 0)

    v = ChordRecognizer::toString(std::make_tuple(ChordRecognizer::Type::MinorTriad, ChordRecognizer::Inversion::Root, 0));
    assertEQ(v.size(), 2);
    assertGT(v[0].size(), 0);
    assertEQ(v[1].size(), 0)

    v = ChordRecognizer::toString(std::make_tuple(ChordRecognizer::Type::Sus2Triad, ChordRecognizer::Inversion::Root, 0));
    assertEQ(v.size(), 2);
    assertGT(v[0].size(), 0);
    assertEQ(v[1].size(), 0);

    v = ChordRecognizer::toString(std::make_tuple(ChordRecognizer::Type::Sus4Triad, ChordRecognizer::Inversion::Root, 0));
    assertEQ(v.size(), 2);
    assertGT(v[0].size(), 0);
    assertEQ(v[1].size(), 0);
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
    testCMajorSecondInversion();
    testToString();
    testCMinorRecognized();
    testCSus4Recognized();
     testCSus2Recognized();

    // To add:

    // chord inverted
    // non-major triads
    // other chord types
    // chords larger than an octave (9th)
}

#if 1
void testFirst() {
    //  testCMajorRecognized();
    //  testCMajor4Voice();
    // testCMajorFirstInversion();
    testCMajorSecondInversion();
    //  testToString();
}
#endif