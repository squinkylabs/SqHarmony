

#include "ChordRecognizer.h"
#include "asserts.h"

extern void show(const char* msg, const int* p, int num);

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

  //  show("testRecognizer", chord, length);
  //  SQINFO("type = %d", int(expectedType));
    auto const result = ChordRecognizer::recognize(chord, length);
    assert(ChordRecognizer::typeFromInfo(result) == expectedType);
    assert(ChordRecognizer::inversionFromInfo(result) == expectedInversion);
    assertEQ(ChordRecognizer::pitchFromInfo(result), expectedRoot);
}

static void testTypeAndChordTransposed(int transpose, Type type, int* inputChord, unsigned size) {
    int chord[16];
    copy(chord, inputChord, size);
    for (unsigned i=0; i<size; ++i) {
        chord[i] += transpose;
    }
    int expectedRoot = (MidiNote::C + transpose) % 12;
    if (type == Type::Unrecognized) {
        expectedRoot = 0;
    }
    testRecognizer(
        chord,
        size,
        type,
        Inversion::Root,
        expectedRoot);
}

static void testTypeAndChord(Type type, int* chord, unsigned size) {
    for (int i = 0; i < 24; ++i) {
      //  SQINFO("in loop, i=%d", i);
        testTypeAndChordTransposed(i, type, chord, size);
    //    testTypeAndChordTransposed(70 + i, type, chord, size);
    }
}

static void generalChordTestType(ChordRecognizer::Type type) {
    int chord[16];
    int size = 0;
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