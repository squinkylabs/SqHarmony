#pragma once

#include <assert.h>

#include "Options.h"
#include "Style.h"

class RawChordGenerator {
public:
    const static int chordSize = 4;
    friend class TestRawChordGenerator;

private:
    // Returns true on success. If false returned, chord will be invalid
    static bool getNextChordInRange(int* chord, const Style& style);
    static void fixCrossingIfRequired(int* chord, const Style& style);

    static void initialize(int* chord, const Style& style);
    static bool isChordOk(const int* chord, const Options&);
    static std::vector<ScaleRelativeNote> getSRN(const int* chord, const Options&);
    static bool allNotesInScale(const Options&, const ScaleRelativeNote* );

    enum class VOICE_NAME { BASS,
                            TENOR,
                            ALTO,
                            SOP };
    static const int iBass = int(VOICE_NAME::BASS);
    static const int iTenor = int(VOICE_NAME::TENOR);
    static const int iAlto = int(VOICE_NAME::ALTO);
    static const int iSop = int(VOICE_NAME::SOP);
};

inline void RawChordGenerator::fixCrossingIfRequired(int* chord, const Style& style) {
    if (style.allowVoiceCrossing() || style.maxUnison() != 0) {
        assert(false);
        return;
    }

    for (int i = iBass; i < iSop; ++i) {
        if (chord[i + 1] <= chord[i]) {
            chord[i + 1] = chord[i] + 1;
        }
    }
}

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

    chord[iSop] = style.minSop();
    chord[iAlto] = style.minAlto();
    chord[iTenor]++;
    if (chord[iTenor] <= style.maxTenor()) {
        return true;
    }

    chord[iSop] = style.minSop();
    chord[iAlto] = style.minAlto();
    chord[iTenor] = style.minTenor();
    chord[iBass]++;
    if (chord[iBass] <= style.maxBass()) {
        return true;
    }
    return false;
}

inline void RawChordGenerator::initialize(int* chord, const Style& style) {
    chord[int(VOICE_NAME::SOP)] = style.minSop();
    chord[int(VOICE_NAME::ALTO)] = style.minAlto();
    chord[int(VOICE_NAME::TENOR)] = style.minTenor();
    chord[int(VOICE_NAME::BASS)] = style.minBass();
}

inline bool RawChordGenerator::isChordOk(const int* chord, const Options&) {
    return false;
}

inline std::vector<ScaleRelativeNote> RawChordGenerator::getSRN(const int* chord, const Options&) {
    return {ScaleRelativeNote(), ScaleRelativeNote(), ScaleRelativeNote(), ScaleRelativeNote()};
}