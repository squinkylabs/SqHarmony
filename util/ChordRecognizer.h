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
    std::tuple<Type, int> recognize(const int * chord) {
      //  const int base = MidiNote::C;
        return std::make_tuple(Type::Unrecognized, MidiNote::C);
    }
    // std::tuple<int, int> abc() {
    //     return std::make_tuple(1, 2);
    // }

};
