
#pragma once

#include <string>
#include <vector>

#include "MidiNote.h"
#include "ScaleNote.h"

class Scale {
public:
    enum class Scales {
        Major,
        Dorian,
        Phrygian,
        Lydian,
        Mixolydian,
        Minor,
        Locrian,
        MinorPentatonic,
        HarmonicMinor,
        Diminished,
        DominantDiminished,
        WholeStep,
        Chromatic
    };

    static std::vector<std::string> getScaleLabels(bool onlyDiatonic);
    static std::vector<std::string> getShortScaleLabels(bool);
    static std::vector<std::string> getRootLabels();

    void set(const MidiNote& base, Scales mode);
    std::pair<const MidiNote, Scales> get() const;

    /**
     * @brief convert a scale relative degree to an absolute pitch
     *
     * @param degree is scale degree, 0=root, 1=2nd...
     *      must be >= 0 and not wrap above
     * @return int is absolute pitch offset, semitones
     */
    int degreeToSemitone(int degree) const;

    /**
     * @brief quantize a pitch to a scale
     *
     * @param offset is pitch in semitones, >= 0 and < 12
     * @return int is the scale degree
     */
    int quantize(int offset) const;

    const MidiNote& base() const {
        return baseNote;
    }

    /**
     * @brief convert an absolute pitch to a scale relative one.
     *
     * @return ScaleNote
     */
    ScaleNote m2s(const MidiNote&) const;

    MidiNote s2m(const ScaleNote&) const;

    bool getWasSet() const { return wasSet; }

    /**
     * @brief data for draing key signatures
     * 
     */
    class ScoreInfo {
    public:
        int numSharps = 0;
        int numFlats = 0;
        const MidiNote* sharpsInTrebleClef = nullptr;
        const MidiNote* sharpsInBassClef = nullptr;
        const MidiNote* flatsInTrebleClef = nullptr;
        const MidiNote* flatsInBassClef = nullptr;
    };

    /**
     * @brief Get the Score Info object for current scale
     * Only works for the diatonic modes.
     * 
     * @return ScoreInfo 
     */
    ScoreInfo getScoreInfo() const;

    MidiNote getRelativeMajor() const;

private:
    ScaleNote makeScaleNote(int offset) const;

    MidiNote baseNote;
    Scales scale;
    bool wasSet = false;

    /** get the intervals of the current scale
     * example: major is 0, 2, 4, 5....
     * list of pitches is terminated with a negative number
     */
    const int* getNormalizedScalePitches() const;

    // returns < - if note isn't in scale.
    int quantizeInScale(int offset) const;
};