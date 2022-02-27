
#pragma once
#include "HarmonyNote.h"
#include "ScaleRelativeNote.h"

#include <assert.h>

enum class Roots { C = 1,
             C_SHARP,
             D };

/**
 * @brief a very limited keysig class. It's from the ancient "Piston" program. the
 * Scale class is a lot more general. Need to either get rid of this or generalize it.
 */

class Keysig {
    //////////////////////////////////
public:
    ScaleRelativeNote ScaleDeg(HarmonyNote Pitch);  // converts a midi pitch to a Scale degree (1..8)
                                                    // 0 for not a degree
    Keysig(Roots rt);

private:
    const Roots root;
    int nDegreeTable[13];  // maps chromatic pitch (key relative)
                           // to scale interval
};

using KeysigPtr = std::shared_ptr<Keysig>;

inline ScaleRelativeNote Keysig::ScaleDeg(HarmonyNote Pitch) {
    ScaleRelativeNote ret;
    int chro;

    chro = PitchKnowledge::chromaticFromAbs(Pitch);  // get c, c#, etc...
    chro -= (int) root;                 // normalize relative to our root
    chro += 1;                          // root is one, not zero
    if (chro < 1) chro += 12;     // keep positive
    assert(chro > 0 && chro <= 12);
    ret.set(nDegreeTable[chro]);  // coerce us into the ScaleRelativeNote
    return ret;
}
