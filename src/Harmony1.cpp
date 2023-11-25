#include "Harmony.h"
#include "Harmony1Module.h"
#include "ParamSelectorMenu.h"
#include "PopupMenuParamWidget.h"
#include "Score.h"
#include "SqLabel.h"
#include "SqMenuItem.h"
#include "SqStream.h"
#include "WidgetComposite.h"
#include "plugin.hpp"

template <>
int BufferingParent<Score>::_refCount = 0;

struct Harmony1Widget : ModuleWidget {
    const float vlx = 10;
    const float vdelta = 30;
    const float vy = 320;
    Score* _score = nullptr;
    SqLabel* voiceLabels[4] = {};
    int voicesLastTime[4] = {};
    Harmony1Module* const hmodule;
    PopupMenuParamWidget* _keyRootWidget = nullptr;
    void addScore(Harmony1Module* module);

    Harmony1Widget(Harmony1Module* module) : hmodule(module) {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/harmony.svg")));

#ifdef _LAB
        addLabel(Vec(28, 5), "Harmony");
#endif
        addInputL(Vec(34, 257.76), Comp::CV_INPUT, "Root");
        addScore(module);
        addInputL(Vec(93, 257.79), Comp::TRIGGER_INPUT, "Trig");

        addKeysig();
        addOutputs();

        float ySwitch = 187;

        addParam(createParam<CKSSThree>(Vec(14, ySwitch), module, Comp::INVERSION_PREFERENCE_PARAM));
        addParam(createParam<CKSSThree>(Vec(38, ySwitch), module, Comp::CENTER_PREFERENCE_PARAM));
        addParam(createParam<CKSS>(Vec(66, ySwitch), module, Comp::NNIC_PREFERENCE_PARAM));

        // RoundBlackSnapKnob
        auto param = createParam<RoundBlackSnapKnob>(Vec(102, 190), module, Comp::TRANSPOSE_STEPS_PARAM);
        addParam(param);
#ifdef _LAB
        addLabel(Vec(77, 212), "X-pose");
#endif
    }

    void addOutputs() {
        const float y = 317;
        addOutputL(Vec(19, y), Comp::BASS_OUTPUT, "B");
        addOutputL(Vec(49, y), Comp::TENOR_OUTPUT, "T");
        addOutputL(Vec(79, y), Comp::ALTO_OUTPUT, "A");
        addOutputL(Vec(109, y), Comp::SOPRANO_OUTPUT, "S");

        const float labelY = 335;
        voiceLabels[0] = addLabel(Vec(19, labelY), "");
        voiceLabels[1] = addLabel(Vec(49, labelY), "");
        voiceLabels[2] = addLabel(Vec(79, labelY), "");
        voiceLabels[3] = addLabel(Vec(109, labelY), "");
        for (int i = 0; i < 4; ++i) {
            voiceLabels[i]->fontSize = 11;
        }
    }

    void addKeysig() {
        const float yScale = 135;
        const float yMode = yScale;

        PopupMenuParamWidget* p = createParam<PopupMenuParamWidget>(
            Vec(8, yScale),
            module,
            Comp::KEY_PARAM);
        p->setLabels(Scale::getRootLabels(false));  // just default to sharps, we will change. TODO: do it right.
        p->box.size.x = 40;                         // width
        p->box.size.y = 22;
        p->text = "C";
        addParam(p);
        _keyRootWidget = p;  // remember this so we can poll it.

        p = createParam<PopupMenuParamWidget>(
            Vec(74, yMode),
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
        theMenu->addChild(new MenuLabel());

        if (module) {
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

        // add the sharps/flats
        if (module) {
            SqMenuItem_BooleanParam2* item = new SqMenuItem_BooleanParam2(module, Comp::USE_FLATS_PARAM);
            item->text = "Flats/not sharps";
            theMenu->addChild(item);
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

    void _processUseSharpsParam() {
        const bool useFlats = APP->engine->getParamValue(module, Comp::USE_FLATS_PARAM) > .5;
        auto cSharp = _keyRootWidget->getShortLabel(1);

        auto iter = cSharp.find("#");
        const bool isUsingFlats = iter == std::string::npos;

        if (useFlats != isUsingFlats) {
            INFO("changing sharps/flats, now useFlats=%d isUsingFlats= %d cSharp=%s", useFlats, isUsingFlats, cSharp.c_str());
            _keyRootWidget->setLabels(Scale::getRootLabels(useFlats));
            // _score->setUseFlats(useFlats);
        }
    }

    void step() override {
        ModuleWidget::step();
        if (module) {
            // process the menu check item for color
            const bool whiteOnBlack = APP->engine->getParamValue(module, Comp::SCORE_COLOR_PARAM) < .5;
            _score->setWhiteOnBlack(whiteOnBlack);

            // process flats/sharps
            _processUseSharpsParam();

            // process the voice indicators
            for (int i = 0; i < 4; ++i) {
                const int ch = hmodule->comp->getOutputChannels(i);
                // INFO("chan[%d] = %d", i, ch);
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
#ifdef _LAB
        Vec vlabel(vec.x, vec.y);
        vlabel.y -= 18;
        const float xOffset = -2 + text.size() * 2.5;  // crude attempt to center text.
        vlabel.x -= xOffset;
        addLabel(vlabel, text);
#endif
    }

    void addInputL(const Vec& vec, int outputNumber, const std::string& text) {
        addInput(createInput<PJ301MPort>(vec, module, outputNumber));
#ifdef _LAB
        Vec vlabel(vec.x, vec.y);
        vlabel.y -= 20;
        const float xOffset = -2 + text.size() * 2.5;  // crude attempt to center text.
        vlabel.x -= xOffset;
        addLabel(vlabel, text);
#endif
    }

    SqLabel* addLabel(const Vec& v, const std::string& str) {
        NVGcolor white = nvgRGB(0xe0, 0xe0, 0xe0);
        SqLabel* label = new SqLabel();
        label->box.pos = v;
        label->text = str;
        label->color = white;
        addChild(label);
        return label;
    }
};

void Harmony1Widget::addScore(Harmony1Module* module) {
    _score = new Score(module);
    auto size = Vec(134, 100);
    auto vu = new BufferingParent<Score>(_score, size, _score);

    vu->box.pos = Vec(8, 28),
    addChild(vu);
}

Model* modelHarmony1 = createModel<Harmony1Module, Harmony1Widget>("sqh-harmony1");