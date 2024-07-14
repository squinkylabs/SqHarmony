#pragma once

#include <assert.h>
#include <stdio.h>

#include <algorithm>
#include <tuple>

#include "SharpsFlatsPref.h"
#include "SqLog.h"

class MidiNote {
public:
    // Note: this constructor should probably be "explicit", but a lot of stuff breaks!
    explicit MidiNote(int p) : _pitch(p) {}
    MidiNote() : _pitch(64) {}

    int get() const { return _pitch; }

    bool operator==(const MidiNote& other) const {
        return this->_pitch == other._pitch;
    }

    int getLegerLine(bool bassStaff) const;
    int getLegerLine(SharpsFlatsPref sharpsFlats, bool bassStaff) const;

    bool isBlackKey() const;

    /**
     * @brief convert from music staff to midi pitch.
     *
     * @param bassStaff - true if bass, false if treble.
     * @param legerLine - line of the staff (or off of it), starting at the lowest (middle C is -2 in treble clef).
     * @param accidental - don't care means none here.
     * @return int - the midi pitch
     */
    //static int pitchFromLeger(bool bassStaff, int legerLine, SharpsFlatsPref accidental);
   // static int pitchFromLeger(bool bassStaff, int legerLine, NotationNote::accidental, const Scale& );

    static const int C3 = 60;  // C3 is 60 in midi spec.
    static const int C = 0;
    static const int D = 2;
    static const int E = 4;
    static const int F = 5;
    static const int G = 7;
    static const int A = 9;
    static const int B = 11;

    static const int MiddleC = 60 + 12;

    void _changePitch(int newPitch) {
        _pitch = newPitch;
    }
private:
    int _pitch = C3;
};

inline int MidiNote::getLegerLine(bool bassStaff) const {
    return getLegerLine(SharpsFlatsPref::Sharps, bassStaff);
}

#if 0
inline int MidiNote::pitchFromLeger(bool bassStaff, int legerLine, SharpsFlatsPref accidental) {
    if (bassStaff) {
        legerLine -= 3;  // push C to leger line zero in bass
    } else {
        legerLine += 2;  // push C to leger line zero in treble
    }
    unsigned pitch = 0;
    int octave = (legerLine / 7);
    int remainder = legerLine % 7;
    if (remainder < 0) {
        remainder += 7;
        octave--;
    }
    if (bassStaff) {
        octave--;
    }
    switch (remainder) {
        case 0:
            pitch = MidiNote::MiddleC + MidiNote::C;
            break;
        case 1:
            pitch = MidiNote::MiddleC + MidiNote::D;
            break;
        case 2:
            pitch = MidiNote::MiddleC + MidiNote::E;
            break;
        case 3:
            pitch = MidiNote::MiddleC + MidiNote::F;
            break;
        case 4:
            pitch = MidiNote::MiddleC + MidiNote::G;
            break;
        case 5:
            pitch = MidiNote::MiddleC + MidiNote::A;
            break;
        case 6:
            pitch = MidiNote::MiddleC + MidiNote::B;
            break;
        default:
            assert(false);  // case not implemented yet.
    }

    switch (accidental) {
        case SharpsFlatsPref::DontCare:
            break;
        case SharpsFlatsPref::Sharps:
            pitch++;
            break;
        case SharpsFlatsPref::Flats:
            pitch--;
            break;
        default:
            assert(false);
    }

    return pitch + octave * 12;
}
#endif

inline int MidiNote::getLegerLine(SharpsFlatsPref sharpsFlats, bool bassStaff) const {
    const int normalizedPitch = _pitch - MiddleC;
    int octave = (normalizedPitch / 12);
    int semi = normalizedPitch % 12;
    if (semi < 0) {
        semi += 12;
        octave -= 1;
    }
    int line = 0;
    const bool preferFlats = sharpsFlats == SharpsFlatsPref::Flats;
    switch (semi) {
        case 0:  // C
            line = -2;
            break;
        case 1:  // C#/D-
            line = preferFlats ? -1 : -2;
            // SQINFO("in MidiNote::getLEdgetLine semi=%d line=%d prefFlats = %d", semi, line, preferFlats);
            break;
        case 2:  // D
            line = -1;
            break;
        case 3:  // D# / E-
            line = preferFlats ? 0 : -1;
            break;
        case 4:  // E
            line = 0;
            break;
        case 5:  //
            line = 1;
            break;
        case 6:  // F# / G-
            line = preferFlats ? 2 : 1;
            break;
        case 7:  // G
            line = 2;
            break;
        case 8:  // G# / A-
            line = preferFlats ? 3 : 2;
            break;
        case 9:  // A
            line = 3;
            break;
        case 10:  // A# / B-
            line = preferFlats ? 4 : 3;
            break;
        case 11:  // B
            line = 4;
            break;
        default:
            assert(false);
    }
    line += (octave * 7);
    if (bassStaff) {
        line += 12;
    }
    // printf("MidiNote::Ll ret %d for pitch %d\n", line, pitch);
    return line;
}

inline bool MidiNote::isBlackKey() const {
    bool isBlackKey = false;
    int pitch = this->_pitch % 12;
    if (pitch < 0) {
        pitch += 12;
    }
    switch (pitch) {
        case 0:
        case 2:
        case 4:
        case 5:
        case 7:
        case 9:
        case 11:
            isBlackKey = false;
            break;
        case 1:
        case 3:
        case 6:
        case 8:
        case 10:
            isBlackKey = true;
            break;
        default:
            SQINFO("pitch is %d", pitch);
            assert(false);
    }

    return isBlackKey;
}
