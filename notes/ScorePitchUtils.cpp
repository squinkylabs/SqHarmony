
#include "ScorePitchUtils.h"

// #include <tuple>
#include "ChordRecognizer.h"
#include "MidiNote.h"
#include "NotationNote.h"
#include "Scale.h"
#include "ScaleNote.h"
// #include "SqLog.h"

#include <assert.h>

NotationNote ScorePitchUtils::getNotationNote(const Scale& scale, const MidiNote& midiNote, bool bassStaff) {
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

bool ScorePitchUtils::_makeNoteAtLegerLine(NotationNote& nn, int legerLineTarget, const Scale& scale) {
    auto newNote = NotationNote(nn._midiNote, NotationNote::Accidental::flat, legerLineTarget);
    if (validate(newNote, scale)) {
        nn = newNote;
        return true;
    }
    newNote = NotationNote(nn._midiNote, NotationNote::Accidental::none, legerLineTarget);
    if (validate(newNote, scale)) {
        nn = newNote;
        return true;
    }

    newNote = NotationNote(nn._midiNote, NotationNote::Accidental::sharp, legerLineTarget);
    if (validate(newNote, scale)) {
        nn = newNote;
        return true;
    }

    // If no accidentals work, give up. We can't do it.
    return false;
}

bool ScorePitchUtils::reSpell(NotationNote& nn, bool moreSharps, const Scale& scale) {
    assert(validate(nn, scale));
    const int legerLineTarget = nn._legerLine + ((moreSharps) ? -1 : 1);
    return _makeNoteAtLegerLine(nn, legerLineTarget, scale);
}

int ScorePitchUtils::pitchFromLeger(bool bassStaff, int _legerLine, NotationNote::Accidental accidental, const Scale& scale) {
    int normalizedLegerLine = _legerLine;
    if (bassStaff) {
        normalizedLegerLine -= 3;  // push C to leger line zero in bass
    } else {
        normalizedLegerLine += 2;  // push C to leger line zero in treble
    }
    unsigned pitch = 0;
    int octave = (normalizedLegerLine / 7);
    int remainder = normalizedLegerLine % 7;
    if (remainder < 0) {
        remainder += 7;
        octave--;
    }
    if (bassStaff) {
        octave--;
    }

    switch (remainder) {
        case 0:
            pitch = MidiNote::MiddleC + MidiNote::C;
            break;
        case 1:
            pitch = MidiNote::MiddleC + MidiNote::D;
            break;
        case 2:
            pitch = MidiNote::MiddleC + MidiNote::E;
            break;
        case 3:
            pitch = MidiNote::MiddleC + MidiNote::F;
            break;
        case 4:
            pitch = MidiNote::MiddleC + MidiNote::G;
            break;
        case 5:
            pitch = MidiNote::MiddleC + MidiNote::A;
            break;
        case 6:
            pitch = MidiNote::MiddleC + MidiNote::B;
            break;
        default:
            assert(false);  // case not implemented yet.
    }

    switch (accidental) {
        case NotationNote::Accidental::none: {
            // const auto info = scale.getScoreInfo();
            const auto x = _getAjustmentForLeger(scale, bassStaff, _legerLine);
            // SQINFO("got adjustment %d", x);
            pitch += x;
        }
        case NotationNote::Accidental::natural:
            break;
        case NotationNote::Accidental::sharp:
            pitch++;
            break;
        case NotationNote::Accidental::flat:
            pitch--;
            break;
        default:
            assert(false);
    }
    return pitch + octave * 12;
}

inline int normalizeIntPositive(int input, int rangeTop) {
    assert(rangeTop > 0);

    const int rem = input % rangeTop;
    return (rem < 0) ? rem + rangeTop : rem;
}

// static int _getAjustmentForLeger(const Scale& scale, bool bassStaff, int legerLine);
int ScorePitchUtils::_getAjustmentForLeger(const Scale& scale, bool bassStaff, int legerLine) {
    // SQINFO("\n\n----- enter _getAjustmentForLeger line=%d, bass=%d", legerLine, bassStaff);
    const auto info = scale.getScoreInfo();

    const bool treble = !bassStaff;
    const MidiNote* accidentals = nullptr;
    bool areFlats = false;
    int num = 0;
    //   int num = 0;
    if (info.numFlats == 0 && info.numSharps == 0) {
        // cmaj, we are good.
    } else if (info.numFlats == 0) {
        areFlats = false;
    } else if (info.numSharps == 0) {
        areFlats = true;
    } else {
        areFlats = info.numFlats < info.numSharps;
    }
    if (areFlats) {
        accidentals = treble ? info.flatsInTrebleClef : info.flatsInBassClef;
        num = info.numFlats;
    } else {
        accidentals = treble ? info.sharpsInTrebleClef : info.sharpsInBassClef;
        num = info.numSharps;
    }
    // SQINFO("num=%d, accidentals=%p", num, accidentals);

    if (num) {
        for (int i = 0; i < num; ++i) {
            // SQINFO("in loop i=%d, ");
            const auto accidentalNote = accidentals[i];
            const int candidateLegerLine = normalizeIntPositive(accidentalNote.getLegerLine(bassStaff), 7);

            assert(candidateLegerLine < 8);
            assert(candidateLegerLine >= 0);
            // SQINFO("in loop i=%d, cll=%d accidental note pitch=%d pitch rel mid c=%d", i, candidateLegerLine, accidentalNote.get(), accidentalNote.get() - MidiNote::MiddleC);
            //  SQINFO("  rel c4=%d", accidentalNote.get() - (MidiNote::MiddleC + 12));
            if (legerLine == candidateLegerLine) {
                // here we found an accidental!
                // assert(false);
                return areFlats ? -1 : 1;
            }
        }
    }

    return 0;
}

bool ScorePitchUtils::validate(const NotationNote& _nn, const Scale& scale) {
    // SQINFO("validate called");
    const int midiNotePitch = _nn._midiNote.get();
    const int legerPitch = pitchFromLeger(false, _nn._legerLine, _nn._accidental, scale);
    // SQINFO("midi pitch = %d, ledger = %d", midiNotePitch, legerPitch);
    if (midiNotePitch != legerPitch) {
        return false;
    }

    NotationNote nn = makeCanonical(_nn);
    // still leaves possibilities of bad accidentals:
    // we passed natural, but natural is assumed in the key.
    // we passed flat, but flat is assumed in the key

    // so if the pitch is in the key, it doesn't need an accidental. Otherwise it does.
    const ScaleNote scaleNote = scale.m2s(nn._midiNote);
    const bool noteInScale = (scaleNote.getAdjustment() == ScaleNote::RelativeAdjustment::none);
    if (nn._accidental == NotationNote::Accidental::none) {
        // SQINFO("we are validating a none, returning %d", noteInScale);
        return noteInScale;
    } else {
        // SQINFO("we are validating a sn with accid, returning %d", !noteInScale);
        return !noteInScale;
    }

    assert(false);
}

SqArray<NotationNote, 16> ScorePitchUtils::getVariations(const NotationNote& nn, const Scale& scale) {
    assert(validate(nn, scale));
    NotationNote temp = nn;
    SqArray<NotationNote, 16> ret;
    // ret._push(temp);
    unsigned index = 0;
    ret.putAt(index++, temp);
    while (reSpell(temp, true, scale)) {
        ret.putAt(index++, temp);
    }
    while (reSpell(temp, false, scale)) {
        ret.putAt(index++, temp);
    }
    return ret;
}

NotationNote ScorePitchUtils::makeCanonical(const NotationNote& note) {
    if (note._accidental == NotationNote::Accidental::sharp) {
        // If the notation note has a sharp, but the pitch is a white key pitch
        if (!note._midiNote.isBlackKey()) {
            // Then we can re-spell it as a natural, in key, note one line up.
            // For example, in all keys B sharp is C natural
            return NotationNote(note._midiNote, NotationNote::Accidental::none, note._legerLine + 1);
        }
    } else if (note._accidental == NotationNote::Accidental::flat) {
        // If the notation note has a flat, but the pitch is a white key pitch
        if (!note._midiNote.isBlackKey()) {
            // Then we can re-spell it as a natural, in key, note one line down.
            // For example, in all keys F flat is E natural
            return NotationNote(note._midiNote, NotationNote::Accidental::none, note._legerLine - 1);
        }
    }
    // If note is already canonical, can just return as is.
    return note;
}

int ScorePitchUtils::findSpelling(
    const Scale& scale,
    const SqArray<int, 16>& inputPitches,
    SqArray<NotationNote, 16>& outputNotes,
    bool bassStaff,
    unsigned evalIndex) {
    // // get all the spellings for current notes.
    SQINFO("\n** enter findSpelling %d size so far = %d inputSize= %d", evalIndex, outputNotes.numValid(), inputPitches.numValid());
    //  SQINFO("    addr output = %p", &outputNotes);
    const int currentPitch = inputPitches.getAt(evalIndex);
    const MidiNote currentMidiNote = MidiNote(currentPitch);
    const auto defaultNotationNote = getNotationNote(scale, currentMidiNote, bassStaff);
    const auto currentVariations = getVariations(defaultNotationNote, scale);

    //  SQINFO("will recurse");
    //  unsigned bestIndex = 0;
    int bestScore = -1000000;

    NotationNote bestNote;
    SQINFO("at 266, new blank best = %s", bestNote.toString().c_str());
    for (unsigned i = 0; i < currentVariations.numValid(); ++i) {
        outputNotes.putAt(evalIndex, currentVariations.getAt(i));  // put the current candidate into the test array
        SQINFO("at 268 slot %d < %s", evalIndex, currentVariations.getAt(i).toString().c_str());
        // SQINFO("about to recurse, index=%d", evalIndex);

        int score = 0;
        if (evalIndex < inputPitches.numValid() - 1) {
            score = findSpelling(scale, inputPitches, outputNotes, bassStaff, evalIndex + 1);
        } else {
            score = _evaluateSpelling(outputNotes);
        }
        if (score > bestScore) {
            // SQINFO("recurse found new best score for %d, note=%s", evalIndex, outputNotes.getAt()
            bestScore = score;
            // bestIndex = i;

            bestNote = currentVariations.getAt(i);
            SQINFO("set best note to %s", bestNote.toString().c_str());
            // now need to put best one back
        }
    }
    // SQINFO("after recurse, restored best note at index =%d note=%s", evalIndex, bestNote.toString().c_str());
    outputNotes.putAt(evalIndex, bestNote);
    SQINFO("at 288 slot %d < %s", evalIndex, bestNote.toString().c_str());
    return bestScore;
}

int ScorePitchUtils::_evaluateSpelling(SqArray<NotationNote, 16>& notes) {
    return _evaluateSpellingSecondAttempt(notes);
   // return _evaluateSpellingFirstAttempt(notes);
    // return _evaluateSpelling0(notes);
}

int ScorePitchUtils::_evaluateSpellingFirstAttempt(SqArray<NotationNote, 16>& notes) {
    SQINFO("!! evaluate spelling first attempt");
    const unsigned n = notes.numValid();
    int numAccidentals = 0;
    int numBadThirds = 0;

    const NotationNote* pn1 = nullptr;
    const NotationNote* pn2 = nullptr;
    for (unsigned i = 0; i < n; ++i) {
        pn2 = pn1;
        pn1 = &notes.getAt(i);

        if (pn1->isAccidental()) {
            numAccidentals++;
            SQINFO("see accidental at %s", pn1->toString().c_str());
        }
        if (pn2) {
            SQINFO("looking at %d and %d", i, i - 1);
            const int interval = pn1->_midiNote.get() - pn2->_midiNote.get();
            SQINFO("interval = %d", interval);
            if (interval == 3 || interval == 4) {
                const int distanceLegerLine = pn1->_legerLine - pn2->_legerLine;
                SQINFO("ll dist = %d", distanceLegerLine);
                if (distanceLegerLine != 2) {
                    numBadThirds++;
                }
            }
        }
    }
    const int score = -(numAccidentals + 10 * numBadThirds);
    SQINFO("final score = %d", score);
    return score;
}

int ScorePitchUtils::_evaluateSpelling0(SqArray<NotationNote, 16>& notes) {
    SQWARN("Trivial Eval function");
    return 0;
}

class CanonicalNotationNote {
public:
    CanonicalNotationNote(const NotationNote& nn, unsigned index) : _notationNote(nn), _index(index) {
    }
    CanonicalNotationNote() {}

    bool operator<(const CanonicalNotationNote& other) { return this->pitch() < other.pitch(); }
    operator int() const { return pitch(); }
    void operator=(int pitch) { this->_notationNote._midiNote._changePitch(pitch); }

    int pitch() const {
        return _notationNote._midiNote.get();
    }
    NotationNote _notationNote;
    unsigned _index = 0;  // do we need this?
};

/**
 * ScorePitchUtils::_evaluateSpellingSecondAttempt
 */
int ScorePitchUtils::_evaluateSpellingSecondAttempt(SqArray<NotationNote, 16>& rawNotes) {
    //SQINFO("!! evaluate spelling second attempt");
   // NotationNote nn = rawNotes.getAt(0);
   // int ii = (int) nn;
    ChordRecognizer::_show("!! evaluate spelling second attempt", rawNotes);
    const unsigned n = rawNotes.numValid();
    int numAccidentals = 0;
    int numBadThirds = 0;

    // Canonicalize all the notes, but preserve index.
    SqArray<CanonicalNotationNote, 16> convertedNotes;
    SqArray<CanonicalNotationNote, 16> canonicalNotes;
    for (unsigned i = 0; i < n; ++i) {
        convertedNotes.putAt(i, CanonicalNotationNote(rawNotes.getAt(i), i));
    }
    int foo = 0;
    ChordRecognizer::_makeCanonical(canonicalNotes, convertedNotes, foo);

    assert(canonicalNotes.numValid() == convertedNotes.numValid());

    const CanonicalNotationNote* pn1 = nullptr;
    const CanonicalNotationNote* pn2 = nullptr;
    for (unsigned i = 0; i < n; ++i) {
        pn2 = pn1;
        pn1 = &canonicalNotes.getAt(i);

        if (pn1->_notationNote.isAccidental()) {
            numAccidentals++;
            SQINFO("see accidental at %s", pn1->_notationNote.toString().c_str());
        }
        if (pn2) {
            SQINFO("looking at %d and %d", i, i - 1);
            const int interval = pn1->_notationNote._midiNote.get() - pn2->_notationNote._midiNote.get();
            SQINFO("interval = %d", interval);
            if (interval == 3 || interval == 4) {
                const int distanceLegerLine = pn1->_notationNote._legerLine - pn2->_notationNote._legerLine;
                SQINFO("ll dist = %d", distanceLegerLine);
                if (distanceLegerLine != 2) {
                    numBadThirds++;
                }
            }
        }
    }
    const int score = -(numAccidentals + 10 * numBadThirds);
    SQINFO("final score = %d", score);
    return score;
}
