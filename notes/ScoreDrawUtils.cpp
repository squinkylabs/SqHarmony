

#include "ScoreDrawUtils.h"

#include "ChordRecognizer.h"
#include "ScorePitchUtils.h"

 #define _LOG

ScoreDrawUtilsPtr ScoreDrawUtils::make() {
    return std::make_unique<ScoreDrawUtils>();
}

void ScoreDrawUtils::_divideClefs(ScorePitchUtils::SpellingResults& s) {
    const unsigned size = s.notes.numValid();
    bool areBassNotes = false;
    bool areTrebleNotes = false;
    for (unsigned i = 0; i < size; ++i) {
        NotationNote& note = s.notes.getAt(i);
        const unsigned pitch = note.pitch();
        if (pitch > MidiNote::MiddleC) {
            areTrebleNotes = true;
        }
        if (pitch < MidiNote::MiddleC) {
            areBassNotes = true;
        }
    }

    for (unsigned i = 0; i < size; ++i) {
        NotationNote& note = s.notes.getAt(i);
        const unsigned pitch = note.pitch();
        // group middle C with other notes, if they are all in bass.
        const bool wasBassStaff = note._bassStaff;
        note._bassStaff = (pitch == MidiNote::MiddleC) ? (areBassNotes && !areTrebleNotes) : pitch < MidiNote::MiddleC;
        if (note._bassStaff && !wasBassStaff) {
            note._legerLine += 12;
        }
#ifdef _LOG
        SQINFO(
            "_divideClefs pitch=%d bass=%d areBass=%d areTreb =%d midc=%d",
            pitch,
            note._bassStaff,
            areBassNotes,
            areTrebleNotes,
            MidiNote::MiddleC);
#endif
    }
}

const ScoreDrawUtils::DrawInfo ScoreDrawUtils::getDrawInfo(
    const DrawPositionParams& drawPos,
    const Scale& scale,
    const SqArray<int, 16>& input,
    UIPrefSharpsFlats pref) {
    // Find the spelling for treble clef
    auto spelling = ScorePitchUtils::findSpelling(scale, input, false, pref);

    _divideClefs(spelling);
    for (unsigned pitchIterator = 0; pitchIterator < spelling.notes.numValid(); ++pitchIterator) {
        const auto notationNote = spelling.notes.getAt(pitchIterator);
        // put it into the map
        const int legerLine = notationNote._legerLine;

#ifdef _LOG
        SQINFO("!! in getDrawInfo pitch=%d ll=%d nn.pitch=%d bassClef=%d", pitchIterator, legerLine, notationNote.pitch(), notationNote._bassStaff);
        SQINFO("notationNote = %s", notationNote.toString().c_str());
#endif
        bool isAlreadyNoteOnThisLine = true;

        // TODO: rename this
        std::map<int, LegerLineInfo>& _info = notationNote._bassStaff ? _infoBassClef : _infoTrebleClef;
        auto iter = _info.find(legerLine);
        // If there is no entry already, make one.
        if (iter == _info.end()) {
            LegerLineInfo llinfo;
            _info.insert(std::make_pair(legerLine, llinfo));
            isAlreadyNoteOnThisLine = false;
        }
        iter = _info.find(legerLine);
        assert(iter != _info.end());

        LegerLineInfo& info = iter->second;
        float noteXPosition = drawPos.noteXPosition;
        if (isAlreadyNoteOnThisLine) {
            // if there are two notes, put the second one column to the right.
            noteXPosition += drawPos.noteColumnWidth;
        }

        assert(drawPos.noteYPosition);
        const float yPosition = drawPos.noteYPosition(notationNote._midiNote, notationNote._legerLine, notationNote._bassStaff);
        info.legerLinesLocInfo = drawPos.llDrawInfo(notationNote._midiNote, notationNote._legerLine, notationNote._bassStaff);
        info.addNote(_wholeNote, noteXPosition, yPosition);  // add this glyph for this note.

        const float accidentalXPosition = drawPos.noteXPosition - drawPos.accidentalColumnWidth;
#ifdef _LOG
        SQINFO("drawing note at %f, accidental at %f y=%f", drawPos.noteXPosition, accidentalXPosition, yPosition);
#endif
        switch (notationNote._accidental) {
            case NotationNote::Accidental::flat:
                info.addAccidental(_flat, accidentalXPosition, yPosition);
                break;
            case NotationNote::Accidental::natural:
                info.addAccidental(_natural, accidentalXPosition, yPosition);
                break;
            case NotationNote::Accidental::sharp:
                info.addAccidental(_sharp, accidentalXPosition, yPosition);
                break;
            case NotationNote::Accidental::none:
                break;
            default:
                assert(false);
        }

        // info.sort();
    }

#if 0 // def _LOG
    for (auto iterator = _info.begin(); iterator != _info.end(); ++iterator) {
        SQINFO("map[%d] = %s", iterator->first, iterator->second.toString().c_str());
    }
#endif

    _adjustNoteSpacing(drawPos);
    _adjustAccidentalSpacing(drawPos);

    ScoreDrawUtils::DrawInfo retValues;
    for (auto iter : _infoBassClef) {
      //  retValues.push_back(iter.second);
        retValues.push_back(std::make_tuple(iter.first, true, iter.second));
    }
    for (auto iter : _infoTrebleClef) {
       // retValues.push_back(iter.second);
        retValues.push_back(std::make_tuple(iter.first, false, iter.second));
    }

    return retValues;
}

