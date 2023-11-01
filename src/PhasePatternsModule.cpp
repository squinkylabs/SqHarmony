
#include "PhasePatterns.h"
#include "SqLog.h"
#include "WidgetComposite.h"

#include "plugin.hpp"

#include <iomanip>
#include <sstream>

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
      //  setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/blank_panel2.svg")));
      setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/phase-patterns.svg")));
#ifdef _LAB
        addLabel(Vec(20, 6), "Phase Patterns", 16);
        addLabel(Vec(26, 358), "Squinkytronix", 15);

#endif
        addControls(module);
        addIO(module);
    }

private:
    Label* _shiftDisplay = nullptr;

    void step() override {
        ModuleWidget::step();
        if (module) {
            const float shift = APP->engine->getParamValue(module, Comp::COMBINED_SHIFT_INTERNAL_PARAM);
            std::stringstream str;
            str << std::setprecision(3) << shift << std::endl;
            _shiftDisplay->text = str.str();
        }
    }
    void addControls(PhasePatternsModule* module) {
        auto param = createParam<RoundBigBlackKnob>(Vec(38, 131), module, Comp::SHIFT_PARAM);
        addParam(param);
#ifdef _LAB
        addLabel(Vec(44, 107), "Shift");
#endif
        addParam(createLightParam<VCVLightButton<MediumSimpleLight<WhiteLight>>>(
            Vec(70, 193),
            module,
            Comp::RIB_BUTTON_PARAM,
            Comp::RIB_LIGHT)
        );

        _shiftDisplay = addLabel(Vec(80, 190), "");
    }
    void addIO(PhasePatternsModule* module) {
            addInput(createInput<PJ301MPort>(Vec(13, 300), module, Comp::CK_INPUT));
            addInput(createInput<PJ301MPort>(Vec(25, 190), module, Comp::SHIFT_INPUT));
            addOutput(createOutput<PJ301MPort>(Vec(85, 300), module, Comp::CK_OUTPUT));
    }

#ifdef _LAB
    Label* addLabel(const Vec& v, const std::string& str, float fontSize = 14) {
             NVGcolor white = nvgRGB(0xff, 0xff, 0xff);
            Label* label = new Label();
            auto adjustedPos = v;
            adjustedPos.x -= 1.5f * str.size();
            label->box.pos = adjustedPos;
            label->text = str;
            label->color = white;
            label->fontSize = fontSize;
            addChild(label);
            return label;
    }
#endif
};

Model* modelPhasePatterns = createModel<PhasePatternsModule, PhasePatternsWidget>("sqh-phasepatterns");