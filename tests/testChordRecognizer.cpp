
#include "ChordRecognizer.h"

#include "asserts.h"

static void testCanCreate() {
  //  ChordRecognizer ch;
    const int chord[] = { 1,2,3,-1 };
    ChordRecognizer::recognize(chord);
}

static void testJunkNotRecognized() {
    ChordRecognizer ch;
    const int chord[] = { 1,2,3,-1 };
    auto const result = ch.recognize(chord);
    ChordRecognizer::Type t = std::get<0>(result);
    assert(t == ChordRecognizer::Type::Unrecognized);
}

static void testCMajorRecognized() {
    ChordRecognizer ch;
    const int chord[] = { MidiNote::C, MidiNote::E, MidiNote::G, -1 };
    auto const result = ch.recognize(chord);
    ChordRecognizer::Type t = std::get<0>(result);
    assert(t == ChordRecognizer::Type::MajorTriad);
    assert(std::get<1>(result) == MidiNote::C);
}

static void testDMajorRecognized() {
    ChordRecognizer ch;
    const int chord[] = { MidiNote::D, MidiNote::F+1, MidiNote::A, -1 };
    auto const result = ch.recognize(chord);
    ChordRecognizer::Type t = std::get<0>(result);
    assert(t == ChordRecognizer::Type::MajorTriad);
    assert(std::get<1>(result) == MidiNote::D);
}

static void testCMajorOctaveRecognized() {
    ChordRecognizer ch;
    const int chord[] = { MidiNote::C+12, MidiNote::E+12, MidiNote::G+12, -1 };
    auto const result = ch.recognize(chord);
    ChordRecognizer::Type t = std::get<0>(result);
    assert(t == ChordRecognizer::Type::MajorTriad);
    assert(std::get<1>(result) == MidiNote::C);
}

static void testCMajorOrder() {
    ChordRecognizer ch;
    const int chord[] = { MidiNote::C, MidiNote::G, MidiNote::E, -1 };
    auto const result = ch.recognize(chord);
    ChordRecognizer::Type t = std::get<0>(result);
    assert(t == ChordRecognizer::Type::MajorTriad);
    assert(std::get<1>(result) == MidiNote::C);
}

static void testCMajor4Voice() {
 //   ChordRecognizer ch;
    const int chord[] = { MidiNote::C, MidiNote::E, MidiNote::G, MidiNote::C + 12, -1 };
    auto const result = ChordRecognizer::recognize(chord);
    ChordRecognizer::Type t = std::get<0>(result);
    assert(t == ChordRecognizer::Type::MajorTriad);
    assert(std::get<1>(result) == MidiNote::C);
}

static void testGMajorRecognized() {
    ChordRecognizer ch;
    const int chord[] = { MidiNote::G, MidiNote::B, MidiNote::D+12, -1 };
    auto const result = ch.recognize(chord);
    ChordRecognizer::Type t = std::get<0>(result);
    assert(t == ChordRecognizer::Type::MajorTriad);
    assert(std::get<1>(result) == MidiNote::G);
}

static void testCMajorOneNoteCrazyOctave() {
    ChordRecognizer ch;
    const int chord[] = { MidiNote::C, MidiNote::E, MidiNote::G + 4 * 12, -1 };
    auto const result = ch.recognize(chord);
    ChordRecognizer::Type t = std::get<0>(result);
    assert(t == ChordRecognizer::Type::MajorTriad);
    assert(std::get<1>(result) == MidiNote::C);
}

static void testCMajorFirstInversion() {
    ChordRecognizer ch;
    const int chord[] = { MidiNote::C + 12, MidiNote::E, MidiNote::G, -1 };
    auto const result = ch.recognize(chord);
    ChordRecognizer::Type t = std::get<0>(result);


    assert(t == ChordRecognizer::Type::MajorTriadFirstInversion);
    assert(std::get<1>(result) == MidiNote::C);
}

static void testToString() {
    const char * p = ChordRecognizer::toString( std::make_tuple(ChordRecognizer::Type::Unrecognized, 0));
    assertEQ(strlen(p), 0);
}

void testChordRecognizer() {
    testCanCreate();
    testJunkNotRecognized();
    testCMajorRecognized();
    testDMajorRecognized();
    testCMajorOctaveRecognized();
    testCMajorOrder();
    testCMajor4Voice();
    testGMajorRecognized();
    testCMajorOneNoteCrazyOctave();
    testCMajorFirstInversion();
    testToString();


    // To add:
    
    // chord inverted
    // non-major triads
    // other chord types
    // chords larger than an octave (9th)


}

#if 1
void testFirst() {
   // testCMajorRecognized();
   //  testCMajor4Voice();
   // testCMajorFirstInversion();
   testToString();
}
#endif