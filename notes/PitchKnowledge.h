#pragma once

#include <string>

class PitchKnowledge {
public:
    static int chromaticFromAbs(int nPitch);  // converts midi pitch to a note name, c..b
                                              // 1 = C
    static int octaveFromAbs(int nPitch);

    static std::string nameOfAbs(int nPitch);  // converts midi pitch to Name (a, b, c)

private:
    static char const* const names[13];
};
