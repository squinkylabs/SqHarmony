
#include "ScorePitchUtils.h"

// #include <tuple>

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
            _getAjustmentForLeger(scale, bassStaff, _legerLine);
            assert(false);
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

inline  int normalizeIntPositive(int input, int rangeTop) {
    assert(rangeTop > 0);

    const int rem = input % rangeTop;
    return (rem < 0) ? rem + rangeTop : rem;
}

// static int _getAjustmentForLeger(const Scale& scale, bool bassStaff, int legerLine);
int ScorePitchUtils::_getAjustmentForLeger(const Scale& scale, bool bassStaff, int legerLine) {
    SQINFO("\n\n----- enter _getAjustmentForLeger line=%d, bass=%d", legerLine, bassStaff);
    const auto info = scale.getScoreInfo();
    //  const MidiNote* sharps = bassStaff ? info.sharpsInBassClef : info.sharpsInTrebleClef;
    //  const MidiNote* flats = bassStaff ? info.flatsInBassClef : info.flatsInTrebleClef;

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
    SQINFO("num=%d, accidentals=%p", num, accidentals);

    if (num) {
        for (int i = 0; i < num; ++i) {
            //SQINFO("in loop i=%d, ");
            const auto accidentalNote = accidentals[i];
            const int candidateLegerLine = normalizeIntPositive(accidentalNote.getLegerLine(bassStaff), 7);
            
            assert(candidateLegerLine < 8);
            assert(candidateLegerLine >= 0);
            SQINFO("in loop i=%d, cll=%d accidental note pitch=%d pitch rel mid c=%d", i, candidateLegerLine, accidentalNote.get(), accidentalNote.get() - MidiNote::MiddleC);
            SQINFO("  rel c4=%d", accidentalNote.get() - (MidiNote::MiddleC + 12));
            if (legerLine == candidateLegerLine) {
                // here we found an accidental!
                //assert(false);
                return areFlats ? -1 : 1;
            }
        }
    }

    // this is the secret!
    //   note.getLegderLine(bassStaff);
//    assert(false);


    // ok, here we don't see any accidentals, so no adjustment.

    return 0;
}

bool ScorePitchUtils::validate(const NotationNote& nn, const Scale& scale) {
    const int midiNotePitch = nn._midiNote.get();
    const int legerPitch = pitchFromLeger(false, nn._legerLine, nn._accidental, scale);
    return midiNotePitch == legerPitch;
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

int ScorePitchUtils::findSpelling(
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

    NotationNote bestNote;
    for (unsigned i = 0; i < currentVariations.numValid(); ++i) {
        outputNotes.putAt(evalIndex, currentVariations.getAt(i));  // put the current candidate into the test array
        // SQINFO("about to recurse, index=%d", evalIndex);

        int score = 0;
        if (evalIndex < inputPitches.numValid() - 1) {
            score = findSpelling(scale, inputPitches, outputNotes, bassStaff, evalIndex + 1);
        } else {
            score = _evaluateSpelling(outputNotes);
        }
        if (score < bestScore) {
            // SQINFO("recurse found new best score for %d, note=%s", evalIndex, outputNotes.getAt()
            bestScore = score;
       //     bestIndex = i;

            bestNote = currentVariations.getAt(i);
            // now need to put best one back
        }
    }
    // SQINFO("after recurse, restored best note at index =%d note=%s", evalIndex, bestNote.toString().c_str());
    outputNotes.putAt(evalIndex, bestNote);
    return bestScore;
}

#if 0
int ScorePitchUtils::findSpelling(
    const Scale& scale, 
    const SqArray<int, 16>& inputPitches, 
    SqArray<NotationNote, 16>& outputNotes, 
    bool bassStaff, 
    unsigned evalIndex) {
    // get all the spellings for current notes.
    SQINFO("\n** enter findSpelling %d size so far = %d inputSize= %d", evalIndex, outputNotes.numValid(), inputPitches.numValid());
    SQINFO("    addr output = %p", &outputNotes);
    const int currentPitch = inputPitches.getAt(evalIndex);
    const MidiNote currentMidiNote = MidiNote(currentPitch);
    const auto defaultNotationNote = getNotationNote(scale, currentMidiNote, bassStaff);
    const auto currentVariations = getVariations(defaultNotationNote);

    // if more notes: recurse down
    if (evalIndex < inputPitches.numValid() - 1) {
      //  SQINFO("will recurse");
        unsigned bestIndex = 0;
        int bestScore = -1000000;

        NotationNote bestNote;
        for (unsigned i = 0; i < currentVariations.numValid(); ++i) {
            outputNotes.putAt(evalIndex, currentVariations.getAt(i));   // put the current candidate into the test array
            SQINFO("about to recurse, index=%d", evalIndex);
            const int score = findSpelling(scale, inputPitches, outputNotes, bassStaff, evalIndex + 1);
            if (score < bestScore) {
               // SQINFO("recurse found new best score for %d, note=%s", evalIndex, outputNotes.getAt()
                bestScore = score;
                bestIndex = i;

                bestNote = currentVariations.getAt(i);
                // now need to put best one back
            }
        }
        SQINFO("after recurse, restored best note at index =%d note=%s", evalIndex, bestNote.toString().c_str());
        outputNotes.putAt(evalIndex, bestNote);
        return bestScore;
    } else {
        // if no more notes: evaluate and return
        unsigned bestIndex = 0;
        int bestScore = -1000000;
         NotationNote bestNote;
        for (unsigned i = 0; i < currentVariations.numValid(); ++i) {

        
            outputNotes.putAt(evalIndex, currentVariations.getAt(i));   // put the current candidate into the test array
            const int score = _evaluateSpelling(outputNotes);
            if (score < bestScore) {
                SQINFO("found new terminal best score %d at index %d note=%s", score, i, currentVariations.getAt(i).toString().c_str());
                bestScore = score;
                bestIndex = i;
                bestNote = currentVariations.getAt(i);
            }
        }
        outputNotes.putAt(evalIndex, bestNote);
        return bestScore;
    }
}
#endif

int ScorePitchUtils::_evaluateSpelling(SqArray<NotationNote, 16>& notes) {
    return 0;
}
