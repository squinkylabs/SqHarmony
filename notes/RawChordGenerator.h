#pragma once

#include <assert.h>

#include "ChordRelativeNote.h"
#include "KeysigOld.h"
#include "Options.h"
#include "Style.h"

class RawChordGenerator {
public:
    const static int chordSize = 4;
    friend class TestX;

    /**
     * ctor will make one with all bad pitches. calling getNextChord
     * will return the first one;
     *
     * @param root is 1..7
     */
    RawChordGenerator(const Options&, int root);

    // returns true if can make one
    bool getNextChord();
    void getCurrentChord(int* theChord);

private:
    int _chord[chordSize] = {0};
    const Options& _options;
    const int _root;

    // Returns true on success. If false returned, chord will be invalid
    static bool getNextChordInRange(int* chord, const Style& style);
    static void fixCrossingIfRequired(int* chord, const Style& style);

    static void initialize(int* chord, const Style& style);
    static bool isChordOk(const int* chord, const Options&, int root);
    // static std::vector<ScaleRelativeNote> getSRN(const int* chord, const Options&);
    // static void getSRN(const int* chord, const Options&, ScaleRelativeNote* srnOutputBuffer);
    // static bool allNotesInScale(const ScaleRelativeNote*);
    static bool allNotesInChord(const int* chord, const Options&, int root);
    static ChordRelativeNote chordInterval(const Options& options, int root, int pitch);
    static bool isInChord(const Options& options, int root, int pitch);

    enum class VOICE_NAME { BASS,
                            TENOR,
                            ALTO,
                            SOP };
    static const int iBass = int(VOICE_NAME::BASS);
    static const int iTenor = int(VOICE_NAME::TENOR);
    static const int iAlto = int(VOICE_NAME::ALTO);
    static const int iSop = int(VOICE_NAME::SOP);
};

inline RawChordGenerator::RawChordGenerator(const Options& op, int root) : _options(op), _root(root) {
    assert(_chord[iSop] == 0);
    assert(_chord[iBass] == 0);
    initialize(_chord, *op.style);
}
inline bool RawChordGenerator::getNextChord() {
    for (bool done = false; !done;) {
        bool b = getNextChordInRange(_chord, *_options.style);
        if (!b) {
            return false;
        }
        assert(_chord[0] > 0);
        fixCrossingIfRequired(_chord, *_options.style);
        assert(_chord[0] > 0);
        if (isChordOk(_chord, _options, _root)) {
            // done = true;
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
    assert(chord[iBass] > 0);
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

inline bool RawChordGenerator::isChordOk(const int* chord, const Options& options, int root) {
    assert(chord[iSop] > 0);
    //   ScaleRelativeNote srn[4];
    //   getSRN(chord, options, srn);
    //  return allNotesInScale(srn);
    return allNotesInChord(chord, options, root);
}

// inline void RawChordGenerator::getSRN(const int* chord, const Options& options, ScaleRelativeNote* srnOutputBuffer) {
//     for (int index = 0; index < chordSize; ++index) {
//         const int pitch = chord[index];
//         HarmonyNote hn(options);
//         hn.setPitchDirectly(pitch);
//         srnOutputBuffer[index] = options.keysig->getScaleDeg(hn);
//     }
// }

// inline bool RawChordGenerator::allNotesInScale(
//     const ScaleRelativeNote* scaleRelativeNotes) {
//     for (int i = iBass; i <= iSop; ++i) {
//         if (!scaleRelativeNotes[i].isValid()) {
//             return false;
//         }
//     }
//     return true;
// }

inline ChordRelativeNote RawChordGenerator::chordInterval(const Options& options, int root, int pitch) {
     // static int dumb = -1;
    int nt = 0;  // assume bogus
    ChordRelativeNote ret;
    HarmonyNote hn(options);
    hn.setPitchDirectly(pitch);
    const ScaleRelativeNote srnN = options.keysig->getScaleDeg(hn);  // get scale degree
    if (srnN.isValid()) {
        nt = 1 + srnN - root;  // to go from scale rel to chord rel, just normalize to chord root
        if (nt <= 0) nt += 7;   // keep positive!
    }
    ret.set(nt);
    return ret;
}

inline bool RawChordGenerator::isInChord(const Options& options, int root, int pitch) {
    bool ret = false;

    const ChordRelativeNote crnX = chordInterval(options, root, pitch);
    switch (crnX) {
        case 1:
        case 3:
        case 5:
            ret = true;
    }

    return ret;
}

inline bool RawChordGenerator::allNotesInChord(const int* chord, const Options& options, int root) {
    for (int i = 0; i < 4; ++i) {
        if (!isInChord(options, root, chord[i])) {
            return false;
        }
    }
    return true;
}

inline void RawChordGenerator::getCurrentChord(int* theChord) {
    for (int i = iBass; i <= iSop; ++i) {
        assert(_chord[i] > 0);
        theChord[i] = _chord[i];
    }
    assert(theChord[iSop] >= _options.style->minSop());
    assert(theChord[iSop] <= _options.style->maxSop());

    assert(theChord[iBass] >= _options.style->minBass());
    assert(theChord[iBass] <= _options.style->maxBass());
}