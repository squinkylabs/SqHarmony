

#include "Chord4Manager.h"
#include "HarmonyChords.h"
#include "KeysigOld.h"
#include "Scale.h"
#include "asserts.h"

//---------------------------------------------

// #include "Chord4List.h"

int worstPenalty = 0;

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

/*
  static int progressionPenalty(const Options& options,
                                  int bestSoFar,
                                  const Chord4* prevProv,
                                  const Chord4* prev,
                                  const Chord4* current,
                                  bool show);
*/
static void testAllChords(
    Chord4Manager& mgr,
    const Options& options,
    const Chord4* sourceChord, int destinationRoot) {
    auto chordB = HarmonyChords::findChord(false, options, mgr, *sourceChord, destinationRoot);
    assert(chordB);
    if (!chordB) {
        SQWARN("could not connect chords");
        throw "asb";
    }
    const int x = HarmonyChords::progressionPenalty(
        options,
        0,  // why to we pass best so far?
        nullptr,
        sourceChord,
        chordB,
        false);
    // SQINFO("got penalty %d", x);
    if (x > worstPenalty) {
        SQINFO("new worst penalty: %d", x);
        worstPenalty = x;
    }
}

static void testAllChords(
    Style::Ranges range,
    Style::InversionPreference inversionPref,
    Scale::Scales mode,
    int sourceRoot,
    int destinationRoot) {
    const Options options = makeOptions(sourceRoot, mode);
    Chord4Manager mgr(options);

    // SQINFO("test all 252");
    const Chord4List* sourceRootChords = mgr._getChords(sourceRoot);
    // first, make a set of chords for the root
    //  Chord4List roots(options, sourceRoot);
    // then, for each root chords, make sure something can follow
    for (int i = 0; i < sourceRootChords->size(); ++i) {
        const Chord4* theSourceChord = mgr.get2(sourceRoot, i);
        testAllChords(mgr, options, theSourceChord, destinationRoot);
    }
}

static void testAllChords(
    Style::Ranges range,
    Style::InversionPreference inversionPref,
    Scale::Scales mode,
    int root) {
    //   SQINFO("test all 267");
    for (int i = 1; i < 8; ++i) {
        testAllChords(range, inversionPref, mode, root, i);
    }
}
static void testAllChords(Style::Ranges range, Style::InversionPreference inversionPref, Scale::Scales mode) {
    SQINFO("test all 274 range=%d, invPref=%d, mode=%d", range, inversionPref, mode);
    for (int i = 1; i < 8; ++i) {
        testAllChords(range, inversionPref, mode, i);
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
    // const int sourceRoot = 1;
    // const auto model =
    // const Options options = makeOptions(sourceRoot, mode);
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
                expected = 111;
                break;
            case 4:
                expected = 99;
                break;
            case 5:
                expected = 97;
                break;
            case 6:
                expected = 99;
                break;
            case 7:
                expected = 91;
                break;
            default:
                assert(false);
        }
        SQINFO("remove assert. generated[%d] %d expected %d", i, x, expected);
        // assertEQ(x, expected);
    }

    /*
    data from first release / eliminate leading tone doubling / also elim lead inroot
     mgr size[1] =138 / 138
    ]mgr size[2] =96 / 96
     mgr size[3] =111 / 82
     mgr size[4] =99 / 99
     mgr size[5] =97 / 70
     mgr size[6] =99 / 99
     mgr size[7] =91 / 65 / 47
     */
}

#undef APIENTRY
#include "windows.h"

/*
static void testAllChords(
    Style::Ranges range,
    Style::InversionPreference inversionPref,
    Scale::Scales mode,
    int root) {*/

static void thingToTime() {
    testAllChords(
        Style::Ranges::NORMAL_RANGE,
        Style::InversionPreference::DONT_CARE,
        Scale::Scales::Major,
        1);
}

// 234 ms debug. 242  after rules about sop jumps!
// 748 release old rules 848, so it got faster
// relese 707 after rules about sop jumps and proper V-VI
// 
static void timingCheck() {
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
    SQINFO("timing check, elapsed = %d", y - x);
}

void testAllChords(bool doLongRunning) {
    testNumberOfChords();
    if (doLongRunning) {
        timingCheck();
        xtestAllChords();
    } else {
        SQINFO("skipping long running tests");
    }
    assertEQ(__numChord4, 0);
}
