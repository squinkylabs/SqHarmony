
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
   // SQINFO("getNotationNote, midi pitch = %d, bassStaff = %d", midiNote.get(), bassStaff);
    ScaleNote sn = scale.m2s(midiNote);
    //  scale._validateScaleNote(sn);
  //  SQINFO("converted to scale note degree=%d octave=%d, adj=%d", sn.getDegree(), sn.getOctave(), int(sn.getAdjustment()));

    assert(midiNote.get() < 1000);

    NotationNote::Accidental accidental = NotationNote::Accidental::none;

    // The problem here is that we are getting the accidental and the leger line independently,
    // and they are making different assumptions about spelling.
    // Idea 1: we can try different accidentals to get a valid note.
    // Idea 2: we can try different leger lines to get a valid note.
    if (sn.getAdjustment() == ScaleNote::RelativeAdjustment::none) {
        accidental = NotationNote::Accidental::none;
    } else if (!midiNote.isBlackKey()) {
        accidental = NotationNote::Accidental::natural;
    } else {
        accidental = (sn.getAdjustment() == ScaleNote::RelativeAdjustment::flat) ? NotationNote::Accidental::flat : NotationNote::Accidental::sharp;
    }

    // SQINFO("func determined NN accidental should be %d (none=%d, sharp=%d,flat=%d,nat=%d)",
    //        int(accidental),
    //        int(NotationNote::Accidental::none),
    //        int(NotationNote::Accidental::sharp),
    //        int(NotationNote::Accidental::flat),
    //        int(NotationNote::Accidental::natural));

    const ResolvedSharpsFlatsPref pref = scale.getSharpsFlatsPrefResolved();

    // SQINFO("about to make notation note,pref=%d bassStaff=%d. pref sharp=%d, flat=%d d/c=%d ",
    //        pref,
    //        bassStaff,
    //        int(SharpsFlatsPref::Sharps),
    //        int(SharpsFlatsPref::Flats),
    //        int(SharpsFlatsPref::DontCare)
    //        );
    //SQINFO("MidiNote.get line =%d", midiNote.getLegerLine(pref, bassStaff));
    //SQINFO("");

    const int legerLine = midiNote.getLegerLine(pref, bassStaff);
    auto ret = NotationNote(midiNote, accidental, legerLine, bassStaff);
    //SQINFO("first try = %s using ll=%d", ret.toString().c_str(), legerLine);
    if (validate(ret, scale)) {
        //SQINFO("accepted first try");
        return ret;
    }
    ret = NotationNote(midiNote, accidental, legerLine - 1, bassStaff);
    //SQINFO("");
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
    //SQINFO("normalized ll = %d", normalizedLegerLine);
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
    //SQINFO("after munge, oct=%d rem=%d", octave, remainder);

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
    //SQINFO("no accid pitch is %d", pitch);

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
    //SQINFO("after accid pitch is %d oct=%d will ret %d", pitch, octave, pitch + octave * 12);
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
    //SQINFO("num=%d, accidentals=%p", num, accidentals);

    // for (int i = 0; i < num; ++i) {
    //     const MidiNote& r = accidentals[i];
    //     SQINFO("accidental[%d] at pitch %d", i, r.get());
    // }

    if (num) {
        for (int i = 0; i < num; ++i) {
            const auto accidentalNote = accidentals[i];
            const int candidateLegerLine = normalizeIntPositive(accidentalNote.getLegerLine(bassStaff), 7);

            assert(candidateLegerLine < 8);
            assert(candidateLegerLine >= 0);

            assert(normalizedLegerLine < 8);
            assert(normalizedLegerLine >= 0);

            // SQINFO("in loop i=%d, cll=%d accidental note pitch=%d pitch rel middle c=%d", i, candidateLegerLine, accidentalNote.get(), accidentalNote.get() - MidiNote::MiddleC);
            // SQINFO("  rel c4=%d", accidentalNote.get() - (MidiNote::MiddleC + 12));
            // SQINFO("  ll we try to match %d, candidate=%d", normalizedLegerLine, candidateLegerLine);
            if (normalizedLegerLine == candidateLegerLine) {
                // here we found an accidental!
                return areFlats ? -1 : 1;
            }
        }
    }

    return 0;
}

bool ScorePitchUtils::validate(const NotationNote& _nn, const Scale& scale) {
    //SQINFO("validate called");
    const int midiNotePitch = _nn._midiNote.get();
    const int legerPitch = pitchFromLeger(_nn._bassStaff, _nn._legerLine, _nn._accidental, scale);

    //SQINFO("midi pitch = %d, leger pitch = %d leger line=%d", midiNotePitch, legerPitch, _nn._legerLine);
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
    SharpsFlatsPref pref) {
    // for (unsigned i=0; i< inputPitches.numValid(); ++i) {
    //     SQINFO("findSpell[%d] %d", i, inputPitches.getAt(i));
    // }

    SqArray<NotationNote, 16> outputNotes;
    const auto info = ChordRecognizer::recognize(inputPitches);
    return _findSpelling(info, scale, inputPitches, outputNotes, bassStaff);
}

ScorePitchUtils::SpellingResults ScorePitchUtils::_findSpelling(
    const ChordRecognizer::ChordInfo& info,
    const Scale& scale,
    const SqArray<int, 16>& inputPitches,
    SqArray<NotationNote, 16>& outputNotes,
    bool bassStaff,
    unsigned evalIndex) {
    // // get all the spellings for current notes.
    // SQINFO("\n** enter findSpelling %d size so far = %d inputSize= %d", evalIndex, outputNotes.numValid(), inputPitches.numValid());
    //  SQINFO("    addr output = %p", &outputNotes);
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
            results = _findSpelling(info, scale, inputPitches, outputNotes, bassStaff, evalIndex + 1);
        } else {
            const int score = _evaluateSpelling(info, outputNotes);
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

int ScorePitchUtils::_evaluateSpelling(const ChordRecognizer::ChordInfo& info, SqArray<NotationNote, 16>& notes) {
    return (info.type == ChordRecognizer::Type::Unrecognized) ? _evaluateSpellingFirstAttempt(info, notes) : _evaluateSpellingThirdAttempt(info, notes);
}

int ScorePitchUtils::_evaluateSpellingFirstAttempt(const ChordRecognizer::ChordInfo& info, SqArray<NotationNote, 16>& notes) {
    // SQINFO("!! evaluate spelling first attempt");
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
            // SQINFO("see accidental at %s", pn1->toString().c_str());
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
    const int score = -(numAccidentals + 10 * numBadThirds);
    // SQINFO("final score = %d", score);
    return score;
}

int ScorePitchUtils::_evaluateSpelling0(const ChordRecognizer::ChordInfo& info, SqArray<NotationNote, 16>& notes) {
    SQWARN("Trivial Eval function");
    return 0;
}

int ScorePitchUtils::_evaluateSpellingThirdAttempt(const ChordRecognizer::ChordInfo& info, SqArray<NotationNote, 16>& notes) {
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
    // first penalize all the accidentals

    for (unsigned i = 0; i < notes.numValid(); ++i) {
        const NotationNote& note = notes.getAt(i);
        if (note.isAccidental()) {
            numAccidentals++;
            // SQINFO("see accidental at %s", note.toString().c_str());
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

    const int score = -(numAccidentals + 10 * numBadThirds);
#ifdef _LOG
    SQINFO("------------ leave eval sp final score3 = %d bad3=%d accid=%d", score, numBadThirds, numAccidentals);
#endif
    return score;
}
