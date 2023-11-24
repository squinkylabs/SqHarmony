
#include "Chord4Manager.h"
#include "HarmonyChords.h"
#include "KeysigOld.h"
#include "ProgressionAnalyzer.h"
#include "RawChordGenerator.h"
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
        // SQINFO("new worst penalty: %d", penalty);
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

static std::string pitchToString(int pitch, const Options& options) {
    HarmonyNote hn(options);
    hn.setPitchDirectly(pitch);
    return hn.tellPitchName();
}

static void showNarrow3() {
    SQINFO("------ here are all the allowed range is c maj -----");
    const Options options = makeOptions(false);
    for (int i = 0; i < 2; ++i) {
        const auto range = (i == 0) ? Style::Ranges::NARROW_RANGE : Style::Ranges::NORMAL_RANGE;
        SQINFO("--- range = %d ---", range);
        options.style->setRangesPreference(range);

        //  SQINFO("bass range %d to %d (%s to %s)", options.style->minBass(), options.style->maxBass()
        //  pitchToString()
        // );

        SQINFO("bass range %d to %d (%s to %s)",
               options.style->minBass(),
               options.style->maxBass(),
               pitchToString(options.style->minBass(), options).c_str(),
               pitchToString(options.style->maxBass(), options).c_str());
        SQINFO("tenor range %d to %d (%s to %s)",
               options.style->minTenor(),
               options.style->maxTenor(),
               pitchToString(options.style->minTenor(), options).c_str(),
               pitchToString(options.style->maxTenor(), options).c_str());

        SQINFO("alto range %d to %d (%s to %s)",
               options.style->minAlto(),
               options.style->maxAlto(),
               pitchToString(options.style->minAlto(), options).c_str(),
               pitchToString(options.style->maxAlto(), options).c_str());

        SQINFO("sops range %d to %d (%s to %s)",
               options.style->minSop(),
               options.style->maxSop(),
               pitchToString(options.style->minSop(), options).c_str(),
               pitchToString(options.style->maxSop(), options).c_str());
    }
}

static void testNumberOfChords(bool narrow) {
    SQINFO("--- will generate all possible chords. narrow = %d", narrow);
    const Options options = makeOptions(false);
    if (narrow) {
        options.style->setRangesPreference(Style::Ranges::NARROW_RANGE);
    }
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
        SQINFO("removed assert. generated[%d] %d expected %d", i, x, expected);
        // assertEQ(x, expected);
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
     *
     * for narrow:
     * generated[1] 3
     * generated[2] 7
     * generated[3] 2
     * generated[4] 7
     * generated[5] 2
     * generated[6] 7
     * generated[7] 4
     */
}

#undef APIENTRY
#include "windows.h"

// 234 ms debug. 242  after rules about sop jumps!
// 748 release old rules 848, so it got faster
// release 692 after rules about sop jumps and proper V-VI
// 231 release after change rule order!
// now back up to 673
// last on main: init chords was 351, prog was 743
// with 2023 ranges prog goes to 1612
static int timeChordProgressionGen() {
    testAllChords(
        Style::Ranges::NORMAL_RANGE,
        Style::InversionPreference::DONT_CARE,
        Scale::Scales::Major,
        1,
        nullptr);
    return 0;
}

// 369 release 11/12/2023
// 175 after rule order changed (why would this matter)
// now back up to 387
// with 2023 ranges (wide) goes up to 569
static int timeChordInit() {
    for (int i = 0; i < 8; ++i) {
        const Options options = makeOptions(1, Scale::Scales::Dorian);  // just some arbitrary scale
        Chord4Manager mgr(options);
    }

    const Options options2 = makeOptions(4, Scale::Scales::Dorian);  // just some arbitrary scale
    Chord4Manager mgr2(options2);
    return 3;
}

static int timingCheck(std::function<int(void)> thingToTime, const std::string& msg) {
#ifdef _DEBUG
    const int iterations = 1;
#else
    const int iterations = 30;
#endif
    int ret = 0;
    // prime
    for (int i = 0; i < 5; ++i) {
        ret += thingToTime();
    }
    int x = timeGetTime();
    for (int i = 0; i < iterations; ++i) {
        ret += thingToTime();
    }
    int y = timeGetTime();
    SQINFO("****** timing check %s, elapsed = %d v=%d", msg.c_str(), y - x, ret);
    return ret;
}

void testAllChords(bool doLongRunning) {
    showNarrow3();
    testNumberOfChords(false);
    testNumberOfChords(true);
    if (doLongRunning) {
        timingCheck(timeChordProgressionGen, std::string("progression gen"));
        timingCheck(timeChordInit, std::string("init chords"));
        // timingCheck(timeRawChords, std::string("raw chords"));
        xtestAllChords();
    } else {
        SQINFO("skipping long running tests");
    }
    assertEQ(__numChord4, 0);
}