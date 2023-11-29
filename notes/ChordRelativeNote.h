
#pragma once

class Chord4;

class ChordRelativeNote {
public:
    operator int() const {
        return pitch;
    }

private:
    int pitch;
    friend Chord4;  // So he can set us.
    friend class RawChordGenerator;
    void set(int);
};

inline void ChordRelativeNote::set(int nPitch) {
    pitch = nPitch;
}
