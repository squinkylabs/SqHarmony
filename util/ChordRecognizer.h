#pragma once

// #include <algorithm>
#include <tuple>
#include <string>

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
    static ChordInfo recognize(const int* chord, unsigned size);

    // don't call from audio tread!
    static std::string toString(const ChordInfo&);

private:
    /**
     * 
     * @param chord - chord to analyze, already normalized, sorted, etc...
     * @return 0 is type of chord, 1 is amount it needs to be transposed.
     */
    static std::tuple<Type, int> recognizeType(const int* chord, unsigned length);
    static std::tuple<Type, int> recognizeType3WithFifth(const int* chord);

//    static unsigned getLength(const int* chord);
    static void copy(int* dest, const int* src, unsigned length);
};
