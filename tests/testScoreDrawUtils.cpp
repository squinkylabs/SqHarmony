

#include "MidiNote.h"
#include "Scale.h"
#include "ScoreDrawUtils.h"
#include "SharpsFlatsPref.h"
#include "asserts.h"

static const std::map<int, LegerLineInfo> testCMajorSub(const SqArray<int, 16>& input, DrawPosition pos) {
    Scale scale(MidiNote(MidiNote::C), Scale::Scales::Major);
    ScoreDrawUtilsPtr utils = ScoreDrawUtils::make();
  //  DrawPosition pos;
    return utils->getDrawInfo(pos, scale, input, UIPrefSharpsFlats::Sharps);
    // return utils;
}

static const std::map<int, LegerLineInfo> testCMajorSub(const SqArray<int, 16>& input) {
    DrawPosition pos;
    return testCMajorSub(input, pos);
    // return utils;
}

static void test1() {
    SqArray<int, 16> test = {MidiNote::MiddleC};
    const auto info = testCMajorSub(test);
    assertEQ(info.size(), 1);
    const auto iter = info.find(-2);
    assertEQ(iter->second.symbols.size(), 1);
}

static void test2() {
    SqArray<int, 16> test = {MidiNote::MiddleC, MidiNote::MiddleC + MidiNote::D};
    const auto info = testCMajorSub(test);
    assertEQ(info.size(), 2);

    int i = 0;
    for (auto iter = info.begin(); iter != info.end(); ++iter) {
        assertEQ(iter->second.symbols.size(), 1);
        ++i;
    }
    assertEQ(i, 2);
}

static void testAccidental() {
    SqArray<int, 16> test = {MidiNote::MiddleC + 1};
    const auto info = testCMajorSub(test);
    assertEQ(info.size(), 1);
    const auto iter = info.find(-2);
    assertEQ(iter->second.symbols.size(), 2);
    assertEQ(iter->second.symbols[0].glyph, ScoreDrawUtils::_sharp);
    assertEQ(iter->second.symbols[1].glyph, ScoreDrawUtils::_wholeNote);
}

static void test2OneLine() {
    // C and C# are on same leger line
    SqArray<int, 16> test = {MidiNote::MiddleC, MidiNote::MiddleC + 1};
    const auto info = testCMajorSub(test);
    assertEQ(info.size(), 1);

    const auto iter = info.find(-2);
    assertEQ(iter->second.symbols.size(), 3);  // expect #, C, C
}

static void testToString() {
    SqArray<int, 16> test = {MidiNote::MiddleC};
    const auto info = testCMajorSub(test);
    assertEQ(info.size(), 1);
    const auto iter = info.find(-2);
    assertGT(iter->second.toString().length(), 0);
}

static void testXPos() {
    SqArray<int, 16> input = {MidiNote::MiddleC};
    Scale scale(MidiNote(MidiNote::C), Scale::Scales::Major);
    ScoreDrawUtilsPtr utils = ScoreDrawUtils::make();
    DrawPosition pos;
    pos.noteXPosition = 11;
    const auto info = utils->getDrawInfo(pos, scale, input, UIPrefSharpsFlats::Sharps);
    assertEQ(info.size(), 1);
    assertEQ(info.begin()->second.symbols.size(), 1);
    assertEQ(info.begin()->second.symbols[0].xPosition, 11);
}

static void testYPos() {
    SqArray<int, 16> input = {MidiNote::MiddleC};
    Scale scale(MidiNote(MidiNote::C), Scale::Scales::Major);
    ScoreDrawUtilsPtr utils = ScoreDrawUtils::make();
    DrawPosition pos;
    pos.noteYPosition = [](const MidiNote& note, int legerLine, bool bassStaff) {
        return legerLine;
    };
    const auto info = utils->getDrawInfo(pos, scale, input, UIPrefSharpsFlats::Sharps);
    assertEQ(info.size(), 1);
    assertEQ(info.begin()->second.symbols.size(), 1);
    assertEQ(info.begin()->second.symbols[0].yPosition, -2);
}


static void testClef(bool bass) {
    SqArray<int, 16> inputBass =  {MidiNote::MiddleC - 12 + MidiNote::A};
    SqArray<int, 16> inputTreble = {MidiNote::MiddleC};
    DrawPosition pos;
    pos.noteYPosition = [](const MidiNote& note, int legerLine, bool bassStaff) {
        return bassStaff ? 100 : 200;
    };
    const auto input = bass ? inputBass : inputTreble;
    const auto info = testCMajorSub(input, pos);
    assertEQ(info.size(), 1);
    assertEQ(info.begin()->second.symbols.size(), 1);
    assertEQ(info.begin()->second.symbols[0].yPosition, (bass ? 100.f : 200.f));
}

static void testBassClef() {
    testClef(true);
}

static void testTrebleClef() {
    testClef(false);
}

void testScoreDrawUtils() {
    test1();
    test2();
    testAccidental();
    test2OneLine();
    testToString();
    testXPos();
    testYPos();
    testBassClef();
    testTrebleClef();
}

#if 1
void testFirst() {
    testScoreDrawUtils();
    // testBassClef();
}
#endif