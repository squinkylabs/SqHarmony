
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

bool ScorePitchUtils::_makeNoteAtLegerLine(NotationNote& nn, int legerLineTarget) {
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

bool ScorePitchUtils::reSpell(NotationNote& nn, bool moreSharps) {
    assert(validate(nn));
    const int legerLineTarget = nn._legerLine + ((moreSharps) ? -1 : 1);
    return _makeNoteAtLegerLine(nn, legerLineTarget);
}

bool ScorePitchUtils::validate(const NotationNote& nn) {
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

SqArray<NotationNote, 16> ScorePitchUtils::getVariations(const NotationNote& nn) {
    assert(validate(nn));
    NotationNote temp = nn;
    SqArray<NotationNote, 16> ret;
    // ret._push(temp);
    unsigned index = 0;
    ret.putAt(index++, temp);
    while (reSpell(temp, true)) {
        ret.putAt(index++, temp);
    }
    while (reSpell(temp, false)) {
        ret.putAt(index++, temp);
    }
    return ret;
}

int ScorePitchUtils::findSpelling(
    const Scale& scale, 
    SqArray<int, 16> inputPitches, 
    SqArray<NotationNote, 16>& outputNotes, 
    bool bassStaff, 
    unsigned evalIndex) {
    // get all the spellings for current notes.
    // SQINFO("** enter findSpelling %d size so far = %d nputSize= %d", evalIndex, outputNotes.numValid(), inputPitches.numValid());
    const int currentPitch = inputPitches.getAt(evalIndex);
    const MidiNote currentMidiNote = MidiNote(currentPitch);
    const auto defaultNotationNote = getNotationNote(scale, currentMidiNote, bassStaff);
    const auto currentVariations = getVariations(defaultNotationNote);

    // if more notes: recurse down
    if (evalIndex < inputPitches.numValid() - 1) {
      //  SQINFO("will recurse");
        unsigned bestIndex = 0;
        int bestScore = -1000000;

        for (unsigned i = 0; i < currentVariations.numValid(); ++i) {
            outputNotes.putAt(evalIndex, currentVariations.getAt(i));   // put the current candidate into the test array
            const int score = findSpelling(scale, inputPitches, outputNotes, bassStaff, evalIndex + 1);
            if (score < bestScore) {
            //    SQINFO("found new best score %d at %d", score, i);
                bestScore = score;
                bestIndex = i;
            }
        }
    //    SQINFO("pushing after recursive call ");
      
        //   outputNotes._push(currentVariations[0]);
        return bestScore;
    } else {
        // if no more notes: evaluate and return
     //   SQINFO("---will return at index=%d", evalIndex);

     //   SQINFO("pushing at end");


        unsigned bestIndex = 0;
        int bestScore = -1000000;
        for (unsigned i = 0; i < currentVariations.numValid(); ++i) {
            outputNotes.putAt(evalIndex, currentVariations.getAt(i));   // put the current candidate into the test array
            const int score = _evaluateSpelling(outputNotes);
            if (score < bestScore) {
               // SQINFO("found new terminal best score %d at %d", score, i);
                bestScore = score;
                bestIndex = i;
            }
        }

        return bestScore;
    }
}

int ScorePitchUtils::_evaluateSpelling( SqArray<NotationNote, 16>& notes) {
    return 0;
}