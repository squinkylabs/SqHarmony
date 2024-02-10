
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
    LFN2(Module* module) : TBase(module), _dsp(0) {
        init();
    }
    LFN2() : TBase(), _dsp(0) {
        init();
    }
    enum ParamIds {
        NUM_PARAMS
    };

    enum InputIds {
    //    DEBUG_IN,
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
    PinkFilter<float> _filter;


    void init();
};

template <class TBase>
inline void LFN2<TBase>::init() {
}

template <class TBase>
inline void LFN2<TBase>::process(const typename TBase::ProcessArgs& args) {
    float_4 x = _dsp.process();
   // float input = TBase::inputs[DEBUG_IN].value;
  //  float x = _filter.process(input);
    x *= 10;
    TBase::outputs[OUT].setVoltageSimd(x, 0);
   // TBase::outputs[OUT].setVoltage(x, 0);
}
