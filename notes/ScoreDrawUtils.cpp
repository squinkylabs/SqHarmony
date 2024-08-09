

#include "ScoreDrawUtils.h"

#include "ChordRecognizer.h"
#include "ScorePitchUtils.h"

// #define _LOG

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

/*
Algorithm:
    first find spelling on treble clef
    then divide between clefs fixing legerLine

    then put into map.

    then add the real glyphs.
    add the glyphs for the accidentals
    then fixup the note spacing for doubles.
    then fixup the accidental spacing.
*/

const std::map<int, LegerLineInfo> ScoreDrawUtils::getDrawInfo(
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
        SQINFO("in getDrawInfo pitch=%d ll=%d nn.pitch=%d", pitchIterator, legerLine, notationNote.pitch());
        SQINFO("notationNote = %s", notationNote.toString().c_str());
#endif
        bool isAlreadyNoteOnThisLine = true;
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

#ifdef _LOG
    for (auto iterator = _info.begin(); iterator != _info.end(); ++iterator) {
        SQINFO("map[%d] = %s", iterator->first, iterator->second.toString().c_str());
    }
#endif

    _adjustNoteSpacing(drawPos);
    _adjustAccidentalSpacing(drawPos);
    return _info;
}

void ScoreDrawUtils::_adjustNoteSpacing(const DrawPositionParams& pos) {
    if (_info.size() < 2) {
        return;
    }

    auto line = _info.begin();
    auto nextLine = line;
    nextLine++;
    for (; nextLine != _info.end(); ++line, ++nextLine) {
        const int lineLegerLine = line->first;
        const int lineNextLine = nextLine->first;
        SQINFO("ll=%d n=%d", lineLegerLine, lineNextLine);
        if (lineNextLine == (lineLegerLine + 1)) {
            _adjustNoteSpacing(nextLine, line, pos);
        }
    }
}

void ScoreDrawUtils::_adjustAccidentalSpacing(const DrawPositionParams& pos) {
    if (_info.size() < 2) {
        return;
    }
    LegerLineInfo* currentLine = nullptr;
    LegerLineInfo* firstRefLine = nullptr;
    LegerLineInfo* secondRefLine = nullptr;
    auto lineIterator = _info.begin();
    assert(lineIterator != _info.end());
    currentLine = &lineIterator->second;

    firstRefLine = currentLine;
    ++lineIterator;
    assert(lineIterator != _info.end());
    currentLine = &lineIterator->second;

    SQINFO("--- enter _adjustAccidentalSpacing cur=%p", currentLine);
    for (bool done = false; !done;) {
        assert(currentLine != nullptr);
        assert(firstRefLine != nullptr);
        _adjustAccidentalSpacing(currentLine, firstRefLine, secondRefLine, pos);

        ++lineIterator;
        if (lineIterator != _info.end()) {
            secondRefLine = firstRefLine;
            firstRefLine = currentLine;
            currentLine = &lineIterator->second;
            SQINFO("loop found something cur=");
        } else {
            SQINFO("ending");
            done = true;
        }

      
    }
}

void ScoreDrawUtils::_adjustAccidentalSpacing(
    LegerLineInfo* currentLine,
    LegerLineInfo* firstRefLine,
    LegerLineInfo* secondRefLine,
    const DrawPositionParams& pos) {
    SQINFO("enter aas(%p, %p, %p)", currentLine, firstRefLine, secondRefLine);
    assert(currentLine);
    assert(firstRefLine);

    // first combine the ref lines to find where accidentals are below us
    bool isAccidentalBelow[4]{false};
    for (unsigned i = 0; i < firstRefLine->accidentals.size(); ++i) {
        if (!firstRefLine->accidentals[i].glyph.empty()) {
            isAccidentalBelow[i] = true;
        }
    }
    if (secondRefLine) {
        for (unsigned i = 0; i < secondRefLine->accidentals.size(); ++i) {
            if (!secondRefLine->accidentals[i].glyph.empty()) {
                isAccidentalBelow[i] = true;
            }
        }
    }

     for (unsigned i = 0; i < currentLine->accidentals.size(); ++i) {
        if (isAccidentalBelow[i]) {
            currentLine->accidentals.shift(std::string());
        }
     }

      for (unsigned i = 0; i < currentLine->accidentals.size(); ++i) {
        currentLine->accidentals[i].xPosition = pos.noteXPosition - (i + 1) * pos.accidentalColumnWidth;
      }

    //  drawPos.noteXPosition - drawPos.columnWidth;
}

void ScoreDrawUtils::_adjustNoteSpacing(
    iterator nextLine,
    iterator line,
    const DrawPositionParams& pos) {
    bool isNoteBelow[4]{false};
    const auto& refNotes = line->second.notes;
    for (unsigned i = 0; i < refNotes.size(); ++i) {
        if (!refNotes[i].glyph.empty()) {
            isNoteBelow[i] = true;
        }
    }

    // now, go through the notes in the line we are adjusting
    auto& currentNotes = nextLine->second.notes;
    for (unsigned i = 0; i < currentNotes.size(); ++i) {
        if (isNoteBelow[i]) {
            bool haveNoteHere = !currentNotes[i].glyph.empty();
            if (haveNoteHere) {
                currentNotes.shift(std::string());
            }
        }
    }
    for (unsigned i = 0; i < currentNotes.size(); ++i) {
        // bool haveNoteHere = !currentNotes[i].glyph.empty();
        // SymbolInfo& si = currentNotes[i];
        currentNotes[i].xPosition = pos.noteXPosition + i * pos.noteColumnWidth;
    }
    SQINFO("leave adj next = %s", nextLine->second.toString().c_str());
    SQINFO("leave adj ref = %s", line->second.toString().c_str());
}