
#include <iomanip>
#include <sstream>

#include "Multiplier.h"
#include "SqLog.h"
#include "WidgetComposite.h"
#include "plugin.hpp"

using Comp = Multiplier<WidgetComposite>;

class MultiplierModule : public rack::engine::Module {
public:
    std::shared_ptr<Comp> comp = std::make_shared<Comp>(this);
    MultiplierModule() {
        config(Comp::NUM_PARAMS, Comp::NUM_INPUTS, Comp::NUM_OUTPUTS, Comp::NUM_LIGHTS);
        addParams();
    }

    void process(const ProcessArgs& args) override {
        comp->process(args);
    }

private:
    void addParams();
};

void inline MultiplierModule::addParams() {
    this->configParam(Comp::MULT_PARAM, 1, 4, 1, "Multiply amount");
    this->configParam(Comp::SCHEMA_PARAM, 0, 10, 0, "Schema");
    this->configParam(Comp::RATCHET_1_COUNT_PARAM, 1, 16, 4, "Schema");

    this->configInput(Comp::CK_INPUT, "Master clock");
    this->configInput(Comp::RATCHET_1_TRIGGER_INPUT, "Ratchet Trigger 1");

    this->configOutput(Comp::CK_OUTPUT, "Master clock");

}

#define _LAB

class MultiplierWidget : public ModuleWidget {
public:
    MultiplierWidget(MultiplierModule* module) {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/blank_panel2.svg")));
#ifdef _LAB
        addLabel(Vec(35, 12), "Multiplier", 16);
        addLabel(Vec(24, 340), "Squinkytronix", 16);

#endif
        addControls(module);
        addIO(module);
    }

private:
    Label* _shiftDisplay = nullptr;

    void addControls(MultiplierModule* module) {
        auto param = createParam<RoundBigBlackKnob>(Vec(50, 131), module, Comp::MULT_PARAM);
        addParam(param);
#ifdef _LAB
        addLabel(Vec(58, 107), "Mult");
#endif
        param = createParam<RoundBigBlackKnob>(Vec(50, 220), module, Comp::RATCHET_1_COUNT_PARAM);
        addParam(param);
#ifdef _LAB
        addLabel(Vec(58, 200), "Ratchet Ct");
#endif
    }
    void addIO(MultiplierModule* module) {
        addInput(createInput<PJ301MPort>(Vec(22, 300), module, Comp::CK_INPUT));
        addInput(createInput<PJ301MPort>(Vec(22, 220), module, Comp::RATCHET_1_TRIGGER_INPUT));
        addOutput(createOutput<PJ301MPort>(Vec(77, 300), module, Comp::CK_OUTPUT));
    }

#ifdef _LAB
    Label* addLabel(const Vec& v, const std::string& str, float fontSize = 14) {
        // NVGcolor white = nvgRGB(0xe0, 0xe0, 0xe0);
        NVGcolor black = nvgRGB(0, 0, 0);
        Label* label = new Label();
        auto adjustedPos = v;
        adjustedPos.x -= 1.5f * str.size();
        label->box.pos = adjustedPos;
        label->text = str;
        label->color = black;
        label->fontSize = fontSize;
        addChild(label);
        return label;
    }
#endif
};

Model* modelMult = createModel<MultiplierModule, MultiplierWidget>("sqh-mult");