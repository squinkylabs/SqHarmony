#pragma once

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

    MidiNote _midiNote;
    Accidental _accidental = Accidental::none;
    int _legerLine = 0;
};
