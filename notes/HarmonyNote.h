#pragma once

#include <string>

#include "Options.h"
#include "PitchKnowledge.h"
#include "Style.h"

/**
 * @brief wrapper around an absolute MIDI pitch
 *
 * As per the midi spec, 60 is C3
 * Note that C3 is an octave below 0 v in VCV spec.
 */
class HarmonyNote {
public:
    void setMin(const Options& option);
    HarmonyNote(const Options& option);  // Const: set to min pitch
    std::string tellPitchName() const;
    bool isTooHigh(const Options& options) const;
    operator int() const { return pitch; }
    int operator++() { return pitch++; }
    int operator--() { return pitch--; }

    static const int C3 = 60;
    void setPitchDirectly(int p) { pitch = p; }

private:
    int pitch;
};

inline void HarmonyNote::setMin(const Options& options) {
    pitch = options.style->absMinPitch();
}

inline HarmonyNote::HarmonyNote(const Options& options) {
    setMin(options);
}

inline std::string HarmonyNote::tellPitchName() const {
    return PitchKnowledge::nameOfAbs(pitch);
}

inline bool HarmonyNote::isTooHigh(const Options& options) const {
    return pitch > options.style->absMaxPitch();
}
