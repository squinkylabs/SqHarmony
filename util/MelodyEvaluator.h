
#include "Scale.h"

class MelodyMutateStyle {
public:
    bool keepInScale = true;
  //  bool roundRobin = true;         
    int numToMutate = 1;            // if zero, mutate all.
   // bool mutateAdjacent = true;
    int adjacentStyle = 0;          // 0 = round robin, adjacent
                                    // 1 = round robin, not adjacent
                                    // 2 = all random

    float centerVoltage = 0;
    Scale scale;
   // double nonCenteredWeight = .1; // was .0000000001
    float nonCenteredWeight = 1;
    float idealPitchRange2 = 2 * 12;     // two octaves
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
