
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
    static bool _makeNoteAtLegerLine(NotationNote& nn, int legerLine);
    static bool _makeNoteAtHigherLeger(NotationNote& nn, int legerLine);
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

inline bool ScorePitchUtils::_reSpellFlats(NotationNote& nn) {
    SQINFO("in _reSpellFlats");
    int evalLedger = nn._legerLine + 1;
    if (_makeNoteAtLegerLine(nn, evalLedger)) {
        return true;
    }
    assert(false);  // what now?
    return false;
}

inline bool ScorePitchUtils::_makeNoteAtLegerLine(NotationNote& nn, int legerLineTarget) {
    assert(validate(nn));

    assert(legerLineTarget != nn._legerLine);  // unexpected if asked to make a note where we are.
    if (legerLineTarget > nn._legerLine) {
        // Make a note at a higher leger line.
        return _makeNoteAtHigherLeger(nn, legerLineTarget);
    } else {
        assert(false);
    }
    return false;
}

inline bool ScorePitchUtils::_makeNoteAtHigherLeger(NotationNote& nn, int legerLineTarget) {
    auto newNote = NotationNote(nn._midiNote, NotationNote::Accidental::flat, legerLineTarget);
    if (validate(newNote)) {
        nn = newNote;
        return true;
    }
    newNote = NotationNote(nn._midiNote, NotationNote::Accidental::none, legerLineTarget);
    if (validate(newNote)) {
        nn = newNote;
        return true;
    }

    newNote = NotationNote(nn._midiNote, NotationNote::Accidental::sharp, legerLineTarget);
    if (validate(newNote)) {
        nn = newNote;
        return true;
    }

    assert(false);
    return false;
}

#if 0
inline bool ScorePitchUtils::_makeNoteAtHigherLeger(NotationNote& nn, int legerLineTarget) {
    NotationNote newNote(nn);
    const unsigned refPitch = nn._midiNote.get();
    const NotationNote::Accidental refAccidental = nn._accidental;
    while (true) {
        NotationNote::Accidental nextTry;
        switch (nn._accidental) {       // could this logic be in NotationNote itself?
            case NotationNote::Accidental::none:
            case NotationNote::Accidental::natural:
                nextTry = NotationNote::Accidental::sharp;
                break;
            case NotationNote::Accidental::sharp:
                // if the target is a sharp, let's try a natural.
                nextTry = NotationNote::Accidental::none;
                break;
            default:
                assert(false);
                return false;
        }
        newNote = NotationNote(nn._midiNote, nextTry, legerLineTarget);
        if (validate(newNote)) {
            nn = newNote;
            return true;
        }

        assert(false);
    }
    return false;
}
#endif

inline bool ScorePitchUtils::_reSpellSharps(NotationNote& nn) {
    assert(false);
    return false;
}

inline bool ScorePitchUtils::reSpell(NotationNote& nn, bool moreSharps) {
    assert(validate(nn));
    return moreSharps ? _reSpellSharps(nn) : _reSpellFlats(nn);
}

inline bool ScorePitchUtils::validate(const NotationNote& nn) {
    const int midiNotePitch = nn._midiNote.get();

    // This unfortunate conversion is due to our somewhat scatter shot approach to accidental handling....
    SharpsFlatsPref sharpsPref = SharpsFlatsPref::DontCare;
    switch (nn._accidental) {
        case NotationNote::Accidental::flat:
            sharpsPref = SharpsFlatsPref::Flats;
            break;
        case NotationNote::Accidental::sharp:
            sharpsPref = SharpsFlatsPref::Sharps;
            break;
        case NotationNote::Accidental::natural:
        case NotationNote::Accidental::none:
            sharpsPref = SharpsFlatsPref::DontCare;
            break;
        default:
            assert(false);
    }
    const int legerPitch = MidiNote::pitchFromLeger(false, nn._legerLine, sharpsPref);
    return midiNotePitch == legerPitch;
}