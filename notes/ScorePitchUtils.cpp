
#include "ScorePitchUtils.h"

// #include <tuple>
#include <assert.h>

#include "ChordRecognizer.h"
#include "MidiNote.h"
#include "NotationNote.h"
#include "Scale.h"
#include "ScaleNote.h"
#include "SqLog.h"

// #define _LOG

NotationNote ScorePitchUtils::getNotationNote(const Scale& scale, const MidiNote& midiNote, bool bassStaff) {
    // SQINFO("getNotationNote, midi pitch = %d, bassStaff = %d", midiNote.get(), bassStaff);
    ScaleNote sn = scale.m2s(midiNote);
    //  scale._validateScaleNote(sn);
    //  SQINFO("converted to scale note degree=%d octave=%d, adj=%d", sn.getDegree(), sn.getOctave(), int(sn.getAdjustment()));

    assert(midiNote.get() < 1000);

    NotationNote::Accidental accidental = NotationNote::Accidental::none;

    if (sn.getAdjustment() == ScaleNote::RelativeAdjustment::none) {
        accidental = NotationNote::Accidental::none;
    } else if (!midiNote.isBlackKey()) {
        accidental = NotationNote::Accidental::natural;
    } else {
        accidental = (sn.getAdjustment() == ScaleNote::RelativeAdjustment::flat) ? NotationNote::Accidental::flat : NotationNote::Accidental::sharp;
    }

    ResolvedSharpsFlatsPref pref;
    switch (scale.getSharpsFlatsPref()) {
        case SharpsFlatsPref::Sharps:
            pref = ResolvedSharpsFlatsPref::Sharps;
            break;
        case SharpsFlatsPref::Flats:
            pref = ResolvedSharpsFlatsPref::Flats;
            break;
        case SharpsFlatsPref::DontCare:
            // SQINFO("ScorePitchUtils.cpp 56. turning don't care into sharp");
            pref = ResolvedSharpsFlatsPref::Sharps;
            break;
        default:
            assert(false);
    }

    const int legerLine = midiNote.getLegerLine(pref, bassStaff);
    auto ret = NotationNote(midiNote, accidental, legerLine, bassStaff);
    if (validate(ret, scale)) {
        return ret;
    }
    ret = NotationNote(midiNote, accidental, legerLine - 1, bassStaff);
    if (validate(ret, scale)) {
        return ret;
    }
    ret = NotationNote(midiNote, accidental, legerLine + 1, bassStaff);
    if (validate(ret, scale)) {
        return ret;
    }

    assert(false);  // shouldn't happen
    //assert(validate(ret, scale));
    return ret;
}

bool ScorePitchUtils::_makeNoteAtLegerLine(NotationNote& nn, int legerLineTarget, const Scale& scale, bool bassStaff) {
    //  SQINFO("enter make note at ll %d", legerLineTarget);
    auto newNote = NotationNote(nn._midiNote, NotationNote::Accidental::flat, legerLineTarget, bassStaff);
    //  SQINFO("about to call validate on flat");
    if (validate(newNote, scale)) {
        //    SQINFO("will use it");
        nn = newNote;
        return true;
    }
    //  SQINFO("about to call validate on none");
    newNote = NotationNote(nn._midiNote, NotationNote::Accidental::none, legerLineTarget, bassStaff);
    if (validate(newNote, scale)) {
        //   SQINFO("will use it");
        nn = newNote;
        return true;
    }

    //  SQINFO("about to call validate on sharp");
    newNote = NotationNote(nn._midiNote, NotationNote::Accidental::sharp, legerLineTarget, bassStaff);
    if (validate(newNote, scale)) {
        //     SQINFO("will use it");
        nn = newNote;
        return true;
    }

    //  SQINFO("about to call validate on natural");
    newNote = NotationNote(nn._midiNote, NotationNote::Accidental::natural, legerLineTarget, bassStaff);
    if (validate(newNote, scale)) {
        //    SQINFO("will use it");
        nn = newNote;
        return true;
    }

    // SQINFO("falling off end of _makeNoteAtLegerLine");

    // If no accidentals work, give up. We can't do it.
    return false;
}

bool ScorePitchUtils::reSpell(NotationNote& nn, bool moreSharps, const Scale& scale) {
    assert(validate(nn, scale));
    const int legerLineTarget = nn._legerLine + ((moreSharps) ? -1 : 1);
    return _makeNoteAtLegerLine(nn, legerLineTarget, scale, nn._bassStaff);
}

