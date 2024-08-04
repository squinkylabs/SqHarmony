

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
    SqArray<int, 16> input = {MidiNote::MiddleC + 1};
    Scale scale(MidiNote(MidiNote::C), Scale::Scales::Major);
    ScoreDrawUtilsPtr utils = ScoreDrawUtils::make();
    DrawPosition pos;
    pos.noteXPosition = 11;
    const auto info = utils->getDrawInfo(pos, scale, input, UIPrefSharpsFlats::Sharps);
    assertEQ(info.size(), 1);
    assertEQ(info.begin()->second.symbols.size(), 2);
    assertEQ(info.begin()->second.symbols[0].xPosition, 10);
    assertEQ(info.begin()->second.symbols[0].glyph, ScoreDrawUtils::_sharp);

    assertEQ(info.begin()->second.symbols[1].xPosition, 11);
    assertEQ(info.begin()->second.symbols[1].glyph, ScoreDrawUtils::_wholeNote);
}

static void testYPos() {
    SqArray<int, 16> input = {MidiNote::MiddleC + 1};           // c sharp
    Scale scale(MidiNote(MidiNote::C), Scale::Scales::Major);
    ScoreDrawUtilsPtr utils = ScoreDrawUtils::make();
    DrawPosition pos;
    pos.noteYPosition = [](const MidiNote& note, int legerLine, bool bassStaff) {
        return legerLine;
    };
    const auto info = utils->getDrawInfo(pos, scale, input, UIPrefSharpsFlats::Sharps);
    assertEQ(info.size(), 1);       // expect C and sharp on one line
    assertEQ(info.begin()->second.symbols.size(), 2);

    assertEQ(info.begin()->second.symbols[0].yPosition, -2);
    assertEQ(info.begin()->second.symbols[0].yPosition, -2);
}

