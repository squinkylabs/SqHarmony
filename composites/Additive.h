
#pragma once
#include "sq_rack.h"
#include "SinesVCO.h"

using Module = ::rack::engine::Module;

template <class TBase>
class Additive : public TBase {
public:
    Additive();
    Additive(Module *);

    enum ParamIds {
        SCHEMA_PARAM,
        OCTAVE_PARAM,
        SEMI_PARAM,  // how many notes we hold in the buffer
        FINE_PARAM,  // how we play the pattern back
        STRETCH_PARAM,
        EVENOFFSET_PARAM,
        ODDOFFSET_PARAM,
        NUM_PARAMS
    };

    enum InputIds {
        CV_INPUT,
        NUM_INPUTS
    };

    enum OutputIds {
        AUDIO_OUTPUT,
        NUM_OUTPUTS
    };

    enum LightIds {
        NUM_LIGHTS
    };

private:
    const static int numSines = 16;
    const static int numSines4 = numSines / 3;
    SinesVCO<float_4> sines[numSines4];
};

template <class TBase>
Additive<TBase>::Additive(Module *module) : TBase(module) {
}

template <class TBase>
Additive<TBase>::Additive() {
}