bool ScorePitchUtils::reSpellOnLegerLine(NotationNote& nn, int legerLine, const Scale& scale) {
    assert(validate(nn, scale));
    //  const int legerLineTarget = nn._legerLine + ((moreSharps) ? -1 : 1);
    return _makeNoteAtLegerLine(nn, legerLine, scale, nn._bassStaff);
}

// Now problem is pitch from leger is wrong for keys that are on "accidentals"
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
            const auto x = _getAjustmentForLeger(scale, bassStaff, _legerLine);
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

int ScorePitchUtils::_getAjustmentForLeger(const Scale& scale, bool bassStaff, int _legerLine) {
    const int normalizedLegerLine = normalizeIntPositive(_legerLine, 7);
    //  SQINFO("\n\n----- enter _getAjustmentForLeger line=%d, bass=%d", _legerLine, bassStaff);
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

    if (num) {
        for (int i = 0; i < num; ++i) {
            const auto accidentalNote = accidentals[i];
            //  SQINFO("possible fake pref 229");
            const int candidateLegerLine = normalizeIntPositive(accidentalNote.getLegerLine(ResolvedSharpsFlatsPref::Sharps, bassStaff), 7);

            assert(candidateLegerLine < 8);
            assert(candidateLegerLine >= 0);

            assert(normalizedLegerLine < 8);
            assert(normalizedLegerLine >= 0);

            if (normalizedLegerLine == candidateLegerLine) {
                // here we found an accidental!
                return areFlats ? -1 : 1;
            }
        }
    }

    return 0;
}

bool ScorePitchUtils::validate(const NotationNote& _nn, const Scale& scale) {
    const int midiNotePitch = _nn._midiNote.get();
    const int legerPitch = pitchFromLeger(
        _nn._bassStaff,
        _nn._legerLine,
        _nn._accidental, scale);

    // If the note is inconsistent in pitch, it can't be valid
    if (midiNotePitch != legerPitch) {
        return false;
    }

    if (_nn.isAccidental()) {
        NotationNote nnStripped = _nn;
        nnStripped._accidental = NotationNote::Accidental::none;
        const int strippedLegerPitch = pitchFromLeger(
            nnStripped._bassStaff,
            nnStripped._legerLine,
            nnStripped._accidental,
            scale);
        // if a note is accidental, make sure the accidental is affecting the pitch.
        return (strippedLegerPitch != legerPitch);
    } else {
        return true;
    }
    assert(false);
    return false;
}

SqArray<NotationNote, 16> ScorePitchUtils::getVariations(const NotationNote& nn, const Scale& scale) {
    assert(validate(nn, scale));
    NotationNote temp = nn;
    SqArray<NotationNote, 16> ret;
    const int inputLegerLine = nn._legerLine;

    unsigned index = 0;
    ret.putAt(index++, temp);
    if (reSpellOnLegerLine(temp, inputLegerLine + 1, scale)) {
        ret.putAt(index++, temp);
    }
    temp = nn;
    if (reSpellOnLegerLine(temp, inputLegerLine - 1, scale)) {
        ret.putAt(index++, temp);
    }

#ifdef _LOG
    SQINFO("+++ getVariations on %s found %d", nn.toString().c_str(), ret.numValid());
    for (unsigned i = 0; i < ret.numValid(); ++i) {
        SQINFO("  [%d] = %s", i, ret.getAt(i).toString().c_str());
    }
    SQINFO("+++");
#endif
    return ret;
}

#if 0
NotationNote ScorePitchUtils::makeCanonical(const NotationNote& note) {
    if (note._accidental == NotationNote::Accidental::sharp) {
        // If the notation note has a sharp, but the pitch is a white key pitch
        if (!note._midiNote.isBlackKey()) {
            // Then we can re-spell it as a natural, in key, note one line up.
            // For example, in all keys B sharp is C natural
            return NotationNote(note._midiNote, NotationNote::Accidental::none, note._legerLine + 1, note._bassStaff);
        }
    } else if (note._accidental == NotationNote::Accidental::flat) {
        // If the notation note has a flat, but the pitch is a white key pitch
        if (!note._midiNote.isBlackKey()) {
            // Then we can re-spell it as a natural, in key, note one line down.
            // For example, in all keys F flat is E natural
            return NotationNote(note._midiNote, NotationNote::Accidental::none, note._legerLine - 1, note._bassStaff);
        }
    }
    // If note is already canonical, can just return as is.
    return note;
}
#endif

