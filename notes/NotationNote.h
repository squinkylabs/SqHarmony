#pragma once

#include "MidiNote.h"

class NotationNote {
public:
    enum class Accidental {
        none,
        sharp,
        flat,
        natural
    };
    NotationNote() {}
 //   NotationNote(ScaleNote sn, Accidental ac, int ll) : _accidental(ac), _legerLine(ll) {}

    // TODO: this ctor may need midi pitch?
    NotationNote(const MidiNote& mn, Accidental ac, int ll) : _accidental(ac), _legerLine(ll) {}

    /**
     * @brief  change accidental and leger line for an alternate enharmonic spelling
     * 
     * @param moreSharps - if true will attemt to re-spell at a lower pitch, with more sharps.
     * @return  true if success.
     */
    bool reSpell(bool moreSharps);

    Accidental _accidental = Accidental::none;
    int _legerLine = 0;
    MidiNote _midiNote;
};

inline bool NotationNote::reSpell(bool moreSharps) {
    return false;
}