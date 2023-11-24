#include <set>

#include "Chord4.h"
#include "Chord4Manager.h"
#include "HarmonyChords.h"
#include "KeysigOld.h"
#include "Options.h"
#include "ProgressionAnalyzer.h"
#include "SqLog.h"
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

static void testHistory(int setting) {
    int minRepeat = 0;
    int maxRepeat = 0;
    // int depth = 1;
    switch (setting) {
        case 1:
            minRepeat = 1;
            maxRepeat = 6;
            break;
        case 4:
            minRepeat = 4;
            maxRepeat = 8;
            break;
        case 8:
            minRepeat = 8;
            maxRepeat = 12;
            break;
        case 13:
            minRepeat = 13;
            maxRepeat = 16;
            break;

        default:
            assert(false);
    }
    auto options = makeOptions(false);
    auto keysig = options.keysig;
    const int basePitch = MidiNote::C;
    auto mode = Scale::Scales::Major;
    keysig->set(basePitch, mode);
    Chord4Manager mgr(options);

    HarmonyChords::ChordHistory history;
    HarmonyChords::ChordHistory* historyPtr;
    historyPtr = &history;
    history.setSize(setting);

    std::set<std::string> results;
    const Chord4* prev = nullptr;
    const Chord4* prevPrev = nullptr;

    int repeat = -1;
    for (int i = 0; i < 20; ++i) {
        auto chord = HarmonyChords::findChord2(
            false,
            1,
            options,
            mgr,
            historyPtr,
            prevPrev,
            prev);

        //  SQINFO("[%d] %s", i, chord->toString().c_str());
        assert(chord);
        std::string s = chord->toStringShort();
        if (results.find(s) != results.end()) {
            // SQINFO("got a dupe! hp = %p", historyPtr);
            if (repeat < 0) {
                repeat = i;
            }

            // with no history, do cycle of 4.
            // with def hist (4) get 8
            // with (8) get 10
            // 15 on 12
        }
        results.insert(s);
        prevPrev = prev;
        prev = chord;
    }
    // use prev and prevPrev, above.
    // extend toString to dump ranks, inversion
    assertGE(repeat, minRepeat);
    assertLE(repeat, maxRepeat);
}

static void testHistory1() {
    testHistory(1);
    testHistory(4);
    testHistory(8);
    testHistory(13);
}

void testHarmonyChords2() {
    testCMaj();
    testCMin();
    testDMixo();
    testBPhryg();
    testHistory1();
    // SQINFO("put back test history");
    assertEQ(__numChord4, 0);
}