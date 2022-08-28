#include "Additive.h"

#include "WidgetComposite.h"
#include "plugin.hpp"
#include "rack.hpp"

using Comp = Additive<WidgetComposite>;
#define _LAB

class AdditiveModule : public Module {
public:
    AdditiveModule() {
        config(Comp::NUM_PARAMS, Comp::NUM_INPUTS, Comp::NUM_OUTPUTS, Comp::NUM_LIGHTS);
        addParams();
    }

    void process(const ProcessArgs& args) override {
        comp->process(args);
    }

private:
    void addParams();

private:
    std::shared_ptr<Comp> comp = std::make_shared<Comp>(this);
};

void AdditiveModule::addParams() {
    this->configParam(Comp::SCHEMA_PARAM, 0, 10, 0, "Schema");  // invisible - just for the future.
}

//---------------------------------
class AdditiveWidget : public ModuleWidget {
public:
    AdditiveWidget(AdditiveModule*& module) {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/blank-panel.svg")));
#ifdef _LAB
        addLabel(Vec(20, 12), "Additive", 24);
#endif
        addControls();
        addIO();
    }

private:
    void addControls();
    void addIO();
#ifdef _LAB
    Label* addLabel(const Vec& v, const std::string& str, float fontSize = 14) {
        NVGcolor white = nvgRGB(0xe0, 0xe0, 0xe0);
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

const int dx = 40;
const int dy = 60;
const float labelDy = 22;
const float y0 = 60;
const float x0 = 14;

Vec position(int row, int col) {
    float x = col * dx + x0;
    float y = (row * dy) + y0;
    return Vec(x, y);
}

Vec labelPos(int row, int col) {
    float x = col * dx + x0;
    float y = (row * dy) + y0 - labelDy;
    return Vec(x, y);
}

const float jackDx = 40;
const float jackX0 = 18;
const float jackY0 = 330;
const float jackLabelDy = 22;

Vec jackPosition(int row, int col) {
    float x = col * jackDx + jackX0;
    float y = jackY0;
    return Vec(x, y);
}

Vec jackLabelPosition(int row, int col) {
    float x = col * jackDx + jackX0;
    x -= 3;
    float y = jackY0 - jackLabelDy;
    return Vec(x, y);
}

void AdditiveWidget::addIO() {
    addInput(createInput<PJ301MPort>(jackPosition(0, 0), module, Comp::CV_INPUT));
    addLabel(jackLabelPosition(0, 0), "V/Oct");

    addOutput(createOutput<PJ301MPort>(jackPosition(0, 2), module, Comp::AUDIO_OUTPUT));
    addLabel(jackLabelPosition(0, 2), "Out");
}

void AdditiveWidget::addControls() {
    auto param = createParam<RoundBlackSnapKnob>(position(0, 0), module, Comp::OCTAVE_PARAM);
    addParam(param);
    addLabel(labelPos(0, 0), "oct");

    param = createParam<RoundBlackSnapKnob>(position(0, 1), module, Comp::SEMI_PARAM);
    addParam(param);
    addLabel(labelPos(0, 1), "semi");

    param = createParam<RoundBlackSnapKnob>(position(0, 2), module, Comp::FINE_PARAM);
    addParam(param);
    addLabel(labelPos(0, 2), "fine");

    param = createParam<RoundBlackSnapKnob>(position(1, 0), module, Comp::STRETCH_PARAM);
    addParam(param);
    addLabel(labelPos(1, 0), "strch");

    param = createParam<RoundBlackSnapKnob>(position(1, 1), module, Comp::EVENOFFSET_PARAM);
    addParam(param);
    addLabel(labelPos(1, 1), "e shft");

    param = createParam<RoundBlackSnapKnob>(position(1, 2), module, Comp::ODDOFFSET_PARAM);
    addParam(param);
    addLabel(labelPos(1, 2), "o shft");
}

rack::Model* modelAdditive = createModel<AdditiveModule, AdditiveWidget>("sqh-additive");