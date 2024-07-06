
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
    template <typename T, int capacity>
    class vlenArray {
    public:
        T data[capacity];
        int size() const {
            return index;
        }

        void _push(T t) {
            data[index] = t;
            ++index;
            assert(index <= capacity);
        }

    private:
        int index = 0;
    };

    static NotationNote getNotationNote(const Scale&, const MidiNote&, bool bassStaff);

    // static std::vector<NotationNote> getVariations(const NotationNote&);
    static vlenArray<NotationNote, 16> getVariations(const NotationNote&);

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
    static bool _makeNoteAtLegerLine(NotationNote& nn, int legerLine);
};

inline NotationNote
ScorePitchUtils::getNotationNote(const Scale& scale, const MidiNote& midiNote, bool bassStaff) {
    ScaleNote sn = scale.m2s(midiNote);
    scale._validateScaleNote(sn);

    assert(midiNote.get() < 1000);

    NotationNote::Accidental accidental = NotationNote::Accidental::none;

    if (sn.getAdjustment() == ScaleNote::RelativeAdjustment::none) {
        accidental = NotationNote::Accidental::none;
    } else if (!midiNote.isBlackKey()) {
        accidental = NotationNote::Accidental::natural;
    } else {
        accidental = (sn.getAdjustment() == ScaleNote::RelativeAdjustment::flat) ? NotationNote::Accidental::flat : NotationNote::Accidental::sharp;
    }

    const auto pref = scale.getSharpsFlatsPref();
    return NotationNote(midiNote, accidental, midiNote.getLegerLine(pref, bassStaff));
}

inline bool ScorePitchUtils::_makeNoteAtLegerLine(NotationNote& nn, int legerLineTarget) {
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

    // If no accidentals work, give up. We can't do it.
    return false;
}

inline bool ScorePitchUtils::reSpell(NotationNote& nn, bool moreSharps) {
    assert(validate(nn));
    const int legerLineTarget = nn._legerLine + ((moreSharps) ? -1 : 1);
    return _makeNoteAtLegerLine(nn, legerLineTarget);
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

inline ScorePitchUtils::vlenArray<NotationNote, 16> ScorePitchUtils::getVariations(const NotationNote& nn) {
    assert(validate(nn));
    NotationNote temp = nn;
    vlenArray<NotationNote, 16> ret;
    ret._push(temp);
    while (reSpell(temp, true)) {
        ret._push(temp);
    }
     while (reSpell(temp, false)) {
        ret._push(temp);
    }
    return ret;
}