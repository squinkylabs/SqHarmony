
#pragma once
#include "AdditiveGainLogic.h"
#include "AdditivePitchLogic.h"
#include "SinesVCO.h"
#include "sq_rack.h"

using Module = ::rack::engine::Module;

template <class TBase>
class Additive : public TBase {
public:
    Additive();
    Additive(Module*);

    enum ParamIds {
        SCHEMA_PARAM,
        OCTAVE_PARAM,
        SEMI_PARAM,  // how many notes we hold in the buffer
        FINE_PARAM,  // how we play the pattern back
        STRETCH_PARAM,
        EVENOFFSET_PARAM,
        ODDOFFSET_PARAM,
        EVENLEVEL_PARAM,
        ODDLEVEL_PARAM,
        SLOPE_PARAM,
        H0_PARAM,
        H1_PARAM,
        H2_PARAM,
        H3_PARAM,
        H4_PARAM,
        H5_PARAM,
        H6_PARAM,
        H7_PARAM,
        H8_PARAM,
        H9_PARAM,
        H10_PARAM,
        H11_PARAM,
        H12_PARAM,
        H13_PARAM,
        H14_PARAM,
        H15_PARAM,
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

    void process(const typename TBase::ProcessArgs& args) override;

    const static int numSines = 16;
    const static int numSines4 = numSines / 4;
    const static int numHarmonics = 16;

private:
    SinesVCO<float_4> sines[numSines4];
    AdditivePitchLogic<numHarmonics> apl;
    AdditiveGainLogic<numHarmonics> agl;

    void processPitchInfo();
    void processGainInfo();
    void processAudio(const typename TBase::ProcessArgs& args);
};

template <class TBase>
Additive<TBase>::Additive(Module* module) : TBase(module) {
}

template <class TBase>
Additive<TBase>::Additive() {
}

template <class TBase>
inline void Additive<TBase>::processPitchInfo() {
    // Round up all the pitch info
    const int oct = int(std::round(TBase::params[OCTAVE_PARAM].value));
    apl.setOctave(oct);

    const int semi = int(std::round(TBase::params[SEMI_PARAM].value));
    apl.setSemitone(semi);

    const float fine = TBase::params[FINE_PARAM].value;
    apl.setFine(fine);

    const float cv = TBase::inputs[CV_INPUT].value;
    apl.setCV(cv);

    const float stretch = TBase::params[STRETCH_PARAM].value;
    apl.setStretch(stretch);

    const float eo = TBase::params[EVENOFFSET_PARAM].value;
    apl.setEvenOffset(eo);

    const float oo = TBase::params[ODDOFFSET_PARAM].value;
    apl.setOddOffset(oo);

    // send pitch to sines
    const float sampleRate = TBase::engineGetSampleRate();
    for (auto bank = 0; bank < numSines4; ++bank) {
        float_4 x;
        for (auto sub = 0; sub < 4; ++sub) {
            const int harmonic = (bank * 4) + sub;
            x[sub] = apl.getPitch(harmonic);
        }
        sines[bank].setPitch(x, sampleRate);
    }
}

template <class TBase>
inline void Additive<TBase>::processGainInfo() {
    // Round up all the level info
    const float even = TBase::params[EVENLEVEL_PARAM].value;
    agl.setEven(even);

    const float odd = TBase::params[ODDLEVEL_PARAM].value;
    agl.setOdd(odd);

    const float slope = TBase::params[SLOPE_PARAM].value;
    agl.setSlope(slope);

    for (unsigned i = 0; i < numHarmonics; ++i) {
        agl.setHarmonic(i, TBase::params[H0_PARAM + i].value);
    }
}

template <class TBase>
inline void Additive<TBase>::processAudio(const typename TBase::ProcessArgs& args) {
    // process the sines:
    float results = 0;
    const float_4 deltaT(args.sampleTime);
    for (auto bank = 0; bank < numSines4; ++bank) {
        auto out = sines[bank].process(deltaT);
        const unsigned baseHarmonic = bank * 4;
        assert(numSines4 == 4);
        results += (out[0] * agl.getLevel(baseHarmonic + 0));
        results += (out[1] * agl.getLevel(baseHarmonic + 1));
        results += (out[2] * agl.getLevel(baseHarmonic + 2));
        results += (out[3] * agl.getLevel(baseHarmonic + 3));
    }

    TBase::outputs[AUDIO_OUTPUT].setVoltage(results);
}

template <class TBase>
void Additive<TBase>::process(const typename TBase::ProcessArgs& args) {
    processPitchInfo();
    processGainInfo();
    processAudio(args);
}