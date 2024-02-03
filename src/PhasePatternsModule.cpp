
#include <iomanip>
#include <sstream>

#include "plugin.hpp"
#include "BufferingParent.h"
#include "NumberFormatter.h"
#include "PhasePatterns.h"
#include "PopupMenuParamWidget.h"
#include "SqLabel.h"
#include "SqLog.h"
#include "WidgetComposite.h"

using Comp = PhasePatterns<WidgetComposite>;
using Lab = SqLabel;


int ClockShifter5::llv = 0;

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
    this->configParam(Comp::SHIFT_PARAM, -1, 1, 0, "Shift amount");
    this->configParam(Comp::SCHEMA_PARAM, 0, 10, 0, "Schema");
    this->configParam(Comp::COMBINED_SHIFT_INTERNAL_PARAM, 0, 10, 0, "[internal]");

    //this->configParam(Comp::RIB_POSITIVE_BUTTON_PARAM, 0, 10, 0, "RIB+ trigger");
    //this->configParam(Comp::RIB_NEGATIVE_BUTTON_PARAM, 0, 10, 0, "RIB- trigger");
    // configSwitch(MUTE_PARAMS + i, 0.f, 1.f, 0.f, string::f("Row %d mute", i + 1));
    this->configSwitch(Comp::RIB_POSITIVE_BUTTON_PARAM, 0, 10, 0, "RIB+ trigger");
    this->configSwitch(Comp::RIB_NEGATIVE_BUTTON_PARAM, 0, 10, 0, "RIB- trigger");
    this->configParam(Comp::RIB_DURATION_PARAM, 0, 4, 2, "Rib total shift (numerator)");
    this->configParam(Comp::RIB_SPAN_PARAM, 1, 32, 8, "Rib shift time (denominator)");

    this->configInput(Comp::CK_INPUT, "Master clock");
    this->configInput(Comp::SHIFT_INPUT, "Shift amount");
    this->configInput(Comp::RIB_POSITIVE_INPUT, "Rib trigger");
    this->configInput(Comp::RIB_NEGATIVE_INPUT, "Rib negative trigger");
    this->configOutput(Comp::CK_OUTPUT, "Shifted output");
}

#define _LAB

class PhasePatternsWidget : public ModuleWidget {
public:
    PhasePatternsWidget(PhasePatternsModule* module) {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/phase-patterns.svg")));
#ifdef _LAB
        addLabel(Vec(16, 6), "Phase Patterns", 17); // was 20
        addLabel(Vec(24, 356), "Squinktronix", 16);
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
        addParam(createParam<RoundBlackKnob>(Vec(42, 51), module, Comp::SHIFT_PARAM));
#ifdef _LAB
        addLabel(Vec(40, 29), "Shift");
#endif
        _shiftDisplay = addLabel(Vec(42, 83), "");

        // now all the RIB controls
        auto p = createParam<PopupMenuParamWidget>(
            Vec(6,138),
            module,
            Comp::RIB_DURATION_PARAM);
        //   p->setShortLabels(Comp::getShortScaleLabels(true));
        p->setLabels(Comp::getRibDurationLabels());
        p->box.size.x = 40;  // width
        p->box.size.y = 22;
        p->text = "1";
        addParam(p);
        addLabel(Vec(11, 110), "Total");

        auto param = createParam<RoundBlackSnapKnob>(Vec(64, 134), module, Comp::RIB_SPAN_PARAM);
        addParam(param);
        addLabel(Vec(67, 110), "Dur");

        // RIB trigger button
        addParam(createLightParam<VCVLightButton<MediumSimpleLight<WhiteLight>>>(
            Vec(17, 178),
            module,
            Comp::RIB_POSITIVE_BUTTON_PARAM,
            Comp::RIB_POSITIVE_LIGHT));
        addLabel(Vec(37, 170), "+");
        addParam(createLightParam<VCVLightButton<MediumSimpleLight<WhiteLight>>>(
            Vec(73, 178),
            module,
            Comp::RIB_NEGATIVE_BUTTON_PARAM,
            Comp::RIB_NEGATIVE_LIGHT));
        addLabel(Vec(89, 170), "-");
    }

    void addIO(PhasePatternsModule* module) {
        addInput(createInput<PJ301MPort>(Vec(12, 273), module, Comp::CK_INPUT));
        addLabel(Vec(9, 255), "CkIn");
        addInput(createInput<PJ301MPort>(Vec(68, 273), module, Comp::SHIFT_INPUT));
        addLabel(Vec(68, 255), "Shft");

        addOutput(createOutput<PJ301MPort>(Vec(40, 322), module, Comp::CK_OUTPUT));
        addLabel(Vec(37, 300), "CkOut");
        addInput(createInput<PJ301MPort>(Vec(12, 226), module, Comp::RIB_POSITIVE_INPUT));
        addLabel(Vec(11, 206), "RIB+");
        addInput(createInput<PJ301MPort>(Vec(67, 226), module, Comp::RIB_NEGATIVE_INPUT));
        addLabel(Vec(67, 206), "RIB-");
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
        // with 200, 20 it disappears at zoom > 250clichéd
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