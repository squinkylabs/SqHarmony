

#include "ChordRecognizer.h"
#include "asserts.h"


void testMajor() {
    const int cmaj[] = { MidiNote::C, MidiNote::E, MidiNote::G };
    int transposed[16];

    for (int i = 0; i < 40; ++i) {
        for (int j = 0; j < 3; ++j) {
            transposed[j] = cmaj[j] + i;
        }
        int test[16];
        const auto x = ChordRecognizer::_makeCanonical(test, transposed, 3);
        assertEQ(test[0], 0);
        assertEQ(test[1], 4);
        assertEQ(test[2], 7);
        assertEQ(std::get<0>(x), 3);
        assertEQ(std::get<1>(x), i);
    }
}

void testMajorOpen() {
    const int cmaj[] = { MidiNote::C, MidiNote::E, MidiNote::G };
    int transposed[16];

    for (int i = 0; i < 40; ++i) {
        int test[16];
        for (int j = 0; j < 3; ++j) {
            transposed[j] = cmaj[j] + i;
        }
       
        // transpose the 3rd and the 5th, not the root. that would invert it.
        for (int k = 1; k < 3; ++k) {
            int transposed2[16];
            ChordRecognizer::copy(transposed2, transposed, 3);
            transposed2[k] += 48;
            const auto x = ChordRecognizer::_makeCanonical(test, transposed2, 3);
            assertEQ(test[0], 0);
            assertEQ(test[1], 4);
            assertEQ(test[2], 7);
            assertEQ(std::get<0>(x), 3);
            assertEQ(std::get<1>(x), i);
        }
    }
}

static void testMajorFirstInversion() {
    const int cmajInverted[] = { MidiNote::C + 12, MidiNote::E, MidiNote::G };
    int transposed[16];

    for (int i = 0; i < 1; ++i) {
        for (int j = 0; j < 3; ++j) {
            transposed[j] = cmajInverted[j] + i;
        }
        int test[16];
        const auto x = ChordRecognizer::_makeCanonical(test, transposed, 3);
        assertEQ(test[0], 0);
        assertEQ(test[1], 3);
        assertEQ(test[2], 8);
        assertEQ(std::get<0>(x), 3);
        assertEQ(std::get<1>(x), i + 4);        // expect the third now
    }
}


static void testNormalizeIntPositive() {
    assertEQ(ChordRecognizer::normalizeIntPositive(0, 12), 0);
    assertEQ(ChordRecognizer::normalizeIntPositive(5, 12), 5);
    assertEQ(ChordRecognizer::normalizeIntPositive(12, 12), 0);
    assertEQ(ChordRecognizer::normalizeIntPositive(110, 100), 10);
    assertEQ(ChordRecognizer::normalizeIntPositive(10010, 100), 10);

    assertEQ(ChordRecognizer::normalizeIntPositive(-4, 12), 8);
    assertEQ(ChordRecognizer::normalizeIntPositive(-10010, 100), 90);
}

void testChordRecognizer3() {
    testMajor();
    testMajorOpen();
    testMajorFirstInversion();
    testNormalizeIntPositive();

}


#if 0
void testFirst() {
    // Not working yet.
    testMajorFirstInversion();
}
#endif