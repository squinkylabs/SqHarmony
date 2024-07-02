
#include "ChordRecognizer.h"

#include "PitchKnowledge.h"
#include "SqLog.h"

#define _LOG

void ChordRecognizer::show(const char* msg, const int* p, unsigned num) {
    if (num == 3) {
        SQINFO("%s = %d, %d, %d", msg, p[0], p[1], p[2]);
    } else if (num == 4) {
        SQINFO("%s = %d, %d, %d, %d", msg, p[0], p[1], p[2], p[3]);
    } else if (num == 5) {
        SQINFO("%s = %d, %d, %d, %d %d", msg, p[0], p[1], p[2], p[3], p[4]);
    }else {
        SQINFO("??? num=%d", num);
    }
}

void ChordRecognizer::copy(int* dest, const int* src, unsigned length) {
    while (length--) {
        *dest++ = *src++;
    }
}

// returns 0 is the new length, return 1 is how much it was transposed
std::tuple<unsigned, int> ChordRecognizer::_makeCanonical(int* outputChord, const int* inputChord, unsigned length) {
    const auto error = std::make_tuple(0, 0);
#if defined _LOG
    SQINFO("In normalize %d", length);
    show("input chord", inputChord, length);
#endif

    if (length < 1) {
#ifdef _LOG
        SQINFO("In recognize exit early");
#endif
        return error;
    }

    int sortedChord[16];
    copy(sortedChord, inputChord, length);
    std::sort(sortedChord, sortedChord + (length));

#ifdef _LOG
    show("sorted chord", sortedChord, length);
#endif
    const int base = sortedChord[0];
    // if (base < 0) {
    //     // SQFATAL("!! logical error in norm !!");
    //     // show(" the bad sort was", sortedChord, length);
    //     // return error;
    //     SQINFO("was error - norm had so move pitch up. base=" + base);
    // }

#ifdef _LOG
    SQINFO("... in normalize pass, adding %d", -base);
#endif
    unsigned i;
    int normalizedChord[16];
    for (i = 0; i < length; ++i) {
        int note = sortedChord[i] - base;  // normalize to base
        note = note % 12;                  // normalize to octave
        normalizedChord[i] = note;
    }

#ifdef _LOG
    show("normalizedChord", normalizedChord, length);
#endif
    std::sort(normalizedChord, normalizedChord + (length));
#ifdef _LOG
    show("sorted normalizedChord", normalizedChord, length);
#endif

    //--- Remove dupes ---
    unsigned j;
    for (i = j = 0; i < length; ++i) {
        // Copy this one over if it is not a dupe. last one is never a dupe.
        if ((i == (length-1)) || (normalizedChord[i] != normalizedChord[i + 1])) {
            outputChord[j++] = normalizedChord[i]; 
        }
    }
    length = j;

#ifdef _LOG
    show("final", outputChord, length);
#endif
    for (unsigned i = 0; i < length; ++i) {
        assert(outputChord[i] >= 0);
    }
    return std::make_tuple(length, base);
}

