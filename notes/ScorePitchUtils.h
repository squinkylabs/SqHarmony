
#pragma once

#include <assert.h>

#include "ChordRecognizer.h"
#include "NotationNote.h"
#include "SqArray.h"

class Scale;
class MidiNote;

class ScorePitchUtils {
public:
    ScorePitchUtils() = delete;

    class LegerLineTracker {
    public:
        int getSawMulti() const {
            return _count;
        }

        // lineSeen will be >= -2, <= 12
        void sawLine(int lineSeen) {
            const int index = line2Index(lineSeen);
            _llCount[index]++;
            if (_llCount[index] > 1) {
                ++_count;
            }
        }
    private:
        // index -2 is zero
        char _llCount[256 + 1] = {0};
        int _count = 0;
        const int _minLine = -128;
        const int _maxLine = 128;

        int line2Index(int line) {
            assert((_maxLine - _minLine) >= 256);

            assert(line >= _minLine);
            assert(line <= _maxLine);
            if (line >= _minLine && line <= _maxLine) return line - _minLine;
            else return 0;
        }
    };

    /**
     * requirements:
     *      returns a valid note
     *      returns a note at the right pitch
     * do we care about the spelling of the returned note?

     */
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
    class SpellingResults {
    public:
        int score = 0;
        SqArray<NotationNote, 16> notes;
    };

    class SpellingPreferences {
    public:
        bool sharpsOrFlats = true;
    };

    static SpellingResults findSpelling(
        const Scale& scale,
        const SqArray<int, 16>& inputPitches,
        bool bassStaff,
        UIPrefSharpsFlats pref);

    static SpellingResults _findSpelling(
        const SpellingPreferences& prefs,
        const ChordRecognizer::ChordInfo& info,
        const Scale& scale,
        const SqArray<int, 16>& inputPitches,
        SqArray<NotationNote, 16>& outputNotes,
        bool bassStaff,
        unsigned evalIndex = 0);

    /**
     * @brief  change accidental and leger line for an alternate enharmonic spelling
     *
     * @param note - input note, and output note if successful.
     * @param moreSharps - if true will attempt to re-spell at a lower pitch, with more sharps.
     * @return  true if success.
     */
    static bool reSpell(NotationNote& note, bool moreSharps, const Scale&);
    static bool reSpellOnLegerLine(NotationNote&note, int legerLine, const Scale&);

    static bool validate(const NotationNote&, const Scale&);

    /**
     * @brief convert from music staff to midi pitch.
     *
     * @param bassStaff - true if bass, false if treble.
     * @param legerLine - line of the staff (or off of it), starting at the lowest (middle C is -2 in treble clef).
     * @param accidental - don't care means none here.
     * @param scale - the current scale.
     * @return int - the midi pitch
     */
    static int pitchFromLeger(bool bassStaff, int legerLine, NotationNote::Accidental, const Scale&);

    /**
     * @brief returns the adjustment in leger lines due to note getting a sharp or flat from the scale
     *
     * @param scale - The scale currently in effect.
     * @param bassStaff - Which staff we are asking about.
     * @param legerLine - the line the note would be on, if not for the Scale (key signature).
     * @return int - the adjustment in leger lines to move the note to the correct pitch, based on scale.
     */
    static int _getAjustmentForLeger(const Scale& scale, bool bassStaff, int legerLine);


    // This isn't used any more?
    //static NotationNote makeCanonical(const NotationNote&);

private:
    static bool _makeNoteAtLegerLine(NotationNote& nn, int legerLine, const Scale&, bool bassStaff);
    static int _evaluateSpelling(const SpellingPreferences& prefs, const ChordRecognizer::ChordInfo& info, SqArray<NotationNote, 16>& notes);
    static int _evaluateSpelling0(const SpellingPreferences& pref, const ChordRecognizer::ChordInfo& info, SqArray<NotationNote, 16>& notes);
    static int _evaluateSpellingFirstAttempt(const SpellingPreferences& pref, const ChordRecognizer::ChordInfo& info, SqArray<NotationNote, 16>& notes);
    static int _evaluateSpellingSecondAttempt(const SpellingPreferences& pref, const ChordRecognizer::ChordInfo& info, SqArray<NotationNote, 16>& notes);
    static int _evaluateSpellingThirdAttempt(const SpellingPreferences& pref, const ChordRecognizer::ChordInfo& info, SqArray<NotationNote, 16>& notes);
};
