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

    static std::vector<std::string> getTransposeLabels();

    enum ParamIds {
        XPOSE1_PARAM,
        XPOSE2_PARAM,
        XPOSE3_PARAM,
        XPOSE4_PARAM,
        XPOSE5_PARAM,
        XPOSE6_PARAM,
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
        NUM_LIGHTS
    };

     void process(const typename TBase::ProcessArgs& args) override;

private:
    void init();
};

template <class TBase>
inline std::vector<std::string>  Harmony2<TBase>::getTransposeLabels() {
    return {
        "off",
        "+1",
        "+2",
        "+3",
        "+4",
        "+5",
        "+6",
        "+7",
        "1:+0"
    };
}

template <class TBase>
inline void Harmony2<TBase>::init() {
}

template <class TBase>
inline void Harmony2<TBase>::process(const typename TBase::ProcessArgs& args) {

 }