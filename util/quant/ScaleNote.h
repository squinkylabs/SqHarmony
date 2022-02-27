#pragma once

class ScaleNote {
public:
 enum class Accidental {
        none,
        sharp,
        flat
    };

    ScaleNote(int degree, int oct, Accidental a) : 
        scaleDegree(degree), octave(oct), accidental(a) {}
    ScaleNote(int degree, int oct) : scaleDegree(degree), octave(oct) {}
    ScaleNote() {}
    int getDegree() const { return scaleDegree; }
    int getOctave() const { return octave; }
    bool isAccidental() const { return accidental != Accidental::none; }
    Accidental getAccidental() const { return accidental; }
private:
    /**
     * 0 = root, 6 = 7th
     * strictly between 0 and 6 (fully normalized)
     */
    int scaleDegree = 0;
    int octave = 0;
    Accidental accidental = Accidental::none;
};