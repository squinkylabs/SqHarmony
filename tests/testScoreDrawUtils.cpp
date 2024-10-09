

#include "MidiNote.h"
#include "Scale.h"
#include "ScoreDrawUtils.h"
#include "SharpsFlatsPref.h"
#include "asserts.h"

static const  ScoreDrawUtils::DrawInfo testCMajorSub(const SqArray<int, 16>& input, DrawPositionParams pos) {
    Scale scale(MidiNote(MidiNote::C), Scale::Scales::Major);
    ScoreDrawUtilsPtr utils = ScoreDrawUtils::make();
    //  DrawPosition pos;
    return utils->getDrawInfo(pos, scale, input, UIPrefSharpsFlats::Sharps);
    // return utils;
}

static const ScoreDrawUtils::DrawInfo testCMajorSub(const SqArray<int, 16>& input) {
    DrawPositionParams pos;
    return testCMajorSub(input, pos);
    // return utils;
}

LegerLineInfo find(const ScoreDrawUtils::DrawInfo& info, int line, bool bassClef = false) {
    for (auto x : info) {
        if (std::get<0>(x) == line &&
            std::get<1>(x) == bassClef) {
            return std::get<2>(x);
        }
    }
   // assert(false);
    return LegerLineInfo();
}

static void test1() {
    SqArray<int, 16> test = {MidiNote::MiddleC};
    const auto info = testCMajorSub(test);
    assertEQ(info.size(), 1);
   // const auto iter = info.find(-2);
   // assertEQ(iter->second.notes.size(), 1);
   const auto x = find(info, -2);
   assertEQ(x.notes.size(), 1);
}

static void test2() {
    SqArray<int, 16> test = {MidiNote::MiddleC, MidiNote::MiddleC + MidiNote::E};
    const auto info = testCMajorSub(test);
    assertEQ(info.size(), 2);

    int i = 0;
    for (auto iter = info.begin(); iter != info.end(); ++iter) {
      //  assertEQ(iter->notes.size(), 1);

        const LegerLineInfo& info = std::get<2>(*iter);
        assertEQ(info.notes.size(), 1);
        ++i;
    }
    assertEQ(i, 2);
}

static void testAccidental() {
    SqArray<int, 16> test = {MidiNote::MiddleC + 1};
    const auto info = testCMajorSub(test);
    assertEQ(info.size(), 1);
  //  const auto iter = info.find(-2);
    const auto x = find(info, -2);

    assertEQ(x.notes.size(), 1);
    assertEQ(x.accidentals.size(), 1);

    assertEQ(x.accidentals[0].glyph, ScoreDrawUtils::_sharp);
    assertEQ(x.notes[0].glyph, ScoreDrawUtils::_wholeNote);
}


static void test2OneLine() {
    // C and C# are on same leger line
    SqArray<int, 16> test = {MidiNote::MiddleC, MidiNote::MiddleC + 1};
    const auto info = testCMajorSub(test);
    assertEQ(info.size(), 1);


  //  const auto lli = find(info, -2);
  //  assertEQ(lli.symbols.size(), 3);   // expect #, C, C
}

static void testToString() {
    SqArray<int, 16> test = {MidiNote::MiddleC};
    const auto info = testCMajorSub(test);
    assertEQ(info.size(), 1);
   // const auto iter = info.find(-2);
  //  assertGT(iter->second.toString().length(), 0);
    const auto lli = find(info, -2);
    assertGT(lli.toString().length(), 0);
}


