
#pragma once

#include <tuple>

#include "MidiNote.h"
#include "Scale.h"
#include "ScaleNote.h"
//#include "SqLog.h"

class ScorePitchUtils {
public:
    ScorePitchUtils() = delete;
    enum class Accidental {
        none,
        sharp,
        flat,
        natural
    };

    class NotationNote {
    public:
        NotationNote() {}
        NotationNote(ScaleNote sn, ScorePitchUtils::Accidental ac, int ll) : _scaleNote(sn), _accidental(ac), _legerLine(ll) {}
        ScaleNote _scaleNote;
        Accidental _accidental = ScorePitchUtils::Accidental::none;
        int _legerLine = 0;
    };

    static NotationNote getNotationNote(const Scale&, const MidiNote&, bool bassStaff);

    /**
     * Not needed right now.
     * @return returns true if accidental1 <= accidental2
     * @return false if  accidental1 > accidental2
     */
    //  static bool compareAccidentals(Accidental accidental1, Accidental accidental2);
};

inline ScorePitchUtils::NotationNote
ScorePitchUtils::getNotationNote(const Scale& scale, const MidiNote& midiNote, bool bassStaff) {
    ScaleNote sn = scale.m2s(midiNote);
    scale._validateScaleNote(sn);
    //SQINFO("--in getNotationNote srn octave=%d degree=%d adj=%d (none, sharp, flat)", sn.getOctave(), sn.getDegree(), int(sn.getAdjustment()));
    //SQINFO("-- midiPitch is %d", midiNote.get());

    assert(midiNote.get() < 1000);

    Accidental accidental = Accidental::none;

    if (sn.getAdjustment() == ScaleNote::RelativeAdjustment::none) {
        //SQINFO("none at 38");
        accidental = Accidental::none;
    } else if (!midiNote.isBlackKey()) {
        //SQINFO("natural at 41");
        accidental = Accidental::natural;
    } else {
      //  SQINFO("default to sharps - might not be right (it's a black key, and not in scale)");
        accidental = (sn.getAdjustment() == ScaleNote::RelativeAdjustment::flat) ? Accidental::flat :  Accidental::sharp;
        //SQINFO("getting acciendtal from adj, accid = %d", int(accidental));
    }

    const auto pref = scale.getSharpsFlatsPref();
    // SQINFO("return from getNotationNote with leger line %d pref = %d flats=%d",
    //        midiNote.getLegerLine(pref, bassStaff),
    //        int(pref),
    //        int(SharpsFlatsPref::Flats));
    return NotationNote(sn, accidental, midiNote.getLegerLine(pref, bassStaff));
}

#if 0
inline bool ScorePitchUtils::compareAccidentals(ScorePitchUtils::Accidental accidental1, ScorePitchUtils::Accidental accidental2) {
    SQINFO("compare %d vs %d. flat=%d, sharp=%d, natural=%d, none=%d",
        int(accidental1), int(accidental2),
    int(ScorePitchUtils::Accidental::flat), int(ScorePitchUtils::Accidental::sharp), int(ScorePitchUtils::Accidental::natural), int(ScorePitchUtils::Accidental::none)); 

    assert(accidental1 != Accidental::none);    
    assert(accidental2 != Accidental::none);
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
#endif