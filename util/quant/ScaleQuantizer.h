#pragma once

#include "MidiNote.h"

#include <memory>

/**
 * Ideas for representing pitch in a more sane way.
 * 
 *  MidiNote - wraps an integer, 1..127
 *  FloatNote - wraps a float in VCV format (1v/oct 0 = middle C)
 *  DiatonicNote
 *      int degree (0 = root)
 *      int octave
 *      (could add sharp and flat, if desired) 
 * 
 * What old Piston code uses:
 *  HarmonyNote - basically a MidiNote, but with some mutators like ++
 *  ScaleRelativeNote - a simplified DiatonicNote, with just the scale degree (1 == root)
 * 
 * For key signature:
 *  Piston Keysig
 *      always major
 *      wraps a Root (enum for C, C#, D...). and a table for internal calcs.
 *      can convert a HarmonyNote to a ScaleRelativeNote.
 * Seq++ Scale
 *      has a root and a scale.
 *      not limited to Diatonic scales
 *      can do some conversions
 *      uses an std::map
 * 
 * Proposal:
 *  rename Diatonicxxx to Scalexxx
 *  make the Scale object, and make it changeable on audio thread.
 *  Quantizer gets a ref to the Scale.
 */
class Scale;
class ScaleQuantizer;
using ConstScalePtr = std::shared_ptr<const Scale>;
using ScalePtr = std::shared_ptr<Scale>;
using ScaleQuantizerPtr = std::shared_ptr<ScaleQuantizer>;

/**
 * needs:
 *      hysteresis
 *      chromatic option?
 *      selection for equal interval?
 *      selection for rounding of non scale tones?
 *      take shared ptr to scale (done)
 * 
 */

class ScaleQuantizer {
public:
    class Options {
    public:
        ScalePtr scale;
        float   hysteresis = 0;
    };
    using ConstOptionsPtr = std::shared_ptr<const Options>;
    using OptionsPtr = std::shared_ptr<Options>;

    ScaleQuantizer(ConstOptionsPtr opt);
    MidiNote run(float);

    static ScaleQuantizerPtr makeTestCMaj();
    static ScaleQuantizerPtr makeTest(int baseMidiPitch, Scale::Scales scale);

private:
    float lastValue = 0;
    MidiNote lastQuantizedNote;
    ConstOptionsPtr options;
};