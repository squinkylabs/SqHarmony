

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
    const DrawPosition& drawPos,
    const Scale& scale,
    const SqArray<int, 16>& input,
    UIPrefSharpsFlats pref) {

    // Find the spelling for treble cleff
    auto spelling = ScorePitchUtils::findSpelling(scale, input, false, pref);

    _divideClefs(spelling);
    for (unsigned pitchIterator = 0; pitchIterator < spelling.notes.numValid(); ++pitchIterator) {
        const auto notationNote = spelling.notes.getAt(pitchIterator);
        //  const MidiNote &mn = notationNote._midiNote;

        // put it into the map
        const int legerLine = notationNote._legerLine;

#ifdef _LOG
        SQINFO("in getDrawInfo pitch=%d ll=%d nn.pitch=%d", pitchIterator, legerLine, notationNote.pitch());
#endif
        auto iter = _info.find(legerLine);
        if (iter == _info.end()) {
            LegerLineInfo llinfo;
            _info.insert(std::make_pair(legerLine, llinfo));
        }
        iter = _info.find(legerLine);

        assert(iter != _info.end());
        LegerLineInfo& info = iter->second;

        assert(drawPos.noteYPosition);
        const float yPosition = drawPos.noteYPosition(notationNote._midiNote, notationNote._legerLine, notationNote._bassStaff);
        info.addOne(false, _wholeNote, drawPos.noteXPosition, yPosition);  // add this glyph for this note.
        switch (notationNote._accidental) {
            case NotationNote::Accidental::flat:
                info.addOne(true, _sharp, 0, 0);

                break;
            case NotationNote::Accidental::natural:
                info.addOne(true, _natural, 0, 0);
                break;
            case NotationNote::Accidental::sharp:
                info.addOne(true, _sharp, 0, 0);
                break;
            case NotationNote::Accidental::none:
                break;
            default:
                assert(false);
        }

        info.sort();
    }

#ifdef _LOG
    for (auto iterator = _info.begin(); iterator != _info.end(); ++iterator) {
        SQINFO("map[%d] = %s", iterator->first, iterator->second.toString().c_str());
    }
#endif
    return _info;
}