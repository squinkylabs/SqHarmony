
#include "Scale.h"

#include <assert.h>
#include <stdio.h>

void Scale::set(const MidiNote& bs, Scales md) {
    baseNote = bs;
    scale = md;
    wasSet = true;

    assert(baseNote.get() < 12 && baseNote.get() >= 0);
}

std::pair<const MidiNote, Scale::Scales> Scale::get() const {
    return std::make_pair(baseNote, scale);
}

ScaleNote Scale::m2s(const MidiNote& mn) const {
    int scaleBasePitch = baseNote.get() % 12;  // now C == 4;
    int inputBasePitch = mn.get() % 12;
    int offset = inputBasePitch - scaleBasePitch;

    int octave = mn.get() / 12;
    octave -= 2;

    while (offset < 0) {
        offset += 12;
        octave -= 1;
    }

    auto sn = makeScaleNote(offset);
    ScaleNote final(sn.getDegree(), octave, sn.getAccidental());
    // need octave into
    return final;
}

MidiNote Scale::s2m(const ScaleNote& scaleNote) const {
    const int semitones = degreeToSemitone(scaleNote.getDegree());

    int accidentalOffset = 0;
    switch (scaleNote.getAccidental()) {
        case ScaleNote::Accidental::sharp:
            accidentalOffset = 1;
            break;
        case ScaleNote::Accidental::flat:
            accidentalOffset = -1;
            break;
        case ScaleNote::Accidental::none:
            accidentalOffset = 0;
            break;
        default:
            assert(false);
    }
    MidiNote scaleBase = this->base();
    assert(scaleBase.get() < 12 && scaleBase.get() >= 0);
    const int midiPitch = accidentalOffset + scaleBase.get() + semitones + (scaleNote.getOctave() + 2) * 12;  // 60 = c3 in midi;
    return MidiNote(midiPitch);
}

ScaleNote Scale::makeScaleNote(int offset) const {
    assert(offset >= 0 && offset < 12);
    int degree = quantizeInScale(offset);
    if (degree >= 0) {
        return ScaleNote(degree, 0);
    }
    degree = quantizeInScale(offset - 1);
    if (degree >= 0) {
        return ScaleNote(degree, 0, ScaleNote::Accidental::sharp);
    }
    degree = quantizeInScale(offset + 1);
    if (degree >= 0) {
        return ScaleNote(degree, 0, ScaleNote::Accidental::flat);
    }
    assert(false);
    return ScaleNote();
}

std::vector<std::string> Scale::getShortScaleLabels(bool justDiatonic) {
    assert(justDiatonic);
    return {
        "Major",
        "Dorian",
        "Phrygian",
        "Lydian",
        "Mixo.",
        "Minor",
        "Locrian"};
}

std::vector<std::string>
Scale::getScaleLabels(bool justDiatonic) {
    if (justDiatonic)
        return {
            "Ionian (Major)",
            "Dorian",
            "Phrygian",
            "Lydian",
            "Mixolydian",
            "Aeolean (Minor)",
            "Locrian"};
    else
        return {"Major", "Dorian", "Phrygian", "Lydian", "Mixolydian", "Minor", "Locrian",
                "Minor Pentatonic", "Harmonic Minor", "Diminished", "Dom. Diminished", "Whole Tone"};
}

