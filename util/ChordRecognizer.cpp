
#include "ChordRecognizer.h"

#include "SqLog.h"

void show(const char* msg, const int* p, int num) {
#ifdef _DEBUG
    if (num == 3) {
        // show the terminator
        SQINFO("%s = %d, %d, %d, %d", msg, p[0], p[1], p[2], p[3]);
    } else if (num == 4) {
        SQINFO("%s = %d, %d, %d, %d %d", msg, p[0], p[1], p[2], p[3], p[4]);
    } else {
        SQINFO("??? num=%d", num);
    }
#endif
}

unsigned ChordRecognizer::getLength(const int* chord) {
    int len = 0;
    while (*chord++ >= 0) {
        ++len;
    }
    // if (len) {
    //     ++len;
    // }
    return len;
}

void ChordRecognizer::copy(int* dest, const int* src) {
    while ((*dest++ = *src++) >= 0) {
    }
}

std::tuple<ChordRecognizer::Type, int> ChordRecognizer::recognize(const int* inputChord) {
    int normalizedChord[16];
    int chord2[16];
    int sortedChord[16];

    const int length = getLength(inputChord);
    show("input chord", inputChord, length);
    if (length < 1) {
         return std::make_tuple(Type::Unrecognized, MidiNote::C);
    }

    copy(sortedChord, inputChord);
    std::sort(sortedChord, sortedChord + (length-1));

    show("sorted chord", sortedChord, length);

    const int base = sortedChord[0];
    if (base < 0) {
        return std::make_tuple(Type::Unrecognized, MidiNote::C);
    }

   // show("chord", chord, 4);

    // normalize pitches to C

    int i;
    for (i = 0; sortedChord[i] >= 0; ++i) {
        int note = sortedChord[i] - base;  // normalize to base
        note = note % 12;                  // normalize to octave
        normalizedChord[i] = note;
    }
    normalizedChord[i] = -1;


    show("normalizedChord", normalizedChord, length);

    std::sort(normalizedChord, normalizedChord + (length));

    show("sorted normalizedChord", normalizedChord, length);
    // remove dupes

    int j;
    for (i = j = 0; normalizedChord[i] >= 0; ++i) {
        if (normalizedChord[i] != (normalizedChord[i + 1])) {
            chord2[j++] = normalizedChord[i];
        }
    }
    chord2[j] = -1;

    show("chord2", chord2, getLength(chord2));

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