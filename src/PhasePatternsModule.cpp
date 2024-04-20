
#include <iomanip>
#include <sstream>

#include "plugin.hpp"  // MUST BE FIRST

#include "BufferingParent.h"
#include "GfxUtils.h"
#include "NumberFormatter.h"
#include "PhasePatterns.h"
#include "PopupMenuParamWidget.h"
#include "SqLabel.h"
#include "SqLog.h"
#include "SqMenuItem.h"
#include "WidgetComposite.h"


using Comp = PhasePatterns<WidgetComposite>;
using Lab = SqLabel;

// This is a debug feature. should always be zero here.
// int ClockShifter5::llv = 0;
// int ClockShifter6::llv = 0;
int logLevel = 0;

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

    void onReset() override {
        comp->onReset();
        Module::onReset();
    }

private:
    void addParams();
};

#define RIB_SPAN_PARAM_MAX 32

void inline PhasePatternsModule::addParams() {
    INFO("BGF: in add params, this p=%llx comp p=%llx",
        (size_t)(void *) &(this->params),
        (size_t)(void *) &(comp->params));
    class ShiftAmountParam : public ParamQuantity {
    public:
        std::string getDisplayValueString() override {
            const auto value = getValue();
            std::stringstream str;
            str << std::setprecision(2) << value;
            return str.str();
        }
    };
    this->configParam<ShiftAmountParam>(Comp::SHIFT_PARAM, 0, 1, 0, "Global shift amount");

    class ShiftRangeParam : public ParamQuantity {
    public:
        std::string getDisplayValueString() override {
            const auto value = int(std::round(getValue()));
            switch (value) {
                case 0:
                    return "0..1";
                case 1:
                    return "0..10";
                case 2:
                    return "0..100";
                default:
                    assert(false);
                    return "";
            }
        }
    };
    this->configParam<ShiftRangeParam>(Comp::SHIFT_RANGE_PARAM, 0, 2, 0, "Shift range");

    this->configParam(Comp::SCHEMA_PARAM, 0, 10, 0, "Schema");
    this->configParam(Comp::COMBINED_SHIFT_INTERNAL_PARAM, 0, 10, 0, "[internal]");
    this->configSwitch(Comp::RIB_POSITIVE_BUTTON_PARAM, 0, 10, 0, "RIB+ trigger");
    this->configSwitch(Comp::RIB_NEGATIVE_BUTTON_PARAM, 0, 10, 0, "RIB- trigger");
    this->configParam(Comp::RIB_SPAN_PARAM, 1, RIB_SPAN_PARAM_MAX, 8, "RIB total duration clocks");

    this->configInput(Comp::CK_INPUT, "Master clock");
    this->configInput(Comp::SHIFT_INPUT, "Shift amount");
    this->configInput(Comp::RIB_POSITIVE_INPUT, "Rib trigger");
    this->configInput(Comp::RIB_NEGATIVE_INPUT, "Rib negative trigger");
    this->configOutput(Comp::CK_OUTPUT, "Shifted output");

    // class TotalParam : public ParamQuantity {
    // public:
    //     std::string getDisplayValueString() override {
    //         auto const labels = Comp::getRibDurationLabels();
    //         const int index = int(std::round(module->params[Comp::RIB_DURATION_PARAM].value));
    //         std::string ret = "";
    //         if (index < int(labels.size()) && index >= 0) {
    //             ret = labels[index];
    //         }
    //         return ret;
    //     }
    // };
    // this->configParam<TotalParam>(Comp::RIB_DURATION_PARAM, 0, 4, 2, "Total clocks per RIB");
    this->configParam(Comp::RIB_DURATION_PARAM, 0, 4, 1, "Total clocks per RIB");
    this->configParam(Comp::USE_ADVANCED_UI_PARAM, 0, 1, 0, "Expert UI");
}

#define _LAB

class PhasePatternsWidget : public ModuleWidget {
public:
    PhasePatternsWidget(PhasePatternsModule* module) {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/phase-patterns.svg")));
#ifdef _LAB
        addLabel(Vec(17, 3), "Phase Patterns", 17);  // was 20
        addLabel(Vec(24, 356), "Squinktronix", 16);
#endif
        addControls(module);
        _addRibsControls();
        addIO(module);
    }

private:
    BufferingParent<SqLabel>* _shiftDisplay = nullptr;

    void step() override {
        ModuleWidget::step();
        if (!module) {
            return;
        }

        if (_shiftDisplay) {
            const float shift = APP->engine->getParamValue(module, Comp::COMBINED_SHIFT_INTERNAL_PARAM);
            SqLabel* label = _shiftDisplay->getChild();
            if (shift > 99) {
                std::stringstream str;
                str << std::setprecision(4) << shift;
                label->updateText(str.str());
                // SQINFO("a shift=%f lab=%s", shift, str.str().c_str());
            } else {
                std::stringstream str;
                str << std::setprecision(4) << shift;
                const auto s = str.str();
                label->updateText(NumberFormatter::formatFloat(2, s));
                // SQINFO("b shift=%f lab=%s final=%s", shift, s.c_str(), NumberFormatter::formatFloat(2, s).c_str());
            }
        }

        const int expert = APP->engine->getParamValue(module, Comp::USE_ADVANCED_UI_PARAM) > .5;
        if (expert != _lastExpert) {
            _lastExpert = expert;
            _addRibsControls();
        }
    }

