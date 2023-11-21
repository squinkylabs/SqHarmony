

#include "RawChordGenerator.h"
#include "asserts.h"

static StylePtr makeStyle() {
    return std::make_shared<Style>();
}

static void testCanCall() {
    RawChordGenerator ch;
    (void)ch;
}

static void assertChordInRange(const int* chord, const Style& style) {
    assertGE(chord[0], style.minBass());
    assertGE(chord[1], style.minTenor());
    assertGE(chord[2], style.minAlto());
    assertGE(chord[3], style.minSop());
    assertLE(chord[0], style.maxBass());
    assertLE(chord[1], style.maxTenor());
    assertLE(chord[2], style.maxAlto());
    assertLE(chord[3], style.maxSop());
}

static void copyChord(int* dest, const int* src) {
    for (int i = 0; i < 4; ++i) {
        dest[i] = src[i];
    }
}

class TestRawChordGenerator {
public:
    static void init() {
        int temp[RawChordGenerator::chordSize];
        auto style = makeStyle();
        RawChordGenerator::initialize(temp, *style);
        assertEQ(temp[RawChordGenerator::iSop], style->minSop());
        assertEQ(temp[RawChordGenerator::iAlto], style->minAlto());
        assertEQ(temp[RawChordGenerator::iTenor], style->minTenor());
        assertEQ(temp[RawChordGenerator::iBass], style->minBass());
    }

    static void nextChordInRange() {
        int temp[RawChordGenerator::chordSize];
        auto style = makeStyle();
        RawChordGenerator::initialize(temp, *style);
        RawChordGenerator::getNextChordInRange(temp, *style);
        assertEQ(temp[RawChordGenerator::iSop], style->minSop() + 1);
        assertEQ(temp[RawChordGenerator::iAlto], style->minAlto());
        assertEQ(temp[RawChordGenerator::iTenor], style->minTenor());
        assertEQ(temp[RawChordGenerator::iBass], style->minBass());
    }

    static void nextChordInRange2() {
        int temp[RawChordGenerator::chordSize];
        auto style = makeStyle();
        RawChordGenerator::initialize(temp, *style);
        int good = 0;
        bool done = false;
        while (!done) {
            const bool b = RawChordGenerator::getNextChordInRange(temp, *style);
            if (!b) {
                done = true;
            } else {
                assertChordInRange(temp, *style);
            }
            good++;
        }
        assert(good > 100000);
    }

    static void fixCrossingIfRequired() {
        int temp[RawChordGenerator::chordSize];
        auto style = makeStyle();
        RawChordGenerator::initialize(temp, *style);
        int good = 0;
        bool done = false;
        while (!done) {
            const bool b = RawChordGenerator::getNextChordInRange(temp, *style);
            if (!b) {
                done = true;
            } else {
                int otherTemp[4];
                copyChord(otherTemp, temp);
                RawChordGenerator::fixCrossingIfRequired(otherTemp, *style);
                assertChordInRange(temp, *style);
            }
            good++;
        }
    }
};

void testRawChordGenerator() {
    testCanCall();
    TestRawChordGenerator::init();
    TestRawChordGenerator::nextChordInRange();
    TestRawChordGenerator::nextChordInRange2();
    TestRawChordGenerator::fixCrossingIfRequired();
}