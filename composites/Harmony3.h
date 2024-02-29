#pragma once

namespace rack {
namespace engine {
struct Module;
}
}  // namespace rack
using Module = ::rack::engine::Module;

template <class TBase>
class Harmony3 : public TBase {
public:
    Harmony3(Module* module) : TBase(module) {
        this->init();
    }
    Harmony3() : TBase() {
        this->init();
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
    void init();
};

template <class TBase>
inline void Harmony3<TBase>::init() {
}

template <class TBase>
inline void process(const typename TBase::ProcessArgs& args) {

 }