    int _lastExpert = -1;

    void appendContextMenu(ui::Menu* menu) override {
        if (!module) {
            return;
        }

        SqMenuItem_BooleanParam2* item = new SqMenuItem_BooleanParam2(module, Comp::USE_ADVANCED_UI_PARAM);
        item->text = "Use Expert UI";
        menu->addChild(item);
    }

    ParamWidget* _ribDurationControl = nullptr;
    ParamWidget* _ribSpanControl = nullptr;

    void _addRibsControls() {
        // First, get rid of what's there, if anything
        if (_ribDurationControl) {
            this->removeChild(_ribDurationControl);
            delete (_ribDurationControl);
            _ribDurationControl = nullptr;
        }
        if (_ribSpanControl) {
            this->removeChild(_ribSpanControl);
            delete (_ribSpanControl);
            _ribSpanControl = nullptr;
        }

        if (_useAdvancedUI()) {
            if (module) {
                module->getParamQuantity(Comp::RIB_SPAN_PARAM)->maxValue = 128;
            }
            _ribDurationControl = createParam<RoundBlackKnob>(Vec(13, 134), module, Comp::RIB_DURATION_PARAM);
            _ribSpanControl = createParam<RoundBlackKnob>(Vec(64, 134), module, Comp::RIB_SPAN_PARAM);
        } else {
            if (module) {
                module->getParamQuantity(Comp::RIB_SPAN_PARAM)->maxValue = RIB_SPAN_PARAM_MAX;
            }
            // Make the combo box
            auto p = createParam<PopupMenuParamWidget>(
                Vec(8, 138),
                module,
                Comp::RIB_DURATION_PARAM);
            p->setLabels(Comp::getRibDurationLabels());
            p->setIndexToValueFunction(Comp::indexToValueRibDuration);
            p->setValueToIndexFunction(Comp::valueToIndexRibDuration);
            p->box.size.x = 40;  // width
            p->box.size.y = 22;
            p->text = "1";
            _ribDurationControl = p;
            _ribSpanControl = createParam<RoundBlackSnapKnob>(Vec(66, 134), module, Comp::RIB_SPAN_PARAM);
            ;
        }
        addParam(_ribDurationControl);
        addLabel(Vec(14, 112), "Total");

        addParam(_ribSpanControl);
        addLabel(Vec(65, 112), "Dur");
    }

    /**
     */
    bool _useAdvancedUI() const {
        if (!module) {
            return false;
        }
        const float p = APP->engine->getParamValue(module, Comp::USE_ADVANCED_UI_PARAM);
        return p > .5;
    }

    /**
     */
    void addControls(PhasePatternsModule* module) {
        addParam(createParam<RoundBlackKnob>(Vec(13, 52), module, Comp::SHIFT_PARAM));
        addParam(createParam<CKSSThree>(Vec(72, 52), module, Comp::SHIFT_RANGE_PARAM));
#ifdef _LAB
        addLabel(Vec(14, 29), "Shift");
        addLabel(Vec(62, 29), "Range");
#endif
        _shiftDisplay = addLabel(Vec(8, 83), "");

        // now all the RIB controls

        // RIB trigger button
        addParam(createLightParam<VCVLightButton<MediumSimpleLight<WhiteLight>>>(
            Vec(18.5, 179),
            module,
            Comp::RIB_POSITIVE_BUTTON_PARAM,
            Comp::RIB_POSITIVE_LIGHT));
        addLabel(Vec(17, 160), "+");
        addParam(createLightParam<VCVLightButton<MediumSimpleLight<WhiteLight>>>(
            Vec(68, 179),
            module,
            Comp::RIB_NEGATIVE_BUTTON_PARAM,
            Comp::RIB_NEGATIVE_LIGHT));
        addLabel(Vec(68, 160), "-", 16);
    }

    void addIO(PhasePatternsModule* module) {
        addInput(createInput<PJ301MPort>(Vec(16, 273), module, Comp::CK_INPUT));
        addLabel(Vec(14, 253), "CkIn");
        addInput(createInput<PJ301MPort>(Vec(65, 273), module, Comp::SHIFT_INPUT));
        addLabel(Vec(64, 253), "Shft");

        addInput(createInput<PJ301MPort>(Vec(16, 226), module, Comp::RIB_POSITIVE_INPUT));
        addLabel(Vec(16, 206), "RIB+");
        addInput(createInput<PJ301MPort>(Vec(65, 226), module, Comp::RIB_NEGATIVE_INPUT));
        addLabel(Vec(66, 206), "RIB-");

        RoundedRect* r = new RoundedRect(Vec(32, 302), Vec(40, 50));
        addChild(r);
        addOutput(createOutput<PJ301MPort>(Vec(39, 322), module, Comp::CK_OUTPUT));
        addLabel(Vec(34.5, 300), "CkOut");
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