
#include "ChordRecognizer.h"
#include "SqLog.h"


std::tuple<ChordRecognizer::Type, int> ChordRecognizer::recognize(const int* chord) {
    const int base = chord[0];
    if (base < 0) {
        return std::make_tuple(Type::Unrecognized, MidiNote::C);
    }

#ifdef _DEBUG
    SQINFO("chord = %d, %d, %d, %d", chord[0], chord[1], chord[2], chord[3]);
#endif


    // normalize pitches to C
    int normalizedChord[16];
    int i;
    for (i=0; chord[i] >=0; ++i) {
        int note = chord[i] - base; // normalize to base
        note = note % 12;           // normalize to octave
        normalizedChord[i] = note;
    }
    normalizedChord[i] = -1;

    // sort
    //
    std::sort(normalizedChord, normalizedChord + i);

    // remove dupes
    int chord2[16];
    int j;
    for (i = j = 0; normalizedChord[i] >= 0; ++i) {
        if (normalizedChord[i] != (normalizedChord[i + 1])) {
            chord2[j++] = normalizedChord[i];
        }
    }
    chord2[j] = -1;



    const auto t = recognizeType(chord2);
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