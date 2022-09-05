
#include "Chord4.h"
#include "Chord4List.h"
#include "HarmonyNote.h"
#include "KeysigOld.h"
#include "Options.h"
#include "ScaleRelativeNote.h"
#include "asserts.h"

static bool verifyChordContains(const Chord4* chord, std::vector<int> degrees) {
    const ScaleRelativeNote* srn = chord->fetchSRNNotes();
    for (int i = 0; i < 4; ++i) {
        const int degree = srn[i];
        const int ct = int(std::count(degrees.begin(), degrees.end(), degree));
        if (ct != 1) {
            return false;
        }
    }
    return true;
}

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

static void test0(bool minor) {
    Options o = makeOptions(minor);
    assert(__numChord4 == 0);
    Chord4 x(o, 1);
    assert(__numChord4 == 1);
}

static void test0() {
    test0(false);
    test0(true);
}

static void testConstructor(bool minor) {
    Options o = makeOptions(minor);
    Chord4 c(o, 1);
    assert(verifyChordContains(&c, {1, 3, 5}));

    Chord4 d(o, 2);
    assert(verifyChordContains(&d, {2, 4, 6}));

    Chord4 e(o, 3);
    assert(verifyChordContains(&e, {3, 5, 7}));

    Chord4 f(o, 4);
    assert(verifyChordContains(&f, {4, 6, 1}));

    Chord4 g(o, 5);
    assert(verifyChordContains(&g, {5, 7, 2}));

    Chord4 a(o, 6);
    assert(verifyChordContains(&a, {6, 1, 3}));

    Chord4 b(o, 7);
    assert(verifyChordContains(&b, {7, 2, 4}));
}

static void testConstructor() {
    testConstructor(false);
    testConstructor(true);
}

static void testRoot(bool minor) {
    Options o = makeOptions(minor);
    Chord4 x(o, 1);
    x.print();
    // printf("\n");
    assertEQ(x.fetchRoot(), 1);

    // TODO: assert on these or stop printing?
    auto notes = x.fetchSRNNotes();
    auto hn = x.fetchNotes();
    for (int i = 0; i < 4; ++i) {
        const int np = (int)notes[i];
        const int hnp = hn[i];
        // printf("chord[%d] = %d (srn), %d (midi)\n", i, (int)notes[i], (int)hn[i]);
    }
}

static void testRoot() {
    testRoot(false);
    testRoot(true);
}

static void testList(bool minor) {
    {
        Options o = makeOptions(minor);
        Chord4List l(o, 4);
        assert(__numChord4 > 10);
        assert(l.size() > 10);

        const Chord4* c = l.get2(3);
    }
    assert(__numChord4 == 0);
}

static void testList() {
    testList(false);
    testList(true);
}

static void testListSub(bool minor, int degree) {
    assert(degree >= 1);
    assert(degree <= 7);

    Options o = makeOptions(minor);

    // make a list of root chords
    Chord4List l(o, degree);
    const int size = l.size();
    const Chord4* last = nullptr;
    for (int i = 0; i < size; ++i) {
        const Chord4* c = l.get2(i);

        const int root = degree;
        int third = (degree + 2);
        if (third > 7) third -= 7;
        int fifth = (degree + 4);
        if (fifth > 7) fifth -= 7;

        assert(verifyChordContains(c, {root, third, fifth}));
    }
}

static void testList2(bool minor) {
    testListSub(minor, 1);
    for (int degree = 1; degree <= 7; ++degree) {
        testListSub(minor, degree);
    }
}

static void testList2() {
    testList2(false);
    testList2(true);
}

void specialDumpList(bool minor) {
    printf("-- special dump lists, all Am in CMaj\n");
    Options o = makeOptions(minor);
    const int degree = 6;
    // make a list of root chords
    Chord4List l(o, degree);

    const int size = l.size();

    for (int i = 0; i < size; ++i) {
        const Chord4* c = l.get2(i);
        if (i == 0) {
            printf("first abs = %d, %d, %d, %d\n",
                   int(c->fetchNotes()[0]),
                   int(c->fetchNotes()[1]),
                   int(c->fetchNotes()[2]),
                   int(c->fetchNotes()[3]));
        }
        assert(c->toStringShort() != "E2A2C3A3");
        printf("list chord[%d]=%s\n", i, c->toString().c_str());
    }

    printf("bass min=%d bass max=%d\n", o.style->minBass(), o.style->maxBass());
    printf("T min=%d T max=%d\n", o.style->minTenor(), o.style->maxTenor());
    printf("A min=%d A max=%d\n", o.style->minAlto(), o.style->maxAlto());
    printf("sap min=%d sop max=%d\n", o.style->minSop(), o.style->maxSop());
    assert(false);
}

