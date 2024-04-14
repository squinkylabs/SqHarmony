
#include "Scale.h"

#include <assert.h>
#include <stdio.h>

#include "SqLog.h"

void Scale::set(const MidiNote& bs, Scales md) {
    _baseNote = bs;
    _scale = md;
    _wasSet = true;

    assert(_baseNote.get() < 12 && _baseNote.get() >= 0);
}

std::pair<const MidiNote, Scale::Scales> Scale::get() const {
    return std::make_pair(_baseNote, _scale);
}

ScaleNote Scale::m2s(const MidiNote& mn) const {
    int scaleBasePitch = _baseNote.get() % 12;  // now C == 4;
    int inputBasePitch = mn.get() % 12;
    int offset = inputBasePitch - scaleBasePitch;

    int octave = mn.get() / 12;
    octave -= 2;

    while (offset < 0) {
        offset += 12;
        octave -= 1;
    }

    auto sn = _makeScaleNote(offset);
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

ScaleNote Scale::_makeScaleNote(int offset) const {
    assert(offset >= 0 && offset < 12);
    int degree = _quantizeInScale(offset);
    if (degree >= 0) {
        return ScaleNote(degree, 0);
    }
    degree = _quantizeInScale(offset - 1);
    if (degree >= 0) {
        return ScaleNote(degree, 0, ScaleNote::Accidental::sharp);
    }
    degree = _quantizeInScale(offset + 1);
    if (degree >= 0) {
        return ScaleNote(degree, 0, ScaleNote::Accidental::flat);
    }
    assert(false);
    return ScaleNote();
}

std::vector<std::string> Scale::getShortScaleLabels(bool justDiatonic) {
    if (justDiatonic) {
        return {
            "Major",
            "Dorian",
            "Phrygian",
            "Lydian",
            "Mixo.",
            "Minor",
            "Locrian"};
    } else {
        return {
            "Major",
            "Dorian",
            "Phrygian",
            "Lydian",
            "Mixo.",
            "Minor",
            "Locrian",
            "M Penta", "m Penta", "H Minor", "Dimin", "Dm Dim", "Whole T", "Chrom"};
    }
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
        return {"Ionian (Major)", "Dorian", "Phrygian", "Lydian", "Mixolydian", "Aeolean (Minor)", "Locrian",
                "Major Pentatonic", "Minor Pentatonic", "Harmonic Minor", "Diminished", "Dom. Diminished", "Whole Tone", "Chromatic"};
}

std::vector<std::string> Scale::getRootLabels(bool useFlats) {
    // SQINFO("getting root labels useF=%d", useFlats);
    if (!useFlats) {
        // SQINFO("returning the shrp labels");
        return {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
    } else {
        // SQINFO("returning the flats labels");
        return {"C", "D-", "D", "E-", "E", "F", "G-", "G", "A-", "A", "B-", "B"};
    }
}

int Scale::quantize(int offset) const {
    assert(_wasSet);
    assert(offset >= 0 && offset <= 11);
    int x = _quantizeInScale(offset);
    if (x >= 0) {
        return x;
    }

    // get the scale degrees for above and below
    //  const int* pitches = getNormalizedScalePitches();
    int qUpOne = _quantizeInScale(offset + 1);
    const int qDnOne = _quantizeInScale(offset - 1);

    if (offset == 11 && qUpOne < 0 && qDnOne < 0) {
      //  SQINFO("In bad case. offset == 11");
        qUpOne = 12;            // special case - we know we can go to octave here.
                                // seems to only happen with Major Pentatonic
    }

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
        case 5:
            // if it's a perfect fourth, but we don't have one in this scale
            // raise to tritone if we have it
            if (qUpOne >= 0) return qUpOne;
            // if no fourth and no tritone, low er it
            if (qDnOne >= 0) return qDnOne;
            break;
        case 6:
            // if triton, try P 5th
            if (qUpOne >= 0) return qUpOne;
            break;
        case 7:
            // p5 but we don't have a P5
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
            // If no 7th at all, try the major 6
            if (qDnOne >= 0) return qDnOne;
            break;
        case 11:
            // M7 -> m7
            if (qDnOne >= 0) return qDnOne;
            // If no 7th at all, go to octave
            if (qUpOne >= 0) return qUpOne;
            break;
        default:
            SQWARN("got a strange number in Scale::quantize: %d", offset);
            assert(false);
    }
    assert(false);
    return 0;
}

int Scale::_quantizeInScale(int offset) const {
    const int* pitches = _getNormalizedScalePitches();
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
    const int* pitches = _getNormalizedScalePitches();
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

const int* Scale::_getNormalizedScalePitches() const {
    assert(_wasSet);
    switch (_scale) {
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
        case Scales::MajorPentatonic: {
            static const int ret[] = { 0, 2, 4, 7, 9, -1 };
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

MidiNote Scale::getRelativeMajor() const {
    int pitch = _baseNote.get();
    switch (_scale) {
        case Scales::Major:
            break;

        case Scales::Dorian:
            pitch -= 2;
            break;
        case Scales::Phrygian:
            pitch -= 4;
            break;
        case Scales::Lydian:
            pitch -= 5;
            break;
        case Scales::Mixolydian:
            pitch += 5;
            break;
        case Scales::Minor:
            pitch += 3;
            break;
        case Scales::Locrian:
            pitch += 1;
            break;
        default:
            assert(false);
    }
    if (pitch >= 12) {
        pitch -= 12;
    }
    if (pitch < 0) {
        pitch += 12;
    }
    return MidiNote(pitch);
}

// how many sharps are in the Major scale based on these
const int numsharps[12] = {
    0,  // C Maj
    7,  // C#
    2,  // D
    0,  // D#
    4,  // E
    0,  // F
    6,  // F#
    1,  // G
    0,  // G#
    3,  // A
    0,  // A#
    5   // B
};

const int numflats[12] = {
    0,  // C Maj
    5,  // C# / D flat
    0,  // D
    3,  // D# / E flat
    0,  // E
    1,  // F
    6,  // F#
    0,  // G
    4,  // G#
    0,  // A
    2,  // B -
    7   // B
};

const Scale::SharpsFlatsPref preferSharps[12] = {
    Scale::SharpsFlatsPref::DontCare,  // C Maj (
    Scale::SharpsFlatsPref::Flats,     // C# / D flat (notated as D flat)
    Scale::SharpsFlatsPref::Sharps,    // D
    Scale::SharpsFlatsPref::Flats,     // D# / E flat
    Scale::SharpsFlatsPref::Sharps,    // E
    Scale::SharpsFlatsPref::Flats,     // F
    Scale::SharpsFlatsPref::DontCare,  // F# G flat (this one is ambiguous, could be either)
    Scale::SharpsFlatsPref::Sharps,    // G
    Scale::SharpsFlatsPref::Flats,     // G# / A flat
    Scale::SharpsFlatsPref::Sharps,    // A
    Scale::SharpsFlatsPref::Flats,     // A# / B -
    Scale::SharpsFlatsPref::Sharps     // B
};

const MidiNote sharpsInTreble[12] = {
    MidiNote(MidiNote::MiddleC + 12 + 5),  // first sharp on F (F#)
    MidiNote(MidiNote::MiddleC + 12),      // second sharp on C (C#)
    MidiNote(MidiNote::MiddleC + 12 + 7),  // third sharp on g (G#)
    MidiNote(MidiNote::MiddleC + 12 + 2),  // 4 sharp on D (D#)
    MidiNote(MidiNote::MiddleC + 9),       // 5 sharp on A (A#)
    MidiNote(MidiNote::MiddleC + 12 + 4),  // 6 sharp on E (E#)
    MidiNote(MidiNote::MiddleC + 11)       // 7 sharp on B (B#)
};

const MidiNote sharpsInBass[12] = {
    MidiNote(MidiNote::MiddleC - 12 + 5),  // first sharp F#
    MidiNote(MidiNote::MiddleC - 12),
    MidiNote(MidiNote::MiddleC - 12 + 7),
    MidiNote(MidiNote::MiddleC - 12 + 2),
    MidiNote(MidiNote::MiddleC - 24 + 9),
    MidiNote(MidiNote::MiddleC - 12 + 4),
    MidiNote(MidiNote::MiddleC - 24 + 11)};

const MidiNote flatsInTreble[12] = {
    MidiNote(MidiNote::MiddleC + 12 - 1),  // first flat is on B (b flat)
    MidiNote(MidiNote::MiddleC + 12 + 4),  // second flat is on E (e flat)
    MidiNote(MidiNote::MiddleC + 12 - 3),  // 3rh flat is on A (a flat)
    MidiNote(MidiNote::MiddleC + 12 + 2),  // 4th is D-
    MidiNote(MidiNote::MiddleC + 7),       // 5th is g
    MidiNote(MidiNote::MiddleC + 12),      // 6th is C-
    MidiNote(MidiNote::MiddleC + 5)        // 7th is f-
};

const MidiNote flatsInBass[12] = {
    MidiNote(MidiNote::MiddleC - 12 - 1),  // B-
    MidiNote(MidiNote::MiddleC - 12 + 4),  // E-
    MidiNote(MidiNote::MiddleC - 12 - 3),  // A-
    MidiNote(MidiNote::MiddleC - 12 + 2),  // D-
    MidiNote(MidiNote::MiddleC - 24 + 7),
    MidiNote(MidiNote::MiddleC - 12),
    MidiNote(MidiNote::MiddleC - 24 + 5 + 12)};

Scale::ScoreInfo Scale::getScoreInfo() const {
    Scale::ScoreInfo ret;
    assert(int(_scale) <= int(Scales::Locrian));

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

Scale::SharpsFlatsPref Scale::getSharpsFlatsPref() const {
    if (int(_scale) <= int(Scales::Locrian)) {
        const int basePitch = getRelativeMajor().get();
        assert(basePitch >= 0);
        assert(basePitch < 12);

        return preferSharps[basePitch];
    }

    switch (_scale) {
        case Scales::MinorPentatonic:
        case Scales::HarmonicMinor: {
            // Not sure it's true, but let's say all the minors are the same...
            Scale otherScale;
            otherScale.set(this->_baseNote, Scales::Minor);
            return otherScale.getSharpsFlatsPref();
        }
        case Scales::MajorPentatonic: {
            Scale otherScale;
            otherScale.set(this->_baseNote, Scales::Major);
            return otherScale.getSharpsFlatsPref();
        }
        case Scales::Chromatic:
        case Scales::Diminished:
        case Scales::DominantDiminished:
            return SharpsFlatsPref::DontCare;
        case Scales::WholeStep:
            return SharpsFlatsPref::Sharps;

        // These handled above. Just to make gcc not nag
        case Scales::Locrian:
        case Scales::Dorian:
        case Scales::Lydian:
        case Scales::Major:
        case Scales::Minor:
        case Scales::Mixolydian:
        case Scales::Phrygian:
            break;
    }
    assert(false);
    return SharpsFlatsPref::DontCare;
}

int Scale::numNotesInScale(Scales scale) {
    int ret = 0;
    switch (scale) {
        case Scales::MinorPentatonic:
        case Scales::MajorPentatonic:
            ret = 5;
            break;

        case Scales::HarmonicMinor:
            ret = 7;
            break;
        case Scales::Chromatic:
            ret = 12;
            break;
        case Scales::Diminished:
        case Scales::DominantDiminished:
            ret = 8;
            break;

        case Scales::WholeStep:
            ret = 6;
            break;

        // These handled above. Just to make gcc not nag
        case Scales::Locrian:
        case Scales::Dorian:
        case Scales::Lydian:
        case Scales::Major:
        case Scales::Minor:
        case Scales::Mixolydian:
        case Scales::Phrygian:
            ret = 7;
            break;
        default:
            assert(false);
            ret = -1;
    }
    assert(ret >= 0);
    assert(ret <= 12);
    return ret;
}

std::tuple<bool, MidiNote, Scale::Scales> Scale::convert(const Role* const noteRoles, bool diatonicOnly) {
    const auto error = std::make_tuple(false, MidiNote::C, Scale::Scales::Chromatic);
    int roleRoot = 0;
    // First check the role array for validity and find the root.
    {
        int roleCount = 0;
        int rootCount = 0;
        const Role* rp = noteRoles;
        
        bool foundRoot = false;
        while (*rp != Role::End) {
            if (*rp == Role::Root) {
                roleRoot = roleCount;
                foundRoot = true;
                rootCount++;
            }
            ++rp;
            ++roleCount;
        }

        if (!foundRoot) {
            return error;
        }
        if (roleCount != 12) {
            assert(false);
            return error;
        }
        if (rootCount != 1) {
            // Scale::_dumpRoles("more than one root", noteRoles);
            return error;
        }
    }

  //  const int lastValidScale = diatonicOnly ? Scale:: : 
    const int numScales = Scale::numScales(diatonicOnly);
    for (int mode = Scale::firstScale; mode < numScales; ++mode) {
        const auto smode = Scale::Scales(mode);
        if (_doesScaleMatch(noteRoles, smode, roleRoot)) {
            return {true, roleRoot, smode};
        }
    }
    return error;
}

void Scale::_dumpRoles(const char* message, const Role* roles) {
    SQINFO("%s", message);
    for (int i = 0; roles[i] != Role::End; ++i) {
        SQINFO("role[%d] = %d r=%d in =%d out=%d end=%d", i, int(roles[i]), int(Role::Root), int(Role::InScale), int(Role::NotInScale), int(Role::End));
    }
}

bool Scale::_doesScaleMatch(const Role* const rawRoles, Scales scale, MidiNote root) {
    Role rotatedRoles[13];  // Enough room for any 12 tone scale.
    Scale s;
    s.set(root, scale);

    // get the relative pitches for the scale (does not take into account root)
    const int* relativePitchesInScale = s._getNormalizedScalePitches();

    if (rawRoles[root.get()] != Role::Root) {
        // If the role root is not in the right place, then a) it's not a match,
        // and b) the logic farther down won't work.
        assert(false);
        return false;
    }

    // Now, rotate the roles so that the root is at the start
    for (int index = 0; index <= 12; ++index) {
        int destIndex = index - root.get();
        if (destIndex < 0) {
            destIndex += 12;
        }
        const Role r = rawRoles[index];
        if (r == Role::End) {
            destIndex = index;  // put the end in the same place - don't rotate it
        }
        rotatedRoles[destIndex] = r;
    }
   
    {
        // a little sanity check.
        int roleCount = 0;
        const Role* rp = rotatedRoles;
        // int roleRoot = 0;
        while (*rp++ != Role::End) {
            ++roleCount;
        }
        assert(roleCount == 12);
    }

    int roleIndex = 0;
    int pitchIndex = 0;
    while (true) {
        const auto role = rotatedRoles[roleIndex];
        const auto pitch = relativePitchesInScale[pitchIndex];
        if (role == Role::End) {
            return (pitch < 0) ? true : false;  // if they both end, it's a match
        }
        if (pitch < 0) {  // if we ran out of pitches, done
            // Note: when scale has a minor 7th, we will get here here
            while (true) {
                ++roleIndex;
                const auto role = rotatedRoles[roleIndex];
                switch (role) {
                    case Role::End:
                        return true;  // No more active roles, so we ran out of notes and roles, ok.
                    case Role::NotInScale:
                        return true;
                    case Role::InScale:
                        return false;  // Still roles left over.
                    default:
                        assert(false);
                        return false;
                }
            }
        }

        const auto roleIn = (role == Role::InScale || role == Role::Root);
        // if the chromatic degree we are examining is in the scale
        if (roleIn) {
            if (roleIndex != pitch) {  // and we are looking note that degree
                return false;  // then no match
            }
            ++pitchIndex;  // if a match, look at the nextPitch.
        }
        ++roleIndex;  // and always look at the next role.
    }
}

const Scale::RoleArray Scale::convert(MidiNote root, Scales mode) {
    RoleArray roles;
    Scale scale;

    scale.set(root, mode);
    const auto norm = scale._getNormalizedScalePitches();

    for (int index = 0; norm[index] >= 0; ++index) {
        // This should be transposed to real pitch
        int pitch = norm[index] + root.get();
        if (pitch >= 12) {
            pitch -= 12;
        }
        roles.data[pitch] = Role::InScale;
    }
    

    // put in the root where it should be
     roles.data[root.get()] = Role::Root;
    return roles;
}