static void testXPos() {
    SqArray<int, 16> input = {MidiNote::MiddleC + 1};
    Scale scale(MidiNote(MidiNote::C), Scale::Scales::Major);
    ScoreDrawUtilsPtr utils = ScoreDrawUtils::make();
    DrawPositionParams pos;
    pos.noteXPosition = 11;
    const auto info = utils->getDrawInfo(pos, scale, input, UIPrefSharpsFlats::Sharps);

    assertEQ(info.size(), 1);

    const auto lli = std::get<2>(*info.begin());
    assertEQ(lli.accidentals.size(), 1);
    assertEQ(lli.notes.size(), 1);
    assertEQ(lli.accidentals[0].xPosition, 7.5);
    assertEQ(lli.accidentals[0].glyph, ScoreDrawUtils::_sharp);

    assertEQ(lli.notes[0].xPosition, 11);
    assertEQ(lli.notes[0].glyph, ScoreDrawUtils::_wholeNote);
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

    const auto lli = std::get<2>(*info.begin());
    assertEQ(lli.notes.size(), 1)
    assertEQ(lli.accidentals.size(), 1);

    assertEQ(lli.notes[0].yPosition, -2);
    assertEQ(lli.accidentals[0].yPosition, -2);
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

    const auto lli = std::get<2>(*info.begin());
    assertEQ(lli.notes.size(), 1);
    assertEQ(lli.notes[0].yPosition, (bass ? 100.f : 200.f)); 
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

    const auto lli = std::get<2>(*info.begin());
    //auto iterator = info.begin();

    assertEQ(lli.notes.size(), 1);
    const float y1 = lli.notes[0].yPosition;
   // iterator++;
    const auto lli2 = std::get<2>(*(info.begin()++));
    assertEQ(lli2.notes.size(), 1);
    const float y2 = lli2.notes[0].yPosition;

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

    const auto lli = std::get<2>(*info.begin());
    // look at first note
    assertEQ(lli.notes.size(), 1);

    // look at first symbol of first note
   // auto iterator = info.begin();

    const float y1 = lli.notes[0].yPosition;
   // iterator++;
    const auto lli2 = std::get<2>(*(info.begin()++));
    const float y2 = lli2.notes[0].yPosition;

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
    const auto lli = std::get<2>(*info.begin());
    assertEQ(lli.notes.size(), 1);

 //   auto iterator = info.begin();
    const float y1 = lli.notes[0].yPosition;
 //   iterator++;
     const auto lli2 = std::get<2>(*(info.begin()++));
    const float y2 = lli2.notes[0].yPosition;

    assertEQ(y1, expectedY);
    assertEQ(y2, expectedY);
}

static void testCTreble() {
    testCGroup({MidiNote::MiddleC, MidiNote::MiddleC + MidiNote::G}, 200);
}

static void testCBass() {
    testCGroup({MidiNote::MiddleC, MidiNote::MiddleC - 12 + MidiNote::G}, 100);
}

#if 0

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

