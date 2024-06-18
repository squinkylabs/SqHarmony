
#pragma once

#include "MidiNote.h"
#include "Scale.h"
#include "ScaleNote.h"

#include <tuple>

class ScorePitchUtils {
public:
    ScorePitchUtils() = delete;
    enum class Accidental {
        none,
        sharp,
        flat,
        natural
    };

    static std::tuple<ScaleNote, Accidental> getNotationNote(const Scale&, const MidiNote&);
    // (SRN, Accidental) getAccidental(scale, MidiNote)
};


 inline std::tuple<ScaleNote, ScorePitchUtils::Accidental> ScorePitchUtils::getNotationNote(const Scale&, const MidiNote&) {
     ScaleNote sn;
     return std::make_tuple(sn, Accidental::none);
 }

