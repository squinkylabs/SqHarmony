
#pragma once

#include <tuple>

#include "MidiNote.h"
#include "Scale.h"
#include "ScaleNote.h"

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

    if (sn.getAdjustment() == ScaleNote::RelativeAdjustment::none) {
        accidental = Accidental::none;
    } else if (!midiNote.isBlackKey()) {
        accidental = Accidental::natural;
    } else {
        assert(false);
        accidental = Accidental::sharp;
    }

    return std::make_tuple(sn, accidental);
#if 0
    // Just pass on the stuff from the ScaleNote. It's not quite right, but for now will get tests started.
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
#endif
}
