
#include "Scale.h"

class MelodyMutateStyle {
public:
    bool keepInScale = true;
    bool roundRobin = true;
    float centerVoltage = 0;
    Scale scale;
    double nonCenteredWeight = .0000000001;
};

class MelodyEvaluator {
public:
    static float getPenalty(const class MelodyRow&, const MelodyMutateStyle&);
    static float leapsPenalty(const MelodyRow&);
    static float unisonsPenalty(const MelodyRow&);
    static float nonCenteredPenalty(const MelodyRow&, const MelodyMutateStyle&);
};
