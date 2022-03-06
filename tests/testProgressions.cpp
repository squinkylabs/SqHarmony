
#include "Chord4.h"
#include "Chord4Manager.h"
#include "HarmonyChords.h"
#include "Keysig.h"
#include "Options.h"
#include "ProgressionAnalyzer.h"
#include "asserts.h"

static StylePtr makeStyle() {
    return std::make_shared<Style>();
}

static KeysigPtr makeKeysig() {
    return std::make_shared<Keysig>(Roots::C);
}

static Options makeOptions() {
    Options o(makeKeysig(), makeStyle());
    return o;
}

static void testAtoB(bool show, const char* chordAs, int degA, const char* chordBs, int degB, bool callFinder) {
    Options options = makeOptions();
    Chord4Ptr chord1 = Chord4::fromString(options, degA, chordAs);
    assert(chord1);
    printf("here is first chord: %s\n", chord1->toString().c_str());
    printf("first chord correct doubling = %d, acceptable = %d\n",
        chord1->isCorrectDoubling(options),
        chord1->isAcceptableDoubling(options));

    Chord4Ptr chord2 = Chord4::fromString(options, degB, chordBs);
    assert(chord2);
    printf("here is second chord: %s\n", chord2->toString().c_str());
    printf("second chord correct doubling = %d, acceptable = %d\n",
        chord2->isCorrectDoubling(options),
        chord2->isAcceptableDoubling(options));


    printf("about to analyze for display\n");
    ProgressionAnalyzer pa(*chord1, *chord2, true);
  //  if (show) {
   //     pa.showAnalysis();
  //  }
    pa.getPenalty(options, ProgressionAnalyzer::MAX_PENALTY);

    printf("done with showing penalty\n");

    if (callFinder) {
        auto chordManager = std::make_shared<Chord4Manager>(options);
        printf("-- now will call findChord\n");
        auto chordB = HarmonyChords::findChord(show, options, *chordManager, *chord1, 2);
        assert(chordB);
        printf("find chord ret %s\n", chordB->toString().c_str());
    }
}

static void testCtoD() {
    testAtoB(false,
             "C2C3E3G3",
             1,
             "D2A2F3A3",
             2,
             false);
}

// my module producted this
static void testCtoD2() {
    testAtoB(false,
             "G1E3G3C4",
             1,
             "D2A2F3F4",
             2,
             false);
}

// Andy T suggested this instead
static void testCtoD3() {
    testAtoB(false,
             "G1E3G3C4",
             1,
             "D2A2F3D4",
             2, false);
}
void testProgressions() {
    // testCtoD();
    printf("\n*********** Here's what my program did\n");
    testCtoD2();
    printf("\n*********** Here's AT\n");
    testCtoD3();
    fflush(stdout);
    assert(false);
}