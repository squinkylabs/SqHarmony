
template <class TBase>
class Mutator : public TBase {
public:
    Mutator(Module* module) : TBase(module) {
        _init();
    }
    Mutator() : TBase() {
        _init();
    }


    enum ParamIds {
        NUM_PARAMS
    };
    enum InputIds {
        NUM_INPUTS
    };

    enum OutputIds {
        NUM_OUTPUTS
    };

    enum LightIds {
        NUM_LIGHTS
    };

private:
    void _init() {}
};