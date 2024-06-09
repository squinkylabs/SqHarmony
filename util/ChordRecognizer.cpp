
#include "ChordRecognizer.h"

std::tuple<ChordRecognizer::Type, int> ChordRecognizer::recognize(const int* chord) {
    //  const int base = MidiNote::C;
    if ((chord[0] == MidiNote::C) &&
        (chord[1] == MidiNote::E) &&
        (chord[2] == MidiNote::G) &&
        (chord[3] < 0)) {
        return std::make_tuple(Type::MajorTriad, MidiNote::C);        
    }
    return std::make_tuple(Type::Unrecognized, MidiNote::C);
}