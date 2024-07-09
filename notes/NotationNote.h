#pragma once

#include <sstream>
#include <string>

#include "MidiNote.h"
#include "SqLog.h"

class NotationNote {
public:
    enum class Accidental {
        none,
        sharp,
        flat,
        natural
    };
    NotationNote() {}
    NotationNote(const MidiNote& mn, Accidental ac, int ll) : _midiNote(mn), _accidental(ac), _legerLine(ll) {}

    std::string toString() const {
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

    MidiNote _midiNote;
    Accidental _accidental = Accidental::none;
    int _legerLine = 0;
};
