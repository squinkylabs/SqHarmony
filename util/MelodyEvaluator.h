#pragma once
#include "Scale.h"

enum SlotSelectionMethod {
    ROUND_ROBIN_ADJACENT,
    ROUND_ROBIN_DISTRIBUTED,
    RANDOM_ADJACENT,
    RANDOM_DISTRIBUTED,
    RANDOM_RANDOM,  // selection of all slots is random.
};


enum class Styles {
    OnlySeekCenter,
    OnlySeekRange,
    OnlyDiscorageLeaps,
    OnlyDissonant,
    OnlyUnison, 
    Disabled,           // 5
    Default
};

const int numStyles = int(Styles::Disabled);

class MelodyMutateStyle {
public:

    MelodyMutateStyle() {
        scale.set(MidiNote(MidiNote::C), Scale::Scales::Major);
    }
    bool keepInScale = true;
    unsigned int numToMutate = 1;  // if zero, mutate all.

    SlotSelectionMethod slotSelectionMethod = ROUND_ROBIN_ADJACENT;
    float centerVoltage = 0;
    Scale scale;
    float nonCenteredWeight = 1;
    float idealPitchRange2 = 2 * 12;  // two octaves
    float pitchRangeWeight = 1;
    float leapsWeight = 1;
    float unisonWeight = 1;
    float dissonantWeight = 1;

    std::string toString() const;
    void setStyles(Styles);
};

class MelodyEvaluator {
public:
    // lowest probability = highest Probability * probabilityDynamicRange 
    // must be between 0 and 1
    // drift test passed at .22 (with +-4 semis, 500 iter)
    inline const static float probabilityDynamicRange = .33f;
    static float getPenalty(const class MelodyRow&, const MelodyMutateStyle&);

    static float leapsPenalty(const MelodyRow&, const MelodyMutateStyle&);
    static float unisonsPenalty(const MelodyRow&, const MelodyMutateStyle&);
    static float nonCenteredPenalty(const MelodyRow&, const MelodyMutateStyle&);
    static float pitchRangePenalty(const MelodyRow&, const MelodyMutateStyle&);
    static float disonnantPenalty(const MelodyRow&, const MelodyMutateStyle&);

    static std::string toString(const MelodyRow&, const MelodyMutateStyle&);
};
