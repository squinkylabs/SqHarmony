
#include "KeysigOld.h"
#include "RawChordGenerator.h"
#include "asserts.h"

static StylePtr makeStyle() {
    return std::make_shared<Style>();
}

static KeysigOldPtr makeKeysig(bool minor) {
    auto ret = std::make_shared<KeysigOld>(Roots::C);
    if (minor) {
        ret->set(MidiNote::C, Scale::Scales::Minor);
    }
    return ret;
}

static Options makeOptions(bool minor) {
    Options o(makeKeysig(minor), makeStyle());
    return o;
}

static void testCanCall() {
    const Options options = makeOptions(false);
    RawChordGenerator ch(options);
    ch.getNextChord();
}

static void testCanGen() {
    const Options options = makeOptions(false);
    RawChordGenerator ch(options);
    const bool b = ch.getNextChord();
    assert(b);
}

static void testCanGen2() {
    const Options options = makeOptions(false);
    RawChordGenerator ch(options);
    int count = 0;
    bool done = false;
    while (!done) {
        const bool b = ch.getNextChord();
        if (b) {
            ++count;
        }
        else {
            done = true;
        }
    }
    assert(count > 10000);
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
                assertChordInRange(otherTemp, *style);
                assert(otherTemp[RawChordGenerator::iTenor] > otherTemp[RawChordGenerator::iBass]);
                assert(otherTemp[RawChordGenerator::iAlto] > otherTemp[RawChordGenerator::iTenor]);
                assert(otherTemp[RawChordGenerator::iSop] > otherTemp[RawChordGenerator::iAlto]);
            }
        }
    }

    static void isChordOkSub(std::initializer_list<int> x, bool expected) {
        int temp[RawChordGenerator::chordSize];
        int index = 0;
        for (int i : x) {
            temp[index++] = i;
        }
        //auto style = makeStyle();
        auto options = makeOptions(false);      // cmaj

        const bool b = RawChordGenerator::isChordOk(temp, options);
        assertEQ(b, expected);

    }
    static void isChordOk() {
        isChordOkSub({ 72, 73, 74, 75 }, false);
        isChordOkSub({ 72, 72 + 4, 72 + 7, 72 + 12 }, true);
    }

    static void getSRN() {
        auto options = makeOptions(false);      // cmaj
        int harmony[4] = { 72, 72 + 4, 72 + 7, 72 + 12 };   // cmaj chord
        const auto x = RawChordGenerator::getSRN(harmony, options);
        assertEQ(x.size(), RawChordGenerator::chordSize);
        for (auto srn : x) {
            assert(srn.isValid());
        }
        assertEQ(x[0].getScaleDegree(), 1);
        assertEQ(x[1].getScaleDegree(), 3);
        assertEQ(x[2].getScaleDegree(), 5);
        assertEQ(x[3].getScaleDegree(), 1);

        assertEQ(x[0].isTonal(), true);
        assertEQ(x[1].isTonal(), false);
        assertEQ(x[2].isTonal(), true);
        assertEQ(x[3].isTonal(), true);

        assertEQ(x[0].isLeadingTone(), false);
        assertEQ(x[1].isLeadingTone(), false);
        assertEQ(x[2].isLeadingTone(), false);
        assertEQ(x[3].isLeadingTone(), false);
    }

    static void allNotesInScale() {
        auto options = makeOptions(false);      // cmaj

        int chord[4] = { 72, 72 + 1, 72 + 2, 72 + 3 };   // chormatic chord
        std::vector<ScaleRelativeNote> scaleRelativeNotes = RawChordGenerator::getSRN(chord, options);
        const bool b = RawChordGenerator::allNotesInScale(scaleRelativeNotes);
        assert(!b);
    }

    static void allNotesInScale2() {
        auto options = makeOptions(false);      // cmaj

        int chord[4] = { 72, 72 + 4, 72 + 7, 72 + 12 };   // cmaj chord
        std::vector<ScaleRelativeNote> scaleRelativeNotes = RawChordGenerator::getSRN(chord, options);
        const bool b = RawChordGenerator::allNotesInScale(scaleRelativeNotes);
        assert(b);
    }
};

void testRawChordGenerator() {
    testCanCall();
    testCanGen();
    testCanGen2();
    TestRawChordGenerator::init();
    TestRawChordGenerator::nextChordInRange();
    TestRawChordGenerator::nextChordInRange2();
    TestRawChordGenerator::fixCrossingIfRequired();
    TestRawChordGenerator::getSRN();
    TestRawChordGenerator::allNotesInScale();
    TestRawChordGenerator::allNotesInScale2();
    TestRawChordGenerator::isChordOk();
}