static void testInversions(bool minor) {
    // generate a bunch of c major chords
    Options options = makeOptions(minor);
    Chord4 chord(options, 1);

    bool seenFirst = false;
    bool seenSecond = false;
    bool seenRoot = false;
    bool done = false;
    while (!done) {
        auto srn = chord.fetchSRNNotes();
        // TOTO: chech the chords ourself
        switch (chord.inversion(options)) {
            case ROOT_POS_INVERSION:
                seenRoot = true;
                assertEQ(int(srn[0]), 1);  // should have 1 in the bottom
                break;
            case FIRST_INVERSION:
                seenFirst = true;
                assertEQ(int(srn[0]), 3);  // should have 3 in the bottom
                break;
            case SECOND_INVERSION:
                seenSecond = true;
                assertEQ(int(srn[0]), 5);  // should have 5 in the bottom
                break;
            default:
                assert(false);
        }
        bool b = chord.makeNext(options);
        assert(!b);
        done = (seenFirst && seenSecond && seenRoot);
    }
}

static void testInversions() {
    testInversions(false);
    testInversions(true);
}

static void testFromString(bool minor) {
    Options options = makeOptions(minor);

    // get the string for the test chord on the 6th
    const char* target = minor ? "D#2G#2C3G#3" : "E2A2C3A3";
    Chord4Ptr chord = Chord4::fromString(options, 6, target);
    assert(chord);
    assertEQ(chord->toStringShort(), target);
}

// see if that C chord we need for 2-1 is acceptable
static void testFromString2(bool minor) {
    Options options = makeOptions(minor);
    const char* target = minor ? "D#2G2C3D#3" : "E2G2C3E3";
    Chord4Ptr chord = Chord4::fromString(options, 1, target);
    assert(chord);
    assertEQ(chord->toStringShort(), target);
}

static void testFromString() {
    testFromString(false);
    testFromString(true);
    testFromString2(false);
    testFromString2(true);
}

static void testRanges(bool minor) {
    int sizeNorm = 0;
    int sizeNarrow = 0;

    Options options = makeOptions(minor);
    Chord4ListPtr lNorm = std::make_shared<Chord4List>(options, 1);
    options.style->setRangesPreference(Style::Ranges::NARROW_RANGE);
    Chord4ListPtr lNarrow = std::make_shared<Chord4List>(options, 1);

    sizeNorm = lNorm->size();
    sizeNarrow = lNarrow->size();
    assert(sizeNarrow < sizeNorm);
}

static void testRanges() {
    testRanges(false);
    testRanges(true);
}

static void allGood(Chord4ListPtr list, StylePtr style) {
    for (int i = 0; i < list->size(); ++i) {
        const Chord4* chord = list->get2(i);
        const HarmonyNote* notes = chord->fetchNotes();

        const int bass = notes[0];
        assertGE(bass, style->minBass());
        assertLE(bass, style->maxBass());

        const int tenor = notes[1];
        assertGE(tenor, style->minTenor());
        assertLE(tenor, style->maxTenor());

        const int alto = notes[2];
        assertGE(alto, style->minAlto());
        assertLE(alto, style->maxAlto());

        const int soprano = notes[3];
        assertGE(soprano, style->minSop());
        assertLE(soprano, style->maxSop());
    }
}

static void testMinMax(bool minor) {
    Options options = makeOptions(minor);
    Chord4ListPtr lNorm = std::make_shared<Chord4List>(options, 1);  // root chord
    allGood(lNorm, options.style);
}

static void testMinMax() {
    testMinMax(false);
    testMinMax(true);
}

void testChord() {
    assert(__numChord4 == 0);
    test0();
    testRoot();
    testConstructor();
    testList();
    testList2();

    testInversions();
    // specialDumpList();
    // TODO: add more tests?
    testFromString();
    //   testFromString2();

    testRanges();
    testMinMax();

    assert(__numChord4 == 0);
}