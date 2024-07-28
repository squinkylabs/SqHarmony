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
        natural  // Note that natural and none are fairly similar, but natural means there is an actual
                 // Natural glyph next to the note. The differences between none and natural are only
                 // important when drawing the note, or when there is a Scale associated with the note.
    };
    static const unsigned llInOctave = 7;
    NotationNote() {}
    NotationNote(const MidiNote& mn, Accidental ac, int ll, bool bs);
    bool operator==(const NotationNote& other) const;
    bool operator!=(const NotationNote& other) const;
    // next three (int, operator=, pitch to make it sortable)
    operator int() const { return pitch(); }
    void operator=(int pitch) { this->_midiNote._changePitch(pitch); }
    int pitch() const {
        return _midiNote.get();
    }

    bool isAccidental() const;

    std::string toString() const;
    MidiNote _midiNote;
    Accidental _accidental = Accidental::none;
    int _legerLine = 0;
    bool _bassStaff = false;
};

inline NotationNote::NotationNote(const MidiNote& mn, Accidental ac, int ll, bool bs) : _midiNote(mn),
                                                                                        _accidental(ac),
                                                                                        _legerLine(ll),
                                                                                        _bassStaff(bs) {
}

inline bool NotationNote::isAccidental() const {
    return _accidental != Accidental::none;
}

inline bool NotationNote::operator==(const NotationNote& other) const {
    return (other._midiNote.get() == this->_midiNote.get()) &&
           (other._accidental == this->_accidental) &&
           (other._legerLine == this->_legerLine) &&
           (other._bassStaff == this->_bassStaff);
            
}

inline bool NotationNote::operator!=(const NotationNote& other) const {
    return !(other == *this);
}
inline std::string NotationNote::toString() const {
    std::stringstream s;
    s << _midiNote.get();
    s << ", ";
    switch (_accidental) {
        case Accidental::none:
            s << "accid none";
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