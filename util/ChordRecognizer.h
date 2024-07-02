#pragma once

// #include <algorithm>
#include <string>
#include <tuple>
#include <vector>

#include "MidiNote.h"
#include "PitchKnowledge.h"

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
    static ChordInfo recognize(const int* chord, unsigned size);

    // Don't call from audio thread!
    static std::vector<std::string> toString(const ChordInfo&);

    static unsigned notesInChord(Type);

    /**
     * @brief sorts and normalizes a chord to put it in a canonical form for regocnizing
     * 
     * @param outputChord 
     * @param inputChord 
     * @param length 
     * @return std::tuple<unsigned, int> 
     *  0 is the new length, 
     *  1 is how much it was transposed (the base)
     */
    static std::tuple<unsigned, int> _makeCanonical(int* outputChord, const int* inputChord, unsigned length);

    static void copy(int* dest, const int* src, unsigned length);
    static void show(const char* msg, const int* p, unsigned num);

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
    static std::tuple<Type, int> recognizeType(const int* chord, unsigned length);
    static std::tuple<Type, int> recognizeType3WithFifth(const int* chord);
    static std::tuple<Type, int> recognizeType3WithAugFifth(const int* chord);
    static std::tuple<Type, int> recognizeType3WithTritone(const int* chord);
    static std::tuple<Type, int> recognizeType7th(const int* chord);
    static ChordInfo figureOutInversion(Type type, int recognizedPitch, int firstOffset);

    /**
     *
     * @param outputChord
     * @param inputChord
     * @param length
     * @return 0: unsigned the length of the output, 1:int the chord nomralization base
     */
   // static std::tuple<unsigned, int> normalize(int* outputChord, const int* inputChord, unsigned length);
};
