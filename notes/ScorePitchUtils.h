
#pragma once

// #include <tuple>

// #include "MidiNote.h"
// #include "NotationNote.h"
// #include "Scale.h"
// #include "ScaleNote.h"
//  #include "SqLog.h"

#include <assert.h>

class NotationNote;
class Scale;
class MidiNote;

class ScorePitchUtils {
public:
    ScorePitchUtils() = delete;
    template <typename T, unsigned capacity>
    class vlenArray {
    public:
        unsigned size() const {
            return index;
        }

        const T operator[](unsigned i) const {
            assert(i < capacity);
            return data[i];
        }

        void _push(T t) {
            data[index] = t;
            ++index;
            assert(index <= capacity);
        }

    private:
        unsigned index = 0;
        T data[capacity];
    };

    static NotationNote getNotationNote(const Scale&, const MidiNote&, bool bassStaff);
    static vlenArray<NotationNote, 16> getVariations(const NotationNote&);
    static void findSpelling(vlenArray<int, 16> inputPitches, vlenArray<NotationNote, 16> outputNotes, bool bassStaff, unsigned evalIndex = 0);

    /**
     * Not needed right now.
     * @return returns true if accidental1 <= accidental2
     * @return false if  accidental1 > accidental2
     */
    //  static bool compareAccidentals(Accidental accidental1, Accidental accidental2);

    /**
     * @brief  change accidental and leger line for an alternate enharmonic spelling
     *
     * @param moreSharps - if true will attemt to re-spell at a lower pitch, with more sharps.
     * @return  true if success.
     */
    static bool reSpell(NotationNote& note, bool moreSharps);

    static bool validate(const NotationNote&);

private:
    static bool _makeNoteAtLegerLine(NotationNote& nn, int legerLine);
};
