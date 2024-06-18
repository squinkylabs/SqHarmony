
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


 inline std::tuple<ScaleNote, ScorePitchUtils::Accidental> ScorePitchUtils::getNotationNote(const Scale& scale, const MidiNote& midiNote) {
    ScaleNote sn = scale.m2s(midiNote);
    Accidental accidental = Accidental::none;
    switch (sn.getAdjustment()) {
        case ScaleNote::RelativeAdjustment::none:
            break;
        case ScaleNote::RelativeAdjustment::sharp:
            accidental = Accidental::sharp;
            break;
        case ScaleNote::RelativeAdjustment::flat:
            accidental = Accidental::flat;
            break;
        default:
            assert(false);
    }

     return std::make_tuple(sn, accidental);
 }

