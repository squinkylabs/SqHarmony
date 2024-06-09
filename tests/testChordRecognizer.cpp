
#include "ChordRecognizer.h"

#include "asserts.h"

static void testCanCreate() {
    ChordRecognizer ch;
    const int chord[] = { 1,2,3,-1 };
    ch.recognize(chord);
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

void testChordRecognizer() {
    testCanCreate();
    testJunkNotRecognized();
    testCMajorRecognized();
}

void testFirst() {
    testCMajorRecognized();
}