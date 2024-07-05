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

private:
    bool _reSpellFlats();
    bool _reSpellSharps();
    bool _canFlatCurrentLeger();
};


inline bool NotationNote::_canFlatCurrentLeger() {
    assert(false);
    return false;
}

// Algorithm, for more flats:
// can we flat the existing leger (depends on scale, and _accidental), then flat it.

inline bool NotationNote::_reSpellFlats() {
    SQINFO("in _reSpellFlats");
    if (_canFlatCurrentLeger()) {
        assert(false);              // don't know how
        return false;
    }

    // OK, here we know we can't flat current, so we need to go to a lower leger.

    assert(false);      // shouldn't get here.
    return false;
}

inline bool NotationNote::_reSpellSharps() {
    return false;
}

inline bool NotationNote::reSpell(bool moreSharps) {
    return moreSharps ? _reSpellSharps() : _reSpellFlats();
}