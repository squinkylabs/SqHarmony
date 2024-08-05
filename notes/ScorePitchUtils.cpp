
#include "ScorePitchUtils.h"

// #include <tuple>
#include "ChordRecognizer.h"
#include "MidiNote.h"
#include "NotationNote.h"
#include "Scale.h"
#include "ScaleNote.h"
#include "SqLog.h"

#include <assert.h>

#define _LOG

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
    // SQINFO("first try = %s using ll=%d", ret.toString().c_str(), legerLine);
    if (validate(ret, scale)) {
        // SQINFO("accepted first try");
        return ret;
    }
    ret = NotationNote(midiNote, accidental, legerLine - 1, bassStaff);
    // SQINFO("");
    // SQINFO("second try = %s using ll=%d", ret.toString().c_str(), legerLine - 1);
    if (validate(ret, scale)) {
        // SQINFO("accepted line down");
        return ret;
    }
    ret = NotationNote(midiNote, accidental, legerLine + 1, bassStaff);
    // SQINFO("");
    // SQINFO("third try = %s using ll=%d", ret.toString().c_str(), legerLine + 1);
    if (validate(ret, scale)) {
        // SQINFO("accepted line up");
        return ret;
    }

    assert(false);  // shouldn't happen
                    // SQINFO("made note %s ", ret.toString().c_str());
    assert(validate(ret, scale));
    return ret;
}

bool ScorePitchUtils::_makeNoteAtLegerLine(NotationNote& nn, int legerLineTarget, const Scale& scale, bool bassStaff) {
    auto newNote = NotationNote(nn._midiNote, NotationNote::Accidental::flat, legerLineTarget, bassStaff);
    if (validate(newNote, scale)) {
        nn = newNote;
        return true;
    }
    newNote = NotationNote(nn._midiNote, NotationNote::Accidental::none, legerLineTarget, bassStaff);
    if (validate(newNote, scale)) {
        nn = newNote;
        return true;
    }

    newNote = NotationNote(nn._midiNote, NotationNote::Accidental::sharp, legerLineTarget, bassStaff);
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
    return _makeNoteAtLegerLine(nn, legerLineTarget, scale, nn._bassStaff);
}

// Now problem is pitch from leger is wrong for keys that are on "accidentals"
int ScorePitchUtils::pitchFromLeger(bool bassStaff, int _legerLine, NotationNote::Accidental accidental, const Scale& scale) {
    // SQINFO("SPU::pitch from ledger(%d,%d, %d)", bassStaff, _legerLine, int(accidental));
    int normalizedLegerLine = _legerLine;
    if (bassStaff) {
        normalizedLegerLine -= 3;  // push C to leger line zero in bass
    } else {
        normalizedLegerLine += 2;  // push C to leger line zero in treble
    }
    // SQINFO("normalized ll = %d", normalizedLegerLine);
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
    // SQINFO("after munge, oct=%d rem=%d", octave, remainder);

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
    // SQINFO("no accid pitch is %d", pitch);

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
    // SQINFO("after accid pitch is %d oct=%d will ret %d", pitch, octave, pitch + octave * 12);
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
    // SQINFO("num=%d, accidentals=%p", num, accidentals);

    // for (int i = 0; i < num; ++i) {
    //     const MidiNote& r = accidentals[i];
    //     SQINFO("accidental[%d] at pitch %d", i, r.get());
    // }

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
    // SQINFO("validate called");
    const int midiNotePitch = _nn._midiNote.get();
    const int legerPitch = pitchFromLeger(_nn._bassStaff, _nn._legerLine, _nn._accidental, scale);

    // SQINFO("midi pitch = %d, leger pitch = %d leger line=%d", midiNotePitch, legerPitch, _nn._legerLine);
    if (midiNotePitch != legerPitch) {
        return false;
    }

    NotationNote nn = makeCanonical(_nn);

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
    // SQINFO("getVariations returning %d", ret.numValid());
    return ret;
}

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

ScorePitchUtils::SpellingResults ScorePitchUtils::findSpelling(
    const Scale& scale,
    const SqArray<int, 16>& inputPitches,
    bool bassStaff,
    UIPrefSharpsFlats pref) {
    SpellingPreferences thePrefs;

    SQINFO("--- enter findSpelling ---");
    for (unsigned i=0; i< inputPitches.numValid(); ++i) {
        SQINFO("input[%d] = %d", i, inputPitches.getAt(i));
        
    }


    thePrefs.sharpsOrFlats = resolveSharpPref(pref, scale);


    SqArray<NotationNote, 16> outputNotes;
    const auto info = ChordRecognizer::recognize(inputPitches);
    const auto ret = _findSpelling(thePrefs, info, scale, inputPitches, outputNotes, bassStaff);
    SQINFO("--- leave findSpelling ---");
    for (unsigned i=0; i< ret.notes.numValid(); ++i) {
        SQINFO("output[%d] = %s", i, ret.notes.getAt(i).toString().c_str());
        
    }
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

    //  SQINFO("will recurse");
    //  unsigned bestIndex = 0;
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
    // SQINFO("after recurse, restored best note at index =%d note=%s", evalIndex, bestNote.toString().c_str());
    outputNotes.putAt(evalIndex, bestNote);
    // SQINFO("at 288 slot %d < %s", evalIndex, bestNote.toString().c_str());

    // SQINFO("returning best score %d level =%d on:size=%d" , bestScore, evalIndex, outputNotes.numValid());
    // for (unsigned i = 0; i<outputNotes.numValid(); ++i) {
    //     SQINFO("outputNotes[%d] = %s", i, outputNotes.getAt(i).toString().c_str());
    // }
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
    int numMultiNotesOnLeger = 0;

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
    SQINFO("third attempt needs leger stuff, or at least an assert");

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