ScorePitchUtils::SpellingResults ScorePitchUtils::findSpelling(
    const Scale& scale,
    const SqArray<int, 16>& inputPitches,
    bool bassStaff,
    UIPrefSharpsFlats pref) {
    SpellingPreferences thePrefs;

#ifdef _LOG
    SQINFO("--- enter findSpelling ---");
    for (unsigned i = 0; i < inputPitches.numValid(); ++i) {
        SQINFO("input[%d] = %d", i, inputPitches.getAt(i));
    }
#endif

    thePrefs.sharpsOrFlats = resolveSharpPref(pref, scale);

    SqArray<NotationNote, 16> outputNotes;
    const auto info = ChordRecognizer::recognize(inputPitches);
    const auto ret = _findSpelling(thePrefs, info, scale, inputPitches, outputNotes, bassStaff);
#ifdef _LOG
    SQINFO("--- leave findSpelling ---");
    for (unsigned i = 0; i < ret.notes.numValid(); ++i) {
        SQINFO("output[%d] = %s", i, ret.notes.getAt(i).toString().c_str());
    }
#endif
    return ret;
}

ScorePitchUtils::SpellingResults ScorePitchUtils::_findSpelling(
    const SpellingPreferences& prefs,
    const ChordRecognizer::ChordInfo& info,
    const Scale& scale,
    const SqArray<int, 16>& inputPitches,
    SqArray<NotationNote, 16>& outputNotes,
    bool bassStaff,
    unsigned evalIndex) {
    const int currentPitch = inputPitches.getAt(evalIndex);
    const MidiNote currentMidiNote = MidiNote(currentPitch);
    const auto defaultNotationNote = getNotationNote(scale, currentMidiNote, bassStaff);
    const auto currentVariations = getVariations(defaultNotationNote, scale);

    int bestScore = -1000000;
    ScorePitchUtils::SpellingResults bestResult;

    NotationNote bestNote;
    // SQINFO("at 266, new blank best = %s", bestNote.toString().c_str());
    for (unsigned i = 0; i < currentVariations.numValid(); ++i) {
        outputNotes.putAt(evalIndex, currentVariations.getAt(i));  // put the current candidate into the test array
                                                                   // SQINFO("at 268 slot %d < %s", evalIndex, currentVariations.getAt(i).toString().c_str());
        // SQINFO("about to recurse, index=%d", evalIndex);

        ScorePitchUtils::SpellingResults results;
        if (evalIndex < inputPitches.numValid() - 1) {
            // recurse down to search
            results = _findSpelling(prefs, info, scale, inputPitches, outputNotes, bassStaff, evalIndex + 1);
        } else {
            const int score = _evaluateSpelling(prefs, info, outputNotes);
            results.score = score;
            results.notes = outputNotes;
        }
        if (results.score > bestScore) {
            // SQINFO("recurse found new best score for %d, note=%s", evalIndex, outputNotes.getAt()
            bestScore = results.score;
            // bestIndex = i;

            bestNote = currentVariations.getAt(i);
            // SQINFO("set best note to %s", bestNote.toString().c_str());
            // now need to put best one back
            bestResult = results;
        }
    }

    outputNotes.putAt(evalIndex, bestNote);
    assert(bestResult.score == bestScore);
    return bestResult;
}

int ScorePitchUtils::_evaluateSpelling(const SpellingPreferences& pref, const ChordRecognizer::ChordInfo& info, SqArray<NotationNote, 16>& notes) {
    return (info.type == ChordRecognizer::Type::Unrecognized) ? _evaluateSpellingFirstAttempt(pref, info, notes) : _evaluateSpellingThirdAttempt(pref, info, notes);
}

static const int penaltyForBadThird = 10;
static const int penaltyForMultiNotesOnLeger = 3;

