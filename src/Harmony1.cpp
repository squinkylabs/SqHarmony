#include "Harmony.h"
#include "Harmony1Module.h"
#include "PopupMenuParamWidget.h"
#include "Score.h"
#include "SqMenuItem.h"
#include "WidgetComposite.h"
#include "plugin.hpp"

struct Harmony1Widget : ModuleWidget {
    Harmony1Widget(Harmony1Module* module) {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/blank-panel-4.svg")));

#if 1
        auto svg = APP->window->loadSvg(asset::plugin(pluginInstance, "res/pattern-four.svg"));
        SvgWidget* logo = new SvgWidget();
        logo->setSvg(svg);
        addChild(logo);
#endif

        addLabel(Vec(28, 5), "Harmony");
       
        const float vlx = 12;
        const float vdelta = 30;
        const float vy = 320;

        addOutputL(Vec(vlx + 0 * vdelta, vy), Comp::BASS_OUTPUT, "B");
        addOutputL(Vec(vlx + 1 * vdelta, vy), Comp::TENOR_OUTPUT, "T");
        addOutputL(Vec(vlx + 2 * vdelta, vy), Comp::ALTO_OUTPUT, "A");
        addOutputL(Vec(vlx + 3 * vdelta, vy), Comp::SOPRANO_OUTPUT, "S");

        addInputL(Vec(vlx, 280), Comp::CV_INPUT, "Root");
        addScore(module);

        addKeysig();

        float ySwitch = 176;
         const float xx = 38;
        addParam(createParam<CKSSThree>(Vec(xx + 50, ySwitch), module, Comp::INVERSION_PREFERENCE_PARAM));
        addLabel(Vec(xx - 30, ySwitch), "Inv. Pref");

        ySwitch += 30;
        addParam(createParam<CKSSThree>(Vec(xx + 50, ySwitch), module, Comp::CENTER_PREFERENCE_PARAM));
        addLabel(Vec(xx - 30, ySwitch), "Cent Pref");

        ySwitch += 30;
        addParam(createParam<CKSS>(Vec(xx + 50, ySwitch), module, Comp::NNIC_PREFERENCE_PARAM));
        addLabel(Vec(xx - 30, ySwitch), "NNIC rule");
    }

    void addKeysig() {
        const float yScale = 140;
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
        MenuLabel* spacerLabel = new MenuLabel();
        theMenu->addChild(spacerLabel);

        SqMenuItem_BooleanParam2* item = new SqMenuItem_BooleanParam2(module, Comp::SCORE_COLOR_PARAM);
        item->text = "Black notes on white paper";
        theMenu->addChild(item);
    }

    void step() override {
        ModuleWidget::step();
        if (module) {
            bool whiteOnBlack = APP->engine->getParamValue(module, Comp::SCORE_COLOR_PARAM) < .5;
            _score->setWhiteOnBlack(whiteOnBlack);
        }
    }

    void addOutputL(const Vec& vec, int outputNumber, const std::string& text) {
        addOutput(createOutput<PJ301MPort>(vec, module, outputNumber));
        Vec vlabel(vec.x, vec.y);
        vlabel.y -= 20;
        const float xOffset = -4 + text.size() * 2.5;  // crude attempt to center text.
        vlabel.x -= xOffset;
        addLabel(vlabel, text);
    }

    void addInputL(const Vec& vec, int outputNumber, const std::string& text) {
        addInput(createInput<PJ301MPort>(vec, module, outputNumber));
        Vec vlabel(vec.x, vec.y);
        vlabel.y -= 20;
        const float xOffset = text.size() * 2.5;  // crude attempt to center text.
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
    void addScore(Harmony1Module* module);
    Score* _score = nullptr;
};

void Harmony1Widget::addScore(Harmony1Module* module) {
    _score = new Score(module);
    auto size = Vec(120, 100);
    auto vu = new BufferingParent(_score, size, _score);

    vu->box.pos = Vec(7, 26),
    addChild(vu);
}

Model* modelHarmony1 = createModel<Harmony1Module, Harmony1Widget>("sqh-harmony1");