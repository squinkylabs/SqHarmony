/* Chord.hpp
 */

#pragma once

#include <assert.h>

#include <memory>
#include <vector>

#include "ChordRelativeNote.h"
#include "HarmonyNote.h"
#include "ScaleRelativeNote.h"

/**
 * Taxonomy of note types, and when the internal value means.
 *
 * Chord4
 *      contains 4 ScaleRelativeNote (the four notes in the chord)
 *      also contains 4 HarmonyNotes (I think abs pitch of the four notes)
 *      the ScaleRelativeNote are all derived from the HarmonyNotes, with Chords::makeSrnNotes
 *          which calls directly into Keysig::ScaleDeg()
 *          Keysig::ScaleDeg(HarmonyNote Pitch) does
 *              chro = PitchKnowledge::chromaticFromAbs(Pitch);  // get c, c#, etc...
 *              chro -= (int) root;                 // normalize relative to our root
 *              if (chro < 1) chro += 12;     // keep positive
 *              assert(chro > 0 && chro <= 12);
 *             ret.set(nDegreeTable[chro]);  // coerce us into the ScaleRelativeNote
 *      contains int root,
 *          1 = tonic
 *          5 = dominant
 *      has ::print methods :
 *          prints root with just %d
 *          prints 4 notes with HarmonyNote::tellPitchName(), which calls right to PitchKnowledge::nameOfAbs
 *
 * Keysig
 *      constructed from Roots rt
 *      fixed to major key
 *          nDegreeTable[0] = 0;  // 0 is not used
 *          nDegreeTable[1] = 1;
 *          nDegreeTable[2] = 0;
 *          nDegreeTable[3] = 2;
 *          nDegreeTable[4] = 0;
 *          nDegreeTable[5] = 3;
 * HarmonyNote
 *      Just a type-safe wrapper around an integer midi pitch
 *
 * ScaleRelativeNote
 *      contains int pitch 1..8: root, second, third...
 *      print method:
 *
 */

#define CHORD_SIZE 4
#define OCTAVE_SPAN 1

extern int __numChord4;

class Options;
using Chord4Ptr = std::shared_ptr<Chord4>;
using ConstChord4Ptr = std::shared_ptr<const Chord4>;

enum VOICE_NAME { BASS,
                  TENOR,
                  ALTO,
                  SOP };

enum INVERSION { ROOT_POS_INVERSION,
                 FIRST_INVERSION,
                 SECOND_INVERSION,
                 NO_INVERSION };

class Chord4 {
public:
    Chord4(const Options& options, int nDegree);  // pass scale degree in constructor
                                                  // This construct will advance us to valid guy

    bool operator==(const Chord4& that) const {
        return _notes == that._notes;
    }
    // TODO: get rid of this default ctor
    Chord4();
    ~Chord4();

    /**
     * @brief makes a specific string, ex "E2A2C3A3", BUT:
     *      it can only do this if the chord is "legal" according to options
     *      it is not super fast.
     *
     * @return Chord4Ptr
     */
    static Chord4Ptr fromString(const Options& options, int degree, const char*);

    bool makeNext(const Options& op);  // returns false if made another one, true if could not
    void print() const;
    int quality(const Options& options, bool fTalk) const;  // tell how "good" this chord is
                                                            // if ftalk is true, will tell why

    int penaltForFollowingThisGuy(const Options&, int lowestPenaltySoFar, const Chord4* ThisGuy, bool show) const;

    const HarmonyNote* fetchNotes() const;  // This returns pointer so you can get at all 4
    const ScaleRelativeNote* fetchSRNNotes() const;
    bool isInChord(const Options& op, HarmonyNote test) const;  // Tells if a note pitch is valid note in this chord
    int fetchRoot() const;                                      // tell root of chord
    INVERSION inversion(const Options& op) const;               // 0 if root, 1 it 1st inv, etc..

#ifdef _DEBUG
    void dump() const;
#endif
    std::string toString() const { return getString(); }
    std::string toStringShort() const;

    bool isAcceptableDoubling(const Options& option) const;
    bool isCorrectDoubling(const Options& option) const;

    bool isValid() const { return valid; }

private:
    // friend ChordList;  // so he can "construct" us

    bool isChordOk(const Options&) const;  // Tells if the current chord is "good"
    bool pitchesInRange(const Options&) const;
    ChordRelativeNote chordInterval(const Options&, HarmonyNote) const;  // converts from scale rel to chord rel

    bool inc(const Options&);  // go to next chord (valid or not), return true if can't

    // This is deprecated
    //  bool isStdDoubling(const Options& options);  // true is root doubled, etc...

    void bumpToNextInChord(const Options& options, HarmonyNote& note);

    int divergence(const Options& options) const;  // for judging quality, compute how far from center of range
    void analyze();                                // print our analysis
    void makeSrnNotes(const Options& op);          // init the srnNotes array

    //    int InCommon(const Chord4 * ThisGuy) const;
    //    void FigureMotion(int *, bool *, const Chord4 * ThisGuy) const;

    std::string getString() const;  // printer helper function.  Gets string ascii representation of chord

    // **** guys who allocate storage ******
    // static int size;

    ScaleRelativeNote srnNotes[CHORD_SIZE];  // After MakeNext is called, these will be valid
                                             //   used for analysis

    int root = 1;  // 1..8 1 = chord is tonic, 5 = dominant, etc..
                   // is scale relative
    std::vector<HarmonyNote> _notes;
    bool valid = false;
};

inline int Chord4::fetchRoot() const {
    return root;
}

inline const HarmonyNote* Chord4::fetchNotes() const {
    assert(_notes.size() == 4);
    return _notes.data();
}

inline const ScaleRelativeNote* Chord4::fetchSRNNotes() const {
    return srnNotes;
}
