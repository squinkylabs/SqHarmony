

#include "MidiNote.h"
#include "Scale.h"
#include "ScoreDrawUtils.h"
#include "SharpsFlatsPref.h"
#include "asserts.h"

static const std::map<int, LegerLineInfo> testCMajorSub(const SqArray<int, 16>& input, DrawPositionParams pos) {
    Scale scale(MidiNote(MidiNote::C), Scale::Scales::Major);
    ScoreDrawUtilsPtr utils = ScoreDrawUtils::make();
    //  DrawPosition pos;
    return utils->getDrawInfo(pos, scale, input, UIPrefSharpsFlats::Sharps);
    // return utils;
}

static const std::map<int, LegerLineInfo> testCMajorSub(const SqArray<int, 16>& input) {
    DrawPositionParams pos;
    return testCMajorSub(input, pos);
    // return utils;
}

static void test1() {
    SqArray<int, 16> test = {MidiNote::MiddleC};
    const auto info = testCMajorSub(test);
    assertEQ(info.size(), 1);
    const auto iter = info.find(-2);
    assertEQ(iter->second.notes.size(), 1);
}

static void test2() {
    SqArray<int, 16> test = {MidiNote::MiddleC, MidiNote::MiddleC + MidiNote::E};
    const auto info = testCMajorSub(test);
    assertEQ(info.size(), 2);

    int i = 0;
    for (auto iter = info.begin(); iter != info.end(); ++iter) {
        assertEQ(iter->second.notes.size(), 1);
        ++i;
    }
    assertEQ(i, 2);
}

static void testAccidental() {
    SqArray<int, 16> test = {MidiNote::MiddleC + 1};
    const auto info = testCMajorSub(test);
    assertEQ(info.size(), 1);
    const auto iter = info.find(-2);

    assertEQ(iter->second.notes.size(), 1);
    assertEQ(iter->second.accidentals.size(), 1);

    assertEQ(iter->second.accidentals[0].glyph, ScoreDrawUtils::_sharp);
    assertEQ(iter->second.notes[0].glyph, ScoreDrawUtils::_wholeNote);
}

static void test2OneLine() {
    // C and C# are on same leger line
    SqArray<int, 16> test = {MidiNote::MiddleC, MidiNote::MiddleC + 1};
    const auto info = testCMajorSub(test);
    assertEQ(info.size(), 1);

    const auto iter = info.find(-2);
    //  assertEQ(iter->second.symbols.size(), 3);  // expect #, C, C
    assert(false);
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
    DrawPositionParams pos;
    pos.noteXPosition = 11;
    const auto info = utils->getDrawInfo(pos, scale, input, UIPrefSharpsFlats::Sharps);

    assertEQ(info.size(), 1);

    assertEQ(info.begin()->second.accidentals.size(), 1);
    assertEQ(info.begin()->second.notes.size(), 1);
    assertEQ(info.begin()->second.accidentals[0].xPosition, 7.5);
    assertEQ(info.begin()->second.accidentals[0].glyph, ScoreDrawUtils::_sharp);

    assertEQ(info.begin()->second.notes[0].xPosition, 11);
    assertEQ(info.begin()->second.notes[0].glyph, ScoreDrawUtils::_wholeNote);
}

static void testYPos() {
    SqArray<int, 16> input = {MidiNote::MiddleC + 1};  // c sharp
    Scale scale(MidiNote(MidiNote::C), Scale::Scales::Major);
    ScoreDrawUtilsPtr utils = ScoreDrawUtils::make();
    DrawPositionParams pos;
    pos.noteYPosition = [](const MidiNote& note, int legerLine, bool bassStaff) {
        return legerLine;
    };
    const auto info = utils->getDrawInfo(pos, scale, input, UIPrefSharpsFlats::Sharps);
    assertEQ(info.size(), 1);  // expect C and sharp on one line

    assertEQ(info.begin()->second.notes.size(), 1)
        assertEQ(info.begin()->second.accidentals.size(), 1);

    assertEQ(info.begin()->second.notes[0].yPosition, -2);
    assertEQ(info.begin()->second.accidentals[0].yPosition, -2);
}

