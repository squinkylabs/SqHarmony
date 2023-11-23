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

static void testFirstChord(bool minor) {
    auto options = makeOptions(minor);
    Chord4Manager mgr(options);
    const int root = 1;
    auto next = HarmonyChords::findChord(false, options, mgr, root);
    assert(next);
}

static void testFirstChord() {
    testFirstChord(false);
    testFirstChord(true);
}

static void testBasic1(bool minor) {
    auto options = makeOptions(minor);
    Chord4Manager mgr(options);
    auto cp = HarmonyChords::findChord(false, options, mgr, 1);

    const int root = 3;  // 1 to 3 progression should be legal
    auto next = HarmonyChords::findChord(false, options, mgr, *cp, root);
    assert(next);
}

static void testBasic1() {
    testBasic1(false);
    testBasic1(true);
}

static void testAtoB(int a, int b, bool minor, int bestExpected = 0) {
    auto options = makeOptions(minor);
    Chord4Manager mgr(options);
    const Chord4* cp = HarmonyChords::findChord(false, options, mgr, a);

    auto next = HarmonyChords::findChord(false, options, mgr, *cp, b);
    assert(next);
    const int penalty = next->penaltForFollowingThisGuy(options, ProgressionAnalyzer::MAX_PENALTY, cp, false, nullptr);
    if (penalty != bestExpected) {
         next->penaltForFollowingThisGuy(options, ProgressionAnalyzer::MAX_PENALTY, cp, true, nullptr);
    }
    assertEQ(penalty, bestExpected);
}

static void testAtoBtoA(int a, int b, int maxAcceptablePenalty, bool minor) {
    auto options = makeOptions(minor);
    Chord4Manager mgr(options);
    const Chord4* chordA = HarmonyChords::findChord(false, options, mgr, a);

    const Chord4* chordB = HarmonyChords::findChord(false, options, mgr, *chordA, b);
    assert(chordB);
    assertEQ(chordB->penaltForFollowingThisGuy(options, ProgressionAnalyzer::MAX_PENALTY, chordA, false, nullptr), 0);

    auto chordC = HarmonyChords::findChord(false, options, mgr, *chordA, *chordB, a);
    assert(chordC);
    assertLE(chordC->penaltForFollowingThisGuy(options, ProgressionAnalyzer::MAX_PENALTY, chordB, false, nullptr), maxAcceptablePenalty);
}

static void test1to2to1() {
    testAtoBtoA(1, 2, ProgressionAnalyzer::SLIGHTLY_LOWER_PENALTY_PER_RULE, false);
    testAtoBtoA(1, 2, ProgressionAnalyzer::SLIGHTLY_LOWER_PENALTY_PER_RULE, true);
}

static void testBasic2() {
    //testAtoB(1, 3, false, 0);
    //testAtoB(1, 3, true, 0);
    SQINFO("put back 1 to 3");
}

static void test3to5() {
    testAtoB(3, 5, false);
    testAtoB(3, 5, true);
}

static void test5to4() {
   // testAtoB(5, 4, false);
   // testAtoB(5, 4, true, 100);
    SQINFO("put back 5 to 4");
}

static void test5to6() {
    testAtoB(5, 6, false);
    testAtoB(5, 6, true, 100);
}

static void test1to2() {
    testAtoB(1, 2, false);
    testAtoB(1, 2, true);
}

static void test2to1() {
    testAtoB(2, 1, false);
    testAtoB(2, 1, true);
}



static void test2to1a() {
    // We are having problems with this simple 2-1 progression
    auto options = makeOptions(false);
    Chord4Manager mgr(options);
    Chord4Ptr chordA = Chord4::fromString(options, 2, "D2A2F3A3");
    assert(chordA);

    auto next = HarmonyChords::findChord(false, options, mgr, *chordA, 1);
    assert(next);
    assertEQ(next->penaltForFollowingThisGuy(options, ProgressionAnalyzer::MAX_PENALTY, chordA.get(), false, nullptr), 0);
}