static void testAdjustSpacingFunc(
    SqArray<NotationNote, 16> notes,
    SqArray<float, 16> expectedNotePositions,
    SqArray<float, 16> expectedAccidentalPositions) {
    assertGT(notes.numValid(), 0);
    assertEQ(notes.numValid(), expectedNotePositions.numValid());
    assertEQ(notes.numValid(), expectedAccidentalPositions.numValid());

    DrawPositionParams pos;
    pos.noteYPosition = [](const MidiNote& note, int legerLine, bool bassStaff) {
        if (bassStaff) {
            legerLine -= 8;        // this isn't quite right...
        }
        return legerLine * 10;
    };
    pos.accidentalHeight = 55;      // make this work with my yPos function.
                                    // 55 makes CA work as expected.
    const float xPosition = pos.noteXPosition;

    ScoreDrawUtilsPtr utils = ScoreDrawUtils::make();
    for (unsigned i = 0; i < notes.numValid(); ++i) {
        const auto& notationNote = notes.getAt(i);
        const float yPosition = pos.noteYPosition(notationNote._midiNote, notationNote._legerLine, false);
        LegerLineInfo ll;
        ll.addNote(ScoreDrawUtils::_wholeNote, xPosition, yPosition);
        const float accidentalXPosition = pos.noteXPosition - pos.accidentalColumnWidth;
        //SQINFO("In test, adding accidental at y=%f ", yPosition);
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
    utils->_adjustAccidentalSpacing(pos);

    // Loop over every leger line
    unsigned index = 0;
    for (ScoreDrawUtils::iterator iter = utils->_info.begin(); iter != utils->_info.end(); ++iter) {
        const LegerLineInfo& lineInfo = iter->second;

        //SQINFO("eval test restuls, ll=%d", iter->first);

        // Find the note glyphs that has the actual note.
        unsigned noteGlyphIndex = 100;
        ;
        for (unsigned i = 0; i < lineInfo.notes.size(); ++i) {
            if (lineInfo.notes[i].glyph == ScoreDrawUtils::_wholeNote) {
                noteGlyphIndex = i;
            }
        }
        assert(noteGlyphIndex < 100);

        // Find the accidental glyphs that has the actual accidental.
        unsigned accidentalGlyphIndex = 100;
        for (unsigned i = 0; i < lineInfo.accidentals.size(); ++i) {
            if (!lineInfo.accidentals[i].glyph.empty()) {
                accidentalGlyphIndex = i;
            }
        }
        assert(lineInfo.accidentals.empty() || (accidentalGlyphIndex < 100));

        assertEQ(lineInfo.notes[noteGlyphIndex].xPosition, expectedNotePositions.getAt(index));
        if (!lineInfo.accidentals.empty()) {
            assertEQ(lineInfo.accidentals[accidentalGlyphIndex].xPosition, expectedAccidentalPositions.getAt(index));
        }
        ++index;
    }
}

static void testAdjustSpacingMiddleC() {
    DrawPositionParams pos;
    MidiNote mn = MidiNote(MidiNote::MiddleC);
    NotationNote x{mn, NotationNote::Accidental::sharp, -2, false};
    SqArray<NotationNote, 16> notes{x};
    SqArray<float, 16> expectedNotePositions{pos.noteXPosition};
    SqArray<float, 16> expectedAccidentalPositions{pos.noteXPosition - pos.accidentalColumnWidth};
    testAdjustSpacingFunc(notes, expectedNotePositions, expectedAccidentalPositions);
}

static void testAdjustSpacingMiddleCAndD() {
    DrawPositionParams pos;
    MidiNote mnC = MidiNote(MidiNote::MiddleC);  // can use the same midi note - it's wrong, but doesn't affect this test.
    NotationNote c{mnC, NotationNote::Accidental::none, -2, false};
    NotationNote d{mnC, NotationNote::Accidental::none, -1, false};
    SqArray<NotationNote, 16> notes{c, d};
    SqArray<float, 16> expectedNotePositions{pos.noteXPosition, pos.noteXPosition + pos.noteColumnWidth};
    SqArray<float, 16> expectedAccidentalPositions{0, 0};  // doesn't matter
    testAdjustSpacingFunc(notes, expectedNotePositions, expectedAccidentalPositions);
}

static void testAdjustSpacingCSharpESharp() {
    DrawPositionParams pos;
    MidiNote mnC = MidiNote(MidiNote::MiddleC);  // can use the same midi note - it's wrong, but doesn't affect this test.
    SqArray<NotationNote, 16> notes{
        {mnC, NotationNote::Accidental::sharp, -2, false},
        {mnC, NotationNote::Accidental::sharp, 0, false}};
    SqArray<float, 16> expectedNotePositions{pos.noteXPosition, pos.noteXPosition};
    SqArray<float, 16> expectedAccidentalPositions{
        pos.noteXPosition - pos.accidentalColumnWidth,
        pos.noteXPosition - 2 * pos.accidentalColumnWidth};
    testAdjustSpacingFunc(notes, expectedNotePositions, expectedAccidentalPositions);
}

static void testAdjustSpacing3Stack() {
    DrawPositionParams pos;
    MidiNote mnC = MidiNote(MidiNote::MiddleC);  // can use the same midi note - it's wrong, but doesn't affect this test.
    SqArray<NotationNote, 16> notes{
        {mnC, NotationNote::Accidental::sharp, -2, false},  // C#
        {mnC, NotationNote::Accidental::sharp, -1, false},  // D#
        {mnC, NotationNote::Accidental::sharp, 0, false},   // E#
    };
    SqArray<float, 16> expectedNotePositions{
        pos.noteXPosition,
        pos.noteXPosition + pos.noteColumnWidth,
        pos.noteXPosition};
    SqArray<float, 16> expectedAccidentalPositions{
        pos.noteXPosition - pos.accidentalColumnWidth,
        pos.noteXPosition - 2 * pos.accidentalColumnWidth,
        pos.noteXPosition - 3 * pos.accidentalColumnWidth};
    testAdjustSpacingFunc(notes, expectedNotePositions, expectedAccidentalPositions);
}

// fails currently
static void testAdjustSpacing4Stack() {
    DrawPositionParams pos;
    MidiNote mnC = MidiNote(MidiNote::MiddleC);  // can use the same midi note - it's wrong, but doesn't affect this test.
    SqArray<NotationNote, 16> notes{
        {mnC, NotationNote::Accidental::sharp, -2, false},  // C#
        {mnC, NotationNote::Accidental::sharp, -1, false},  // D#
        {mnC, NotationNote::Accidental::sharp, 0, false},   // E#
        {mnC, NotationNote::Accidental::sharp, 1, false},   // F#
    };
    SqArray<float, 16> expectedNotePositions{
        pos.noteXPosition,
        pos.noteXPosition + pos.noteColumnWidth,
        pos.noteXPosition,
        pos.noteXPosition + pos.noteColumnWidth};
    SqArray<float, 16> expectedAccidentalPositions{
        pos.noteXPosition - pos.accidentalColumnWidth,
        pos.noteXPosition - 2 * pos.accidentalColumnWidth,
        pos.noteXPosition - 3 * pos.accidentalColumnWidth,
        pos.noteXPosition - 4 * pos.accidentalColumnWidth};
    testAdjustSpacingFunc(notes, expectedNotePositions, expectedAccidentalPositions);
}

static void testAdjustSpacingCA() {
    DrawPositionParams pos;
    MidiNote mnC = MidiNote(MidiNote::MiddleC);  // can use the same midi note - it's wrong, but doesn't affect this test.
    SqArray<NotationNote, 16> notes{
        {mnC, NotationNote::Accidental::sharp, -2, false},  // C#
        {mnC, NotationNote::Accidental::sharp, 3, false}    // A#
    };
    SqArray<float, 16> expectedNotePositions{
        pos.noteXPosition,
        pos.noteXPosition};
    SqArray<float, 16> expectedAccidentalPositions{
        pos.noteXPosition - pos.accidentalColumnWidth,
        pos.noteXPosition - 2 * pos.accidentalColumnWidth};
    testAdjustSpacingFunc(notes, expectedNotePositions, expectedAccidentalPositions);
}

static void testAdjustSpacingCB() {
    DrawPositionParams pos;
    MidiNote mnC = MidiNote(MidiNote::MiddleC);  // can use the same midi note - it's wrong, but doesn't affect this test.
    SqArray<NotationNote, 16> notes{
        {mnC, NotationNote::Accidental::sharp, -2, false},  // C#
        {mnC, NotationNote::Accidental::sharp, 4, false}    // B#
    };
    SqArray<float, 16> expectedNotePositions{
        pos.noteXPosition,
        pos.noteXPosition};
    SqArray<float, 16> expectedAccidentalPositions{
        pos.noteXPosition - pos.accidentalColumnWidth,
        pos.noteXPosition - pos.accidentalColumnWidth};
    testAdjustSpacingFunc(notes, expectedNotePositions, expectedAccidentalPositions);
}

static void testAdjustSpacingOctave() {
    DrawPositionParams pos;
    MidiNote mnC = MidiNote(MidiNote::MiddleC);  // can use the same midi note - it's wrong, but doesn't affect this test.
    SqArray<NotationNote, 16> notes{
        {mnC, NotationNote::Accidental::sharp, -2, false},  // C#
        {mnC, NotationNote::Accidental::sharp, 5, false}    // C#
    };
    SqArray<float, 16> expectedNotePositions{
        pos.noteXPosition,
        pos.noteXPosition };
    SqArray<float, 16> expectedAccidentalPositions{
        pos.noteXPosition - pos.accidentalColumnWidth,
        pos.noteXPosition - pos.accidentalColumnWidth };
    testAdjustSpacingFunc(notes, expectedNotePositions, expectedAccidentalPositions);
}

static void testAdjustSpacingTwoStaves() {
    DrawPositionParams pos;
    MidiNote mnC = MidiNote(MidiNote::MiddleC);  // can use the same midi note - it's wrong, but doesn't affect this test.

    // two notes, same pos on two staves
    SqArray<NotationNote, 16> notes{
        {mnC, NotationNote::Accidental::sharp, 2, false}, 
        {mnC, NotationNote::Accidental::sharp, 2, true}   
    };
    SqArray<float, 16> expectedNotePositions{
        pos.noteXPosition,
        pos.noteXPosition };
    // SqArray<float, 16> expectedAccidentalPositions{
    //     pos.noteXPosition - pos.accidentalColumnWidth,
    //     pos.noteXPosition - pos.accidentalColumnWidth };
    SqArray<float, 16> expectedAccidentalPositions{0, 0};  // doesn't matter
    testAdjustSpacingFunc(notes, expectedNotePositions, expectedAccidentalPositions);
}

static void testAdjustSpacing() {
    testAdjustSpacingMiddleC();
    testAdjustSpacingMiddleCAndD();
    testAdjustSpacingCSharpESharp();
    testAdjustSpacing3Stack();
    testAdjustSpacing4Stack();
    testAdjustSpacingCA();
    testAdjustSpacingCB();
    testAdjustSpacingOctave();
    testAdjustSpacingTwoStaves();
}

#endif

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
    #if 0
    testTrebleLegerLine();
    testBassLegerLine();

    testAdjustSpacing();

    SQINFO("get new test for multi notes on a line");
    //   testTwoNotes();

    testLegerLineCTreble();

    testCD();
    TestCSharpESharp();

    //  testNoAdjustCandE();
#endif
}

#if 1
void testFirst() {

 //  testScoreDrawUtils();
    testBothClefs();
}
#endif