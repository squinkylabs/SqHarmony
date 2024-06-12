
#include "ChordRecognizer.h"

#include "asserts.h"

static void testCanCreate() {
  //  ChordRecognizer ch;
    const int chord[] = { 1,2,3 };
    ChordRecognizer::recognize(chord, 3);
}

static void testJunkNotRecognized() {
    ChordRecognizer ch;
    const int chord[] = { 1,2,3 };
    auto const result = ch.recognize(chord, 3);
    ChordRecognizer::Type t = std::get<0>(result);
    assert(t == ChordRecognizer::Type::Unrecognized);
}

static void testCMajorRecognized() {
    ChordRecognizer ch;
    const int chord[] = { MidiNote::C, MidiNote::E, MidiNote::G };
    auto const result = ch.recognize(chord, 3);
    ChordRecognizer::Type t = std::get<0>(result);
    assert(t == ChordRecognizer::Type::MajorTriad);
    assert(std::get<1>(result) == MidiNote::C);
}

static void testCMinorRecognized() {
    ChordRecognizer ch;
    const int chord[] = { MidiNote::C, MidiNote::E -1, MidiNote::G };
    auto const result = ch.recognize(chord, 3);
    ChordRecognizer::Type t = std::get<0>(result);
    assert(t == ChordRecognizer::Type::MinorTriad);
    assert(std::get<1>(result) == MidiNote::C);
}

static void testDMajorRecognized() {
    ChordRecognizer ch;
    const int chord[] = { MidiNote::D, MidiNote::F+1, MidiNote::A};
    auto const result = ch.recognize(chord, 3);
    ChordRecognizer::Type t = std::get<0>(result);
    assert(t == ChordRecognizer::Type::MajorTriad);
    assert(std::get<1>(result) == MidiNote::D);
}

static void testCMajorOctaveRecognized() {
    ChordRecognizer ch;
    const int chord[] = { MidiNote::C+12, MidiNote::E+12, MidiNote::G+12 };
    auto const result = ch.recognize(chord, 3);
    ChordRecognizer::Type t = std::get<0>(result);
    assert(t == ChordRecognizer::Type::MajorTriad);
    assert(std::get<1>(result) == MidiNote::C);
}

static void testCMajorOrder() {
    ChordRecognizer ch;
    const int chord[] = { MidiNote::C, MidiNote::G, MidiNote::E };
    auto const result = ch.recognize(chord, 3);
    ChordRecognizer::Type t = std::get<0>(result);
    assert(t == ChordRecognizer::Type::MajorTriad);
    assert(std::get<1>(result) == MidiNote::C);
}

static void testCMajor4Voice() {
 //   ChordRecognizer ch;
    const int chord[] = { MidiNote::C, MidiNote::E, MidiNote::G, MidiNote::C + 12 };
    auto const result = ChordRecognizer::recognize(chord, 4);
    ChordRecognizer::Type t = std::get<0>(result);
    assert(t == ChordRecognizer::Type::MajorTriad);
    assert(std::get<1>(result) == MidiNote::C);
}

static void testGMajorRecognized() {
    ChordRecognizer ch;
    const int chord[] = { MidiNote::G, MidiNote::B, MidiNote::D+12 };
    auto const result = ch.recognize(chord, 3);
    ChordRecognizer::Type t = std::get<0>(result);
    assert(t == ChordRecognizer::Type::MajorTriad);
    assert(std::get<1>(result) == MidiNote::G);
}

static void testCMajorOneNoteCrazyOctave() {
    ChordRecognizer ch;
    const int chord[] = { MidiNote::C, MidiNote::E, MidiNote::G + 4 * 12};
    auto const result = ch.recognize(chord, 3);
    ChordRecognizer::Type t = std::get<0>(result);
    assert(t == ChordRecognizer::Type::MajorTriad);
    assert(std::get<1>(result) == MidiNote::C);
}

static void testCMajorFirstInversion() {
    ChordRecognizer ch;
    const int chord[] = { MidiNote::C + 12, MidiNote::E, MidiNote::G };
    auto const result = ch.recognize(chord, 3);
    ChordRecognizer::Type t = std::get<0>(result);


    assert(t == ChordRecognizer::Type::MajorTriadFirstInversion);
    assert(std::get<1>(result) == MidiNote::C);
}

static void testToString() {
    std::string s = ChordRecognizer::toString( std::make_tuple(ChordRecognizer::Type::Unrecognized, 0));
    assertEQ(s.length(), 0);

    s = ChordRecognizer::toString( std::make_tuple(ChordRecognizer::Type::MajorTriad, 0));
    assertGT(s.length(), 0);

     s = ChordRecognizer::toString( std::make_tuple(ChordRecognizer::Type::MajorTriadFirstInversion, 0));
     assertGT(s.length(), 0);

     s = ChordRecognizer::toString( std::make_tuple(ChordRecognizer::Type::MinorTriad, 0));
     assertGT(s.length(), 0);

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
    testCMinorRecognized();

    // To add:
    
    // chord inverted
    // non-major triads
    // other chord types
    // chords larger than an octave (9th)


}

#if 0
void testFirst() {
   testCMajorRecognized();
   //  testCMajor4Voice();
   // testCMajorFirstInversion();
 //  testToString();
}
#endif