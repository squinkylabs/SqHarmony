
#include "Chord4Manager.h"
#include "HarmonyChords.h"
#include "KeysigOld.h"
#include "ProgressionAnalyzer.h"
#include "Scale.h"
#include "asserts.h"

int worstPenalty = 0;
int penaltyTotal = 0;
int numEvals = 0;
int numWithPenalty = 0;

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

static void testAllChords(
    Chord4Manager& mgr,
    const Options& options,
    const Chord4* sourceChord,
    int destinationRoot,
    PAStats* stats) {
    auto chordB = HarmonyChords::findChord(false, options, mgr, *sourceChord, destinationRoot, stats);
    assert(chordB);
    if (!chordB) {
        SQWARN("could not connect chords");
        throw "asb";
    }

    const int penalty = HarmonyChords::progressionPenalty(
        options,
        ProgressionAnalyzer::MAX_PENALTY,  // Why to we pass best so far? (because it makes us do work!).
        nullptr,
        sourceChord,
        chordB,
        false,
        nullptr);
    if (penalty > worstPenalty) {
        SQINFO("new worst penalty: %d", penalty);
        worstPenalty = penalty;
    }

    ++numEvals;
    penaltyTotal += penalty;
    if (penalty > 0) {
        numWithPenalty++;
    }
}

static void testAllChords(
    Style::Ranges range,
    Style::InversionPreference inversionPref,
    Scale::Scales mode,
    int sourceRoot,
    int destinationRoot,
    PAStats* stats) {
    const Options options = makeOptions(sourceRoot, mode);
    Chord4Manager mgr(options);

    const Chord4List* sourceRootChords = mgr._getChords(sourceRoot);
    // First, make a set of chords for the root.
    // Chord4List roots(options, sourceRoot);
    // Then, for each root chords, make sure something can follow.
    for (int i = 0; i < sourceRootChords->size(); ++i) {
        const Chord4* theSourceChord = mgr.get2(sourceRoot, i);
        testAllChords(mgr, options, theSourceChord, destinationRoot, stats);
    }
}

static void testAllChords(
    Style::Ranges range,
    Style::InversionPreference inversionPref,
    Scale::Scales mode,
    int root,
    PAStats* stats) {
    for (int i = 1; i < 8; ++i) {
        testAllChords(range, inversionPref, mode, root, i, stats);
    }
}
static void testAllChords(Style::Ranges range, Style::InversionPreference inversionPref, Scale::Scales mode) {
    SQINFO("test all 274 range=%d, invPref=%d, mode=%d", range, inversionPref, mode);
    std::shared_ptr<PAStats> stats;
    if (range == Style::Ranges::ENCOURAGE_CENTER && inversionPref == Style::InversionPreference::DONT_CARE) {
        stats = std::make_shared<PAStats>();
    }

    for (int i = 1; i < 8; ++i) {
        testAllChords(range, inversionPref, mode, i, stats.get());
    }

    if (stats) {
        stats->dump();

        const double avgPenalty = double(penaltyTotal) / double(numEvals);
        const double rate = double(numWithPenalty) / double(numEvals);
        SQINFO("avg penalty = %f rate of penalty = %f", avgPenalty, rate);
        worstPenalty = 0;
        penaltyTotal = 0;
        numEvals = 0;
        numWithPenalty = 0;
    }
}

static void testAllChords(Style::Ranges range, Style::InversionPreference inversionPref) {
    // The first scales are the diatonic.
    for (int i = int(Scale::Scales::Major); i != int(Scale::Scales::MinorPentatonic); ++i) {
        testAllChords(range, inversionPref, Scale::Scales(i));
    }
}

static void testAllChords(Style::Ranges range) {
    testAllChords(range, Style::InversionPreference::DISCOURAGE);
    testAllChords(range, Style::InversionPreference::DISCOURAGE_CONSECUTIVE);
    testAllChords(range, Style::InversionPreference::DONT_CARE);
}

static void xtestAllChords() {
    // for all possible styles...
    testAllChords(Style::Ranges::NORMAL_RANGE);
    testAllChords(Style::Ranges::ENCOURAGE_CENTER);
    testAllChords(Style::Ranges::NARROW_RANGE);
}

static void testNumberOfChords() {
    const Options options = makeOptions(false);
    Chord4Manager mgr(options);
    for (int i = 1; i < 8; ++i) {
        const auto x = mgr.size(i);
        //   SQINFO("mgr size[%d] =%d", i, x);
        int expected = 0;
        switch (i) {
            case 1:
                expected = 138;
                break;
            case 2:
                expected = 96;
                break;
            case 3:
                expected = 82;
                break;
            case 4:
                expected = 99;
                break;
            case 5:
                expected = 70;
                break;
            case 6:
                expected = 99;
                break;
            case 7:
                expected = 47;
                break;
            default:
                assert(false);
        }
        // SQINFO("removed assert. generated[%d] %d expected %d", i, x, expected);
        assertEQ(x, expected);
    }

    /* data from first release / eliminate leading tone doubling
     * also elim lead inroot
     * mgr size[1] =138 / 138
     * mgr size[2] =96 / 96
     * mgr size[3] =111 / 82
     * mgr size[4] =99 / 99
     * mgr size[5] =97 / 70
     * mgr size[6] =99 / 99
     * mgr size[7] =91 / 65 / 47
     */
}

#undef APIENTRY
#include "windows.h"

static void timeChordProgressionGen() {
    testAllChords(
        Style::Ranges::NORMAL_RANGE,
        Style::InversionPreference::DONT_CARE,
        Scale::Scales::Major,
        1,
        nullptr);
}

// 369 release 11/12/2023
// 175 after rule order changed (why would this matter)
// now back up to 387
static void timeChordInit() {
    for (int i = 0; i < 8; ++i) {
        const Options options = makeOptions(1, Scale::Scales::Dorian);  // just some arbitrary scale
        Chord4Manager mgr(options);
    }

    const Options options2 = makeOptions(4, Scale::Scales::Dorian);  // just some arbitrary scale
    Chord4Manager mgr2(options2);
}

// 234 ms debug. 242  after rules about sop jumps!
// 748 release old rules 848, so it got faster
// release 692 after rules about sop jumps and proper V-VI
// 231 release after change rule order!
// now back up to 673
//
static void timingCheck(std::function<void(void)> thingToTime, const std::string& msg) {
#ifdef _DEBUG
    const int iterations = 1;
#else
    const int iterations = 30;
#endif
    for (int i = 0; i < 5; ++i) {
        thingToTime();
    }
    int x = timeGetTime();
    for (int i = 0; i < iterations; ++i) {
        thingToTime();
    }
    int y = timeGetTime();
    SQINFO("****** timing check %s, elapsed = %d", msg.c_str(), y - x);
}

void testAllChords(bool doLongRunning) {
    testNumberOfChords();
    if (doLongRunning) {
        timingCheck(timeChordProgressionGen, std::string("progression gen"));
        timingCheck(timeChordInit, std::string("init chords"));
        xtestAllChords();
    } else {
        SQINFO("skipping long running tests");
    }
    assertEQ(__numChord4, 0);
}
