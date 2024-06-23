
#pragma once

#include <tuple>

#include "MidiNote.h"
#include "Scale.h"
#include "ScaleNote.h"
#include "SqLog.h"

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

    /** 
     * @return returns true if accidental1 <= accidental2 
     * @return false if  accidental1 > accidental2 
     */
    static bool compareAccidentals(Accidental accidental1, Accidental accidental2);
};

inline std::tuple<ScaleNote, ScorePitchUtils::Accidental> ScorePitchUtils::getNotationNote(const Scale& scale, const MidiNote& midiNote) {
    ScaleNote sn = scale.m2s(midiNote);
    Accidental accidental = Accidental::none;

    if (sn.getAdjustment() == ScaleNote::RelativeAdjustment::none) {
      //  SQINFO("none at 29");
        accidental = Accidental::none;
    } else if (!midiNote.isBlackKey()) {
      //  SQINFO("natural at 31");
        accidental = Accidental::natural;
    } else {
      //  SQINFO("default to sharps - might not be right");
        accidental = Accidental::sharp;
    }

    return std::make_tuple(sn, accidental);
}


inline bool ScorePitchUtils::compareAccidentals(ScorePitchUtils::Accidental accidental1, ScorePitchUtils::Accidental accidental2) {
    SQINFO("compare %d vs %d. flat=%d, sharp=%d, natural=%d, none=%d",
        int(accidental1), int(accidental2),
    int(ScorePitchUtils::Accidental::flat), int(ScorePitchUtils::Accidental::sharp), int(ScorePitchUtils::Accidental::natural), int(ScorePitchUtils::Accidental::none)); 

    if (accidental1 == accidental2) {
        return true;
    }

    if (accidental2 == ScorePitchUtils::Accidental::sharp) {
        // if the second is a sharp, it must be GE the other one.
        return true;                    
    }
    if (accidental1 == ScorePitchUtils::Accidental::sharp) {
        return false;
    }
    // More cases to implement.
    assert(false);
    return false;
} 