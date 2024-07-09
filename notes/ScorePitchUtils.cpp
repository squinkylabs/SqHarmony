
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
    const auto currentVariations = getVariations(defaultNotationNote);

    //  SQINFO("will recurse");
    unsigned bestIndex = 0;
    int bestScore = -1000000;

    NotationNote bestNote;
    for (unsigned i = 0; i < currentVariations.numValid(); ++i) {
        outputNotes.putAt(evalIndex, currentVariations.getAt(i));  // put the current candidate into the test array
        //SQINFO("about to recurse, index=%d", evalIndex);

        int score = 0;
        if (evalIndex < inputPitches.numValid() - 1) {
            score = findSpelling(scale, inputPitches, outputNotes, bassStaff, evalIndex + 1);
        } else {
            score = _evaluateSpelling(outputNotes);
        }
        if (score < bestScore) {
            // SQINFO("recurse found new best score for %d, note=%s", evalIndex, outputNotes.getAt()
            bestScore = score;
            bestIndex = i;

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
