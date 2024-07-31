

#include "MidiNote.h"
#include "Scale.h"
#include "ScoreDrawUtils.h"
#include "SharpsFlatsPref.h"
#include "asserts.h"

#if 0
getDrawInfo(
    const Scale& scale,
    const SqArray<int, 16>& input,
    bool bassStaff,
    UIPrefSharpsFlats pref) {
#endif

static ScoreDrawUtilsPtr testCMajorSub(const SqArray<int, 16>& input) {
    Scale scale(MidiNote(MidiNote::C), Scale::Scales::Major);
    ScoreDrawUtilsPtr utils = ScoreDrawUtils::make();
    utils->getDrawInfo(scale, input, false, UIPrefSharpsFlats::Sharps);
    return utils;
}

static void test1() {
   // Scale scale(MidiNote(MidiNote::C), Scale::Scales::Major);

    SqArray<int, 16> test = {MidiNote::MiddleC};
  //  ScoreDrawUtilsPtr utils = ScoreDrawUtils::make();
  //  utils->getDrawInfo(scale, test, false, UIPrefSharpsFlats::Sharps);
   // assertEQ(utils->_info.size(), 1);
    const auto utils = testCMajorSub(test);
    assertEQ(utils->_info.size(), 1);
}

void testScoreDrawUtils() {
    test1();
}

#if 1
void testFirst() {
    testScoreDrawUtils();
}

#endif