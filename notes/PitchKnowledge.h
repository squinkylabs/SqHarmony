#pragma once

#include <string>

class PitchKnowledge {
public:
    /**
     * @brief extract the chromatic pitch, minus octave;
     *
     * @param nPitch is MIDI pitch
     * @return int chromatic pitch, C == 1 .... B ==12
     */
    static int chromaticFromAbs(int nPitch);
    static int octaveFromAbs(int nPitch);
    static std::string nameOfAbs(int nPitch);        // converts midi pitch to Name (a-0, b-7, c-2)
    static std::string nameOfShort(int nPitch);     // no octave
    static int pitchFromName(const std::string& s);  // Opposite of nameOfAbs.
private:
    static char const* const names[13];
};
