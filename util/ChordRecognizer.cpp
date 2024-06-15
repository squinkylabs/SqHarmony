
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
 //       assert(false);
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

    SQINFO("... in nomralize pass, adding %d", -base);
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
    for (unsigned i = 0; i < length; ++i) {
        assert(outputChord[i] >= 0);
    }
    return std::make_tuple(length, base);
}

ChordRecognizer::ChordInfo ChordRecognizer::recognize(const int* inputChord, unsigned inputLength) {
    SQINFO("----------------- enter recognize ------------------");
    int outputChord[16];
    const auto error = std::make_tuple(Type::Unrecognized, Inversion::Root, MidiNote::C);
    const auto normalized = normalize(outputChord, inputChord, inputLength);
    const unsigned finalLength = std::get<0>(normalized);
    const int baseNonInverted = std::get<1>(normalized);
    if (finalLength == 0) {
        SQINFO("normalize failed");
        return error;
    }

    const auto nonInvertedRecognize = recognizeType(outputChord, finalLength);
    const auto nonInvertedRecognizedType = std::get<0>(nonInvertedRecognize);
    if (nonInvertedRecognizedType != Type::Unrecognized) {
        return std::make_tuple(nonInvertedRecognizedType, Inversion::Root, (baseNonInverted + std::get<1>(nonInvertedRecognize)) % 12);
    }

    SQINFO(" ");
    for (unsigned i = 0; i < finalLength; ++i) {
         SQINFO(" ");
        SQINFO("+++ in loop, i=%d", i);
        // try knocking a note up an octave to look for inversions

        const int delta = -12;
        //   outputChord[i] += delta;
        int possibleInversion[16];
        copy(possibleInversion, outputChord, finalLength);
        possibleInversion[i] += delta;
        show("in inv search, pre norm", possibleInversion, finalLength);
        const auto normalized = normalize(possibleInversion, outputChord, finalLength);
        const int basePossibleInversion = std::get<1>(normalized);
        show("in inv search, post norm", possibleInversion, finalLength);
        const unsigned l = std::get<0>(normalized);
        assert(l == finalLength);  // should not have changed length due to this

        const auto t = recognizeType(possibleInversion, finalLength);
        const auto recognizedType = std::get<0>(t);
        if (recognizedType != Type::Unrecognized) {
            // here we found an inversion!
            SQINFO("found inversion at i=%d lengh=%d", i, finalLength);
            //SQINFO("c=%d", c);

            return figureOutInversion(recognizedType, basePossibleInversion, baseNonInverted );
           
        }
        outputChord[i] -= delta;
    }
    return error;
}

ChordRecognizer::ChordInfo ChordRecognizer::figureOutInversion(Type type, int recognizedPitch, int firstOffset) {
    Inversion inversion = Inversion::Root;
    int pitch = 0;
  //  Type type = Type::Unrecognized;
    SQINFO("called to figure out inversion, with type=%d, %d, %d", int(type), recognizedPitch, firstOffset);

    if (firstOffset == 3 || firstOffset == 4) {
        // If the lowest note is a third, then it's a first inversion.
        inversion = Inversion::First;
    } else if (firstOffset == 7) {
        inversion = Inversion::Second;
    } else {
        SQINFO("can't figure out inversion first offset=%d", firstOffset);
       // assert(false);
    }
#if 0
    if ((type == Type::MajorTriad) && (firstOffset == 4)) {
        inversion = Inversion::First;
    } else if (type == Type::MajorTriad && firstOffset == 7) {
        inversion = Inversion::Second;
    } else if (type == Type::MinorTriad && firstOffset == 3) {
        inversion = Inversion::First;
    } else if (type == Type::MajMinSeventh && firstOffset == 4) {
        inversion = Inversion::First;
    } else if (type == Type::MajMinSeventh && firstOffset == 7) {
        inversion = Inversion::Second;
    } else {
        assert(false);
    }
#endif

    pitch = (recognizedPitch + firstOffset) % 12;

    return std::make_tuple(type, inversion, pitch);

}

#if 0
  ChordRecognizer::Inversion inversion = ChordRecognizer::Inversion::First;
            if (finalLength == 3) {
                switch (i) {
                case 2:
                    inversion = ChordRecognizer::Inversion::First;
                    break;
                case 1:
                    inversion = ChordRecognizer::Inversion::Second;
                    break;
                default:
                    assert(false);
                }
            } else if (finalLength == 4) {
                switch (i) {
                case 3:
                    inversion = ChordRecognizer::Inversion::First;
                    break;
                default:
                    SQINFO("don't know inversion for 4 note where i=%d", i);
                }
            } else {
                SQINFO("can't find iversion of chords of this size");
            }
        //    const auto inversion = (i == 2) ? ChordRecognizer::Inversion::First : ChordRecognizer::Inversion::Second;

            const int baseSum = c + baseNonInverted;
           SQINFO(" base sum = %d mod=%d", baseSum, baseSum % 12);
            return std::make_tuple(recognizedType, inversion, baseSum % 12);
    #endif

