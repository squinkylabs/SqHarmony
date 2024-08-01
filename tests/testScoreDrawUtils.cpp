

#include "MidiNote.h"
#include "Scale.h"
#include "ScoreDrawUtils.h"
#include "SharpsFlatsPref.h"
#include "asserts.h"

static ScoreDrawUtilsPtr testCMajorSub(const SqArray<int, 16>& input) {
    Scale scale(MidiNote(MidiNote::C), Scale::Scales::Major);
    ScoreDrawUtilsPtr utils = ScoreDrawUtils::make();
    utils->getDrawInfo(scale, input, false, UIPrefSharpsFlats::Sharps);
    return utils;
}

static void test1() {
    SqArray<int, 16> test = {MidiNote::MiddleC};
    const auto utils = testCMajorSub(test);
    assertEQ(utils->_info.size(), 1);
    const auto iter = utils->_info.find(-2);
    assertEQ(iter->second.numSymbols, 1);
}

static void test2() {
    SqArray<int, 16> test = {MidiNote::MiddleC, MidiNote::MiddleC + MidiNote::D };
    const auto utils = testCMajorSub(test);
    assertEQ(utils->_info.size(), 2);

    int i = 0;
    for (auto iter = utils->_info.begin(); iter != utils->_info.end(); ++iter) {
        assertEQ(iter->second.numSymbols, 1);
        ++i;
    }
    assertEQ(i, 2);   
}

static void test2OneLine() {
    // C and C# are on same leger line
    SqArray<int, 16> test = {MidiNote::MiddleC, MidiNote::MiddleC + 1 };
    const auto utils = testCMajorSub(test);
    assertEQ(utils->_info.size(), 1);

    const auto iter = utils->_info.find(-2);
    assertEQ(iter->second.numSymbols, 2);
}

void testScoreDrawUtils() {
    test1();
    test2();
    test2OneLine();
}

#if 1
void testFirst() {
    testScoreDrawUtils();
}
#endif