ChordRecognizer::ChordInfo ChordRecognizer::recognize(const int* inputChord, unsigned inputLength) {
#if defined(_LOG)
    SQINFO("----------------- enter recognize ------------------");
    show("input chord ", inputChord, inputLength);
#endif
    int outputChord[16];
    const auto error = std::make_tuple(Type::Unrecognized, Inversion::Root, MidiNote::C);
    const auto normalized = _makeCanonical(outputChord, inputChord, inputLength);
    const unsigned finalLength = std::get<0>(normalized);
    const int baseNonInverted = std::get<1>(normalized);
    if (finalLength == 0) {
#ifdef _LOG
        SQINFO("normalize failed");
#endif
        return error;
    }

#ifdef _LOG
    show("final processed chord", outputChord, finalLength);
#endif

    const auto nonInvertedRecognize = recognizeType(outputChord, finalLength);
    const auto nonInvertedRecognizedType = std::get<0>(nonInvertedRecognize);
    if (nonInvertedRecognizedType != Type::Unrecognized) {
        const int finalRecognizedPitch = (baseNonInverted + std::get<1>(nonInvertedRecognize)) % 12;
        assert(finalRecognizedPitch >= 0);
        return std::make_tuple(nonInvertedRecognizedType, Inversion::Root, finalRecognizedPitch);
    }

#ifdef _LOG
    SQINFO(" ");
#endif
    for (unsigned i = 0; i < finalLength; ++i) {
#ifdef _LOG
        SQINFO(" ");
        SQINFO("+++ in loop, i=%d", i);
#endif
        // try knocking a note down an octave to look for inversions
        const int delta = -12;
        int possibleInversion[16];
        int possibleInversionCanonical[16];

        copy(possibleInversion, outputChord, finalLength);
        possibleInversion[i] += delta;
#ifdef _LOG
        show(":: in inv search, pre norm", possibleInversion, finalLength);
#endif
        const auto normalized = _makeCanonical(possibleInversionCanonical, possibleInversion, finalLength);
        const int basePossibleInversion = std::get<1>(normalized);
#ifdef _LOG
        show(":: in inv search, post norm", possibleInversionCanonical, finalLength);
#endif
        const unsigned l = std::get<0>(normalized);

        if (l != finalLength) {
            SQINFO("length changed in normalize was %d, now %d", finalLength, l);
            show("here is norm output", possibleInversionCanonical, l);
            if (l == 1) {
                SQINFO("one note chord is %d", possibleInversionCanonical[0]);
            }
        }


        // make this not an error - we might have errored out of _makeCacnonical
        assert(l == finalLength);  // should not have changed length due to this
        // if (l == 0) {
        //    #ifdef _LOG
        //     SQINFO("skipping bad normalization");
        //    #endif
        //     continue;

        // }

        const auto t = recognizeType(possibleInversionCanonical, finalLength);
        const auto recognizedType = std::get<0>(t);
        if (recognizedType != Type::Unrecognized) {
// here we found an inversion!
#ifdef _LOG
            SQINFO("found inversion at i=%d lengh=%d", i, finalLength);
#endif
            // SQINFO("c=%d", c);

            return figureOutInversion(recognizedType, basePossibleInversion, baseNonInverted);
        }
        // outputChord[i] -= delta;
    }
    return error;
}

ChordRecognizer::ChordInfo ChordRecognizer::figureOutInversion(Type _type, int _recognizedPitch, int _firstOffset) {
    Inversion inversion = Inversion::Root;
    //   int pitch = 0;
    //  Type type = Type::Unrecognized;

    const int effectiveFirstOffset = _firstOffset % 12;
    const int finalRootPitch = (_recognizedPitch + effectiveFirstOffset) % 12;

    const int relativeEffectiveFirstOffset = effectiveFirstOffset - finalRootPitch;

    // TODO: the compares, below should normRootPitch == effectiveFirstOffset - finalRootPitch?
    // Maybe have to normalize that to keep >= 0?

    // SQINFO("")

    //  firstOffset %= 12;
    assert(effectiveFirstOffset < 12);
    assert(effectiveFirstOffset >= 0);

    //  SQINFO("xneg recognized rem 12 = %d", (-_recognizedPitch) % 12);

    if (relativeEffectiveFirstOffset == 3 || relativeEffectiveFirstOffset == 4) {
        // If the lowest note is a third, then it's a first inversion.
        inversion = Inversion::First;
    }
    else if (relativeEffectiveFirstOffset == 7 || relativeEffectiveFirstOffset == 6) {
        // If the lowest note is a 5th, or a tritone, it's second inversions.
        // (tritone for diminished chords)
        inversion = Inversion::Second;
    } 
    else if (relativeEffectiveFirstOffset == 10 || relativeEffectiveFirstOffset == 11) {
        // If the lowest note is a 7th, it's third inversions.

        inversion = Inversion::Third;
    } else {
#ifdef _LOG
        assert(false);
        SQINFO("can't figure out inversion refo=%d", relativeEffectiveFirstOffset);
#endif
    }

    if (relativeEffectiveFirstOffset == 6) assert(_type == Type::DiminishedTriad);

#if defined(_LOG)
    SQINFO("leaving figure out inversion, with type= %d, recognized =%d, fistOffset= %d finalRootPitch=%d",
           int(_type), _recognizedPitch, _firstOffset, finalRootPitch);
    SQINFO("type=%d, inversion=%d, pitch = %d", int(_type), int(inversion), _recognizedPitch);
    SQINFO("relativeEffectiveFirstOffset=%d", relativeEffectiveFirstOffset);
#endif

    return std::make_tuple(_type, inversion, normalizeIntPositive(finalRootPitch, 12));
}

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

    if (length == 5) {
        return recognizeType9th(chord);
    }


    return std::make_tuple(Type::Unrecognized, 0);
}

