
#pragma once

#include <tuple>

#include "MidiNote.h"
#include "NotationNote.h"
#include "Scale.h"
#include "ScaleNote.h"
// #include "SqLog.h"

class ScorePitchUtils {
public:
    ScorePitchUtils() = delete;
    static NotationNote getNotationNote(const Scale&, const MidiNote&, bool bassStaff);

    /**
     * Not needed right now.
     * @return returns true if accidental1 <= accidental2
     * @return false if  accidental1 > accidental2
     */
    //  static bool compareAccidentals(Accidental accidental1, Accidental accidental2);

    /**
     * @brief  change accidental and leger line for an alternate enharmonic spelling
     *
     * @param moreSharps - if true will attemt to re-spell at a lower pitch, with more sharps.
     * @return  true if success.
     */
    static bool reSpell(NotationNote& note, bool moreSharps);

    static bool validate(const NotationNote&);

private:
    static bool _reSpellFlats(NotationNote& note);
    static bool _reSpellSharps(NotationNote& note);
    //  static bool _canFlatCurrentLeger(NotationNote& note);
    static bool _makeNoteAtLegerLine(NotationNote& nn, int legerLine);
};

inline NotationNote
ScorePitchUtils::getNotationNote(const Scale& scale, const MidiNote& midiNote, bool bassStaff) {
    ScaleNote sn = scale.m2s(midiNote);
    scale._validateScaleNote(sn);
    // SQINFO("--in getNotationNote srn octave=%d degree=%d adj=%d (none, sharp, flat)", sn.getOctave(), sn.getDegree(), int(sn.getAdjustment()));
    // SQINFO("-- midiPitch is %d", midiNote.get());

    assert(midiNote.get() < 1000);

    NotationNote::Accidental accidental = NotationNote::Accidental::none;

    if (sn.getAdjustment() == ScaleNote::RelativeAdjustment::none) {
        // SQINFO("none at 38");
        accidental = NotationNote::Accidental::none;
    } else if (!midiNote.isBlackKey()) {
        // SQINFO("natural at 41");
        accidental = NotationNote::Accidental::natural;
    } else {
        //  SQINFO("default to sharps - might not be right (it's a black key, and not in scale)");
        accidental = (sn.getAdjustment() == ScaleNote::RelativeAdjustment::flat) ? NotationNote::Accidental::flat : NotationNote::Accidental::sharp;
        // SQINFO("getting acciendtal from adj, accid = %d", int(accidental));
    }

    const auto pref = scale.getSharpsFlatsPref();
    // SQINFO("return from getNotationNote with leger line %d pref = %d flats=%d",
    //        midiNote.getLegerLine(pref, bassStaff),
    //        int(pref),
    //        int(SharpsFlatsPref::Flats));
    return NotationNote(midiNote, accidental, midiNote.getLegerLine(pref, bassStaff));
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

// inline bool ScorePitchUtils::_canFlatCurrentLeger(NotationNote& nn) {
//     assert(false);
//     return false;
// }

// Algorithm, for more flats:
// can we flat the existing leger (depends on scale, and _accidental), then flat it.

inline bool ScorePitchUtils::_reSpellFlats(NotationNote& nn) {
    SQINFO("in _reSpellFlats");
    int evalLedger = nn._legerLine + 1;
    if (_makeNoteAtLegerLine(nn, evalLedger)) {
        return true;
    }
    assert(false);  // what now?

    return false;
}

inline bool ScorePitchUtils::_makeNoteAtLegerLine(NotationNote& nn, int legerLine) {
    assert(false);
    return false;
}

inline bool ScorePitchUtils::_reSpellSharps(NotationNote& nn) {
    return false;
}

inline bool ScorePitchUtils::reSpell(NotationNote& nn, bool moreSharps) {
    return moreSharps ? _reSpellSharps(nn) : _reSpellFlats(nn);
}

inline bool ScorePitchUtils::validate(const NotationNote&) {
    return true;
}