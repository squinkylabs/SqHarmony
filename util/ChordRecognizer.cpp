
#include "ChordRecognizer.h"


std::tuple<ChordRecognizer::Type, int> ChordRecognizer::recognize(const int* chord) {
    const int base = chord[0];
    if (base < 0) {
        return std::make_tuple(Type::Unrecognized, MidiNote::C);
    }
    int normalizedChord[16];
    int i;
    for (i=0; chord[i] >=0; ++i) {
        int note = chord[i] - base; // normalize to base
        note = note % 12;           // normalize to octave
        normalizedChord[i] = note;
    }
    normalizedChord[i] = -1;
    const auto t = recognizeType(normalizedChord);
    return std::make_tuple(t, base % 12);

}

 ChordRecognizer::Type ChordRecognizer::recognizeType(const int* chord) {
    if ((chord[0] == MidiNote::C) &&
        (chord[1] == MidiNote::E) &&
        (chord[2] == MidiNote::G) &&
        (chord[3] < 0)) {
        return Type::MajorTriad;        
    }
    return Type::Unrecognized;
}