std::tuple<ChordRecognizer::Type, int> ChordRecognizer::recognizeType7th(const int* chord) {
#ifdef _LOG
    show("enter recognizeType7th chord=", chord, 4);
#endif
    const auto error = std::make_tuple(Type::Unrecognized, 0);
    assert(chord[0] == MidiNote::C);

    if (chord[2] != MidiNote::G) {
#ifdef _LOG
        SQINFO("not 7th, as doesn't have a perfect fifth");
#endif
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
    if ((chord[1] == MidiNote::E - 1) &&
        (chord[3] == MidiNote::B - 1)) {
        return std::make_tuple(Type::MinMinSeventh, 0);
    }
    if ((chord[1] == MidiNote::E - 1) &&
        (chord[3] == MidiNote::B)) {
        return std::make_tuple(Type::MinMajSeventh, 0);
    }

    return error;
}

std::tuple<ChordRecognizer::Type, int> ChordRecognizer::recognizeType9th(const int* chord) {
    // since chords are sorted, a dom ninth looks like 0 2 4 7 10. The ninth rolls over to the two
#ifdef _LOG
    show("enter recognizeType9th chord=", chord, 5);
#endif
    const auto error = std::make_tuple(Type::Unrecognized, 0);
    assert(chord[0] == MidiNote::C);

    if (chord[3] != MidiNote::G) {
#ifdef _LOG
        SQINFO("not 9th, as doesn't have a perfect fifth");
#endif
        return error;
    }

    if (chord[1] != (MidiNote::D)) {
#ifdef _LOG
        SQINFO("not 9th, as doesn't have a ninth (second)");
#endif
    }

    if ((chord[2] == MidiNote::E) &&
        (chord[4] == MidiNote::B - 1)) {
        return std::make_tuple(Type::MajMinNinth, 0);
    }
    if ((chord[2] == MidiNote::E) &&
        (chord[4] == MidiNote::B)) {
        return std::make_tuple(Type::MajMajNinth, 0);
    }
    if ((chord[2] == MidiNote::E - 1) &&
        (chord[4] == MidiNote::B - 1)) {
        return std::make_tuple(Type::MinMinNinth, 0);
    }
    if ((chord[2] == MidiNote::E - 1) &&
        (chord[4] == MidiNote::B)) {
        return std::make_tuple(Type::MinMajNinth, 0);
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
            sType = "Major";
            break;
        case ChordRecognizer::Type::MinorTriad:
            sType = "minor";
            break;
        case ChordRecognizer::Type::Sus2Triad:
            sType = "Sus2";
            break;
        case ChordRecognizer::Type::Sus4Triad:
            sType = "Sus4";
            break;
        case ChordRecognizer::Type::AugmentedTriad:
            sType = "Augmented";
            break;
        case ChordRecognizer::Type::DiminishedTriad:
            sType = "Diminished";
            break;
        case Type::MajMinSeventh:
            sType = "Seventh";
            break;
        case Type::MajMinNinth:
            sType = "Ninth";
            break;
        case Type::MajMajSeventh:
            sType = "Major Seventh";
            break;
        case Type::MajMajNinth:
            sType = "Major Ninth";
            break;

        case Type::MinMinSeventh:
            sType = "minor Seventh";
            break;
        case Type::MinMinNinth:
            sType = "minor Ninth";
            break;
        case Type::MinMajSeventh:
            sType = "mM Seventh";
            break;
        case Type::MinMajNinth:
            sType = "mM Ninth";
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
        case Inversion::Third:
            sInversion = "Third Inversion";
            break;
        default:
            assert(false);
    }

    return {s + " " + sType, sInversion};
}

unsigned ChordRecognizer::notesInChord(Type type) {
    switch (type) {
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

int ChordRecognizer::normalizeIntPositive(int input, int rangeTop) {
    assert(rangeTop >= 0);

    const int rem = input % rangeTop;
    return (rem < 0) ? rem + rangeTop : rem;
}