
#pragma once


#include "LFNDsp.h"


namespace rack {
namespace engine {
struct Module;
}
}  // namespace rack
using Module = ::rack::engine::Module;

template <class TBase>
class LFN2 : public TBase {
public:
    LFN2(Module* module) : TBase(module) {
        init();
    }
    LFN2() : TBase() {
        init();
    }
    enum ParamIds {
        NUM_PARAMS
    };

    enum InputIds {
        NUM_INPUTS
    };

    enum OutputIds {
        OUT,
        NUM_OUTPUTS
    };

    enum LightIds {
        NUM_LIGHTS
    };

     void process(const typename TBase::ProcessArgs& args) override;

private:
    LFNDsp _dsp;


    void init();
};

template <class TBase>
inline void LFN2<TBase>::init() {
}

template <class TBase>
inline void LFN2<TBase>::process(const typename TBase::ProcessArgs& args) {
    float_4 x = _dsp.process();
    //     void setVoltageSimd(T voltage, int firstChannel) {
    TBase::outputs[OUT].setVoltageSimd(x, 0);
}
