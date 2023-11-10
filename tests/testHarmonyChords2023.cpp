
#include "Chord4Manager.h"
#include "HarmonyChords.h"
#include "KeysigOld.h"
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

static void assertChordExists(Chord4Manager& mgr, int root, const Options& options, const std::string& expected) {
    Chord4List* p5 = mgr._getChords(root);
    assert(p5);

    Chord4Ptr desired5 = Chord4::fromString(options, root, expected.c_str());
    assert(desired5);
    assert(desired5->isValid());

    // Chord4ListPtr chords = mgr._getChords(5);
    int find5 = 0;
    for (int i = 0; i < p5->size(); ++i) {
        const Chord4* chord = p5->get2(i);
        const Chord4* des = desired5.get();
        if (*chord == *des) {
            ++find5;
        }
    }
    assertEQ(find5, 1);
}

static void canFindExpected56() {
    Options options = makeOptions(false);
    Chord4Manager mgr(options);

    assertChordExists(mgr, 5, options, "G2G3B3D4");
    assertChordExists(mgr, 6, options, "E2C3A3E4");
}

//-----------------------------------------
static void testNoneInCommmon56() {
    SQINFO("start testNoneInCommmon56");
    auto options = makeOptions(false);
    Chord4Manager mgr(options);
    Chord4Ptr chordA = Chord4::fromString(options, 5, "G2G3B3D4");
    assert(chordA);
    auto next = HarmonyChords::findChord(false, options, mgr, *chordA, 6);
    assert(next);

    if (options.style->usePistonV_VI_exception()) {
        // this is a known good with the old rule. it should break when we make the new one.
        const std::string s = next->toStringShort();
        assert(next->isValid());
       // assertEQ(next->toStringShort(), std::string("A2E3A3C4"));
        assertEQ(next->toStringShort(), std::string("E2C3A3E4"));
    } else {
        // older harmony rules.
        std::string knownGood("A2E3A3C4");
        assertEQ(next->toStringShort(), knownGood);
    }

    SQINFO("end testNoneInCommmon56");
}

static void testNoneInCommmon12() {
    auto options = makeOptions(false);
    Chord4Manager mgr(options);
    Chord4Ptr chordA = Chord4::fromString(options, 1, "C2C3E3G3");
    assert(chordA);
    auto next = HarmonyChords::findChord(false, options, mgr, *chordA, 2);
    assert(next);

    // This was just capturing a "known good", but it does obey the no notes in common rule
    assertEQ(next->toStringShort(), std::string("D2A2D3F3"));
}

/**
 * These are a grab-bag of harmony tests for new functionality added in 2023.
 */
void testHarmonyChords2023() {
    canFindExpected56();
    testNoneInCommmon12();
    testNoneInCommmon56();
}