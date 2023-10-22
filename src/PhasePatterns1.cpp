
#include "PhasePatterns.h"
#include "SqLog.h"
#include "WidgetComposite.h"
#include "plugin.hpp"

using Comp = PhasePatterns<WidgetComposite>;

class PhasePatternsModule : public rack::engine::Module {
public:
    std::shared_ptr<Comp> comp = std::make_shared<Comp>(this);
    PhasePatternsModule() {
        config(Comp::NUM_PARAMS, Comp::NUM_INPUTS, Comp::NUM_OUTPUTS, Comp::NUM_LIGHTS);
        addParams();
    }

    void process(const ProcessArgs& args) override {
        comp->process(args);
    }

private:
    void addParams();
};

void inline PhasePatternsModule::addParams() {
    this->configParam(Comp::SHIFT_PARAM, 0, 4, 0, "Shift amount");
    this->configParam(Comp::SCHEMA_PARAM, 0, 10, 0, "Schema");
    this->configParam(Comp::COMBINED_SHIFT_INTERNAL_PARAM, 0, 10, 0, "[internal]");
    this->configParam(Comp::RIB_BUTTON_PARAM, 0, 10, 0, "RIB");

    this->configInput(Comp::CK_INPUT, "Master clock");
    this->configInput(Comp::SHIFT_INPUT, "Shift amount");
    this->configOutput(Comp::CK_OUTPUT, "Shifted output");
}

#define _LAB

class PhasePatternsWidget : public ModuleWidget {
public:
    PhasePatternsWidget(PhasePatternsModule* module) {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/blank_panel2.svg")));
#ifdef _LAB
        addLabel(Vec(24, 12), "Phase Patterns", 20);
        addLabel(Vec(24, 340), "Squinkytronix", 20);

#endif
        addControls(module);
        addIO(module);
    }

private:
    void addControls(PhasePatternsModule* module) {
        auto param = createParam<RoundBigBlackKnob>(Vec(50, 131), module, Comp::SHIFT_PARAM);
        addParam(param);
#ifdef _LAB
        addLabel(Vec(58, 107), "Shift");
#endif
        addParam(createLightParam<VCVLightButton<MediumSimpleLight<WhiteLight>>>(
            Vec(80, 193),
            module,
            Comp::RIB_BUTTON_PARAM,
            Comp::RIB_LIGHT)
        );
    }
    void addIO(PhasePatternsModule* module) {
            addInput(createInput<PJ301MPort>(Vec(27, 300), module, Comp::CK_INPUT));
            addInput(createInput<PJ301MPort>(Vec(40, 190), module, Comp::SHIFT_INPUT));
            addOutput(createOutput<PJ301MPort>(Vec(100, 300), module, Comp::CK_OUTPUT));
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

Model* modelPhasePatterns = createModel<PhasePatternsModule, PhasePatternsWidget>("sqh-phasepatterns");