static void test2to1b() {
    // We are having problems with this simple 2-1 progression
    auto options = makeOptions(false);
    Chord4Manager mgr(options);
    auto chordA = Chord4::fromString(options, 2, "D2A2D3F3");
    assert(chordA);

    auto next = HarmonyChords::findChord(false, options, mgr, *chordA, 1);
    assert(next);
    std::string x = next->toString();
    assertLT(next->penaltForFollowingThisGuy(options, ProgressionAnalyzer::MAX_PENALTY, chordA.get(), false, nullptr), ProgressionAnalyzer::AVG_PENALTY_PER_RULE);
}

static void testThreeSequence(bool minor) {
    auto options = makeOptions(minor);
    Chord4Manager mgr(options);

    auto firstChord = HarmonyChords::findChord(false, options, mgr, 1);

    const int secondRoot = 3;
    auto secondChord = HarmonyChords::findChord(false, options, mgr, *firstChord, secondRoot);
    assert(secondChord);
    assertEQ(secondChord->penaltForFollowingThisGuy(options, ProgressionAnalyzer::MAX_PENALTY, firstChord, false, nullptr), 0);

    const int thirdRoot = 5;
    auto thirdChord = HarmonyChords::findChord(false, options, mgr, *firstChord, *secondChord, thirdRoot);
    assert(thirdChord);
    assertEQ(thirdChord->penaltForFollowingThisGuy(options, ProgressionAnalyzer::MAX_PENALTY, secondChord, false, nullptr), 0);
}

static void testThreeSequence() {
    testThreeSequence(false);
    testThreeSequence(true);
}

static void testXtoYtoX() {
    auto options = makeOptions(false);
    Chord4Manager mgr(options);

    for (int rootX = 1; rootX <= 7; ++rootX) {
        auto chordX = HarmonyChords::findChord(false, options, mgr, rootX);
        assert(chordX);
        for (int rootY = 1; rootY <= 7; ++rootY) {
            if (rootX != rootY) {
                printf("** in test xyx rootx = % d rooty = % d\n", rootX, rootY);
                auto chordY = HarmonyChords::findChord(false, options, mgr, *chordX, rootY);
                assert(chordY);
                assertEQ(chordY->penaltForFollowingThisGuy(options, ProgressionAnalyzer::MAX_PENALTY, chordX, false, nullptr), 0);
                auto thirdChord = HarmonyChords::findChord(false, options, mgr, *chordX, *chordY, rootX);
                assert(thirdChord);
                assertEQ(thirdChord->penaltForFollowingThisGuy(options, ProgressionAnalyzer::MAX_PENALTY, chordX, false, nullptr), 0);

                assert(!(*chordX == *thirdChord));
            }
        }
    }
}

static void testXtoY() {
    auto options = makeOptions(false);
    Chord4Manager mgr(options);

    for (int rootX = 1; rootX <= 7; ++rootX) {
        auto chordX = HarmonyChords::findChord(false, options, mgr, rootX);
        assert(chordX);
        for (int rootY = 1; rootY <= 7; ++rootY) {
            if (rootX != rootY) {
                printf("** in test xyx rootx = % d rooty = % d\n", rootX, rootY);
                auto chordY = HarmonyChords::findChord(false, options, mgr, *chordX, rootY);
                assert(chordY);
                assertEQ(chordY->penaltForFollowingThisGuy(options, ProgressionAnalyzer::MAX_PENALTY, chordX, false, nullptr), 0);
            }
        }
    }
}

void testHarmonyChords() {
    testFirstChord();
    testBasic1();
    testBasic2();
    test3to5();
    test5to4();
    test5to6();
    test1to2();

    // broke
    //   testFeb21Case();

    test2to1();
    printf("put back 2 1 test\n");
    // test2to1a();
    test2to1b();

    test1to2to1();

   

    printf("put back 3 seq\n");
    // testThreeSequence();
    // testXtoY();
    // testXtoYtoX();

    // this isn't a real test
    // test1to1();

    assertEQ(__numChord4, 0);
}