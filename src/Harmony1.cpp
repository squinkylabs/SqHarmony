#include "Score.h"

#include "plugin.hpp"
#include "Harmony.h"
#include "WidgetComposite.h"
#include "Harmony1Module.h"

struct Harmony1Widget : ModuleWidget {
    Harmony1Widget(Harmony1Module* module) {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/blank-panel-4.svg")));

        addLabel(Vec(28, 5), "Harmony");
        addInputL(Vec(50, 220), Comp::CV_INPUT, "Root");

        const float vlx = 12;
        const float vdelta = 30;
        const float vy = 340;

        addOutputL(Vec(vlx + 0 * vdelta, vy), Comp::BASS_OUTPUT, "B");
        addOutputL(Vec(vlx + 1 * vdelta, vy), Comp::TENOR_OUTPUT, "T");
        addOutputL(Vec(vlx + 2 * vdelta, vy), Comp::ALTO_OUTPUT, "A");
        addOutputL(Vec(vlx + 3 * vdelta, vy), Comp::SOPRANO_OUTPUT, "S");

        addScore(module);
    }

    void addOutputL(const Vec& vec, int outputNumber, const std::string& text) {
        //assert(module);
        addOutput(createOutput<PJ301MPort>(vec, module, outputNumber));
        Vec vlabel(vec.x, vec.y);
        vlabel.y -= 20;
        const float xOffset = -4 + text.size() * 2.5;            // crude attempt to center text.
        vlabel.x -= xOffset;
        addLabel(vlabel, text);
    }

    void addInputL(const Vec& vec, int outputNumber, const std::string& text) {
        //assert(module);
        addInput(createInput<PJ301MPort>(vec, module, outputNumber));
        Vec vlabel(vec.x, vec.y);
        vlabel.y -= 20;
        const float xOffset = text.size() * 2.5;            // crude attempt to center text.
        vlabel.x -= xOffset;
        addLabel(vlabel, text);
    }

    Label* addLabel(const Vec& v, const std::string& str) {
       // NVGcolor black = nvgRGB(0, 0, 0);
         NVGcolor white = nvgRGB(0xe0, 0xe0, 0xe0);
        Label* label = new Label();
        label->box.pos = v;
        label->text = str;
        label->color = white;
        addChild(label);
        return label;
    }
    void addScore(Harmony1Module* module);
};


void Harmony1Widget::addScore(Harmony1Module *module) {
    auto vu = new Score(module);
    vu->box.size = Vec(120, 100);

    // 9 too far right
    vu->box.pos = Vec(7, 36),
    addChild(vu);
}

Model* modelHarmony1 = createModel<Harmony1Module, Harmony1Widget>("sqt-harmony1");