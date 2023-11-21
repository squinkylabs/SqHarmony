#pragma once

#include "Style.h"

#include <assert.h>

class RawChordGenerator {
public:
    const static int chordSize = 4;
    friend class TestRawChordGenerator;

private:
    // Returns true on success. If false returned, chord will be invalid
    static bool getNextChordInRange(int* chord, const Style& style);

    static void initialize(int* chord, const Style& style);

    enum class VOICE_NAME { BASS,
                            TENOR,
                            ALTO,
                            SOP };
    static const int iBass = int(VOICE_NAME::BASS);
    static const int iTenor = int(VOICE_NAME::TENOR);
    static const int iAlto = int(VOICE_NAME::ALTO);
    static const int iSop = int(VOICE_NAME::SOP);
};

inline bool RawChordGenerator::getNextChordInRange(int* chord, const Style& style) {
    chord[iSop]++;
    if (chord[iSop] <= style.maxSop()) {
        return true;
    }
    chord[iSop] = style.minSop();
    chord[iAlto]++;
    if (chord[iAlto] <= style.maxAlto()) {
        return true;
    }
    assert(false);
    return false;
}

inline void RawChordGenerator::initialize(int* chord, const Style& style) {
    chord[int(VOICE_NAME::SOP)] = style.minSop();
    chord[int(VOICE_NAME::ALTO)] = style.minAlto();
    chord[int(VOICE_NAME::TENOR)] = style.minTenor();
    chord[int(VOICE_NAME::BASS)] = style.minBass();
}