void ScoreDrawUtils::_adjustNoteSpacing(const DrawPositionParams& pos) {
    _adjustNoteSpacing(pos, _infoBassClef);
    _adjustNoteSpacing(pos, _infoTrebleClef);
}

void ScoreDrawUtils::_adjustNoteSpacing(const DrawPositionParams& pos, ScoreDrawUtils::infoMap& _info) {
#ifdef _LOG
    SQINFO("-- enter _adjustNoteSpacing");
#endif
    if (_info.size() < 2) {
        return;
    }

    auto line = _info.begin();
    auto nextLine = line;
    nextLine++;
    for (; nextLine != _info.end(); ++line, ++nextLine) {
        const int lineLegerLine = line->first;
        const int lineNextLine = nextLine->first;

        // Call adjust any time we see the adjacent leger lines.
        if (lineNextLine == (lineLegerLine + 1)) {
            _adjustNoteSpacing(nextLine, line, pos);
        }
    }
#ifdef _LOG
    SQINFO("-- exit _adjustNoteSpacing");
#endif
}


void ScoreDrawUtils::_adjustAccidentalSpacing(const DrawPositionParams& pos) {
    _adjustAccidentalSpacing(pos, _infoBassClef);
    _adjustAccidentalSpacing(pos, _infoTrebleClef);
}

void ScoreDrawUtils::_adjustAccidentalSpacing(const DrawPositionParams& pos,  ScoreDrawUtils::infoMap& _info) {
    if (_info.size() < 2) {
        return;
    }
#ifdef _LOG
    SQINFO("-- enter _adjustAccidentalSpacing");
    for (iterator lineIterator = _info.begin(); lineIterator != _info.end(); ++lineIterator) {
        SQINFO("ll %d info=%s", lineIterator->first, lineIterator->second.toString().c_str());
    }
#endif
    // lineIterator
    for (iterator lineIterator = _info.begin(); lineIterator != _info.end(); ++lineIterator) {
        // assert(false);

        // LegerLineInfo* currentLine = &lineIterator->second;
        _adjustAccidentalSpacing(pos, _info, lineIterator);
    }
    // assert(false);
}

void ScoreDrawUtils::_adjustAccidentalSpacing(
    const DrawPositionParams& pos,
    const infoMap& _info,
    iterator lineToAdjust) {
    std::vector<const LegerLineInfo*> refLines;

    //   const LegerLineInfo& lineToAdjustInfo = lineToAdjust->second;
    for (const_iterator iterator = _info.begin(); iterator != lineToAdjust; ++iterator) {
        if (_linesCouldOverlap(pos, iterator->second, lineToAdjust->second)) {
            // assert(false);
            const LegerLineInfo* p = &iterator->second;
            refLines.push_back(p);
        }
    }
    return _adjustAccidentalSpacing(pos, lineToAdjust, refLines);
}

