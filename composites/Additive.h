
#pragma once
#include "AdditiveGainLogic.h"
#include "AdditiveModLogic.h"
#include "AdditivePitchLogic.h"
#include "SinesVCO.h"
#include "SqLog.h"
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
        MA0_PARAM,
        MA1_PARAM,
        MA2_PARAM,
        MA3_PARAM,
        MA4_PARAM,
        MA5_PARAM,
        MA6_PARAM,
        MA7_PARAM,
        MA8_PARAM,
        MA9_PARAM,
        MA10_PARAM,
        MA11_PARAM,
        MA12_PARAM,
        MA13_PARAM,
        MA14_PARAM,
        MA15_PARAM,
        MB0_PARAM,
        MB1_PARAM,
        MB2_PARAM,
        MB3_PARAM,
        MB4_PARAM,
        MB5_PARAM,
        MB6_PARAM,
        MB7_PARAM,
        MB8_PARAM,
        MB9_PARAM,
        MB10_PARAM,
        MB11_PARAM,
        MB12_PARAM,
        MB13_PARAM,
        MB14_PARAM,
        MB15_PARAM,
        MC0_PARAM,
        MC1_PARAM,
        MC2_PARAM,
        MC3_PARAM,
        MC4_PARAM,
        MC5_PARAM,
        MC6_PARAM,
        MC7_PARAM,
        MC8_PARAM,
        MC9_PARAM,
        MC10_PARAM,
        MC11_PARAM,
        MC12_PARAM,
        MC13_PARAM,
        MC14_PARAM,
        MC15_PARAM,
        MD0_PARAM,
        MD1_PARAM,
        MD2_PARAM,
        MD3_PARAM,
        MD4_PARAM,
        MD5_PARAM,
        MD6_PARAM,
        MD7_PARAM,
        MD8_PARAM,
        MD9_PARAM,
        MD10_PARAM,
        MD11_PARAM,
        MD12_PARAM,
        MD13_PARAM,
        MD14_PARAM,
        MD15_PARAM,
        ME0_PARAM,
        ME1_PARAM,
        ME2_PARAM,
        ME3_PARAM,
        ME4_PARAM,
        ME5_PARAM,
        ME6_PARAM,
        ME7_PARAM,
        ME8_PARAM,
        ME9_PARAM,
        ME10_PARAM,
        ME11_PARAM,
        ME12_PARAM,
        ME13_PARAM,
        ME14_PARAM,
        ME15_PARAM,
        NUM_PARAMS
    };

    enum InputIds {
        CV_INPUT,
        ADSR0_INPUT,
        ADSR1_INPUT,
        ADSR2_INPUT,
        MOD0_INPUT,
        MOD1_INPUT,
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
    AdditiveModLogic<numHarmonics> aml;

    void processPitchInfo();
    void processGainInfo();
    void processModInfo();
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
inline void Additive<TBase>::processModInfo() {
    /*
      void setADSRValue(unsigned adsr, float value, bool connected);
    void setADSRTrimValue(unsigned adsr, unsigned harmonic, float value);
    */
    //SQINFO("processMidInfo");
    for (unsigned i = 0; i < aml.numADSR; ++i) {
        const bool connected = TBase::inputs[ADSR0_INPUT + i].isConnected();
        const float voltage = TBase::inputs[ADSR0_INPUT + i].value;
        //SQINFO("setting connected to %d for %d", connected, i);
        aml.setADSRValue(i, voltage, connected);
    }
    for (unsigned i = 0; i < numHarmonics; ++i) {
        for (unsigned adsr = 0; adsr < aml.numADSR; ++adsr) {
            float pot = TBase::params[MA0_PARAM + i + adsr * numHarmonics].value;
            aml.setADSRTrimValue(adsr, i, pot);
        };
    }
}

template <class TBase>
inline void Additive<TBase>::processAudio(const typename TBase::ProcessArgs& args) {
    //SQINFO("process audio");
    // process the sines:
    float results = 0;
    const float_4 deltaT(args.sampleTime);
    for (auto bank = 0; bank < numSines4; ++bank) {
        auto out = sines[bank].process(deltaT);
        const unsigned baseHarmonic = bank * 4;
        assert(numSines4 == 4);
      //  SQINFO("in loop out = %s", toStr(out).c_str());
        results += (out[0] * aml.getCombinedADSR(baseHarmonic + 0));
        results += (out[1] * aml.getCombinedADSR(baseHarmonic + 1));
        results += (out[2] * aml.getCombinedADSR(baseHarmonic + 2));
        results += (out[3] * aml.getCombinedADSR(baseHarmonic + 3));
      //  SQINFO("in proc look base=baseHarmonic %d, results=%f", baseHarmonic, results);
    }

    TBase::outputs[AUDIO_OUTPUT].setVoltage(results);
}

template <class TBase>
void Additive<TBase>::process(const typename TBase::ProcessArgs& args) {
    // SQINFO("** process **");
    processPitchInfo();
    processGainInfo();
    processModInfo();
    processAudio(args);
}