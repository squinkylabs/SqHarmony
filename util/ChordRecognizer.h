#pragma once

// #include <algorithm>
#include <string>
#include <tuple>
#include <vector>

#include "MidiNote.h"
#include "PitchKnowledge.h"
#include "SqArray.h"

// #define _LOG

class ChordRecognizer {
public:
    enum class Type {
        Unrecognized,
        MajorTriad,
        MinorTriad,
        Sus2Triad,
        Sus4Triad,
        DiminishedTriad,
        AugmentedTriad,

        MajMinSeventh,
        MajMajSeventh,
        MinMinSeventh,
        MinMajSeventh,

        MajMinNinth,  // dominant 7th with a major ninth on top.
        MajMajNinth,  // major major 7th (Major 7th) with a major ninth
        MinMinNinth,  // Minor 7th with a major ninth
        MinMajNinth   // MinMin th with a major ninth

    };

    enum class Inversion {
        Root,
        First,
        Second,
        Third
    };

    class PitchAndIndex {
    public:
        PitchAndIndex(int p, int i) : pitch(p), index(i) {}
        PitchAndIndex() {}
        bool operator<(const PitchAndIndex& other) const {
            return this->pitch < other.pitch;
        }

        int16_t pitch = 0;
        uint16_t index = 0;
    };

    class ChordInfo {
    public:
        ChordInfo(Type t, Inversion i, int p, SqArray<PitchAndIndex, 16> idp) : type(t), inversion(i), pitch(p), identifiedPitches(idp) {
        }

        ChordInfo(Type t, Inversion i, int p) : type(t), inversion(i), pitch(p) {}
        ChordInfo() {}
        Type type = Type::Unrecognized;
        Inversion inversion = Inversion::Root;
        int pitch = 0;

        SqArray<PitchAndIndex, 16> identifiedPitches;

        void setError() {
            type = Type::Unrecognized;
        }
        bool isError() const {
            return type == Type::Unrecognized;
        }
    };

    /**
     * @param chord - chord to analyze.
     */
    static ChordInfo recognize(const SqArray<int, 16>& chord);

    // Don't call from audio thread!
    static std::vector<std::string> toString(const ChordInfo&, bool useSharps);

    static unsigned notesInChord(Type);

    /**
     * @brief Sorts and normalizes a chord to put it in a canonical form for recognizing.
     *
     * @param outputChord
     * @param inputChord
     * @param length
     * @return true if successful
     */
    // template <typename T>
    //  static bool _makeCanonical(SqArray<T, 16>& outputChord, const SqArray<T, 16>& inputChord, int& transposeAmount);
    static bool _makeCanonical(SqArray<PitchAndIndex, 16>& outputChord, const SqArray<PitchAndIndex, 16>& inputChord, int& transposeAmount);

    template <typename T>
    static void _copy(SqArray<T, 16>& outputChord, const SqArray<T, 16>& inputChord);

    //  template <typename T>
    static void _show(const char* msg, const SqArray<PitchAndIndex, 16>& chord);

    /**
     * @brief gets the "fractional part" of a number, wraps into range if negative.
     *
     * @param input
     * @param rangeTop
     * @return int strictly between 0...rangeTop-1
     */
    static int normalizeIntPositive(int input, int rangeTop);

private:
    /**
     *
     * @param chord - chord to analyze, already normalized, sorted, etc...
     * @return 0 is type of chord, 1 is amount it needs to be transposed.
     */
    static std::tuple<Type, int> recognizeType(const SqArray<PitchAndIndex, 16>& inputChord);
    static std::tuple<Type, int> recognizeType3WithFifth(const SqArray<PitchAndIndex, 16>& chord);
    static std::tuple<Type, int> recognizeType3WithAugFifth(const SqArray<PitchAndIndex, 16>& chord);
    static std::tuple<Type, int> recognizeType3WithTritone(const SqArray<PitchAndIndex, 16>& chord);
    static std::tuple<Type, int> recognizeType7th(const SqArray<PitchAndIndex, 16>& chord);
    static std::tuple<Type, int> recognizeType9th(const SqArray<PitchAndIndex, 16>& chord);
    static std::tuple<Type, int> recognizeTypeNinthWithSuspendedFifth(const SqArray<PitchAndIndex, 16>& chord);

