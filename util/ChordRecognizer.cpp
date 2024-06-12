
#include "ChordRecognizer.h"

#include "PitchKnowledge.h"
#include "SqLog.h"

void show(const char* msg, const int* p, int num) {
    // #ifdef _DEBUG
    if (num == 3) {
        // show the terminator
        SQINFO("%s = %d, %d, %d", msg, p[0], p[1], p[2]);
    } else if (num == 4) {
        SQINFO("%s = %d, %d, %d, %d", msg, p[0], p[1], p[2], p[3]);
    } else {
        SQINFO("??? num=%d", num);
    }
    // #endif
}


void ChordRecognizer::copy(int* dest, const int* src, unsigned length) {
    while (length--) {
        *dest++ = *src++;
    }
}

std::tuple<unsigned, int> ChordRecognizer::normalize(int* outputChord, const int* inputChord, unsigned length) {
   // assert(false);
  //  return std::make_tuple(0, 0);

    const auto error = std::make_tuple(0, 0);

    SQINFO("In normalize %d", length);

    show("input chord", inputChord, length);
    if (length < 1) {
        SQINFO("In recognize exit early");
        return error;
    }

    int sortedChord[16];
    copy(sortedChord, inputChord, length);
    std::sort(sortedChord, sortedChord + (length - 1));

    show("sorted chord", sortedChord, length);

    const int base = sortedChord[0];
    if (base < 0) {
        return error;
    }

    unsigned i;
    int normalizedChord[16];
    for (i = 0; i < length; ++i) {
        int note = sortedChord[i] - base;  // normalize to base
        note = note % 12;                  // normalize to octave
        normalizedChord[i] = note;
    }

    show("normalizedChord", normalizedChord, length);
    std::sort(normalizedChord, normalizedChord + (length));
    show("sorted normalizedChord", normalizedChord, length);
    
    // remove dupes
    unsigned j;
    for (i = j = 0; i < length; ++i) {
        if (normalizedChord[i] != (normalizedChord[i + 1])) {
            outputChord[j++] = normalizedChord[i];
        }
    }
    //  chord2[j] = -1;
    length = j;
    show("final", outputChord, length);
    return std::make_tuple(length, base);
}

ChordRecognizer::ChordInfo ChordRecognizer::recognize(const int* inputChord, unsigned length) {
#if 0
    int normalizedChord[16];
    int chord2[16];
    int sortedChord[16];

    SQINFO("In recognize %d", length);

    // const int length = getLength(inputChord);
    show("input chord", inputChord, length);
    if (length < 1) {
        SQINFO("In recognize exit early");
        return std::make_tuple(Type::Unrecognized, MidiNote::C);
    }

    copy(sortedChord, inputChord, length);
    std::sort(sortedChord, sortedChord + (length - 1));

    show("sorted chord", sortedChord, length);

    const int base = sortedChord[0];
    if (base < 0) {
        return std::make_tuple(Type::Unrecognized, MidiNote::C);
    }

    // show("chord", chord, 4);

    // normalize pitches to C

    unsigned i;
    for (i = 0; i < length; ++i) {
        int note = sortedChord[i] - base;  // normalize to base
        note = note % 12;                  // normalize to octave
        normalizedChord[i] = note;
    }
    // normalizedChord[i] = -1;

    show("normalizedChord", normalizedChord, length);

    std::sort(normalizedChord, normalizedChord + (length));

    show("sorted normalizedChord", normalizedChord, length);
    // remove dupes

    unsigned j;
    for (i = j = 0; i < length; ++i) {
        if (normalizedChord[i] != (normalizedChord[i + 1])) {
            chord2[j++] = normalizedChord[i];
        }
    }
    //  chord2[j] = -1;
    length = j;
    show("chord2", chord2, length);
#endif

    int outputChord[16];
    const auto normalized = normalize(outputChord, inputChord, length);
    const unsigned finalLength = std::get<0>(normalized);
    const int base = std::get<1>(normalized);
    if (finalLength == 0) {
        SQINFO("normalize failed");
        return std::make_tuple(Type::Unrecognized, MidiNote::C);
    }

    const auto t = recognizeType(outputChord, finalLength);
    return std::make_tuple(std::get<0>(t), (base + std::get<1>(t)) % 12);
}

std::tuple<ChordRecognizer::Type, int> ChordRecognizer::recognizeType(const int* chord, unsigned length) {
    assert(chord[0] == 0);
    if ((length == 3) && (chord[2] == MidiNote::G)) {
        return recognizeType3WithFifth(chord);
    }

    // This is what an triad in first inversion looks like. strang, but probably correct
    if ((length == 3) &&
        (chord[0] == MidiNote::C) &&
        (chord[1] == MidiNote::E - 1) &&
        (chord[2] == MidiNote::G + 1)) {
        return std::make_tuple(Type::MajorTriadFirstInversion, -4);
    }

    return std::make_tuple(Type::Unrecognized, 0);
}

std::tuple<ChordRecognizer::Type, int> ChordRecognizer::recognizeType3WithFifth(const int* chord) {
    assert(chord[0] == 0);
    assert(chord[2] == MidiNote::G);

    if (chord[1] == MidiNote::E) {
        return std::make_tuple(Type::MajorTriad, 0);
    }
    if (chord[1] == MidiNote::E -1) {
        return std::make_tuple(Type::MinorTriad, 0);
    }

     return std::make_tuple(Type::Unrecognized, 0);
}

std::string ChordRecognizer::toString(const ChordInfo& info) {
    std::string s = PitchKnowledge::nameOfAbs(std::get<1>(info));
    std::string sType;
    switch (std::get<0>(info)) {
        case ChordRecognizer::Type::Unrecognized:
            return "";
        case ChordRecognizer::Type::MajorTriad:
            sType = "Major Triad";
            break;
        case ChordRecognizer::Type::MinorTriad:
            sType = "Minor Triad";
            break;
        case ChordRecognizer::Type::MajorTriadFirstInversion:
            sType = "Major Triad, first inversion";
            break;
    }
    return s + " " + sType;
}