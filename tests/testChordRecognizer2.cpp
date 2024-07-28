

#include "ChordRecognizer.h"
#include "asserts.h"

using Type = ChordRecognizer::Type;
using Inversion = ChordRecognizer::Inversion;

static void testRecognizer(
    const SqArray<int, 16>& chord,
    Type expectedType,
    Inversion expectedInversion,
    int expectedRoot) {
    auto const result = ChordRecognizer::recognize(chord);
    assert(result.type == expectedType);
    assert(result.inversion == expectedInversion);
    assertEQ(result.pitch, expectedRoot);
}

static void testTypeAndChordInverted(Type expectedType, const SqArray<int, 16>& inputChord, int expectedRoot) {
    testRecognizer(inputChord, expectedType, Inversion::Root, expectedRoot);
    if (expectedType == Type::Unrecognized) {
        return;
    }
    if (expectedType == Type::Sus2Triad || expectedType == Type::Sus4Triad || expectedType == Type::AugmentedTriad) {
        return;
    }
    {
        // Inversions shouldn't move the root around, so expectedRoot is still valid
        SqArray<int, 16> invertedChord;
        ChordRecognizer::_copy(invertedChord, inputChord);
        invertedChord.putAt(0, invertedChord.getAt(0) + 12);
        testRecognizer(invertedChord, expectedType, Inversion::First, expectedRoot);

        // try inverted two octaves up.
        invertedChord.putAt(0, invertedChord.getAt(0) + 24);
        testRecognizer(invertedChord, expectedType, Inversion::First, expectedRoot);

        // Try second inversion
        ChordRecognizer::_copy(invertedChord, inputChord);
        invertedChord.putAt(0, invertedChord.getAt(0) + 12);
        invertedChord.putAt(1, invertedChord.getAt(1) + 12);
        testRecognizer(invertedChord, expectedType, Inversion::Second, expectedRoot);
    }
}

static void testTypeAndChordTransposed(int transpose, Type type, const SqArray<int, 16> inputChord) {
    // do the transpose
    SqArray<int, 16> chord;
    ChordRecognizer::_copy(chord, inputChord);
    for (unsigned i = 0; i < chord.numValid(); ++i) {
        chord.putAt(i, chord.getAt(i) + transpose);
    }
    int expectedRoot = (MidiNote::C + transpose) % 12;
    if (type == Type::Unrecognized) {
        expectedRoot = 0;
    }

    testTypeAndChordInverted(type, chord, expectedRoot);
}

static void testTypeAndChord(Type type, const SqArray<int, 16>& chord) {
    for (int i = 0; i < 24; ++i) {
        testTypeAndChordTransposed(i, type, chord);
    }

    for (int i = 0; i < 24; ++i) {
        testTypeAndChordTransposed(i + 70, type, chord);
    }
}

static SqArray<int, 16> makeChord(std::vector<int> input) {
    SqArray<int, 16> chord;
    int i = 0;
    for (auto x : input) {
        chord.putAt(i++, x);
    }
    return chord;
}

static void generalChordTestType(ChordRecognizer::Type type) {
    SqArray<int, 16> chord;

    switch (type) {
        case Type::MajorTriad: {  // only real one, so far
            SqArray<int, 16> chord2 = makeChord({MidiNote::C, MidiNote::E, MidiNote::G});
            ChordRecognizer::_copy(chord, chord2);
        } break;
        case Type::MinorTriad: {
            SqArray<int, 16> chord2 = makeChord({MidiNote::C, MidiNote::E - 1, MidiNote::G});
            ChordRecognizer::_copy(chord, chord2);
        } break;
        case Type::Sus2Triad: {
            SqArray<int, 16> chord2 = makeChord({MidiNote::C, MidiNote::D, MidiNote::G});
            ChordRecognizer::_copy(chord, chord2);
        } break;
        case Type::Sus4Triad: {
            SqArray<int, 16> chord2 = makeChord({MidiNote::C, MidiNote::F, MidiNote::G});
            ChordRecognizer::_copy(chord, chord2);
        } break;
        case Type::AugmentedTriad: {
            SqArray<int, 16> chord2 = makeChord({MidiNote::C, MidiNote::E, MidiNote::G + 1});
            ChordRecognizer::_copy(chord, chord2);
        } break;
        case Type::DiminishedTriad: {
            SqArray<int, 16> chord2 = makeChord({MidiNote::C, MidiNote::E - 1, MidiNote::G - 1});
            ChordRecognizer::_copy(chord, chord2);
        } break;
        case Type::MajMajSeventh: {
            SqArray<int, 16> chord2 = makeChord({MidiNote::C, MidiNote::E, MidiNote::G, MidiNote::B});
            ChordRecognizer::_copy(chord, chord2);
        } break;
        case Type::MajMinSeventh: {
            SqArray<int, 16> chord2 = makeChord({MidiNote::C, MidiNote::E, MidiNote::G, MidiNote::B - 1});
            ChordRecognizer::_copy(chord, chord2);
        } break;
        case Type::MinMajSeventh: {
            SqArray<int, 16> chord2 = makeChord({MidiNote::C, MidiNote::E - 1, MidiNote::G, MidiNote::B});
            ChordRecognizer::_copy(chord, chord2);
        } break;
        case Type::MinMinSeventh: {
            SqArray<int, 16> chord2 = makeChord({MidiNote::C, MidiNote::E - 1, MidiNote::G, MidiNote::B - 1});
            ChordRecognizer::_copy(chord, chord2);
        } break;

        case Type::Unrecognized: {
            SqArray<int, 16> chord2 = makeChord({MidiNote::C, MidiNote::C + 1, MidiNote::D});
            ChordRecognizer::_copy(chord, chord2);
        } break;
        default:
            assert(false);
    }
    testTypeAndChord(type, chord);
}

static void generalChordTest() {
    for (int i = 0; i <= int(ChordRecognizer::Type::MinMajSeventh); ++i) {
        generalChordTestType(ChordRecognizer::Type(i));
    }
}

void testChordRecognizer2() {
    generalChordTest();
}

#if 0
void testFirst() {
    generalChordTest();
}
#endif