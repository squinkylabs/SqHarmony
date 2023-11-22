#pragma once

#include <assert.h>

#include "KeysigOld.h"
#include "Options.h"
#include "Style.h"

class RawChordGenerator {
public:
    const static int chordSize = 4;
    friend class TestRawChordGenerator;

    /**
     * ctor will make one with all bad pitches. calling getNextChord 
     * will return the first one;
     */
    RawChordGenerator(const Options&);

    // returns true if can make one
    bool getNextChord();

    void getCurrentChord(int * theChord);
    
private:
    int _chord[chordSize] = {0};
    const Options _options;

    // Returns true on success. If false returned, chord will be invalid
    static bool getNextChordInRange(int* chord, const Style& style);
    static void fixCrossingIfRequired(int* chord, const Style& style);

    static void initialize(int* chord, const Style& style);
    static bool isChordOk(const int* chord, const Options&);
    static std::vector<ScaleRelativeNote> getSRN(const int* chord, const Options&);
    static bool allNotesInScale(const std::vector<ScaleRelativeNote>&);

    enum class VOICE_NAME { BASS,
                            TENOR,
                            ALTO,
                            SOP };
    static const int iBass = int(VOICE_NAME::BASS);
    static const int iTenor = int(VOICE_NAME::TENOR);
    static const int iAlto = int(VOICE_NAME::ALTO);
    static const int iSop = int(VOICE_NAME::SOP);
};

inline RawChordGenerator::RawChordGenerator(const Options& op) : _options(op)  {
    assert(_chord[iSop] == 0);
    assert(_chord[iBass] == 0);
}
inline bool RawChordGenerator::getNextChord() {
    for (bool done=false; !done; ) {
        bool b= getNextChordInRange(_chord, *_options.style);
        if (!b) {
            return false;
        }
        fixCrossingIfRequired(_chord, *_options.style);
        if (isChordOk(_chord, _options)) {
            //done = true;
            return true;
        } else {
            // TODO: we for sure need some more criteria here
            // assert(false);
        }
    }
    return false;

}

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

inline bool RawChordGenerator::isChordOk(const int* chord, const Options& options) {
    const auto scaleRelativeNotes = getSRN(chord, options);
    return allNotesInScale(scaleRelativeNotes);
}

inline std::vector<ScaleRelativeNote> RawChordGenerator::getSRN(const int* chord, const Options& options) {
    std::vector<ScaleRelativeNote> scaleRelativeNotes(4);
    for (int index = 0; index<chordSize; ++index) {
        const int pitch = chord[index];
        HarmonyNote hn(options);
        hn.setPitchDirectly(pitch);
        //const HarmonyNote xx;
       // const HarmonyNote& xx = hn;

        scaleRelativeNotes[index] = options.keysig->getScaleDeg(hn);
    }
    return scaleRelativeNotes;
}

inline bool RawChordGenerator::allNotesInScale(
    const std::vector<ScaleRelativeNote>& scaleRelativeNotes) {

    for (int i = iBass; i < iSop; ++i) {
        if (!scaleRelativeNotes[i].isValid()) {
            return false;
        }
    }
    return true;
}

inline void RawChordGenerator::getCurrentChord(int * theChord) {
     for (int i = iBass; i < iSop; ++i) {
        theChord[i] = this->_chord[i];
     }
}