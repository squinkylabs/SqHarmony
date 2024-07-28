

#include "ChordRecognizer.h"
#include "asserts.h"


/**
 * More ad hoc tests for Chord Recognizer. Similar to getChordRecognizer, but that module is getting too big.
 */

#if 0
void testMajor() {
    SqArray<int, 16> cmaj ={MidiNote::C, MidiNote::E, MidiNote::G};
    SqArray<int, 16> transposed;

    for (int i = 0; i < 40; ++i) {
        for (int j = 0; j < 3; ++j) {
            transposed.putAt(j, cmaj.getAt(j) + i);
        }
        SqArray<int, 16> test;
        int transposeAmount = 0;
        const auto x = ChordRecognizer::_makeCanonical(test, transposed, transposeAmount);
        assertEQ(test.getAt(0), 0);
        assertEQ(test.getAt(1), 4);
        assertEQ(test.getAt(2), 7);
        assertEQ(test.numValid(), 3);
        assertEQ(transposeAmount, i);
    }
}

void testMajorOpen() {
    SqArray<int, 16> cmaj = {MidiNote::C, MidiNote::E, MidiNote::G};
    SqArray<int, 16> transposed;

    for (int i = 0; i < 40; ++i) {
        SqArray<int, 16> test;
        for (int j = 0; j < 3; ++j) {
            transposed.putAt(j, cmaj.getAt(j) + i);
        }

        // transpose the 3rd and the 5th, not the root. that would invert it.
        for (int k = 1; k < 3; ++k) {
            SqArray<int, 16> transposed2;
            ChordRecognizer::_copy(transposed2, transposed);
            transposed2.putAt(k, transposed2.getAt(k) + 48);

            int transposeAmount = 0;
            const bool canMakeCanonical = ChordRecognizer::_makeCanonical(test, transposed2, transposeAmount);
            assertEQ(test.getAt(0), 0);
            assertEQ(test.getAt(1), 4);
            assertEQ(test.getAt(2), 7);
            assertEQ(test.numValid(), 3);
            assertEQ(canMakeCanonical, true);
            assertEQ(transposeAmount, i);
        }
    }
}

static void testMajorFirstInversion() {
    SqArray<int, 16> cmajInverted = {MidiNote::C + 12, MidiNote::E, MidiNote::G};
    SqArray<int, 16> transposed;

    for (int i = 0; i < 1; ++i) {
        for (int j = 0; j < 3; ++j) {
            transposed.putAt(j, cmajInverted.getAt(j) + i);
        }
        SqArray<int, 16> test;
        int baseTranspose = 0;
        const auto x = ChordRecognizer::_makeCanonical(test, transposed, baseTranspose);
        assertEQ(test.getAt(0), 0);
        assertEQ(test.getAt(1), 3);
        assertEQ(test.getAt(2), 8);
        assertEQ(test.numValid(), 3);
        assertEQ(baseTranspose, i + 4);  // expect the third now
    }
}
#endif

#if 0
static void testMakeCanonicalT() {
    class Cls {
    public:
        Cls() {}
        Cls(int p, int i) : _pitch(p), _index(i) {}

        bool operator <(const Cls& other) { return this->_pitch < other._pitch;  }
        operator int() const { return _pitch;  }
        void operator=(int pitch) { this->_pitch = pitch; }

        int _pitch = 0;
        int _index = 0;
    };

    // Cls x[3];
    // x[0] = Cls(12, 1);
    // x[1] = Cls(22, 0);
    // x[2] = Cls(33, 2);
    SqArray<Cls, 16> inputChord;
    SqArray<Cls, 16> outputChord;
    inputChord.putAt(0, Cls(12, 1));
    inputChord.putAt(1, Cls(22, 0));
    inputChord.putAt(2, Cls(33, 2));

    int transposeAmount;
    ChordRecognizer::_makeCanonical(outputChord, inputChord, transposeAmount);
    assertEQ(outputChord.numValid(), 3);
    const auto a = outputChord.getAt(0);
    const auto b = outputChord.getAt(1);
    const auto c = outputChord.getAt(2);
    // assert(false);
    SQINFO("!finish testMakeCanonicalT");
}
#endif

// just a test for casting
static void testMisc() {
    int x = 5;

    class Cls {
    public:
        Cls() {}
        Cls(int p, int i) : _pitch(p), _index(i) {}
        operator int() const { return _pitch;  }
        int _pitch = 0;
        int _index = 0;
    };

    Cls y(55, 0);

    const int x2 = int(x);
    const int y2 = int(y);
    assertEQ(x2, 5);
    assertEQ(y2, 55);
}

 static void testCMajorWithReturns() {
    ChordRecognizer ch;
 //   SqArray<int, 16> chord = makeChord({MidiNote::C, MidiNote::E, MidiNote::G});
    SqArray<int, 16> chord = {MidiNote::C, MidiNote::E, MidiNote::G};
    auto const result = ch.recognize(chord);

    assert(result.type == ChordRecognizer::Type::MajorTriad);
    assert(result.pitch == MidiNote::C);
    assertEQ(result.identifiedPitches.numValid(), 3);
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
  //  testMajor();
 //   testMajorOpen();
 //   testMajorFirstInversion();
    testNormalizeIntPositive();
     testCMajorWithReturns();
 //   testMakeCanonicalT();
}

#if 0
void testFirst() {
    // Not working yet.
    // testMajorFirstInversion();
  
 //testMisc();
    testCMajorWithReturns();
}
#endif