#pragma once

// #include <algorithm>
#include <tuple>

#include "MidiNote.h"

class ChordRecognizer {
public:
    enum class Type {
        Unrecognized,
        MajorTriad,
        MinorTriad,
    };

    /**
     */
    static std::tuple<Type, int> recognize(const int* chord);

private:
    static Type recognizeType(const int* chord);
    static unsigned getLength(const int* chord);
    static void copy(int* dest, const int* src);
};
