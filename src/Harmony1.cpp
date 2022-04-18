#include "Harmony.h"
#include "Harmony1Module.h"
#include "ParamSelectorMenu.h"
#include "PopupMenuParamWidget.h"
#include "Score.h"
#include "SqMenuItem.h"
#include "SqStream.h"
#include "WidgetComposite.h"
#include "plugin.hpp"

struct Harmony1Widget : ModuleWidget {
    const float vlx = 10;
    const float vdelta = 30;
    const float vy = 320;
    Score* _score = nullptr;
    Label* voiceLabels[4] = {};
    int voicesLastTime[4] = {};
    Harmony1Module* const hmodule;
    void addScore(Harmony1Module* module);

    Harmony1Widget(Harmony1Module* module) : hmodule(module) {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/blank-panel-5.svg")));

#if 0
        auto svg = APP->window->loadSvg(asset::plugin(pluginInstance, "res/pattern-four.svg"));
        SvgWidget* logo = new SvgWidget();
        logo->setSvg(svg);
        addChild(logo);
#endif

        addLabel(Vec(28, 5), "Harmony");

        addInputL(Vec(vlx, 280), Comp::CV_INPUT, "Root");
        addScore(module);
        addInputL(Vec(vlx + 1 * vdelta, 280), Comp::TRIGGER_INPUT, "Trig");
        // addScore(module);

        addKeysig();
        addOutputs();

        float stagger = 30;
        float ySwitch = 170;
        float xx = vlx;
        //   const float switchInset = 36;

        addParam(createParam<CKSSThree>(Vec(xx, ySwitch), module, Comp::INVERSION_PREFERENCE_PARAM));
        // addLabel(Vec(xx - 30, ySwitch), "Inv Pref");

        // ySwitch += 34;
        xx += stagger;
        addParam(createParam<CKSSThree>(Vec(xx, ySwitch), module, Comp::CENTER_PREFERENCE_PARAM));
        // addLabel(Vec(xx - 30, ySwitch), "Cent Pref");

        // ySwitch += 34;
        xx += stagger;
        addParam(createParam<CKSS>(Vec(xx, ySwitch), module, Comp::NNIC_PREFERENCE_PARAM));
        //  addLabel(Vec(xx - 30, ySwitch), "NNIC Rule");

        // RoundBlackSnapKnob
        auto param = createParam<RoundBlackSnapKnob>(Vec(90, 240), module, Comp::TRANSPOSE_STEPS_PARAM);
        addParam(param);
        addLabel(Vec(77, 212), "X-pose");
    }

    void addOutputs() {
        addOutputL(Vec(vlx + 0 * vdelta, vy), Comp::BASS_OUTPUT, "B");
        addOutputL(Vec(vlx + 1 * vdelta, vy), Comp::TENOR_OUTPUT, "T");
        addOutputL(Vec(vlx + 2 * vdelta, vy), Comp::ALTO_OUTPUT, "A");
        addOutputL(Vec(vlx + 3 * vdelta, vy), Comp::SOPRANO_OUTPUT, "S");

        const float labelY = vy + 24;
        voiceLabels[0] = addLabel(Vec(vlx + 0 * vdelta, labelY), "");
        voiceLabels[1] = addLabel(Vec(vlx + 1 * vdelta, labelY), "");
        voiceLabels[2] = addLabel(Vec(vlx + 2 * vdelta, labelY), "");
        voiceLabels[3] = addLabel(Vec(vlx + 3 * vdelta, labelY), "");
    }

    void addKeysig() {
        const float yScale = 136;  // was 140
        const float yMode = yScale;

        PopupMenuParamWidget* p = createParam<PopupMenuParamWidget>(
            Vec(7, yScale),
            module,
            Comp::KEY_PARAM);
        p->setLabels(Scale::getRootLabels());
        p->box.size.x = 40;  // width
        p->box.size.y = 22;
        p->text = "C";
        addParam(p);

        p = createParam<PopupMenuParamWidget>(
            Vec(57, yMode),
            module,
            Comp::MODE_PARAM);
        p->setShortLabels(Scale::getShortScaleLabels(true));
        p->setLabels(Scale::getScaleLabels(true));

        p->box.size.x = 70;  // width
        p->box.size.y = 22;
        p->text = "Maj";
        addParam(p);
    }

