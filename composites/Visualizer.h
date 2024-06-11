#pragma once

#include "Divider.h"

namespace rack {
namespace engine {
struct Module;
}
}  // namespace rack
using Module = ::rack::engine::Module;

template <class TBase>
class Visualizer : public TBase {
public:
    friend class TestX;
    enum ParamIds {
        NUM_PARAMS
    };
    enum InputIds {
        CV_INPUT,
        NUM_INPUTS
    };

    enum OutputIds {
        NUM_OUTPUTS
    };

    enum LightIds {
        NUM_LIGHTS
    };

    Visualizer(Module* module) : TBase(module) {
        _init();
    }

    Visualizer() : TBase() {
        _init();
    }

    void process(const typename TBase::ProcessArgs& args) override;
     static int getSubSampleFactor() { return 32; }

private:
    void _init();
    void _stepn();
     Divider _divn;

};

template <class TBase>
inline void Visualizer<TBase>::_init() {
    _divn.setup(getSubSampleFactor(), [this]() {
        this->_stepn();
    });
}


template <class TBase>
inline void Visualizer<TBase>::_stepn() {

}

template <class TBase>
inline void Visualizer<TBase>::process(const typename TBase::ProcessArgs& args) {
    _divn.step();
}