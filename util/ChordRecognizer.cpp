
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
    const auto error = std::make_tuple(0, 0);
    SQINFO("In normalize %d", length);
    show("input chord", inputChord, length);

    if (length < 1) {
        SQINFO("In recognize exit early");
        return error;
    }

    int sortedChord[16];
    copy(sortedChord, inputChord, length);
    std::sort(sortedChord, sortedChord + (length));

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
    for (unsigned  i = 0; i < length; ++i) {
        assert(outputChord[i] >= 0);
    }
    return std::make_tuple(length, base);
}

ChordRecognizer::ChordInfo ChordRecognizer::recognize(const int* inputChord, unsigned inputLength) {
    int outputChord[16];
    const auto error = std::make_tuple(Type::Unrecognized, Inversion::Root, MidiNote::C);
    const auto normalized = normalize(outputChord, inputChord, inputLength);
    const unsigned finalLength = std::get<0>(normalized);
    const int base = std::get<1>(normalized);
    if (finalLength == 0) {
        SQINFO("normalize failed");
        return error;
    }

    const auto t = recognizeType(outputChord, finalLength);
    const auto recognizedType = std::get<0>(t);
    if (recognizedType != Type::Unrecognized) {
        return std::make_tuple(recognizedType, Inversion::Root, (base + std::get<1>(t)) % 12);
    }

    SQINFO(" ");
    for (unsigned i = 0; i < finalLength; ++i) {
        SQINFO("+++ in loop, i=%d", i);
        // try knocking a note up an octave to look for inversions

        const int delta = -12;
     //   outputChord[i] += delta;
        int possibleInversion[16];
        copy(possibleInversion, outputChord, finalLength);
        possibleInversion[i] += delta;
        show("in inv search, pre norm", possibleInversion, finalLength);
        const auto normalized = normalize(possibleInversion, outputChord, finalLength);
        show("in inv search, post norm", possibleInversion, finalLength);
        const unsigned l = std::get<0>(normalized);
        assert(l == finalLength);  // should not have changed length due to this

        const auto t = recognizeType(possibleInversion, finalLength);
        const auto recognizedType = std::get<0>(t);
        if (recognizedType != Type::Unrecognized) {
            // here we found an inversion!
            SQINFO("found inversion at i=%d", i);
            const auto inversion = (i == 2) ? ChordRecognizer::Inversion::First : ChordRecognizer::Inversion::Second;
            const int offset = (inversion == ChordRecognizer::Inversion::First) ? -4 : -7;
            return std::make_tuple(recognizedType, inversion, (base + std::get<1>(t) + offset) % 12);
          //  assert(false);
        }
        outputChord[i] -= delta;
    }
    SQINFO("not recognized (yet)");
    return error;
}

std::tuple<ChordRecognizer::Type, int> ChordRecognizer::recognizeType(const int* chord, unsigned length) {
    assert(chord[0] == 0);
    if ((length == 3) && (chord[2] == MidiNote::G)) {
        return recognizeType3WithFifth(chord);
    }

    // This is what an triad in first inversion looks like. strange, but probably correct
    // if ((length == 3) &&
    //     (chord[0] == MidiNote::C) &&
    //     (chord[1] == MidiNote::E - 1) &&
    //     (chord[2] == MidiNote::G + 1)) {
    //     return std::make_tuple(Type::MajorTriadFirstInversion, -4);
    // }

    return std::make_tuple(Type::Unrecognized, 0);
}

std::tuple<ChordRecognizer::Type, int> ChordRecognizer::recognizeType3WithFifth(const int* chord) {
    assert(chord[0] == 0);
    assert(chord[2] == MidiNote::G);

    if (chord[1] == MidiNote::E) {
        return std::make_tuple(Type::MajorTriad, 0);
    }
    if (chord[1] == MidiNote::E - 1) {
        return std::make_tuple(Type::MinorTriad, 0);
    }

    return std::make_tuple(Type::Unrecognized, 0);
}

std::string ChordRecognizer::toString(const ChordInfo& info) {
    // std::string s = PitchKnowledge::nameOfAbs(pitchFromInfo(info));
    // std::string sType;
    // switch (std::get<0>(info)) {
    //     case ChordRecognizer::Type::Unrecognized:
    //         return "";
    //     case ChordRecognizer::Type::MajorTriad:
    //         sType = "Major Triad";
    //         break;
    //     case ChordRecognizer::Type::MinorTriad:
    //         sType = "Minor Triad";
    //         break;
    //     case ChordRecognizer::Type::MajorTriadFirstInversion:
    //         sType = "Major Triad, first inversion";
    //         break;
    // }
    // return s + " " + sType;
    return "foo";
}