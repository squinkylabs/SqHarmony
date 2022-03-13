
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
    // This [legacy] constructor always makes a major scale.
    // It could be fixed, but callin KeysigOld::set is easy.
    ScaleRelativeNote ScaleDeg(HarmonyNote Pitch);  // converts a midi pitch to a Scale degree (1..8)
                                                    // 0 for not a degree
    KeysigOld(Roots rt);

    void set(const MidiNote& basePitch, Scale::Scales scale);
    std::pair<const MidiNote, Scale::Scales> get() const;

private:
    ScalePtr scale;
};

using KeysigPtr = std::shared_ptr<Keysig>;
