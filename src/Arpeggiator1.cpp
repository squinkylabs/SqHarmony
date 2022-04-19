
#include "Arpeggiator.h"
#include "PopupMenuParamWidget.h"
#include "SqMenuItem.h"
#include "WidgetComposite.h"
#include "plugin.hpp"

using Comp = Arpeggiator<WidgetComposite>;

class Arpeggiator1Module : public Module {
public:
    std::shared_ptr<Comp> comp = std::make_shared<Comp>(this);

    Arpeggiator1Module() {
        config(Comp::NUM_PARAMS, Comp::NUM_INPUTS, Comp::NUM_OUTPUTS, Comp::NUM_LIGHTS);
        addParams();
    }

    void process(const ProcessArgs& args) override {
        comp->process(args);
    }

private:
    void addParams();
};

inline void Arpeggiator1Module::addParams() {
    const auto numModes = Comp::modes().size();
    this->configParam(Comp::MODE_PARAM, 0, numModes - 1, 0, "Arpeggiator Mode");
    this->configParam(Comp::LENGTH_PARAM, 0, 12, 0, "Note Buffer Length");
    this->configParam(Comp::BEATS_PARAM, 0, 12, 0, "Number of Beats");
    this->configParam(Comp::SCHEMA_PARAM, 0, 10, 0, "Schema");
    this->configParam(Comp::POLY_PARAM, 0, 1, 0, "Poly");
    this->configParam(Comp::RESET_MODE_PARAM, 0, 1, 1, "Reset Mode");
    this->configParam(Comp::GATE_DELAY_PARAM, 0, 1, 0, "Gate+clock Delay");

    this->configSwitch(Comp::HOLD_PARAM, 0, 1, 0, "Hold", {"off", "on"});

    this->configInput(Comp::SHUFFLE_TRIGGER_INPUT, "trigger to re-shuffle the notes (in shuffle mode)");
    this->configInput(Comp::GATE_INPUT, "Gate");
    this->configInput(Comp::CV_INPUT, "V/Oct");
    this->configInput(Comp::CLOCK_INPUT, "Clock");
    this->configInput(Comp::RESET_INPUT, "Reset");
    this->configInput(Comp::HOLD_INPUT, "Hold");
    this->configInput(Comp::MODE_INPUT, "Mode");

    this->configOutput(Comp::CV_OUTPUT, "V/Oct");
    this->configOutput(Comp::GATE_OUTPUT, "Gate");
    this->configOutput(Comp::EOC_OUTPUT, "End of pattern");
}

#if 0
class Logo : public SvgWidget {
public:
    Logo() {
     //   auto svg = APP->window->loadSvg(asset::plugin(pluginInstance, "res/scope.svg"));
        auto svg = APP->window->loadSvg(asset::plugin(pluginInstance, "res/pattern.svg"));
        setSvg(svg);
    }

private:
};
#endif

struct NKK2 : SvgSwitch {
    NKK2() {
        addFrame(APP->window->loadSvg(
            asset::system("res/ComponentLibrary/NKK_0.svg").c_str()));
        addFrame(APP->window->loadSvg(
            asset::system("res/ComponentLibrary/NKK_2.svg").c_str()));
    }
};

struct RoundBigBlackSnapKnob : RoundBigBlackKnob {
    RoundBigBlackSnapKnob() {
        snap = true;
        smooth = false;
    }
};

struct RoundHugeBlackSnapKnob : RoundHugeBlackKnob {
    RoundHugeBlackSnapKnob() {
        snap = true;
        smooth = false;
    }
};

struct RoundSmallBlackSnapKnob : RoundSmallBlackKnob {
    RoundSmallBlackSnapKnob() {
        snap = true;
        smooth = false;
    }
};

