
#include "ChordRecognizer.h"

#include "PitchKnowledge.h"
#include "SqLog.h"

//#define _LOG

#if 0
void ChordRecognizer::_show(const char* msg, const SqArray<int, 16>& inputChord) {
    const unsigned num = inputChord.numValid();
    if (num == 3) {
        SQINFO("%s = %d, %d, %d", msg, inputChord.getAt(0), inputChord.getAt(1), inputChord.getAt(2));
    } else if (num == 4) {
        SQINFO("%s = %d, %d, %d, %d", msg, inputChord.getAt(0), inputChord.getAt(1), inputChord.getAt(2), inputChord.getAt(3));
    } else if (num == 5) {
        SQINFO("%s = %d, %d, %d, %d %d", msg, inputChord.getAt(0), inputChord.getAt(1), inputChord.getAt(2), inputChord.getAt(3), inputChord.getAt(4));
    }else {
        SQINFO("??? num=%d", num);
    }
}
#endif

#if 0
 void ChordRecognizer::_copy(SqArray<int, 16>& outputChord, const SqArray<int, 16>& inputChord) {
    for (unsigned i=0; i<inputChord.numValid(); ++i) {
        outputChord.putAt(i, inputChord.getAt(i));
    }
}
#endif

// return is how much it was transposed
#if 0
bool ChordRecognizer::_makeCanonical(SqArray<int, 16>& outputChord, const SqArray<int, 16>& inputChord, int& transposeAmount) {
#if defined _LOG
    SQINFO("In normalize %d", inputChord.numValid());
    _show("input chord", inputChord);
#endif

    unsigned length = inputChord.numValid();

    if (length < 1) {
#ifdef _LOG
        SQINFO("In recognize exit early");
#endif
        return false;
    }

    SqArray<int, 16> sortedChord;
    _copy(sortedChord, inputChord);
    std::sort(sortedChord.getDirectPtrAt(0), sortedChord.getDirectPtrAt(length));

#ifdef _LOG
    _show("sorted chord", sortedChord);
#endif
    const int base = sortedChord.getAt(0);

#ifdef _LOG
    SQINFO("... in normalize pass, adding %d", -base);
#endif
    unsigned i;
    SqArray<int, 16> normalizedChord;
    for (i = 0; i < length; ++i) {
        int note = sortedChord.getAt(i) - base;  // normalize to base
        note = note % 12;                  // normalize to octave
        normalizedChord.putAt(i, note);
    }

#ifdef _LOG
    _show("normalizedChord", normalizedChord);
#endif
    std::sort(normalizedChord.getDirectPtrAt(0), normalizedChord.getDirectPtrAt(length));
#ifdef _LOG
    _show("sorted normalizedChord", normalizedChord);
#endif

    //--- Remove dupes ---
    unsigned j;
    for (i = j = 0; i < length; ++i) {
        // Copy this one over if it is not a dupe. last one is never a dupe.
        if ((i == (length-1)) || (normalizedChord.getAt(i) != normalizedChord.getAt(i + 1))) {
            outputChord.putAt(j++, normalizedChord.getAt(i)); 
        }
    }
    length = j;

#ifdef _LOG
    _show("final", outputChord);
#endif
    for (unsigned i = 0; i < length; ++i) {
        assert(outputChord.getAt(i) >= 0);
    }
    transposeAmount = base;
    return true;
}
#endif

