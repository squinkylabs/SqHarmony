
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
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/blank-panel-4.svg")));

      

#if 1
        auto svg = APP->window->loadSvg(asset::plugin(pluginInstance, "res/pattern-four.svg"));
        SvgWidget* logo = new SvgWidget();
        logo->setSvg(svg);
        addChild(logo);
#endif

        addLabel(Vec(28, 5), "Arpeggiator");

        float dy = 47;          // 44 a little  too close
        float yOut = 320;
        float yIn2 = yOut -dy ;  

        float yIn1 = yIn2 - dy;

        float xLeft = 10;
        float dx = 45;          // 41 too close, used 43 for first test

        // first row, yIn1
        addInputL(Vec(xLeft + 0 * dx, yIn1), Comp::CLOCK_INPUT, "Clk");
        addInputL(Vec(xLeft + 1 * dx, yIn1), Comp::RESET_INPUT, "Rst");
        addInputL(Vec(xLeft + 2 * dx, yIn1), Comp::SHUFFLE_TRIGGER_INPUT, "Strig");

        //  second row
        addInputL(Vec(xLeft, yIn2), Comp::CV_INPUT, "CV");
        addInputL(Vec(xLeft + dx, yIn2), Comp::CV2_INPUT, "CV2");
        addInputL(Vec(xLeft + 2 * dx, yIn2), Comp::GATE_INPUT, "Gate");
       

        //  third row
        addOutputL(Vec(xLeft, yOut), Comp::CV_OUTPUT, "CV");
        addOutputL(Vec(xLeft + dx, yOut), Comp::CV2_OUTPUT, "CV2");
        addOutputL(Vec(xLeft + 2 * dx, yOut), Comp::GATE_OUTPUT, "Gate");

        const float yMode = 42;
        std::vector<std::string> labels = Comp::modes();
        PopupMenuParamWidget* p = createParam<PopupMenuParamWidget>(
            Vec(38, yMode),
            module,
            Comp::MODE_PARAM);
        p->box.size.x = 80;  // width
        p->box.size.y = 22;
        p->text = labels[3];
        p->setLabels(labels);
        p->setShortLabels(Comp::shortModes());
        addParam(p);

        addInputL(Vec(xLeft, yMode), Comp::MODE_INPUT);

        // RoundBigBlackKnob
        //
        float yKnob = 150;
        float knobLeft = 14;  // 20 too far right
        float dx2 = 40;

        auto param = createParam<RoundBigBlackSnapKnob>(Vec(knobLeft, yKnob), module, Comp::LENGTH_PARAM);
        addParam(param);
        addLabel(Vec(knobLeft - 8, yKnob - 28), "Length");

        auto param2 = createParam<RoundBigBlackSnapKnob>(Vec(knobLeft + 22 + dx2, yKnob), module, Comp::BEATS_PARAM);
        addParam(param2);
        addLabel(Vec(knobLeft + 22 + dx2, yKnob - 28), "Beats");

        float yHold = 70;
        addParam(createParam<CKSS>(Vec(knobLeft + 30, yHold + 10), module, Comp::HOLD_PARAM));
        addLabel(Vec(knobLeft + 46, yHold + 9), "Hold");
        addInputL(Vec(10, yHold + 8), Comp::HOLD_INPUT, "");
    }

    void addOutputL(const Vec& vec, int outputNumber, const std::string& text) {
        // assert(module);
        addOutput(createOutput<PJ301MPort>(vec, module, outputNumber));
        Vec vlabel(vec.x, vec.y);
        vlabel.y -= 20;
        const float xOffset = -4 + text.size() * 2.5;  // crude attempt to center text.
        vlabel.x -= xOffset;
        addLabel(vlabel, text);
    }

    void addInputL(const Vec& vec, int outputNumber, const std::string& text = "") {
        // assert(module);
        addInput(createInput<PJ301MPort>(vec, module, outputNumber));
        if (!text.empty()) {
            Vec vlabel(vec.x, vec.y);
            vlabel.y -= 20;
            const float xOffset = -4 +text.size() * 2.5;  // crude attempt to center text.
            vlabel.x -= xOffset;
            addLabel(vlabel, text);
        }
    }

    Label* addLabel(const Vec& v, const std::string& str) {
        // NVGcolor white = nvgRGB(0, 0, 0);
        NVGcolor white = nvgRGB(0xf0, 0xF0, 0xF0);

        Label* label = new Label();
        label->box.pos = v;
        label->text = str;
        label->color = white;
      //  SQINFO("label size = %f\n", label->fontSize);
        label->fontSize = 12;       // 13 is default
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
        item->text = "Gate Delay";
        theMenu->addChild(item);
    }
};

Model* modelArpeggiator1 = createModel<Arpeggiator1Module, Arpeggiator1Widget>("sqh-arpeggiator1");