bool ScoreDrawUtils::_linesCouldOverlap(const DrawPositionParams& drawPos, const LegerLineInfo& a, const LegerLineInfo& b) {
    const float aPos = _getYPosition(a);
    const float bPos = _getYPosition(b);
    const float dif = std::abs(aPos - bPos);
    const auto ret = dif < drawPos.accidentalHeight;
#ifdef _LOG
    SQINFO("_linesCouldOverlap %f, %f will ret %d, diff=%f, h=%f", aPos, bPos, ret, dif, drawPos.accidentalHeight);
#endif
    return ret;
}

float ScoreDrawUtils::_getYPosition(const LegerLineInfo& lli) {
    //  ShiftingArray<SymbolInfo>::const_iterator iter = lli.notes.begin();
    for (auto iter = lli.notes.begin(); iter != lli.notes.end(); ++iter) {
        if (!iter->glyph.empty()) {
            return iter->yPosition;
        }
    }
    assert(false);
    return 0;
}

void ScoreDrawUtils::_adjustAccidentalSpacing(
    const DrawPositionParams& pos,
    iterator currentLine,
    const std::vector<const LegerLineInfo*>& refLines) {
    //const float yPos = _getYPosition(currentLine->second);
    #define MAX_COLLISION 5
    bool isAccidentalBelow[MAX_COLLISION]{false};

    // Loop through all the lines that are close to us,
    // building up isNoteBelow.
    for (auto lineIter : refLines) {
        const LegerLineInfo& info = *lineIter;
        for (unsigned i = 0; i < info.accidentals.size(); ++i) {
            if (!info.accidentals[i].glyph.empty()) {
                isAccidentalBelow[i] = true;
            }
        }
    }

    for (unsigned i = 0; i < currentLine->second.accidentals.size(); ++i) {
        if (isAccidentalBelow[i]) {
            currentLine->second.accidentals.shift(std::string());
#ifdef _LOG
            SQINFO("shift current line");
#endif
        }
    }

    for (unsigned i = 0; i < currentLine->second.accidentals.size(); ++i) {
        currentLine->second.accidentals[i].xPosition = pos.noteXPosition - (i + 1) * pos.accidentalColumnWidth;
#ifdef _LOG
        SQINFO("setting accidental [%d] xpos to %f", i, currentLine->second.accidentals[i].xPosition);
#endif
    }
}


void ScoreDrawUtils::_adjustNoteSpacing(
    iterator nextLine,
    iterator line,
    const DrawPositionParams& pos) {
#ifdef _LOG
    SQINFO("-- enter _adjustNoteSpacing for line %s", nextLine->second.toString().c_str());
    SQINFO(" first ref %s", line->second.toString().c_str());
#endif

    bool isNoteBelow[4]{false};
    const auto& refNotes = line->second.notes;
    for (unsigned i = 0; i < refNotes.size(); ++i) {
        if (!refNotes[i].glyph.empty()) {
            isNoteBelow[i] = true;
        }
    }

#ifdef _LOG
    for (unsigned i = 0; i < 3; ++i) {
        SQINFO("index %d isBelow=%d", i, isNoteBelow[i]);
    }
#endif

    // now, go through the notes in the line we are adjusting
    auto& currentNotes = nextLine->second.notes;
    for (unsigned i = 0; i < currentNotes.size(); ++i) {
        if (isNoteBelow[i]) {
            bool haveNoteHere = !currentNotes[i].glyph.empty();
            if (haveNoteHere) {
                currentNotes.shift(std::string());
#ifdef _LOG
                SQINFO("shift current line");
#endif
            }
        }
    }
    for (unsigned i = 0; i < currentNotes.size(); ++i) {
        // bool haveNoteHere = !currentNotes[i].glyph.empty();
        // SymbolInfo& si = currentNotes[i];
        currentNotes[i].xPosition = pos.noteXPosition + i * pos.noteColumnWidth;
    }
}