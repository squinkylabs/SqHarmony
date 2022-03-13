
#pragma once
#include "HarmonyNote.h"
#include "ScaleRelativeNote.h"
//#include "ScaleQuantizer.h"
#include "Scale.h"

#include <assert.h>
#include <memory>

enum class Roots { C = 1,
             C_SHARP,
             D };
class Scale;
using ScalePtr = std::shared_ptr<Scale>;

/**
 * @brief a very limited keysig class. It's from the ancient "Piston" program. the
 * Scale class is a lot more general. Need to either get rid of this or generalize it.
 */

class KeysigOld {
    //////////////////////////////////
public:
    ScaleRelativeNote ScaleDeg(HarmonyNote Pitch);  // converts a midi pitch to a Scale degree (1..8)
                                                    // 0 for not a degree
    KeysigOld(Roots rt);

    void set(const MidiNote& basePitch, Scale::Scales scale);

private:
    ScalePtr scale;
  //  const Roots root;
  //  int nDegreeTable[13];  // maps chromatic pitch (key relative)
                           // to scale interval
};

using KeysigPtr = std::shared_ptr<Keysig>;
