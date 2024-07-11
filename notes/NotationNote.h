#pragma once

#include <sstream>
#include <string>

#include "MidiNote.h"
#include "SqLog.h"

/**
 * @brief represents a note on a staff of standard music notation
 * 
 * Because it represents pretty much "ink on paper", it will represent
 * B sharp and C as different notes, even though they sound the same.
 * 
 * There is a MidiNote in here, so the notes do have an absolute pitch.
 * In order to determine if a NotationNote is "valid", or to convert one into some other
 * type of Note, a Scale is required. Most of the code to do that kind of thing is in ScalePitchUtils.
 * 
 */
class NotationNote {
public:
    enum class Accidental {
        none,
        sharp,
        flat,
        natural     // Note that natural and none are fairly similar, but natural means there is an actual
                    // Natural glyph next to the note. The differences between none and natural are only
                    // important when drawing the note, or when there is a Scale associated with the note.
    };
    static const unsigned llInOctave = 7;
    NotationNote() {}
    NotationNote(const MidiNote& mn, Accidental ac, int ll) : _midiNote(mn), _accidental(ac), _legerLine(ll) {}

    std::string toString() const;
    MidiNote _midiNote;
    Accidental _accidental = Accidental::none;
    int _legerLine = 0;
};

 inline std::string NotationNote::toString() const {
        std::stringstream s;
        s << _midiNote.get();
        s << ", ";
        switch (_accidental) {
            case Accidental::none:
                s << "none";
                break;
            case Accidental::sharp:
                s << "sharp";
                break;
            case Accidental::flat:
                s << "flat";
                break;
            case Accidental::natural:
                s << "natural";
                break;
            default:
                assert(false);
        }
        s << ", ";
        s << _legerLine;
        return s.str();
    }