
#pragma once

#include "ScaleNote.h"  // TODO: get rid of this
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
    NotationNote(Accidental ac, int ll) : _accidental(ac), _legerLine(ll) {}
    Accidental _accidental = Accidental::none;
    int _legerLine = 0;
};