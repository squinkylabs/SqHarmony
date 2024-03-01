#pragma once

#include <string>
#include <vector>

namespace rack {
namespace engine {
struct Module;
}
}  // namespace rack
using Module = ::rack::engine::Module;

template <class TBase>
class Harmony2 : public TBase {
public:
    Harmony2(Module* module) : TBase(module) {
        this->init();
    }

    Harmony2() : TBase() {
        this->init();
    }

    static std::vector<std::string> getTransposeOctaveLabels();
    static std::vector<std::string> getTransposeDegreeLabels();

    enum ParamIds {
        XPOSE_DEGREE1_PARAM, XPOSE_DEGREE_2_PARAM, XPOSE_DEGREE_3_PARAM, XPOSE_DEGREE_4_PARAM,XPOSE_DEGREE_5_PARAM, XPOSE_DEGREE_6_PARAM,
        XPOSE_OCTAVE1_PARAM, XPOSE_OCTAVE_2_PARAM, XPOSE_OCTAVE_3_PARAM, XPOSE_OCTAVE_4_PARAM,XPOSEO_OCTAVE_5_PARAM, XPOSE_OCTAVE_6_PARAM,
        XPOSE_ENABLE1_PARAM, XPOSE_ENABLE_2_PARAM, XPOSE_ENABLE_3_PARAM, XPOSE_ENABLE_4_PARAM, XPOSE_ENABLE_5_PARAM, XPOSE_ENABLE_6_PARAM,
        XPOSE_TOTAL1_PARAM, XPOSE_TOTAL_2_PARAM,XPOSE_TOTAL_3_PARAM,XPOSE_TOTAL_4_PARAM,XPOSE_TOTAL_5_PARAM,XPOSE_TOTAL_6_PARAM,
        NUM_PARAMS
    };

    enum InputIds {
    //    DEBUG_IN,
        XPOSE_CV1_INPUT,
        XPOSE_CV2_INPUT,
        XPOSE_CV3_INPUT,
        XPOSE_CV4_INPUT,
        XPOSE_CV5_INPUT,
        XPOSE_CV6_INPUT,
        NUM_INPUTS
    };

    enum OutputIds {
        PITCH_OUTPUT,
        NUM_OUTPUTS
    };

    enum LightIds {
        XPOSE_ENABLE1_LIGHT, XPOSE_ENABLE2_LIGHT,XPOSE_ENABLE3_LIGHT,XPOSE_ENABLE4_LIGHT,XPOSE_ENABLE5_LIGHT,XPOSE_ENABLE6_LIGHT, 
        NUM_LIGHTS
    };

     void process(const typename TBase::ProcessArgs& args) override;

private:
    void init();
};

template <class TBase>
inline std::vector<std::string>  Harmony2<TBase>::getTransposeDegreeLabels() {
    return {
        "0",
        "+1",
        "+2",
        "+3",
        "+4",
        "+5",
        "+6",
        "+7"
    };
}

template <class TBase>
inline std::vector<std::string>  Harmony2<TBase>::getTransposeOctaveLabels() {
    return {
        "-2",
        "-1",
        "0",
        "+1",
        "+2",
    };
}
template <class TBase>
inline void Harmony2<TBase>::init() {
}

template <class TBase>
inline void Harmony2<TBase>::process(const typename TBase::ProcessArgs& args) {

 }