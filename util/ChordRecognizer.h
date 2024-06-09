#pragma once

//#include <algorithm>
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
    std::tuple<Type, int> recognize(const int * chord);
};
