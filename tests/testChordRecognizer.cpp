
#include "ChordRecognizer.h"
#include "asserts.h"


static void testCopy() {
    SqArray<int, 16> array;
     SqArray<int, 16> output;
    array.putAt(0, 11);
    array.putAt(1, 22);
    assertEQ(array.numValid(), 2);
    assertEQ(output.numValid(), 0);
    ChordRecognizer::_copy(output, array);
    assertEQ(output.numValid(), 2);
    assertEQ(output.getAt(0), 11);
    assertEQ(output.getAt(1), 22);
}

static SqArray<int, 16> makeChord(std::vector<int> input) {
    SqArray<int, 16> chord;
    int i=0;
    for (auto x : input) {
        chord.putAt(i++, x);
    }
    return chord;
}

static void testCanCreate() {
    SqArray<int, 16> chord = makeChord({1, 2, 3});
    ChordRecognizer::recognize(chord);
}

static void testJunkNotRecognized() {
    ChordRecognizer ch;
    SqArray<int, 16> chord = makeChord({1, 2, 3});
    auto const result = ch.recognize(chord);
    ChordRecognizer::Type t = std::get<0>(result);
    assert(t == ChordRecognizer::Type::Unrecognized);
}

static void testCMajorRecognized() {
    ChordRecognizer ch;
    SqArray<int, 16> chord = makeChord({MidiNote::C, MidiNote::E, MidiNote::G});
    auto const result = ch.recognize(chord);

    assert(ChordRecognizer::typeFromInfo(result) == ChordRecognizer::Type::MajorTriad);
    assert(ChordRecognizer::pitchFromInfo(result) == MidiNote::C);
}

static void testCDiminishedSecondInversion() {
    SqArray<int, 16> chord = makeChord({MidiNote::C + 12, MidiNote::E - 1 + 12, MidiNote::G - 1});
    auto const result = ChordRecognizer::recognize(chord);

    assert(ChordRecognizer::typeFromInfo(result) == ChordRecognizer::Type::DiminishedTriad);
    assert(ChordRecognizer::pitchFromInfo(result) == MidiNote::C);
    assert(ChordRecognizer::inversionFromInfo(result) == ChordRecognizer::Inversion::Second);
}

static void testCDiminished() {
    SqArray<int, 16> chord = makeChord({MidiNote::C, MidiNote::E - 1, MidiNote::G - 1});
    auto const result = ChordRecognizer::recognize(chord);

    assert(ChordRecognizer::typeFromInfo(result) == ChordRecognizer::Type::DiminishedTriad);
    assert(ChordRecognizer::pitchFromInfo(result) == MidiNote::C);
    assert(ChordRecognizer::inversionFromInfo(result) == ChordRecognizer::Inversion::Root);
}

static void testRecognizer(
    SqArray<int, 16> chord,
    ChordRecognizer::Type expectedType,
    ChordRecognizer::Inversion expectedInversion,
    int expectedRoot) {
    auto const result = ChordRecognizer::recognize(chord);
    assert(ChordRecognizer::typeFromInfo(result) == expectedType);
    assert(ChordRecognizer::inversionFromInfo(result) == expectedInversion);
    assertEQ(ChordRecognizer::pitchFromInfo(result), expectedRoot);
}

static void testCMajMinRecognized() {
    SqArray<int, 16> chord = makeChord({MidiNote::C, MidiNote::E, MidiNote::G, MidiNote::B - 1});
    testRecognizer(
        chord,
        ChordRecognizer::Type::MajMinSeventh,
        ChordRecognizer::Inversion::Root,
        MidiNote::C);
}

static void testCMajMin9Recognized() {
    SqArray<int, 16> chord = makeChord({MidiNote::C, MidiNote::E, MidiNote::G, MidiNote::B - 1, MidiNote::D + 12});
    testRecognizer(
        chord,
        ChordRecognizer::Type::MajMinNinth,
        ChordRecognizer::Inversion::Root,
        MidiNote::C);
}

static void testCMajMin95SRecognized() {
    SqArray<int, 16> chord = makeChord({MidiNote::C, MidiNote::E, MidiNote::B - 1, MidiNote::D + 12});
    testRecognizer(
        chord,
        ChordRecognizer::Type::MajMinNinth,
        ChordRecognizer::Inversion::Root,
        MidiNote::C);
}

