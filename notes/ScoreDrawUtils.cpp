

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
        SQINFO("!! in getDrawInfo pitch=%d ll=%d nn.pitch=%d bassClef=%d", pitchIterator, legerLine, notationNote.pitch(), notationNote._bassStaff);
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
    if (_info.size() < 2) {
        return;
    }
#ifdef _LOG
    SQINFO("-- enter _adjustAccidentalSpacing");
#endif
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

    for (bool done = false; !done;) {
        assert(currentLine != nullptr);
        assert(firstRefLine != nullptr);
        _adjustAccidentalSpacing(currentLine, firstRefLine, secondRefLine, pos);

        ++lineIterator;
        if (lineIterator != _info.end()) {
            secondRefLine = firstRefLine;
            firstRefLine = currentLine;
            currentLine = &lineIterator->second;
        } else {
            done = true;
        }
    }
#ifdef _LOG
    SQINFO("-- exit _adjustAccidentalSpacing");
#endif
}

void ScoreDrawUtils::_adjustAccidentalSpacing(
    LegerLineInfo* currentLine,
    LegerLineInfo* firstRefLine,
    LegerLineInfo* secondRefLine,
    const DrawPositionParams& pos) {
    assert(currentLine);
    assert(firstRefLine);

#ifdef _LOG
    SQINFO("-- enter _adjustAccidentalSpacing for line %s", currentLine->toString().c_str());
    SQINFO(" first ref %s", firstRefLine->toString().c_str());
    if (secondRefLine) SQINFO(" second ref %s", secondRefLine->toString().c_str());
#endif

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

#ifdef _LOG
    for (unsigned i = 0; i < 3; ++i) {
        SQINFO("index %d isBelow=%d", i, isAccidentalBelow[i]);
    }
#endif

    for (unsigned i = 0; i < currentLine->accidentals.size(); ++i) {
        if (isAccidentalBelow[i]) {
            currentLine->accidentals.shift(std::string());
#ifdef _LOG
            SQINFO("shift current line");
#endif
        }
    }

    for (unsigned i = 0; i < currentLine->accidentals.size(); ++i) {
        currentLine->accidentals[i].xPosition = pos.noteXPosition - (i + 1) * pos.accidentalColumnWidth;
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