static void testClef(bool bass) {
    SqArray<int, 16> inputBass = {MidiNote::MiddleC - 12 + MidiNote::A};
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

static void testClef2(bool bass) {
    SqArray<int, 16> inputBass = {MidiNote::MiddleC - 12 + MidiNote::A, MidiNote::MiddleC - 12 + MidiNote::G};
    SqArray<int, 16> inputTreble = {MidiNote::MiddleC, MidiNote::MiddleC + MidiNote::D};
    DrawPosition pos;
    pos.noteYPosition = [](const MidiNote& note, int legerLine, bool bassStaff) {
        return bassStaff ? 100 : 200;
    };
    const auto input = bass ? inputBass : inputTreble;
    const auto info = testCMajorSub(input, pos);

    assertEQ(info.size(), 2);

    auto iterator = info.begin();
    assertEQ(iterator->second.symbols.size(), 1);
    const float y1 = iterator->second.symbols[0].yPosition;
    iterator++;
    assertEQ(iterator->second.symbols.size(), 1);
    const float y2 = iterator->second.symbols[0].yPosition;

    assertEQ(y1, (bass ? 100.f : 200.f));
    assertEQ(y2, (bass ? 100.f : 200.f));
}

static void testBassClef2() {
    testClef2(true);
}

static void testTrebleClef2() {
    testClef2(false);
}

static void testBothClefs() {
    SqArray<int, 16> input = {MidiNote::MiddleC - 12 + MidiNote::A, MidiNote::MiddleC + MidiNote::G};
    DrawPosition pos;
    pos.noteYPosition = [](const MidiNote& note, int legerLine, bool bassStaff) {
        return bassStaff ? 100 : 200;
    };
    // const auto input = bass ? inputBass : inputTreble;
    const auto info = testCMajorSub(input, pos);
    assertEQ(info.size(), 2);
    assertEQ(info.begin()->second.symbols.size(), 1);

    auto iterator = info.begin();
    const float y1 = iterator->second.symbols[0].yPosition;
    iterator++;
    const float y2 = iterator->second.symbols[0].yPosition;

    assertEQ(y1, 100.f);
    assertEQ(y2, 200.f);
}

static void testCGroup(SqArray<int, 16> input, float expectedY) {
    // SqArray<int, 16> input = {MidiNote::MiddleC, MidiNote::MiddleC + MidiNote::G};
    DrawPosition pos;
    pos.noteYPosition = [](const MidiNote& note, int legerLine, bool bassStaff) {
        return bassStaff ? 100 : 200;
    };
    // const auto input = bass ? inputBass : inputTreble;
    const auto info = testCMajorSub(input, pos);
    assertEQ(info.size(), 2);
    assertEQ(info.begin()->second.symbols.size(), 1);

    auto iterator = info.begin();
    const float y1 = iterator->second.symbols[0].yPosition;
    iterator++;
    const float y2 = iterator->second.symbols[0].yPosition;

    assertEQ(y1, expectedY);
    assertEQ(y2, expectedY);
}

static void testCTreble() {
    testCGroup({MidiNote::MiddleC, MidiNote::MiddleC + MidiNote::G}, 200);
}

static void testCBass() {
    testCGroup({MidiNote::MiddleC, MidiNote::MiddleC - 12 + MidiNote::G}, 100);
}

static void testLegerLine(bool bass) {
    SqArray<int, 16> inputBass = {MidiNote::MiddleC - 12 + MidiNote::B};  // B right below middle C
    SqArray<int, 16> inputTreble = {MidiNote::MiddleC + MidiNote::D};     // D right below middle C

    const MidiNote D = MidiNote(MidiNote::MiddleC + MidiNote::D);
    const MidiNote B = MidiNote(MidiNote::MiddleC - 12 + MidiNote::B);

    //  NotationNote(const MidiNote& mn, Accidental ac, int ll, bool bs);
    NotationNote noteD = NotationNote(D, NotationNote::Accidental::none, -1, false);
    NotationNote noteB = NotationNote(B, NotationNote::Accidental::none, -1, false);

    ScorePitchUtils::SpellingResults results;
    results.notes.putAt(0, bass? noteB : noteD);

    ScoreDrawUtilsPtr utils = ScoreDrawUtils::make();
    utils->_divideClefs(results);

    const NotationNote& final = results.notes.getAt(0);
    assertEQ(final._bassStaff, bass);

    const int expectedLegerLine = bass ? 11 : -1;
    assertEQ(final._legerLine, expectedLegerLine);
}

static void testTrebleLegerLine() {
    testLegerLine(false);
}

static void testBassLegerLine() {
    testLegerLine(true);
}


static void testTwoNotes() {
     SqArray<int, 16> input = {MidiNote::MiddleC,  MidiNote::MiddleC + 1};
    Scale scale(MidiNote(MidiNote::C), Scale::Scales::Major);
    ScoreDrawUtilsPtr utils = ScoreDrawUtils::make();
    DrawPosition pos;
    pos.noteXPosition = 11;
    const auto info = utils->getDrawInfo(pos, scale, input, UIPrefSharpsFlats::Sharps); 
    assertEQ(info.size(), 1);
    const auto legerLineIterator = info.begin();

    // make sure we have two notes
    assertEQ(legerLineIterator->second.symbols.size(), 3);      // two notes and an accidental
    assertEQ(legerLineIterator->second.symbols[1].glyph, ScoreDrawUtils::_wholeNote);
    assertEQ(legerLineIterator->second.symbols[2].glyph, ScoreDrawUtils::_wholeNote);

    // Current desire is for two notes on same line to be spaced like this:
    const float hSpacing = std::abs(legerLineIterator->second.symbols[1].xPosition - legerLineIterator->second.symbols[2].xPosition);
    assertClose(hSpacing, pos.columnWidth, .01);
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
    testTrebleClef2();
    testBassClef2();
    testBothClefs();
    testCTreble();
    testCBass();
    testTrebleLegerLine();
    testBassLegerLine();
}

#if 1
void testFirst() {
    // testScoreDrawUtils();
    testTwoNotes();
}
#endif