std::vector<std::string> Scale::getRootLabels() {
    return {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
}

int Scale::quantize(int offset) const {
    assert(wasSet);
    assert(offset >= 0 && offset <= 11);
    int x = quantizeInScale(offset);
    if (x >= 0) {
        return x;
    }

    // get the scale degrees for above and below
    //  const int* pitches = getNormalizedScalePitches();
    const int qUpOne = quantizeInScale(offset + 1);
    const int qDnOne = quantizeInScale(offset - 1);

    switch (offset) {
        case 1:
            // if minor second, try major second
            if (qUpOne >= 0) return qUpOne;
            // if no seconds, go to unison (pentatonic)
            if (qDnOne >= 0) return qDnOne;
            break;
        case 2:
            // if major second, try minor second
            if (qDnOne >= 0) return qDnOne;
            // if no seconds, try m3 (pentatonic)
            if (qUpOne >= 0) return qUpOne;
            break;
        case 3:
            // if minor third, try major
            if (qUpOne >= 0) return qUpOne;
            break;
        case 4:
            // if major third, try minor
            if (qDnOne >= 0) return qDnOne;
            break;
        case 6:
            // if triton, try P 5th
            if (qUpOne >= 0) return qUpOne;
            break;
        case 8:
            // aug 5th -> down to 5th
            if (qDnOne >= 0) return qDnOne;
            break;
        case 9:
            // maj 6, try minor
            if (qDnOne >= 0) return qDnOne;
            // maj 6, try minor 7 (pentatonic)
            if (qUpOne >= 0) return qUpOne;
            break;
        case 10:
            // dim 7th -> 7th
            if (qUpOne >= 0) return qUpOne;
            break;
        case 11:
            // M7 -> m7
            if (qDnOne >= 0) return qDnOne;
            break;
        default:
            assert(false);
    }
    assert(false);
    return 0;
}

int Scale::quantizeInScale(int offset) const {
    const int* pitches = getNormalizedScalePitches();
    int degreeIndex = 0;
    for (bool done = false; !done;) {
        if (pitches[degreeIndex] < 0) {
            // reached end of list
            degreeIndex = -1;  // not in scale
            done = true;
        } else if (pitches[degreeIndex] == offset) {
            done = true;
        } else {
            ++degreeIndex;
        }
    }
    return degreeIndex;
}

int Scale::degreeToSemitone(int degree) const {
    const int* pitches = getNormalizedScalePitches();
    int degreeIndex = 0;
    for (bool done = false; !done;) {
        if (*pitches < 0) {
            // reached end of list

            done = true;
            assert(false);
        } else if (degreeIndex == degree) {
            done = true;
        } else {
            ++degreeIndex;
        }
    }
    return pitches[degreeIndex];
}

const int* Scale::getNormalizedScalePitches() const {
    assert(wasSet);
    switch (scale) {
        case Scales::Major: {
            static const int ret[] = {0, 2, 4, 5, 7, 9, 11, -1};
            return ret;
        } break;
        case Scales::Minor: {
            static const int ret[] = {0, 2, 3, 5, 7, 8, 10, -1};
            return ret;
        } break;
        case Scales::Phrygian: {
            static const int ret[] = {0, 1, 3, 5, 7, 8, 10, -1};
            return ret;
        } break;
        case Scales::Mixolydian: {
            static const int ret[] = {0, 2, 4, 5, 7, 9, 10, -1};
            return ret;
        } break;
        case Scales::Locrian: {
            static const int ret[] = {0, 1, 3, 5, 6, 8, 10, -1};
            return ret;
        } break;

        case Scales::Lydian: {
            static const int ret[] = {0, 2, 4, 6, 7, 9, 11, -1};
            return ret;
        } break;
        case Scales::Dorian: {
            static const int ret[] = {0, 2, 3, 5, 7, 9, 10, -1};
            return ret;
        } break;
        case Scales::MinorPentatonic: {
            static const int ret[] = {0, 3, 5, 7, 10, -1};
            return ret;
        } break;
        case Scales::HarmonicMinor: {
            static const int ret[] = {0, 2, 3, 5, 7, 8, 11, -1};
            return ret;
        }

        break;
        case Scales::Diminished: {
            static const int ret[] = {0, 2, 3, 5, 6, 8, 9, 11, -1};
            return ret;
        } break;
        case Scales::DominantDiminished: {
            static const int ret[] = {0, 1, 3, 4, 6, 7, 9, 10, -1};
            return ret;
        } break;
        case Scales::WholeStep: {
            static const int ret[] = {0, 2, 4, 6, 8, 10, -1};
            return ret;
        } break;
        case Scales::Chromatic: {
            static const int ret[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, -1};
            return ret;
        }
        default: {
            assert(false);
            static const int ret[] = {-1};
            return ret;
        }
    }
}

/*
    class ScoreInfo {
    public:
        int numSharps = 0;
        int numFlats = 0;
        const MidiNote* sharpsInTrebleClef = nullptr;
        const MidiNote* sharpsInBassClef = nullptr;
        const MidiNote* flatsInTrebleClef = nullptr;
        const MidiNote* flatsInBassClef = nullptr;
    };
    */

/*
using MNP = const MidiNote*;
MNP xx[] = {
//const MidiNote* [] xx = {
    &MidiNote(6)
};
*/

MidiNote Scale::getRelativeMajor() const {
    int pitch = baseNote.get();
    switch (scale) {
        case Scales::Major:
            break;
 
        case Scales::Dorian:
        case Scales::Phrygian:
        case Scales::Lydian:
        case Scales::Mixolydian:
            assert(false);
        case Scales::Minor:
            pitch += 3;
            break;
        case Scales::Locrian:
 
        default:
            assert(false);
    }
    if (pitch >= 12) {
        pitch -= 12;
    }
    return MidiNote(pitch);
   
}

const int numsharps[12] = {
    0,  // C Maj
    0,  // C#
    0,  // D
    0,  // D#
    0,  // E
    0,  // F
    0,  // F#
    1,  // G
    0,  // G#
    0,  // A
    0,  // A#
    0   // B
};

const int numflats[12] = {
    0,  // C Maj
    0,  // C#
    0,  // D
    3,  // D# / E flag
    0,  // E
    0,  // F
    0,  // F#
    0,  // G
    0,  // G#
    0,  // A
    0,  // A#
    0   // B
};

const MidiNote sharpsInTreble[12] = {
    MidiNote(MidiNote::MiddleC + 12 + 5)  // first sharp on F (F#)
};

const MidiNote flatsInTreble[12] = {
    MidiNote(MidiNote::MiddleC + 12 - 1),  // first flat is on B (b flat)
    MidiNote(MidiNote::MiddleC + 12 + 4),  // second flat is on E (e flat)
    MidiNote(MidiNote::MiddleC + 12 - 3)   // second flat is on A (a flat)

};

const MidiNote sharpsInBass[12] = {
    MidiNote(MidiNote::MiddleC - 12 + 5)  // first sharp F#
};

const MidiNote flatsInBass[12] = {
    MidiNote(MidiNote::MiddleC - 12 - 1),
    MidiNote(MidiNote::MiddleC - 12 + 4),
    MidiNote(MidiNote::MiddleC - 12 - 3)};

Scale::ScoreInfo Scale::getScoreInfo() const {
    Scale::ScoreInfo ret;
    assert(int(scale) <= int(Scales::Locrian));

    const int basePitch = getRelativeMajor().get();
    assert(basePitch >= 0);
    assert(basePitch < 12);

    ret.numSharps = numsharps[basePitch];
    ret.numFlats = numflats[basePitch];
    ret.sharpsInTrebleClef = sharpsInTreble;
    ret.sharpsInBassClef = sharpsInBass;
    ret.flatsInTrebleClef = flatsInTreble;
    ret.flatsInBassClef = flatsInBass;

    return ret;
}