struct Arpeggiator1Widget : ModuleWidget {
    Arpeggiator1Widget(Arpeggiator1Module* module) {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/arpeggiator.svg")));

#if 0
        auto svg = APP->window->loadSvg(asset::plugin(pluginInstance, "res/pattern-four.svg"));
        SvgWidget* logo = new SvgWidget();
        logo->setSvg(svg);
        addChild(logo);
#endif

#ifdef _LAB
        addLabel(Vec(28, 5), "Arpeggiator");
#endif

        float yOut = 320;
        float yIn2 = 256;
        float yIn1 = 212;

        const float x1 = 19;
        const float x2 = 63.3;
        const float x3 = 108;

        // first row, yIn1
        addInputL(Vec(x1, yIn1), Comp::CLOCK_INPUT, "Clk");
        addInputL(Vec(x2, yIn1), Comp::RESET_INPUT, "Rst");
        addInputL(Vec(x3, yIn1), Comp::SHUFFLE_TRIGGER_INPUT, "Strig");

        //  second row
        addInputL(Vec(x1, yIn2), Comp::CV_INPUT, "CV");
        addInputL(Vec(x2, yIn2), Comp::CV2_INPUT, "CV2");
        addInputL(Vec(x3, yIn2), Comp::GATE_INPUT, "Gate");

        //  third row
        addOutputL(Vec(x1, yOut), Comp::CV_OUTPUT, "CV");
        addOutputL(Vec(x2, yOut), Comp::CV2_OUTPUT, "CV2");
        addOutputL(Vec(x3, yOut), Comp::GATE_OUTPUT, "Gate");

        const float yMode = 42;
        std::vector<std::string> labels = Comp::modes();
        PopupMenuParamWidget* p = createParam<PopupMenuParamWidget>(
            Vec(54, yMode),
            module,
            Comp::MODE_PARAM);
        p->box.size.x = 80;  // width
        p->box.size.y = 22;
        p->text = labels[3];
        p->setLabels(labels);
        p->setShortLabels(Comp::shortModes());
        addParam(p);

        addInputL(Vec(x1, yMode), Comp::MODE_INPUT);

        auto param = createParam<RoundBigBlackSnapKnob>(Vec(86, 131), module, Comp::LENGTH_PARAM);
        addParam(param);
#ifdef _LAB
        addLabel(Vec(knobLeft - 8, yKnob - 28), "Length");
#endif
        auto param2 = createParam<RoundBigBlackSnapKnob>(Vec(19, 131), module, Comp::BEATS_PARAM);
        addParam(param2);
#ifdef _LAB
        addLabel(Vec(knobLeft + 22 + dx2, yKnob - 28), "Beats");
#endif
        addParam(createParam<CKSS>(Vec(54, 79), module, Comp::HOLD_PARAM));
#ifdef _LAB
        addLabel(Vec(knobLeft + 46, yHold + 9), "Hold");
#endif
        addInputL(Vec(x1, 77), Comp::HOLD_INPUT, "");
    }

    void addOutputL(const Vec& vec, int outputNumber, const std::string& text) {
        // assert(module);
        addOutput(createOutput<PJ301MPort>(vec, module, outputNumber));
#ifdef _LAB
        Vec vlabel(vec.x, vec.y);
        vlabel.y -= 20;
        const float xOffset = -4 + text.size() * 2.5;  // crude attempt to center text.
        vlabel.x -= xOffset;

        addLabel(vlabel, text);
#endif
    }

    void addInputL(const Vec& vec, int outputNumber, const std::string& text = "") {
        // assert(module);
        addInput(createInput<PJ301MPort>(vec, module, outputNumber));
#ifdef _LAB
        if (!text.empty()) {
            Vec vlabel(vec.x, vec.y);
            vlabel.y -= 20;
            const float xOffset = -4 + text.size() * 2.5;  // crude attempt to center text.
            vlabel.x -= xOffset;
            addLabel(vlabel, text);
        }
#endif
    }

    Label* addLabel(const Vec& v, const std::string& str) {
        NVGcolor white = nvgRGB(0xf0, 0xF0, 0xF0);

        Label* label = new Label();
        label->box.pos = v;
        label->text = str;
        label->color = white;
        label->fontSize = 12;  // 13 is default
        addChild(label);
        return label;
    }

    void appendContextMenu(Menu* theMenu) override {
        MenuLabel* spacerLabel = new MenuLabel();
        theMenu->addChild(spacerLabel);

        SqMenuItem_BooleanParam2* item = new SqMenuItem_BooleanParam2(module, Comp::RESET_MODE_PARAM);
        item->text = "Reset Mode II";
        theMenu->addChild(item);

        item = new SqMenuItem_BooleanParam2(module, Comp::GATE_DELAY_PARAM);
        item->text = "Gate+Clock Delay";
        theMenu->addChild(item);
    }
};

Model* modelArpeggiator1 = createModel<Arpeggiator1Module, Arpeggiator1Widget>("sqh-arpeggiator1");