
#pragma once

namespace rack {
namespace engine {
struct Module;
}
}  // namespace rack

using Module = ::rack::engine::Module;
template <class TBase>
class PhasePatterns : public TBase {
public:
    enum ParamIds {
        SCHEMA_PARAM,
        SHIFT_PARAM,
        NUM_PARAMS
    };
    enum InputIds {
        CK_INPUT,
        NUM_INPUTS
    };

    enum OutputIds {
        CK_OUTPUT,
        NUM_OUTPUTS
    };

    enum LightIds {
        NUM_LIGHTS
    };

    PhasePatterns(Module* module) : TBase(module) {
        init();
    }
    PhasePatterns() : TBase() {
        init();
    }

private:
    void init();
};

template <class TBase>
inline void PhasePatterns<TBase>::init() {
    // TODO: initialization goes here.
}

