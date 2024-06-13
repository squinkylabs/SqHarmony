#pragma once

// #include <algorithm>
#include <string>
#include <tuple>

#include "MidiNote.h"

class ChordRecognizer {
public:
    enum class Type {
        Unrecognized,
        MajorTriad,
        MajorTriadFirstInversion,
        MajorTriadSecondInversion,
        MinorTriad,
    };

    enum class Inversion {
        Root,
        First,
        Second
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
    static std::string toString(const ChordInfo&);

private:
    /**
     *
     * @param chord - chord to analyze, already normalized, sorted, etc...
     * @return 0 is type of chord, 1 is amount it needs to be transposed.
     */
    static std::tuple<Type, int> recognizeType(const int* chord, unsigned length);
    static std::tuple<Type, int> recognizeType3WithFifth(const int* chord);

    static void copy(int* dest, const int* src, unsigned length);
    /**
     *
     * @param outputChord
     * @param inputChord
     * @param length
     * @return 0: unsigned the length of the output, 1:int the chord nomralization base
     */
    static std::tuple<unsigned, int> normalize(int* outputChord, const int* inputChord, unsigned length);
};