static void testClef(bool bass) {
    SqArray<int, 16> inputBass = {MidiNote::MiddleC - 12 + MidiNote::A};
    SqArray<int, 16> inputTreble = {MidiNote::MiddleC};
    DrawPositionParams pos;
    pos.noteYPosition = [](const MidiNote& note, int legerLine, bool bassStaff) {
        return bassStaff ? 100 : 200;
    };
    const auto input = bass ? inputBass : inputTreble;
    const auto info = testCMajorSub(input, pos);
    assertEQ(info.size(), 1);

    assertEQ(info.begin()->second.notes.size(), 1);
    assertEQ(info.begin()->second.notes[0].yPosition, (bass ? 100.f : 200.f));
}

static void testBassClef() {
    testClef(true);
}

static void testTrebleClef() {
    testClef(false);
}

static void testClef2(bool bass) {
    SqArray<int, 16> inputBass = {MidiNote::MiddleC - 12 + MidiNote::A, MidiNote::MiddleC - 12 + MidiNote::E};
    SqArray<int, 16> inputTreble = {MidiNote::MiddleC, MidiNote::MiddleC + MidiNote::E};
    DrawPositionParams pos;
    pos.noteYPosition = [](const MidiNote& note, int legerLine, bool bassStaff) {
        return bassStaff ? 100 : 200;
    };
    const auto input = bass ? inputBass : inputTreble;
    const auto info = testCMajorSub(input, pos);

    assertEQ(info.size(), 2);

    auto iterator = info.begin();

    assertEQ(iterator->second.notes.size(), 1);
    const float y1 = iterator->second.notes[0].yPosition;
    iterator++;
    assertEQ(iterator->second.notes.size(), 1);
    const float y2 = iterator->second.notes[0].yPosition;

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
    // two input note, one in each clef
    SqArray<int, 16> input = {MidiNote::MiddleC - 12 + MidiNote::A, MidiNote::MiddleC + MidiNote::G};
    DrawPositionParams pos;
    pos.noteYPosition = [](const MidiNote& note, int legerLine, bool bassStaff) {
        const float ret = bassStaff ? 100 : 200;
        return ret;
    };

    // const auto input = bass ? inputBass : inputTreble;
    const auto info = testCMajorSub(input, pos);

    // get back two notes
    assertEQ(info.size(), 2);

    // look at first note
    assertEQ(info.begin()->second.notes.size(), 1);

    // look at first symbol of first note
    auto iterator = info.begin();
    const float y1 = iterator->second.notes[0].yPosition;
    iterator++;
    const float y2 = iterator->second.notes[0].yPosition;

    // These are no longer sorted, but as long as one is 100 and the other is 200...
    assertEQ(y1, 200.f);
    assertEQ(y2, 100.f);
}

