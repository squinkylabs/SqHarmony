
#pragma once

class Chord4;

class ChordRelativeNote {
public:
    operator int() const{ return pitch; 
    }

private:
    int pitch;
    friend Chord4;  // so he can set us
    void set(int);  // I should be private!!!!!
};

inline void ChordRelativeNote::set(int nPitch) {
    pitch = nPitch;
}