static void testCMajMinFirstInversionRecognized() {
    SqArray<int, 16> chord = makeChord({MidiNote::C + 12, MidiNote::E, MidiNote::G, MidiNote::B - 1});
    testRecognizer(
        chord,
        ChordRecognizer::Type::MajMinSeventh,
        ChordRecognizer::Inversion::First,
        MidiNote::C);
}

static void testCMajMajFirstInversionRecognized() {
    SqArray<int, 16> chord = makeChord({MidiNote::C + 12, MidiNote::E, MidiNote::G, MidiNote::B});
    testRecognizer(
        chord,
        ChordRecognizer::Type::MajMajSeventh,
        ChordRecognizer::Inversion::First,
        MidiNote::C);
}

static void testCMajMajSecondInversionRecognized() {
    SqArray<int, 16> chord = makeChord({MidiNote::C + 12, MidiNote::E + 12, MidiNote::G, MidiNote::B});
    testRecognizer(
        chord,
        ChordRecognizer::Type::MajMajSeventh,
        ChordRecognizer::Inversion::Second,
        MidiNote::C);
}

static void testCMajMajThirdInversionRecognized() {
    SqArray<int, 16> chord = makeChord({MidiNote::C + 12, MidiNote::E + 12, MidiNote::G + 12, MidiNote::B});
    testRecognizer(
        chord,
        ChordRecognizer::Type::MajMajSeventh,
        ChordRecognizer::Inversion::Third,
        MidiNote::C);
}
static void testCMajMinSecondInversionRecognized() {
    SqArray<int, 16> chord = makeChord({MidiNote::C + 12, MidiNote::E + 12, MidiNote::G, MidiNote::B - 1});
    testRecognizer(
        chord,
        ChordRecognizer::Type::MajMinSeventh,
        ChordRecognizer::Inversion::Second,
        MidiNote::C);
}

static void testCMinorFirstInversion() {
    SqArray<int, 16> chord = makeChord({MidiNote::C + 12, MidiNote::E - 1, MidiNote::G});
    testRecognizer(
        chord,
        ChordRecognizer::Type::MinorTriad,
        ChordRecognizer::Inversion::First,
        MidiNote::C);
}

static void testCMajMajRecognized() {
    SqArray<int, 16> chord = makeChord({MidiNote::C, MidiNote::E, MidiNote::G, MidiNote::B});
    testRecognizer(
        chord,
        ChordRecognizer::Type::MajMajSeventh,
        ChordRecognizer::Inversion::Root,
        MidiNote::C);
}

static void testCMajMaj9Recognized() {
    SqArray<int, 16> chord = makeChord({MidiNote::C, MidiNote::E, MidiNote::G, MidiNote::B, MidiNote::D + 12});
    testRecognizer(
        chord,
        ChordRecognizer::Type::MajMajNinth,
        ChordRecognizer::Inversion::Root,
        MidiNote::C);
}

static void testCMinMinRecognized() {
    SqArray<int, 16> chord = makeChord({MidiNote::C, MidiNote::E - 1, MidiNote::G, MidiNote::B - 1});
    testRecognizer(
        chord,
        ChordRecognizer::Type::MinMinSeventh,
        ChordRecognizer::Inversion::Root,
        MidiNote::C);
}

static void testCMinMin9Recognized() {
    SqArray<int, 16> chord = makeChord({MidiNote::C, MidiNote::E - 1, MidiNote::G, MidiNote::B - 1, MidiNote::D + 12});
    testRecognizer(
        chord,
        ChordRecognizer::Type::MinMinNinth,
        ChordRecognizer::Inversion::Root,
        MidiNote::C);
}

static void testCMinMajRecognized() {
    SqArray<int, 16> chord = makeChord({MidiNote::C, MidiNote::E - 1, MidiNote::G, MidiNote::B});
    testRecognizer(
        chord,
        ChordRecognizer::Type::MinMajSeventh,
        ChordRecognizer::Inversion::Root,
        MidiNote::C);
}

