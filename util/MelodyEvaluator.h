
#include "Scale.h"

class MelodyMutateStyle {
public:
    bool keepInScale = true;
    bool roundRobin = true;         
    int numToMutate = 1;
    bool mutateAdjacent = true;

    float centerVoltage = 0;
    Scale scale;
   // double nonCenteredWeight = .1; // was .0000000001
    float nonCenteredWeight = 1;
    float idealPitchRange = 24;     // two octaves
    float pitchRangeWeight = 1;
    float leapsWeight = 1;
    float unisonWeight = 1;

    std::string toString() const;
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