std::tuple<ChordRecognizer::Type, int> ChordRecognizer::recognizeType(const int* chord, unsigned length) {
    assert(chord[0] == 0);
    if (length == 3) {
        if (chord[2] == MidiNote::G) {
            return recognizeType3WithFifth(chord);
        } else if (chord[2] == MidiNote::G - 1) {
            return recognizeType3WithTritone(chord);
        } else if (chord[2] == MidiNote::G + 1) {
            return recognizeType3WithAugFifth(chord);
        }
    }

    if (length == 4) {
        return recognizeType7th(chord);
    }

    return std::make_tuple(Type::Unrecognized, 0);
}

std::tuple<ChordRecognizer::Type, int> ChordRecognizer::recognizeType7th(const int* chord) {
    show("enter recognizeType7th chord=", chord, 4);
    const auto error = std::make_tuple(Type::Unrecognized, 0);
    assert(chord[0] == MidiNote::C);

    if (chord[2] != MidiNote::G) {
        SQINFO("not 7th, as doesn't have a perfect fifth");
        return error;
    }

    if ((chord[1] == MidiNote::E) &&
        (chord[3] == MidiNote::B - 1)) {
        return std::make_tuple(Type::MajMinSeventh, 0);
    }
    if ((chord[1] == MidiNote::E) &&
        (chord[3] == MidiNote::B)) {
        return std::make_tuple(Type::MajMajSeventh, 0);
    }
     if ((chord[1] == MidiNote::E -1) &&
        (chord[3] == MidiNote::B -1)) {
        return std::make_tuple(Type::MinMinSeventh, 0);
    }
      if ((chord[1] == MidiNote::E -1) &&
        (chord[3] == MidiNote::B)) {
        return std::make_tuple(Type::MinMajSeventh, 0);
    }


    return error;
}

std::tuple<ChordRecognizer::Type, int> ChordRecognizer::recognizeType3WithAugFifth(const int* chord) {
    assert(chord[0] == 0);
    assert(chord[2] == MidiNote::G + 1);

    switch (chord[1]) {
        case MidiNote::E:
            return std::make_tuple(Type::AugmentedTriad, 0);
    }

    return std::make_tuple(Type::Unrecognized, 0);
}

std::tuple<ChordRecognizer::Type, int> ChordRecognizer::recognizeType3WithTritone(const int* chord) {
    assert(chord[0] == 0);
    assert(chord[2] == MidiNote::G - 1);

    switch (chord[1]) {
        case MidiNote::E - 1:
            return std::make_tuple(Type::DiminishedTriad, 0);
    }

    return std::make_tuple(Type::Unrecognized, 0);
}

std::tuple<ChordRecognizer::Type, int> ChordRecognizer::recognizeType3WithFifth(const int* chord) {
    assert(chord[0] == 0);
    assert(chord[2] == MidiNote::G);

    switch (chord[1]) {
        case MidiNote::E:
            return std::make_tuple(Type::MajorTriad, 0);
        case MidiNote::E - 1:
            return std::make_tuple(Type::MinorTriad, 0);
        case MidiNote::F:
            return std::make_tuple(Type::Sus4Triad, 0);
        case MidiNote::D:
            return std::make_tuple(Type::Sus2Triad, 0);
    }

    return std::make_tuple(Type::Unrecognized, 0);
}

std::vector<std::string> ChordRecognizer::toString(const ChordInfo& info) {
    std::string s = PitchKnowledge::nameOfShort(pitchFromInfo(info));
    std::string sType;
    switch (typeFromInfo(info)) {
        case Type::Unrecognized:
            return {"", ""};
        case ChordRecognizer::Type::MajorTriad:
            sType = "Major Triad";
            break;
        case ChordRecognizer::Type::MinorTriad:
            sType = "Minor Triad";
            break;
        case ChordRecognizer::Type::Sus2Triad:
            sType = "Sus2 Triad";
            break;
        case ChordRecognizer::Type::Sus4Triad:
            sType = "Sus4 Triad";
            break;
        case ChordRecognizer::Type::AugmentedTriad:
            sType = "Aug Triad";
            break;
        case ChordRecognizer::Type::DiminishedTriad:
            sType = "Dim Triad";
            break;
        case Type::MajMinSeventh:
            sType = "Seventh";
            break;
         case Type::MajMajSeventh:
            sType = "MM Seventh";
            break;
         case Type::MinMinSeventh:
            sType = "mm Seventh";
            break;
         case Type::MinMajSeventh:
            sType = "mM Seventh";
            break;
        default:
            assert(false);
    }

    std::string sInversion;
    switch (inversionFromInfo(info)) {
        case Inversion::Root:
            sInversion = "";
            break;
        case Inversion::First:
            sInversion = "First Inversion";
            break;
        case Inversion::Second:
            sInversion = "Second Inversion";
            break;
        default:
            assert(false);
    }

    return {s + " " + sType, sInversion};
}

unsigned ChordRecognizer::notesInChord(Type type) {
    switch(type) {
        case Type::AugmentedTriad:
        case Type::DiminishedTriad:
        case Type::MajorTriad:
        case Type::MinorTriad:
        case Type::Sus2Triad:
        case Type::Sus4Triad:
            return 3;

        case Type::MajMajSeventh:
        case Type::MajMinSeventh:
        case Type::MinMajSeventh:
        case Type::MinMinSeventh:
            return 4;
        case Type::Unrecognized:
            return 0;
        default:
            assert(false);
    }
    return 100;
}