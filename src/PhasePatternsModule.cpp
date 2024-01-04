
#include <iomanip>
#include <sstream>

#include "PhasePatterns.h"
#include "SqLog.h"

#include "plugin.hpp"
#include "SqLabel.h"
#include "plugin.hpp"
#include "BufferingParent.h"
#include "NumberFormatter.h"

#include "plugin.hpp"
#include "WidgetComposite.h"

using Comp = PhasePatterns<WidgetComposite>;
using Lab = SqLabel;

template <>
int BufferingParent<SqLabel>::_refCount = 0;

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
    this->configInput(Comp::RIB_INPUT, "Rib trigger");
    this->configOutput(Comp::CK_OUTPUT, "Shifted output");
}

#define _LAB

class PhasePatternsWidget : public ModuleWidget {
public:
    PhasePatternsWidget(PhasePatternsModule* module) {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/phase-patterns.svg")));
#ifdef _LAB
        addLabel(Vec(15, 6), "Phase Patterns", 20);
        addLabel(Vec(28, 60), "Under Construction", 14);
        addLabel(Vec(30, 356), "Squinktronix", 16);
#endif
        addControls(module);
        addIO(module);
    }

private:
    BufferingParent<SqLabel>* _shiftDisplay = nullptr;

    void step() override {
        ModuleWidget::step();
        if (module) {
            if (_shiftDisplay) {
                const float shift = APP->engine->getParamValue(module, Comp::COMBINED_SHIFT_INTERNAL_PARAM);
                std::stringstream str;
                str << std::setprecision(3) << shift;

                std::string uiString = str.str();

                SqLabel* label = _shiftDisplay->getChild();
                label->updateText(NumberFormatter::formatFloat(2, uiString));
            }
        }
    }
    void addControls(PhasePatternsModule* module) {
        auto param = createParam<RoundBigBlackKnob>(Vec(38, 131), module, Comp::SHIFT_PARAM);
        addParam(param);
#ifdef _LAB
        addLabel(Vec(46, 107), "Shift");
#endif
        addParam(createLightParam<VCVLightButton<MediumSimpleLight<WhiteLight>>>(
            Vec(52, 193),
            module,
            Comp::RIB_BUTTON_PARAM,
            Comp::RIB_LIGHT));

        // 42 too far to left
        // 46 too far right
        _shiftDisplay = addLabel(Vec(44, 210), "");
    }

    void addIO(PhasePatternsModule* module) {
        const int a = 13;
        const int b = 85;
        const int c = (a + b) / 2;

        int jackY = 322;
        const int dL = 20;
        addInput(createInput<PJ301MPort>(Vec(a, jackY), module, Comp::CK_INPUT));
        addLabel(Vec(9, jackY-dL), "CkIn");
        addInput(createInput<PJ301MPort>(Vec(c, jackY), module, Comp::SHIFT_INPUT));
        addLabel(Vec(c-4, jackY-dL), "Shft");
        addOutput(createOutput<PJ301MPort>(Vec(85, jackY), module, Comp::CK_OUTPUT));
        addLabel(Vec(79, jackY-dL), "CkOut");

        jackY -= 50;

        addInput(createInput<PJ301MPort>(Vec(a, jackY), module, Comp::RIB_INPUT));
        addLabel(Vec(a, jackY-dL), "Rib Trig");
    }

    /**
     * @brief
     *
     * @param v is the position, panel relative
     * @param str text to display
     * @param fontSize
     * @return BufferingParent<SqLabel>*
     */
    BufferingParent<SqLabel>* addLabel(const Vec& v, const std::string& str, float fontSize = 14) {
        // TODO: what are these arbitrary numbers?
        // This "max size" is lame - do something better; was 200, 20
        // with 20, 2 is totally invisible.
        // with 200, 20 it disappears at zoom > 250
        // 400, 40 is no better
        // at 100, 10 it's truncated, but it still zooms up to about 240
        const Vec size(200, 20);
        SqLabel* lp = new SqLabel();
        BufferingParent<SqLabel>* parent = new BufferingParent<SqLabel>(lp, size, lp);

        NVGcolor white = nvgRGB(0xff, 0xff, 0xff);
        auto adjustedPos = v;

        adjustedPos.x -= 1.5f * str.size();
        parent->box.pos = adjustedPos; 

        lp->text = str;
        lp->color = white;
        lp->fontSize = fontSize;

        addChild(parent);
        return parent;
    }
};

Model* modelPhasePatterns = createModel<PhasePatternsModule, PhasePatternsWidget>("sqh-phasepatterns");