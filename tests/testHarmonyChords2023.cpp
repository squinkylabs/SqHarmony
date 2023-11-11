
#include "Chord4Manager.h"
#include "HarmonyChords.h"
#include "KeysigOld.h"
#include "ProgressionAnalyzer.h"
#include "Style.h"
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

//---------------------
extern bool _globalShow;

static void assertChordExists(Chord4Manager& mgr, int root, const Options& options, const std::string& expected, bool show=false) {
    Chord4List* chords = mgr._getChords(root);
    assert(chords);

    _globalShow = show;
    Chord4Ptr desiredChord = Chord4::fromString(options, root, expected.c_str());
    assert(desiredChord);
    assert(desiredChord->isValid());
    _globalShow = false;

    int timesChordFound = 0;
    for (int i = 0; i < chords->size(); ++i) {
        const Chord4* chord = chords->get2(i);
        const Chord4* des = desiredChord.get();
        if (*chord == *des) {
            ++timesChordFound;
        }
    }
    assertEQ(timesChordFound, 1);
}

static void canFindExpected56() {
    Options options = makeOptions(false);
    Chord4Manager mgr(options);

    assertChordExists(mgr, 5, options, "G2G3B3D4");
    assertChordExists(mgr, 6, options, "E2C3A3E4");

    // examples from Piston book:
    assertChordExists(mgr, 5, options, "G2D3G3B3");
    SQINFO("---- here goes A3C3E3C4");
    assertChordExists(mgr, 6, options, "A3C3E3C4", true);
}

//-----------------------------------------

static void testGenerateProgression(
    const Chord4Manager& mgr,
    const Options& options,
    ConstChord4Ptr first,
    int nextScaleDegree,
    const std::string& expected) {
    assert(first);
    assert(first->isValid());
    auto next = HarmonyChords::findChord(false, options, mgr, *first, nextScaleDegree);
    assert(next);
    assert(next->isValid());
    assertEQ(next->toStringShort(), expected);
}

static void testNoneInCommmon56() {
    auto options = makeOptions(false);
    Chord4Manager mgr(options);
    Chord4Ptr chordA = Chord4::fromString(options, 5, "G2G3B3D4");

    std::string expectedChord = options.style->usePistonV_VI_exception() ? "E2C3A3E4" : "A2E3A3C4";
    testGenerateProgression(mgr, options, chordA, 6, expectedChord);
}

static void testNoneInCommmon12() {
    auto options = makeOptions(false);
    Chord4Manager mgr(options);
    Chord4Ptr chordA = Chord4::fromString(options, 1, "C2C3E3G3");
    assert(chordA);
    testGenerateProgression(mgr, options, chordA, 2, "D2A2D3F3");
}

static void testAnalyzeProgression(
    const Chord4Manager& mgr,
    const Options& options,
    ConstChord4Ptr first,
    ConstChord4Ptr next) {
    //   ProgressionAnalyzer(const Chord4* C1, const Chord4* C2, bool fShow);
    assert(first.get());
    assert(next.get());
    assert(first->isValid());
    assert(next->isValid());

    ProgressionAnalyzer pa(first.get(), next.get(), true);
    const int p = pa.getPenalty(options, 100000);
    SQINFO("penalty was %d", p);

}

static void testAnalyze56() {
    SQINFO("\n\n---------------------- bgf: start testAnalyze56");
    auto options = makeOptions(false);
    Chord4Manager mgr(options);
    Chord4Ptr chordA = Chord4::fromString(options, 5, "G1B2D3G3");
    Chord4Ptr chordB = Chord4::fromString(options, 6, "E1C3E3A3");      // only one A - should be illegal
    testAnalyzeProgression(mgr, options, chordA, chordB);
    SQINFO("\n-------------------------bgf: end testAnalyze56");
}

/**
 * These are a grab-bag of harmony tests for new functionality added in 2023.
 */
void testHarmonyChords2023() {
    canFindExpected56();
    testNoneInCommmon12();
    testNoneInCommmon56();
    testAnalyze56();
}