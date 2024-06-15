

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
    auto const result = ChordRecognizer::recognize(chord, length);
    assert(ChordRecognizer::typeFromInfo(result) == expectedType);
    assert(ChordRecognizer::inversionFromInfo(result) == expectedInversion);
    assertEQ(ChordRecognizer::pitchFromInfo(result), expectedRoot);
}

static void testTypeAndChord(Type type, int* chord, unsigned size) {
    testRecognizer(
        chord,
        size,
        type,
        Inversion::Root,
        MidiNote::C
    );
}


static void generalChordTestType(ChordRecognizer::Type type) {
    int chord[16];
    int size = 0;
    switch (type) {
        case Type::MajorTriad: { // only real one, so far
            int chord2[] = {MidiNote::C, MidiNote::E, MidiNote::G};
            size = 3;
            copy(chord, chord2, 3);
        } break;
        case Type::MinorTriad: {
            int chord2[] = {MidiNote::C, MidiNote::E-1, MidiNote::G};
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
            int chord2[] = {MidiNote::C, MidiNote::E, MidiNote::G};
            size = 3;
            copy(chord, chord2, 3);
        } break;
        case Type::DiminishedTriad: {
            int chord2[] = {MidiNote::C, MidiNote::E, MidiNote::G};
            size = 3;
            copy(chord, chord2, 3);
        } break;

        case Type::MajMajSeventh: {
            int chord2[] = {MidiNote::C, MidiNote::E, MidiNote::G};
            size = 3;
            copy(chord, chord2, 3);
        } break;
        case Type::MajMinSeventh: {
            int chord2[] = {MidiNote::C, MidiNote::E, MidiNote::G};
            size = 3;
            copy(chord, chord2, 3);
        } break;
        case Type::MinMajSeventh: {
            int chord2[] = {MidiNote::C, MidiNote::E, MidiNote::G};
            size = 3;
            copy(chord, chord2, 3);
        } break;
        case Type::MinMinSeventh: {
            int chord2[] = {MidiNote::C, MidiNote::E, MidiNote::G};
            size = 3;
            copy(chord, chord2, 3);
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