    static ChordInfo figureOutInversion(Type type, int recognizedPitch, int firstOffset);

    static ChordInfo _recognize(const SqArray<PitchAndIndex, 16>& chord);
};

template <typename T>
inline void ChordRecognizer::_copy(SqArray<T, 16>& outputChord, const SqArray<T, 16>& inputChord) {
    for (unsigned i = 0; i < inputChord.numValid(); ++i) {
        outputChord.putAt(i, inputChord.getAt(i));
    }
}

#if 0
template <typename T>
inline bool ChordRecognizer::_makeCanonical(SqArray<T, 16>& outputChord, const SqArray<T, 16>& inputChord, int& transposeAmount) {
#if defined _LOG
    SQINFO("In normalize %d", inputChord.numValid());
    _show("input chord", inputChord);
#endif

    unsigned length = inputChord.numValid();

    if (length < 1) {
#ifdef _LOG
        SQINFO("In recognize exit early");
#endif
        return false;
    }

    SqArray<T, 16> sortedChord;
    _copy(sortedChord, inputChord);
    std::sort(sortedChord.getDirectPtrAt(0), sortedChord.getDirectPtrAt(length));
// up through 49
#ifdef _LOG
    _show("sorted chord", sortedChord);
#endif
    const T tmp = sortedChord.getAt(0);
   // const int xx = tmp;
    const int base = sortedChord.getAt(0);

#ifdef _LOG
    SQINFO("... in normalize pass, adding %d", -base);
#endif
    unsigned i;
    SqArray<T, 16> normalizedChord;
    for (i = 0; i < length; ++i) {
        T note = sortedChord.getAt(i);
        int notePitch = note;
        notePitch -= base;
        notePitch = notePitch % 12;
        note = notePitch;
        normalizedChord.putAt(i, note);
    }
    // up through 66

#ifdef _LOG
    _show("normalizedChord", normalizedChord);
#endif
    std::sort(normalizedChord.getDirectPtrAt(0), normalizedChord.getDirectPtrAt(length));
#ifdef _LOG
    _show("sorted normalizedChord", normalizedChord);
#endif

    //--- Remove dupes ---
    unsigned j;
    for (i = j = 0; i < length; ++i) {
        // Copy this one over if it is not a dupe. last one is never a dupe.
        if ((i == (length - 1)) || ((int)normalizedChord.getAt(i) != (int)normalizedChord.getAt(i + 1))) {
            const T tmp = normalizedChord.getAt(i);
            outputChord.putAt(j++, normalizedChord.getAt(i));
        }
    }
    length = j;

    // through 84

#ifdef _LOG
    _show("final", outputChord);
#endif
    for (unsigned i = 0; i < length; ++i) {
        assert(outputChord.getAt(i) >= 0);
    }
    transposeAmount = base;
    return true;
}
#endif

#if 0
template <typename T>
inline void ChordRecognizer::_show(const char* msg, const SqArray<T, 16>& inputChord) {
    const unsigned num = inputChord.numValid();
    if (num == 3) {
        SQINFO("%s = %d, %d, %d", msg, (int)inputChord.getAt(0), (int)inputChord.getAt(1), (int)inputChord.getAt(2));
    } else if (num == 4) {
        SQINFO("%s = %d, %d, %d, %d", msg, (int)inputChord.getAt(0), (int)inputChord.getAt(1), (int)inputChord.getAt(2), (int)inputChord.getAt(3));
    } else if (num == 5) {
        SQINFO("%s = %d, %d, %d, %d %d", msg, (int)inputChord.getAt(0), (int)inputChord.getAt(1), (int)inputChord.getAt(2), (int)inputChord.getAt(3), (int)inputChord.getAt(4));
    } else {
        SQINFO("??? num=%d", num);
    }
}
#endif