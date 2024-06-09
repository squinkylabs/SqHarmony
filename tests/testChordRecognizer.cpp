
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

void testChordRecognizer() {
    testCanCreate();
    testJunkNotRecognized();
    testCMajorRecognized();
    testDMajorRecognized();
}

void testFirst() {
    testDMajorRecognized();
}