// currently used for unrecognized chords
int ScorePitchUtils::_evaluateSpellingFirstAttempt(const SpellingPreferences& pref, const ChordRecognizer::ChordInfo& info, SqArray<NotationNote, 16>& notes) {
#ifdef _LOG
    SQINFO("!! evaluate spelling first attempt pret sharp/flats=%d", pref.sharpsOrFlats);
#endif

    const unsigned n = notes.numValid();
    int numAccidentals = 0;
    int numBadThirds = 0;
    int numBadAccidentals = 0;
  //  int numMultiNotesOnLeger = 0;

    LegerLineTracker llTracker;

    const NotationNote* pn1 = nullptr;
    const NotationNote* pn2 = nullptr;
    for (unsigned i = 0; i < n; ++i) {
        pn2 = pn1;
        pn1 = &notes.getAt(i);

#ifdef _LOG
        SQINFO("in loop, notationNote=%s", pn1->toString().c_str());
#endif

        //  const int ll = pn1->_midiNote.getLegerLine(pref);
        const int ll = pn1->_legerLine;
        llTracker.sawLine(ll);

        if (pn1->isAccidental()) {
            numAccidentals++;
            // SQINFO("see accidental at %s", pn1->toString().c_str());
            const bool isSharp = pn1->_accidental == NotationNote::Accidental::sharp;
            numBadAccidentals += (isSharp == pref.sharpsOrFlats) ? 0 : 1;
#ifdef _LOG
            SQINFO("saw accidental, now #=%d, bad=%d", numAccidentals, numBadAccidentals);
#endif
        }
        if (pn2) {
            // SQINFO("looking at %d and %d", i, i - 1);
            const int interval = pn1->_midiNote.get() - pn2->_midiNote.get();
            // SQINFO("interval = %d", interval);
            if (interval == 3 || interval == 4) {
                const int distanceLegerLine = pn1->_legerLine - pn2->_legerLine;
                // SQINFO("ll dist = %d", distanceLegerLine);
                if (distanceLegerLine != 2) {
                    numBadThirds++;
                }
            }
        }
    }
    const int score = -(numBadAccidentals + numAccidentals + penaltyForBadThird * numBadThirds + penaltyForMultiNotesOnLeger * llTracker.getSawMulti());
#ifdef _LOG
    SQINFO("final score 436 = %d badAcc=%d, numacc=%d bad3rd=%d, multi=%d", score, numBadAccidentals, numAccidentals, numBadThirds, llTracker.getSawMulti());
    SQINFO("\n");
#endif
    return score;
}

int ScorePitchUtils::_evaluateSpelling0(const SpellingPreferences& pref, const ChordRecognizer::ChordInfo& info, SqArray<NotationNote, 16>& notes) {
    SQWARN("Trivial Eval function");
    return 0;
}

// currently used for recognized chords
int ScorePitchUtils::_evaluateSpellingThirdAttempt(const SpellingPreferences& pref, const ChordRecognizer::ChordInfo& info, SqArray<NotationNote, 16>& notes) {
#ifdef _LOG
    SQINFO("--------        in _evaluateSpellingThirdAttempt type=%d", int(info.type));
    ChordRecognizer::_show("input chord to eval", info.identifiedPitches);
    for (unsigned i = 0; i < notes.numValid(); ++i) {
        const NotationNote& note = notes.getAt(i);
        SQINFO("note[%d] = %s", i, note.toString().c_str());
    }
#endif

    int numAccidentals = 0;
    int numBadThirds = 0;
    int numBadAccidentals = 0;
    // first penalize all the accidentals

    // assert(false);      // leger stuff
    // SQINFO("third attempt needs leger stuff, or at least an assert");

    for (unsigned i = 0; i < notes.numValid(); ++i) {
        const NotationNote& note = notes.getAt(i);
        if (note.isAccidental()) {
            numAccidentals++;
            // SQINFO("see accidental at %s", note.toString().c_str());
            const bool isSharp = note._accidental == NotationNote::Accidental::sharp;
            numBadAccidentals += (isSharp == pref.sharpsOrFlats) ? 0 : 1;
        }
    }

    // now look for intervals
    const ChordRecognizer::PitchAndIndex* p1 = nullptr;
    const ChordRecognizer::PitchAndIndex* p2 = nullptr;
    for (unsigned i = 0; i < info.identifiedPitches.numValid(); ++i) {
        p2 = p1;
        p1 = &info.identifiedPitches.getAt(i);
        if (p2) {
            const int interval = p1->pitch - p2->pitch;

            // SQINFO("looking at %d and %d", i, i - 1);
            // SQINFO("interval = %d", interval);
            // SQINFO("wrapped interval = %d", interval);
            if (interval == 3 || interval == 4) {
                // SQINFO("the two notes at index %d and %d", p1->index, p2->index);
                const auto note1 = notes.getAt(p1->index);
                const auto note2 = notes.getAt(p2->index);
                // SQINFO("notes forming that interval are %s and %s", note1.toString().c_str(), note2.toString().c_str());

                const int ll1 = note1._legerLine;
                const int ll2 = note2._legerLine;

                const int distanceLegerLine = ChordRecognizer::normalizeIntPositive(ll1 - ll2, 7);
                // SQINFO("ll dist = %d", distanceLegerLine);
                if (distanceLegerLine != 2) {
                    numBadThirds++;
                }
            }
        }
    }

    const int score = -(numBadAccidentals + numAccidentals + penaltyForBadThird * numBadThirds);
#ifdef _LOG
    SQINFO("------------ leave eval sp final score3 = %d bad3=%d accid=%d", score, numBadThirds, numAccidentals);
#endif
    return score;
}
