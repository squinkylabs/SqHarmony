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

int PitchKnowledge::pitchFromName(const std::string& sInput) {
    assert(sInput.length() >= 2);
    assert(sInput.length() <= 4);
    const bool isSharp = sInput[1] == '#';

    const int noteNameLen = isSharp ? 2 : 1;
    const std::string noteName = sInput.substr(0, noteNameLen);
    int index = -1;
    for (int i = 1; i <= 12; ++i) {
        std::string s = names[i];
        if (s == noteName) {
            index = i;
            break;
        }
    }
    assert(index >= 0);

    const int octaveStartIndex = noteNameLen;
    int octave = -10;
    const int octNameLen = int(sInput.size()) - noteNameLen;
    if (octNameLen == 1) {
        octave = sInput[octaveStartIndex] - '0';
    } else {
        assert(sInput[octaveStartIndex] == '-');  // -x is the only "modificaton" of octave allowed.
        const int absOctave = sInput[octaveStartIndex + 1] - '0';
        octave = -absOctave;
    }

    assert(octave >= -2);
    assert(octave <= 8);

    int retVal = ((octave + 2) * 12 + (index - 1));

    return retVal;
}

char const* const PitchKnowledge::names[] = {
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
