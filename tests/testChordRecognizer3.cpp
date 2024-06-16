

#include "ChordRecognizer.h"
#include "asserts.h"

void testCMajor() {
    const int cmaj[] = { MidiNote::C, MidiNote::E, MidiNote::G };
    int test[16];

    ChordRecognizer::_makeCanonical(test, cmaj, 3);
    assertEQ(test[0], MidiNote::C);
}
void testChordRecognizer3() {
    testCMajor();
}


#if 1
void testFirst() {
    testCMajor();
}
#endif