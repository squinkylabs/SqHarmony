#pragma once

// #include <algorithm>
#include <tuple>

#include "MidiNote.h"

class ChordRecognizer {
public:
    enum class Type {
        Unrecognized,
        MajorTriad,
        MajorTriadFirstInversion,
        MinorTriad,
    };

    using ChordInfo = std::tuple<Type, int>;

    /**
     * @param chord - chord to analyze.
     * @return 0 is type of chord, 1 is the root.
     */
    static ChordInfo recognize(const int* chord);

    static const char* toString(const ChordInfo&);

private:
    /**
     * 
     * @param chord - chord to analyze, already normalized, sorted, etc...
     * @return 0 is type of chord, 1 is amount it needs to be transposed.
     */
    static std::tuple<Type, int> recognizeType(const int* chord);
    static unsigned getLength(const int* chord);
    static void copy(int* dest, const int* src);
};
