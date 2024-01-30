
#pragma once
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

private:
    void init();
};

template <class TBase>
inline void LFN2<TBase>::init() {
}
