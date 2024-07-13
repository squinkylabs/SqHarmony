#pragma once

// #include <algorithm>
#include <string>
#include <tuple>
#include <vector>

#include "MidiNote.h"
#include "PitchKnowledge.h"
#include "SqArray.h"

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

    using ChordInfo = std::tuple<Type, Inversion, int>;
    static Type typeFromInfo(const ChordInfo& info) {
        return std::get<0>(info);
    }
    static Inversion inversionFromInfo(const ChordInfo& info) {
        return std::get<1>(info);
    }
    static int pitchFromInfo(const ChordInfo& info) {
        return std::get<2>(info);
    }

    /**
     * @param chord - chord to analyze.
     * @return 0 is type of chord, 1 is the root.
     */
    static ChordInfo recognize(const SqArray<int, 16>& chord);

    // Don't call from audio thread!
    static std::vector<std::string> toString(const ChordInfo&);

    static unsigned notesInChord(Type);

    /**
     * @brief Sorts and normalizes a chord to put it in a canonical form for recognizing.
     *
     * @param outputChord
     * @param inputChord
     * @param length
     * @return true if successful
     */
    static bool _makeCanonical(SqArray<int, 16>& outputChord, const SqArray<int, 16>& inputChord, int& transposeAmount);

    static void _copy(SqArray<int, 16>& outputChord, const SqArray<int, 16>& inputChord);
    static void _show(const char* msg, const SqArray<int, 16>& chord);

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
    static std::tuple<Type, int> recognizeType(const SqArray<int, 16>& inputChord);
    static std::tuple<Type, int> recognizeType3WithFifth(const SqArray<int, 16>& chord);
    static std::tuple<Type, int> recognizeType3WithAugFifth(const SqArray<int, 16>& chord);
    static std::tuple<Type, int> recognizeType3WithTritone(const SqArray<int, 16>& chord);
    static std::tuple<Type, int> recognizeType7th(const SqArray<int, 16>& chord);
    static std::tuple<Type, int> recognizeType9th(const SqArray<int, 16>& chord);
    static std::tuple<Type, int> recognizeTypeNinthWithSuspendedFifth(const SqArray<int, 16>& chord);

    static ChordInfo figureOutInversion(Type type, int recognizedPitch, int firstOffset);
};
