
#include "Scale.h"

enum SlotSelectionMethod {
    ROUND_ROBIN_ADJACENT,
    ROUND_ROBIN_DISTRIBUTED,
    RANDOM_ADJACENT,
    RANDOM_DISTRIBUTED,
    RANDOM_RANDOM,  // selection of all slots is random.
};

class MelodyMutateStyle {
public:
    bool keepInScale = true;
    int numToMutate = 1;  // if zero, mutate all.

    SlotSelectionMethod slotSelectionMethod = ROUND_ROBIN_ADJACENT;
    float centerVoltage = 0;
    Scale scale;
    float nonCenteredWeight = 1;
    float idealPitchRange2 = 2 * 12;  // two octaves
    float pitchRangeWeight = 1;
    float leapsWeight = 1;
    float unisonWeight = 1;

    std::string toString() const;
    void disable();  // set to all rules off
};

class MelodyEvaluator {
public:
    static float getPenalty(const class MelodyRow&, const MelodyMutateStyle&);

    static float leapsPenalty(const MelodyRow&, const MelodyMutateStyle&);
    static float unisonsPenalty(const MelodyRow&, const MelodyMutateStyle&);
    static float nonCenteredPenalty(const MelodyRow&, const MelodyMutateStyle&);
    static float pitchRangePenalty(const MelodyRow&, const MelodyMutateStyle&);

    static std::string toString(const MelodyRow&, const MelodyMutateStyle&);
};
