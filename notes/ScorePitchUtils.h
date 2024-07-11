
#pragma once

#include <assert.h>

#include "NotationNote.h"
#include "SqArray.h"

// class NotationNote;
class Scale;
class MidiNote;

class ScorePitchUtils {
public:
    ScorePitchUtils() = delete;

    static NotationNote getNotationNote(const Scale&, const MidiNote&, bool bassStaff);
    static SqArray<NotationNote, 16> getVariations(const NotationNote&, const Scale&);
    /**
     * @brief
     *
     * @param scale
     * @param inputPitches
     * @param outputNotes
     * @param bassStaff
     * @param evalIndex
     * @return int = the score for that spelling
     */
    static int findSpelling(const Scale& scale, const SqArray<int, 16>& inputPitches, SqArray<NotationNote, 16>& outputNotes, bool bassStaff, unsigned evalIndex = 0);

    /**
     * @brief  change accidental and leger line for an alternate enharmonic spelling
     *
     * @param moreSharps - if true will attemt to re-spell at a lower pitch, with more sharps.
     * @return  true if success.
     */
    static bool reSpell(NotationNote& note, bool moreSharps, const Scale&);

    static bool validate(const NotationNote&, const Scale&);

    /**
     * @brief convert from music staff to midi pitch.
     *
     * @param bassStaff - true if bass, false if treble.
     * @param legerLine - line of the staff (or off of it), starting at the lowest (middle C is -2 in treble clef).
     * @param accidental - don't care means none here.
     * @return int - the midi pitch
     */
    // static int pitchFromLeger(bool bassStaff, int legerLine, SharpsFlatsPref accidental);
    static int pitchFromLeger(bool bassStaff, int legerLine, NotationNote::Accidental, const Scale&);

    static int _getAjustmentForLeger(const Scale& scale, bool bassStaff, int legerLine);

    static NotationNote makeCanonical(const NotationNote&);

private:
    static bool _makeNoteAtLegerLine(NotationNote& nn, int legerLine, const Scale&);

    static int _evaluateSpelling(SqArray<NotationNote, 16>& notes);
};
