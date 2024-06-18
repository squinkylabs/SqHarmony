
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
        MajorPentatonic,
        MinorPentatonic,
        HarmonicMinor,
        Diminished,
        DominantDiminished,
        WholeStep,
        Chromatic
    };

    const static int firstScale = int(Scales::Major);
    // KEEP THIS UP TO DATE
    const static int lastScale = int(Scales::Chromatic);

    // deprecated?
    static int numScalesTotal() { return lastScale + 1; }
    static int numDiatonicScales() { return 7; }
    static int numScales(bool onlyDiatonic) { return onlyDiatonic ? numDiatonicScales() : numScalesTotal(); }
    const static int numStepsInDiatonicScale = 7;

    static int numNotesInScale(Scales s);

    static std::vector<std::string> getScaleLabels(bool onlyDiatonic);
    static std::vector<std::string> getShortScaleLabels(bool onlyDiatonic);
    static std::vector<std::string> getRootLabels(bool useFlats);

    void set(const MidiNote& base, Scales mode);
    std::pair<const MidiNote, Scales> get() const;

    // All of this Role stuff is for converting Scale <> PES
    enum class Role {
        Root,
        InScale,
        NotInScale,
        End
    };

    class RoleArray {
    public:
        RoleArray() {
            for (int i = 0; i < 12; ++i) {
                data[i] = Role::NotInScale;
            }
            data[12] = Role::End;
        }
        Role data[13];
    };

    /**
     * the tuple returned is success, root, mode
     */
    static std::tuple<bool, MidiNote, Scales> convert(const Role* noteRole, bool diatonicOnly);
    static const RoleArray convert(MidiNote, Scales);

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
        return _baseNote;
    }

    /**
     * @brief convert an absolute pitch to a scale relative one.
     *
     * @return ScaleNote
     */
    ScaleNote m2s(const MidiNote&) const;

    MidiNote s2m(const ScaleNote&) const;

    bool getWasSet() const { return _wasSet; }

    /**
     * @brief data for drawing key signatures
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

    enum class SharpsFlatsPref {
        Sharps,
        Flats,
        DontCare
    };
    SharpsFlatsPref getSharpsFlatsPref() const;

    MidiNote getRelativeMajor() const;

    static void _dumpRoles(const char* message, const Role* roles);

    /** get the intervals of the current scale
     * example: major is 0, 2, 4, 5....
     * list of pitches is terminated with a negative number
     */
    const int* _getNormalizedScalePitches() const;

private:
    ScaleNote _makeScaleNote(int offset) const;

    MidiNote _baseNote;
    Scales _scale;
    bool _wasSet = false;

    //  bool _getScalePitches(int * destination, unsigned destinationSize);

    // returns < - if note isn't in scale.
    int _quantizeInScale(int offset) const;

    static bool _doesScaleMatch(const Role*, Scales, MidiNote);
};