ChordRecognizer::ChordInfo ChordRecognizer::recognize(const SqArray<int, 16>& inputChord) {
#if defined(_LOG)
    SQINFO("----------------- enter recognize ------------------");
    _show("input chord ", inputChord);
#endif
    SqArray<int, 16> outputChord;
    const auto error = std::make_tuple(Type::Unrecognized, Inversion::Root, MidiNote::C);
    int transposeAmount = 0;
    const int canNormalize = _makeCanonical(outputChord, inputChord, transposeAmount);
    const unsigned finalLength = outputChord.numValid();
    const int baseNonInverted = transposeAmount;
    if (!canNormalize) {
#ifdef _LOG
        SQINFO("normalize failed");
#endif
        return error;
    }

#ifdef _LOG
    _show("final processed chord", outputChord);
#endif

    const auto nonInvertedRecognize = recognizeType(outputChord);
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
        // Try knocking a note down an octave to look for inversions.
        const int delta = -12;
        SqArray<int, 16> possibleInversion;
        SqArray<int, 16>  possibleInversionCanonical;

        _copy(possibleInversion, outputChord);
        possibleInversion.putAt(i, possibleInversion.getAt(i) + delta);
#ifdef _LOG
        _show(":: in inv search, pre norm", possibleInversion);
#endif
        int basePossibleInversion = 0;
        _makeCanonical(possibleInversionCanonical, possibleInversion, basePossibleInversion);
#ifdef _LOG
        _show(":: in inv search, post norm", possibleInversionCanonical);
#endif
        const unsigned l = possibleInversionCanonical.numValid();

        if (l != finalLength) {
            SQINFO("length changed in normalize was %d, now %d", finalLength, l);
            _show("here is norm output", possibleInversionCanonical);
            if (l == 1) {
                SQINFO("one note chord is %d", possibleInversionCanonical.getAt(0));
            }
        }

        // make this not an error - we might have errored out of _makeCanonical
        assert(l == finalLength);  // should not have changed length due to this
        const auto t = recognizeType(possibleInversionCanonical);
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
    const int effectiveFirstOffset = _firstOffset % 12;
    const int finalRootPitch = (_recognizedPitch + effectiveFirstOffset) % 12;

    const int relativeEffectiveFirstOffset = effectiveFirstOffset - finalRootPitch;

    // TODO: the compares, below should normRootPitch == effectiveFirstOffset - finalRootPitch?
    // Maybe have to normalize that to keep >= 0?
    assert(effectiveFirstOffset < 12);
    assert(effectiveFirstOffset >= 0);
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

std::tuple<ChordRecognizer::Type, int> ChordRecognizer::recognizeType( const SqArray<int, 16>& chord) {
    assert(chord.getAt(0) == 0);
    const unsigned length = chord.numValid();
    if (length == 3) {
        if (chord.getAt(2) == MidiNote::G) {
            return recognizeType3WithFifth(chord);
        } else if (chord.getAt(2) == MidiNote::G - 1) {
            return recognizeType3WithTritone(chord);
        } else if (chord.getAt(2) == MidiNote::G + 1) {
            return recognizeType3WithAugFifth(chord);
        }
    }

    if (length == 4) {
        const auto ret = recognizeType7th(chord);
        if (std::get<0>(ret) != Type::Unrecognized) {
            return ret;
        }
        return recognizeTypeNinthWithSuspendedFifth(chord);
       
    }

    if (length == 5) {
        return recognizeType9th(chord);
    }

    return std::make_tuple(Type::Unrecognized, 0);
}

std::tuple<ChordRecognizer::Type, int> ChordRecognizer::recognizeType7th(const SqArray<int, 16>& chord) {
#ifdef _LOG
    _show("enter recognizeType7th chord=", chord);
#endif
    const auto error = std::make_tuple(Type::Unrecognized, 0);
    assert(chord.getAt(0) == MidiNote::C);

    if (chord.getAt(2) != MidiNote::G) {
#ifdef _LOG
        SQINFO("not 7th, as doesn't have a perfect fifth");
#endif
        return error;
    }

    if ((chord.getAt(1) == MidiNote::E) &&
        (chord.getAt(3) == MidiNote::B - 1)) {
        return std::make_tuple(Type::MajMinSeventh, 0);
    }
    if ((chord.getAt(1) == MidiNote::E) &&
        (chord.getAt(3) == MidiNote::B)) {
        return std::make_tuple(Type::MajMajSeventh, 0);
    }
    if ((chord.getAt(1) == MidiNote::E - 1) &&
        (chord.getAt(3) == MidiNote::B - 1)) {
        return std::make_tuple(Type::MinMinSeventh, 0);
    }
    if ((chord.getAt(1) == MidiNote::E - 1) &&
        (chord.getAt(3) == MidiNote::B)) {
        return std::make_tuple(Type::MinMajSeventh, 0);
    }

    return error;
}

std::tuple<ChordRecognizer::Type, int> ChordRecognizer::recognizeType9th(const SqArray<int, 16>& chord) {
    // since chords are sorted, a dom ninth looks like 0 2 4 7 10. The ninth rolls over to the two
#ifdef _LOG
    _show("enter recognizeType9th chord=", chord);
#endif
    const auto error = std::make_tuple(Type::Unrecognized, 0);
    assert(chord.getAt(0) == MidiNote::C);

    if (chord.getAt(3) != MidiNote::G) {
#ifdef _LOG
        SQINFO("not 9th, as doesn't have a perfect fifth");
#endif
        return error;
    }

    if (chord.getAt(1) != (MidiNote::D)) {
#ifdef _LOG
        SQINFO("not 9th, as doesn't have a ninth (second)");
#endif
    return error;
    }

    if ((chord.getAt(2) == MidiNote::E) &&
        (chord.getAt(4) == MidiNote::B - 1)) {
        return std::make_tuple(Type::MajMinNinth, 0);
    }
    if ((chord.getAt(2) == MidiNote::E) &&
        (chord.getAt(4) == MidiNote::B)) {
        return std::make_tuple(Type::MajMajNinth, 0);
    }
    if ((chord.getAt(2) == MidiNote::E - 1) &&
        (chord.getAt(4) == MidiNote::B - 1)) {
        return std::make_tuple(Type::MinMinNinth, 0);
    }
    if ((chord.getAt(2) == MidiNote::E - 1) &&
        (chord.getAt(4) == MidiNote::B)) {
        return std::make_tuple(Type::MinMajNinth, 0);
    }

    return error;
}

std::tuple<ChordRecognizer::Type, int> ChordRecognizer::recognizeTypeNinthWithSuspendedFifth(const SqArray<int, 16>& chord) {
    // since chords are sorted, a dom ninth looks like 0 2 4 10. The ninth rolls over to the two
#ifdef _LOG
    _show("enter recognizeType9th chord=", chord);
#endif
    const auto error = std::make_tuple(Type::Unrecognized, 0);
    assert(chord.getAt(0) == MidiNote::C);


    if (chord.getAt(1) != (MidiNote::D)) {
#ifdef _LOG
        SQINFO("not 9th, as doesn't have a ninth (second)");
#endif
        return error;
    }

    if ((chord.getAt(2) == MidiNote::E) &&
        (chord.getAt(3) == MidiNote::B - 1)) {
        return std::make_tuple(Type::MajMinNinth, 0);
    }
    if ((chord.getAt(2) == MidiNote::E) &&
        (chord.getAt(3) == MidiNote::B)) {
        return std::make_tuple(Type::MajMajNinth, 0);
    }
    if ((chord.getAt(2) == MidiNote::E - 1) &&
        (chord.getAt(3) == MidiNote::B - 1)) {
        return std::make_tuple(Type::MinMinNinth, 0);
    }
    if ((chord.getAt(2) == MidiNote::E - 1) &&
        (chord.getAt(3) == MidiNote::B)) {
        return std::make_tuple(Type::MinMajNinth, 0);
    }

    return error;
}

std::tuple<ChordRecognizer::Type, int> ChordRecognizer::recognizeType3WithAugFifth(const SqArray<int, 16>& chord) {
    assert(chord.getAt(0) == 0);
    assert(chord.getAt(2) == MidiNote::G + 1);

    switch (chord.getAt(1)) {
        case MidiNote::E:
            return std::make_tuple(Type::AugmentedTriad, 0);
    }

    return std::make_tuple(Type::Unrecognized, 0);
}

std::tuple<ChordRecognizer::Type, int> ChordRecognizer::recognizeType3WithTritone(const SqArray<int, 16>& chord) {
    assert(chord.getAt(0) == 0);
    assert(chord.getAt(2) == MidiNote::G - 1);

    switch (chord.getAt(1)) {
        case MidiNote::E - 1:
            return std::make_tuple(Type::DiminishedTriad, 0);
    }

    return std::make_tuple(Type::Unrecognized, 0);
}

std::tuple<ChordRecognizer::Type, int> ChordRecognizer::recognizeType3WithFifth(const SqArray<int, 16>& chord) {
    assert(chord.getAt(0) == 0);
    assert(chord.getAt(2) == MidiNote::G);

    switch (chord.getAt(1)) {
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