static void testCGroup(SqArray<int, 16> input, float expectedY) {
    // SqArray<int, 16> input = {MidiNote::MiddleC, MidiNote::MiddleC + MidiNote::G};
    DrawPositionParams pos;
    pos.noteYPosition = [](const MidiNote& note, int legerLine, bool bassStaff) {
        return bassStaff ? 100 : 200;
    };
    // const auto input = bass ? inputBass : inputTreble;
    const auto info = testCMajorSub(input, pos);
    assertEQ(info.size(), 2);
    assertEQ(info.begin()->second.notes.size(), 1);

    auto iterator = info.begin();
    const float y1 = iterator->second.notes[0].yPosition;
    iterator++;
    const float y2 = iterator->second.notes[0].yPosition;

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
    results.notes.putAt(0, bass ? noteB : noteD);

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

// This test used to find two notes on one leger line, but it doesn't any more
static void testTwoNotes() {
    SqArray<int, 16> input = {MidiNote::MiddleC, MidiNote::MiddleC + 1};
    Scale scale(MidiNote(MidiNote::C), Scale::Scales::Major);
    ScoreDrawUtilsPtr utils = ScoreDrawUtils::make();
    DrawPositionParams pos;
    pos.noteXPosition = 11;
    const auto info = utils->getDrawInfo(pos, scale, input, UIPrefSharpsFlats::Sharps);
    assertEQ(info.size(), 1);
    const auto legerLineIterator = info.begin();

    // make sure we have two notes
    assert(false);
    // assertEQ(legerLineIterator->second.symbols.size(), 3);  // two notes and an accidental
    // assertEQ(legerLineIterator->second.symbols[1].glyph, ScoreDrawUtils::_wholeNote);
    // assertEQ(legerLineIterator->second.symbols[2].glyph, ScoreDrawUtils::_wholeNote);

    // // Current desire is for two notes on same line to be spaced like this:
    // const float hSpacing = std::abs(legerLineIterator->second.symbols[1].xPosition - legerLineIterator->second.symbols[2].xPosition);
    // assertClose(hSpacing, pos.columnWidth, .01);
}

static void testLegerLineCTreble() {
    SqArray<int, 16> input = {MidiNote::MiddleC};
    Scale scale(MidiNote(MidiNote::C), Scale::Scales::Major);
    ScoreDrawUtilsPtr utils = ScoreDrawUtils::make();
    DrawPositionParams pos;
    pos.llDrawInfo = [](const MidiNote& note, int legerLine, bool bassStaff) {
        LegerLinesLocInfo ret;
        ret.legerPos[0] = -1;
        return ret;
    };
    pos.noteXPosition = 11;
    const auto info = utils->getDrawInfo(pos, scale, input, UIPrefSharpsFlats::Sharps);
    assertEQ(info.size(), 1);
    const auto legerLineIterator = info.begin();

    const auto llInfo = legerLineIterator->second.legerLinesLocInfo;

    assertEQ(legerLineIterator->second.notes.size(), 1);
    assertEQ(legerLineIterator->second.notes[0].glyph, ScoreDrawUtils::_wholeNote);

    assertClose(llInfo.legerPos[0], -1, .001);
    assertClose(llInfo.legerPos[1], 0, .001);
    assertClose(llInfo.legerPos[2], 0, .001);
}

static void testCD() {
    SqArray<int, 16> input = {MidiNote::MiddleC, MidiNote::MiddleC + MidiNote::D};
    Scale scale(MidiNote(MidiNote::C), Scale::Scales::Major);
    ScoreDrawUtilsPtr utils = ScoreDrawUtils::make();
    DrawPositionParams pos;

    const auto info = utils->getDrawInfo(pos, scale, input, UIPrefSharpsFlats::Sharps);
    assertEQ(info.size(), 2);
    auto legerLineIterator = info.begin();

    assertEQ(legerLineIterator->second.notes.size(), 1);
    const float xPosC = legerLineIterator->second.notes[0].xPosition;
    legerLineIterator++;

    // expect a blank due to moving over.
    assertEQ(legerLineIterator->second.notes.size(), 2);
    assertEQ(legerLineIterator->second.notes[0].glyph.empty(), true);
    assertEQ(legerLineIterator->second.notes[1].glyph, ScoreDrawUtils::_wholeNote);
    const float xPosD = legerLineIterator->second.notes[1].xPosition;

    assert(pos.noteColumnWidth > 0);
    assert(xPosD == xPosC + pos.noteColumnWidth);
    // It seems like C and D are in the wrong order. Maybe this test NG??
}

static void TestCSharpESharp() {
    SqArray<int, 16> input = {MidiNote::MiddleC + 1, MidiNote::MiddleC + MidiNote::E + 1};
    Scale scale(MidiNote(MidiNote::C), Scale::Scales::Major);
    ScoreDrawUtilsPtr utils = ScoreDrawUtils::make();
    DrawPositionParams pos;

    const auto info = utils->getDrawInfo(pos, scale, input, UIPrefSharpsFlats::Sharps);
    assertEQ(info.size(), 2);
    auto legerLineIterator = info.begin();

    assertEQ(legerLineIterator->second.accidentals.size(), 1);
    const float xPosCSharp = legerLineIterator->second.accidentals[0].xPosition;
    legerLineIterator++;

    // expect a blank due to moving over.
    assertEQ(legerLineIterator->second.accidentals.size(), 2);
    assertEQ(legerLineIterator->second.accidentals[0].glyph.empty(), true);
    assertEQ(legerLineIterator->second.accidentals[1].glyph, ScoreDrawUtils::_sharp);
    const float xPosESharp = legerLineIterator->second.accidentals[1].xPosition;

    assert(pos.accidentalColumnWidth > 0);
    assert(xPosESharp == xPosCSharp - pos.accidentalColumnWidth);
    // It seems like C and D are in the wrong order. Maybe this test NG??
}

static void testFunc(
    SqArray<NotationNote, 16> notes,
    SqArray<float, 16> expectedNotePositions,
    SqArray<float, 16> expectedAccidentalPositions) {
    assertGT(notes.numValid(), 0);
    assertEQ(notes.numValid(), expectedNotePositions.numValid());

    DrawPositionParams pos;
    const float xPosition = pos.noteXPosition;

               ScoreDrawUtilsPtr utils = ScoreDrawUtils::make();
    for (unsigned i = 0; i < notes.numValid(); ++i) {
        const auto& notationNote = notes.getAt(i);
        const float yPosition = pos.noteYPosition(notationNote._midiNote, notationNote._legerLine, false);
        LegerLineInfo ll;
        ll.addNote(ScoreDrawUtils::_wholeNote, xPosition, yPosition);
        const float accidentalXPosition = pos.noteXPosition - pos.accidentalColumnWidth;
        if (notationNote.isAccidental()) {
            switch (notationNote._accidental) {
                case NotationNote::Accidental::flat:
                    ll.addAccidental(ScoreDrawUtils::_flat, accidentalXPosition, yPosition);
                    break;
                case NotationNote::Accidental::natural:
                    ll.addAccidental(ScoreDrawUtils::_natural, accidentalXPosition, yPosition);
                    break;
                case NotationNote::Accidental::sharp:
                    ll.addAccidental(ScoreDrawUtils::_sharp, accidentalXPosition, yPosition);
                    break;
                case NotationNote::Accidental::none:
                    break;
                default:
                    assert(false);
            }
        }
        utils->_info.insert(std::make_pair(notationNote._legerLine, ll));
    }

    assertEQ(utils->_info.size(), notes.numValid());

    utils->_adjustNoteSpacing(pos);

    unsigned index = 0;
    for (ScoreDrawUtils::iterator iter = utils->_info.begin(); iter != utils->_info.end(); ++iter) {
        const LegerLineInfo& lineInfo = iter->second;
        assertEQ(lineInfo.notes.size(), 1);  // one not on this leger line
        assertEQ(lineInfo.notes[0].xPosition, expectedNotePositions.getAt(index));

        ++index;
        // assertEQ()
    }

    // for (unsigned i = 0; i < expectedNotePositions.numValid(); ++i) {
    //     assertEQ(expectedNotePositions.getAt(i),
    // }

    assert(false);
}

static void testAdjustMiddleC() {
    MidiNote mn = MidiNote(MidiNote::MiddleC);
    NotationNote x{mn, NotationNote::Accidental::sharp, 72, false};
    SqArray<NotationNote, 16> notes{x};
    SqArray<float, 16> expectedNotePositions{13};
    SqArray<float, 16> expectedAccidentalPositions;
    testFunc(notes, expectedNotePositions, expectedAccidentalPositions);
}

void testScoreDrawUtils() {
    test1();
    test2();
    testAccidental();

    SQINFO("!!! Find a new version of test2OneLine !!!");
    // test2OneLine();

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

    SQINFO("get new test for multi notes on a line");
    //   testTwoNotes();

    testLegerLineCTreble();

    testCD();
    TestCSharpESharp();

    //  testNoAdjustCandE();
}

#if 1
void testFirst() {
    // testScoreDrawUtils();
    // testClef2();
    // testTrebleClef2();
    // testNoAdjustCandE();
    testAdjustMiddleC();
}
#endif