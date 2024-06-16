

#include "ChordRecognizer.h"
#include "asserts.h"

using Type = ChordRecognizer::Type;
using Inversion = ChordRecognizer::Inversion;

static void copy(int* dest, const int* src, unsigned length) {
    while (length--) {
        *dest++ = *src++;
    }
}

static void testRecognizer(
    const int* chord,
    unsigned length,
    Type expectedType,
    Inversion expectedInversion,
    int expectedRoot) {
    // show("testRecognizer", chord, length);
    // SQINFO("type = %d", int(expectedType));
    auto const result = ChordRecognizer::recognize(chord, length);
    assert(ChordRecognizer::typeFromInfo(result) == expectedType);
    assert(ChordRecognizer::inversionFromInfo(result) == expectedInversion);
    assertEQ(ChordRecognizer::pitchFromInfo(result), expectedRoot);
}

static void testTypeAndChordInverted(Type expectedType, const int* inputChord, unsigned size, int expectedRoot) {
    testRecognizer(inputChord, size, expectedType, Inversion::Root, expectedRoot);
    if (expectedType == Type::Unrecognized) {
        return;
    }
    if (expectedType == Type::Sus2Triad || expectedType == Type::Sus4Triad || expectedType == Type::AugmentedTriad) {
        // SQINFO("these chords can't be inverted");
        return;
    }
#if 1
    {
        int invertedChord[16];
        copy(invertedChord, inputChord, size);
        invertedChord[0] += 12;
        testRecognizer(invertedChord, size, expectedType, Inversion::First, expectedRoot);
    }
#endif
}

static void testTypeAndChordTransposed(int transpose, Type type, const int* inputChord, unsigned size) {
    // do the transpose
    int chord[16];
    copy(chord, inputChord, size);
    for (unsigned i = 0; i < size; ++i) {
        chord[i] += transpose;
    }
    int expectedRoot = (MidiNote::C + transpose) % 12;
    if (type == Type::Unrecognized) {
        expectedRoot = 0;
    }

    testTypeAndChordInverted(type, chord, size, expectedRoot);

// Test
#if 0
    testRecognizer(
        chord,
        size,
        type,
        Inversion::Root,
        expectedRoot);

    // Now try some inversions
    // assume chords are already sorted 1,3,5
    {
        int invertedChord[16];
        copy(invertedChord, chors, size);
        invertedChord[0] += 12;


    }
#endif
}

static void testTypeAndChord(Type type, const int* chord, unsigned size) {
    for (int i = 0; i < 24; ++i) {
        testTypeAndChordTransposed(i, type, chord, size);
    }

    for (int i = 0; i < 24; ++i) {
        testTypeAndChordTransposed(i + 70, type, chord, size);
    }
}

static void generalChordTestType(ChordRecognizer::Type type) {
    int chord[16];
    int size = 0;

    // SQINFO("type = %d", int(type));
    switch (type) {
        case Type::MajorTriad: {  // only real one, so far
            int chord2[] = {MidiNote::C, MidiNote::E, MidiNote::G};
            size = 3;
            copy(chord, chord2, 3);
        } break;
        case Type::MinorTriad: {
            int chord2[] = {MidiNote::C, MidiNote::E - 1, MidiNote::G};
            size = 3;
            copy(chord, chord2, 3);
        } break;
        case Type::Sus2Triad: {
            int chord2[] = {MidiNote::C, MidiNote::D, MidiNote::G};
            size = 3;
            copy(chord, chord2, 3);
        } break;
        case Type::Sus4Triad: {
            int chord2[] = {MidiNote::C, MidiNote::F, MidiNote::G};
            size = 3;
            copy(chord, chord2, 3);
        } break;
        case Type::AugmentedTriad: {
            int chord2[] = {MidiNote::C, MidiNote::E, MidiNote::G + 1};
            size = 3;
            copy(chord, chord2, 3);
        } break;
        case Type::DiminishedTriad: {
            int chord2[] = {MidiNote::C, MidiNote::E - 1, MidiNote::G - 1};
            size = 3;
            copy(chord, chord2, 3);
        } break;

        case Type::MajMajSeventh: {
            int chord2[] = {MidiNote::C, MidiNote::E, MidiNote::G, MidiNote::B};
            size = 4;
            copy(chord, chord2, 4);
        } break;
        case Type::MajMinSeventh: {
            int chord2[] = {MidiNote::C, MidiNote::E, MidiNote::G, MidiNote::B - 1};
            size = 4;
            copy(chord, chord2, 4);
        } break;
        case Type::MinMajSeventh: {
            int chord2[] = {MidiNote::C, MidiNote::E - 1, MidiNote::G, MidiNote::B};
            size = 4;
            copy(chord, chord2, 4);
        } break;
        case Type::MinMinSeventh: {
            int chord2[] = {MidiNote::C, MidiNote::E - 1, MidiNote::G, MidiNote::B - 1};
            size = 4;
            copy(chord, chord2, 4);
        } break;

        case Type::Unrecognized: {
            int chord2[] = {MidiNote::C, MidiNote::C + 1, MidiNote::D};
            size = 3;
            copy(chord, chord2, 3);
        } break;
        default:
            assert(false);
    }
    testTypeAndChord(type, chord, size);
}

static void generalChordTest() {
    for (int i = 0; i <= int(ChordRecognizer::Type::MinMajSeventh); ++i) {
        generalChordTestType(ChordRecognizer::Type(i));
    }
}

void testChordRecognizer2() {
    generalChordTest();
}

#if 1
void testFirst() {
    generalChordTest();
}
#endif