    void appendContextMenu(Menu* theMenu) override {
        // MenuLabel* spacerLabel = new MenuLabel();
        theMenu->addChild(new MenuLabel());
        if (module) {
            // std::vector<std::string> labels = {"off", "4", "8", "13"};
            std::vector<std::string> labels = Comp::getHistoryLabels();
            float initValue = module->paramQuantities[Comp::HISTORY_SIZE_PARAM]->getValue();
            int intValue = int(initValue);
            SqStream s;
            s.add("Repetition avoidance ");
            s.add(labels[intValue]);

            auto psm = new ParamSelectorMenu(s.str(),
                                             labels,
                                             module,
                                             Comp::HISTORY_SIZE_PARAM);
            theMenu->addChild(psm);
        }

        SqMenuItem_BooleanParam2* item = new SqMenuItem_BooleanParam2(module, Comp::SCORE_COLOR_PARAM);
        item->text = "Black notes on white paper";
        theMenu->addChild(item);

        item = new SqMenuItem_BooleanParam2(module, Comp::RETRIGGER_CV_AND_NOTE_PARAM);
        item->text = "Retrig. on notes and CV";
        theMenu->addChild(item);

        item = new SqMenuItem_BooleanParam2(module, Comp::TRIGGER_DELAY_PARAM);
        item->text = "Trigger delay";
        theMenu->addChild(item);
    }

    void step() override {
        ModuleWidget::step();
        if (module) {
            // process the menu check ite,
            bool whiteOnBlack = APP->engine->getParamValue(module, Comp::SCORE_COLOR_PARAM) < .5;
            _score->setWhiteOnBlack(whiteOnBlack);

            // process the voice indicators
            for (int i = 0; i < 4; ++i) {
                const int ch = hmodule->comp->getOutputChannels(i);
                if (ch != voicesLastTime[i]) {
                    voicesLastTime[i] = ch;
                    std::string newLabel;
                    if (ch > 0) {
                        SqStream str;
                        str.add(ch);
                        newLabel = str.str();
                    }
                    voiceLabels[i]->text = newLabel;
                }
            }
        }
    }

    void
    addOutputL(const Vec& vec, int outputNumber, const std::string& text) {
        addOutput(createOutput<PJ301MPort>(vec, module, outputNumber));
        Vec vlabel(vec.x, vec.y);
        vlabel.y -= 18;
        const float xOffset = -2 + text.size() * 2.5;  // crude attempt to center text.
        vlabel.x -= xOffset;
        addLabel(vlabel, text);
    }

    void addInputL(const Vec& vec, int outputNumber, const std::string& text) {
        addInput(createInput<PJ301MPort>(vec, module, outputNumber));
        Vec vlabel(vec.x, vec.y);
        vlabel.y -= 20;
        const float xOffset = -2 + text.size() * 2.5;  // crude attempt to center text.
        vlabel.x -= xOffset;
        addLabel(vlabel, text);
    }

    Label* addLabel(const Vec& v, const std::string& str) {
        NVGcolor white = nvgRGB(0xe0, 0xe0, 0xe0);
        Label* label = new Label();
        label->box.pos = v;
        label->text = str;
        label->color = white;
        addChild(label);
        return label;
    }
};

void Harmony1Widget::addScore(Harmony1Module* module) {
    _score = new Score(module);
    auto size = Vec(120, 97);  // was 100
    auto vu = new BufferingParent(_score, size, _score);

    vu->box.pos = Vec(7, 26),
    addChild(vu);
}

Model* modelHarmony1 = createModel<Harmony1Module, Harmony1Widget>("sqh-harmony1");