static void testCMinMaj9Recognized() {
    SqArray<int, 16> chord = makeChord({MidiNote::C, MidiNote::E - 1, MidiNote::G, MidiNote::B, MidiNote::D + 12});
    testRecognizer(
        chord,
        ChordRecognizer::Type::MinMajNinth,
        ChordRecognizer::Inversion::Root,
        MidiNote::C);
}

static void testCAugRecognized() {
    ChordRecognizer ch;
    SqArray<int, 16> chord = makeChord({MidiNote::C, MidiNote::E, MidiNote::G + 1});
    auto const result = ch.recognize(chord);

    assert(ChordRecognizer::typeFromInfo(result) == ChordRecognizer::Type::AugmentedTriad);
    assert(ChordRecognizer::pitchFromInfo(result) == MidiNote::C);
}

static void testCMinorRecognized() {
    ChordRecognizer ch;
    SqArray<int, 16> chord = makeChord({MidiNote::C, MidiNote::E - 1, MidiNote::G});
    auto const result = ch.recognize(chord);
    ChordRecognizer::Type t = std::get<0>(result);
    assert(ChordRecognizer::typeFromInfo(result) == ChordRecognizer::Type::MinorTriad);
    assert(ChordRecognizer::pitchFromInfo(result) == MidiNote::C);
}

static void testCDimRecognized() {
    ChordRecognizer ch;
    SqArray<int, 16> chord = makeChord({MidiNote::C, MidiNote::E - 1, MidiNote::G - 1});
    auto const result = ch.recognize(chord);
    ChordRecognizer::Type t = std::get<0>(result);
    assert(ChordRecognizer::typeFromInfo(result) == ChordRecognizer::Type::DiminishedTriad);
    assert(ChordRecognizer::pitchFromInfo(result) == MidiNote::C);
}
static void testCSus4Recognized() {
    ChordRecognizer ch;
    SqArray<int, 16> chord = makeChord({MidiNote::C, MidiNote::E + 1, MidiNote::G});
    auto const result = ch.recognize(chord);
    ChordRecognizer::Type t = std::get<0>(result);
    assert(ChordRecognizer::typeFromInfo(result) == ChordRecognizer::Type::Sus4Triad);
    assert(ChordRecognizer::pitchFromInfo(result) == MidiNote::C);
}

static void testCSus2Recognized() {
    ChordRecognizer ch;
    SqArray<int, 16> chord = makeChord({MidiNote::C, MidiNote::D, MidiNote::G});
    auto const result = ch.recognize(chord);
    ChordRecognizer::Type t = std::get<0>(result);
    assert(ChordRecognizer::typeFromInfo(result) == ChordRecognizer::Type::Sus2Triad);
    assert(ChordRecognizer::pitchFromInfo(result) == MidiNote::C);
}

static void testDMajorRecognized() {
    ChordRecognizer ch;
    SqArray<int, 16> chord = makeChord({MidiNote::D, MidiNote::F + 1, MidiNote::A});
    auto const result = ch.recognize(chord);

    assert(ChordRecognizer::typeFromInfo(result) == ChordRecognizer::Type::MajorTriad);
    assert(ChordRecognizer::pitchFromInfo(result) == MidiNote::D);
}

static void testCMajorOctaveRecognized() {
    ChordRecognizer ch;
    SqArray<int, 16> chord = makeChord({MidiNote::C + 12, MidiNote::E + 12, MidiNote::G + 12});
    auto const result = ch.recognize(chord);
    ChordRecognizer::Type t = std::get<0>(result);
    assert(ChordRecognizer::typeFromInfo(result) == ChordRecognizer::Type::MajorTriad);
    assert(ChordRecognizer::pitchFromInfo(result) == MidiNote::C);
}

static void testCMajorOrder() {
    ChordRecognizer ch;
    SqArray<int, 16> chord = makeChord({MidiNote::C, MidiNote::G, MidiNote::E});
    auto const result = ch.recognize(chord);
    ChordRecognizer::Type t = std::get<0>(result);
    assert(ChordRecognizer::typeFromInfo(result) == ChordRecognizer::Type::MajorTriad);
    assert(ChordRecognizer::pitchFromInfo(result) == MidiNote::C);
}

