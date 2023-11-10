/* pitch.cpp
 */

#include "PitchKnowledge.h"

#include <assert.h>

#include <sstream>

// All this junk should be added to note class.

int PitchKnowledge::chromaticFromAbs(int nPitch) {
    int chro;

    chro = nPitch % 12 + 1;  // 0 is C, etc...
    assert(chro > 0);
    return chro;
}

int PitchKnowledge::octaveFromAbs(int nPitch) {
    int oct;

    oct = nPitch / 12 + 1;
    return oct;
}

std::string PitchKnowledge::nameOfAbs(int nPitch) {
    const int chro = chromaticFromAbs(nPitch);
    const int oct = octaveFromAbs(nPitch);

    std::stringstream s;
    s << names[chro];
    s << (oct - 3);
    return s.str();
}

char const* const PitchKnowledge::names[] =
    {
        "X",  // no pitch zero in our world
        "C",
        "C#",
        "D",
        "D#",
        "E",
        "F",
        "F#",
        "G",
        "G#",
        "A",
        "A#",
        "B"};

PitchKnowledge ThePitchKnowledge;  // declare the global instance
