#include "Chord4.h"
#include "Chord4Manager.h"
#include "HarmonyChords.h"
#include "KeysigOld.h"
#include "Options.h"
#include "ProgressionAnalyzer.h"
#include "SqLog.h"
#include "Style.h"
#include "asserts.h"

#include <set>

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

static Options makeOptions(int root, Scale::Scales scale) {
    auto ks = std::make_shared<KeysigOld>(Roots::C);

    ks->set(root, scale);
    Options o(ks, makeStyle());
    return o;
}

static void testScales(const MidiNote& base, Scale::Scales mode, const std::vector<int>& expectedPitches) {
    assert(expectedPitches.size() == 3);
    auto options = makeOptions(false);
    auto keysig = options.keysig;
    keysig->set(base, mode);
    Chord4Manager mgr(options);

    const int rootA = 1;
    auto chordA = HarmonyChords::findChord(false, options, mgr, rootA);
    const HarmonyNote* hn = chordA->fetchNotes();
    const ScaleRelativeNote* srn = chordA->fetchSRNNotes();

    for (int i = 0; i < 4; ++i) {
        const int ab = hn[i];
        const int abNorm = ab % 12;
        assert(std::find(expectedPitches.begin(), expectedPitches.end(), abNorm) != expectedPitches.end());
    }
}

static void testCMaj() {
    const int basePitch = MidiNote::C;
    MidiNote baseNote(basePitch);
    testScales(baseNote, Scale::Scales::Major, {0, 4, 7});
}

static void testCMin() {
    const int basePitch = MidiNote::C;
    MidiNote baseNote(basePitch);
    testScales(baseNote, Scale::Scales::Minor, {0, 3, 7});
}

static void testDMixo() {
    const int basePitch = MidiNote::C + 2;
    MidiNote baseNote(basePitch);
    testScales(baseNote, Scale::Scales::Mixolydian, {2, 6, 9});  // d major
}

static void testBPhryg() {
    const int basePitch = MidiNote::C + 11;
    MidiNote baseNote(basePitch);
    testScales(baseNote, Scale::Scales::Phrygian, {11, 2, 6});  // b min
}

static void testHistory1() {
    /*
        static const Chord4* findChord2(
        bool show,
        int root,
        const Options& options,
        const Chord4Manager& manager,
        ChordHistory& history,
        const Chord4* prevPrev,
        const Chord4* prev);
        */
    auto options = makeOptions(false);
    auto keysig = options.keysig;
    const int basePitch = MidiNote::C;
    auto mode = Scale::Scales::Major;
    keysig->set(basePitch, mode);
    Chord4Manager mgr(options);

    // using ChordHistory = SqRingBuffer<int, 8>;
    HarmonyChords::ChordHistory history;
    std::set<std::string> results;
    const Chord4* prev = nullptr;
    const Chord4* prevPrev = nullptr;
    for (int i = 0; i < 12; ++i) {
        auto chord = HarmonyChords::findChord2(
            false,
            1,
            options,
            mgr,
            &history,
            prevPrev,
            prev);
       
        SQINFO("[%d] %s", i, chord->toString().c_str());
        std::string s = chord->toStringShort();
        if (results.find(s) != results.end()) {
            assert(false);
        }
        results.insert(s);
        prevPrev = prev;
        prev = chord;
    }
    // use prev and prevPrev, above.
    // extend toString to dump ranks, inversion
    assert(false);
}

void testHarmonyChords2() {
    testCMaj();
    testCMin();
    testDMixo();
    testBPhryg();

    testHistory1();

    assertEQ(__numChord4, 0);
}