static void testCMajor4Voice() {
    SqArray<int, 16> chord = makeChord({MidiNote::C, MidiNote::E, MidiNote::G, MidiNote::C + 12});
    auto const result = ChordRecognizer::recognize(chord);
    ChordRecognizer::Type t = std::get<0>(result);
    assert(ChordRecognizer::typeFromInfo(result) == ChordRecognizer::Type::MajorTriad);
    assert(ChordRecognizer::pitchFromInfo(result) == MidiNote::C);
}

static void testGMajorRecognized() {
    ChordRecognizer ch;
    SqArray<int, 16> chord = makeChord({MidiNote::G, MidiNote::B, MidiNote::D + 12});
    auto const result = ch.recognize(chord);

    assert(ChordRecognizer::typeFromInfo(result) == ChordRecognizer::Type::MajorTriad);
    assert(ChordRecognizer::pitchFromInfo(result) == MidiNote::G);
}

static void testCMajorOneNoteCrazyOctave() {
    ChordRecognizer ch;
    SqArray<int, 16> chord = makeChord({MidiNote::C, MidiNote::E, MidiNote::G + 4 * 12});
    auto const result = ch.recognize(chord);
    assert(ChordRecognizer::typeFromInfo(result) == ChordRecognizer::Type::MajorTriad);
    assert(ChordRecognizer::pitchFromInfo(result) == MidiNote::C);
}

static void testCMajorFirstInversion() {
    ChordRecognizer ch;
    SqArray<int, 16> chord = makeChord({MidiNote::C + 12, MidiNote::E, MidiNote::G});
    auto const result = ch.recognize(chord);
    assert(ChordRecognizer::typeFromInfo(result) == ChordRecognizer::Type::MajorTriad);
    assert(ChordRecognizer::inversionFromInfo(result) == ChordRecognizer::Inversion::First);
    assertEQ(ChordRecognizer::pitchFromInfo(result), MidiNote::C);
}

static void testCSharpMajorFirstInversion() {
    ChordRecognizer ch;
    SqArray<int, 16> chord = makeChord({MidiNote::C + 12 + 1, MidiNote::E + 1, MidiNote::G + 1});
    auto const result = ch.recognize(chord);
    assert(ChordRecognizer::typeFromInfo(result) == ChordRecognizer::Type::MajorTriad);
    assert(ChordRecognizer::inversionFromInfo(result) == ChordRecognizer::Inversion::First);
    assertEQ(ChordRecognizer::pitchFromInfo(result), MidiNote::C + 1);
}

static void testGSharpMajorFirstInversion() {
    ChordRecognizer ch;
    SqArray<int, 16> chord = makeChord({MidiNote::G + 1, MidiNote::C, MidiNote::D + 1});  // G#, C D# is a major chord
    auto const result = ch.recognize(chord);
    assert(ChordRecognizer::typeFromInfo(result) == ChordRecognizer::Type::MajorTriad);
    assert(ChordRecognizer::inversionFromInfo(result) == ChordRecognizer::Inversion::First);
    assertEQ(ChordRecognizer::pitchFromInfo(result), MidiNote::G + 1);
}

static void testCMajorFirstInversion2() {
    ChordRecognizer ch;
    SqArray<int, 16> chord = makeChord({MidiNote::C + 12 + 48, MidiNote::E + 48, MidiNote::G + 48});
    auto const result = ch.recognize(chord);
    assert(ChordRecognizer::typeFromInfo(result) == ChordRecognizer::Type::MajorTriad);
    assert(ChordRecognizer::inversionFromInfo(result) == ChordRecognizer::Inversion::First);
    assertEQ(ChordRecognizer::pitchFromInfo(result), MidiNote::C);
}

static void testCMajorSecondInversion() {
    ChordRecognizer ch;
    SqArray<int, 16> chord = makeChord({MidiNote::C + 12, MidiNote::E + 12, MidiNote::G});
    auto const result = ch.recognize(chord);
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
    SqArray<int, 16> chord = makeChord({72, 72, 79});
    auto const result = ch.recognize(chord);
    assert(ChordRecognizer::typeFromInfo(result) == ChordRecognizer::Type::Unrecognized);
}

void testChordRecognizer() {
    testCopy();
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
    testChordRecognizer();
}
#endif