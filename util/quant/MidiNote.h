#pragma once

#include <assert.h>
#include <algorithm>
#include <stdio.h>

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

    static const int C3 = 60;  // C3 is 60 in midi spec.
    static const int C = 0;
    static const int D = 2;
    static const int E = 4;
    static const int F = 5;
    static const int G = 7;
    static const int A = 9;
    static const int B = 11;
    
    static const int MiddleC = 60 + 12;

private:
    int _pitch = C3;
};

inline int MidiNote::getLegerLine(SharpsFlatsPref sharpsFlats, bool bassStaff) const {
    assert(sharpsFlats == SharpsFlatsPref::Sharps || sharpsFlats == SharpsFlatsPref::DontCare);
    return getLegerLine(bassStaff);
}

inline int MidiNote::getLegerLine(bool bassStaff) const {
    const int normalizedPitch = _pitch - MiddleC;
    int octave = (normalizedPitch / 12);
    int semi = normalizedPitch % 12;
    if (semi < 0) {
        semi += 12;
        octave -= 1;
    }
    int line = 0;
    switch (semi) {
        case 0:  // C, C#
        case 1:
            line = -2;
            break;
        case 2:  // D, D#
        case 3:
            line = -1;
            break;
        case 4:  // E
            line = 0;
            break;
        case 5:  // F, F#
        case 6:
            line = 1;
            break;
        case 7:  // G, G#
        case 8:
            line = 2;
            break;
        case 9:  // A, A#
        case 10:
            line = 3;
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
