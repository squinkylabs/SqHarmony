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
    }

private:
    void addControls();
#ifdef _LAB
    Label* addLabel(const Vec& v, const std::string& str, float fontSize = 14) {
        NVGcolor white = nvgRGB(0xe0, 0xe0, 0xe0);
        Label* label = new Label();
        label->box.pos = v - 1.5f * str.size();
        label->text = str;
        label->color = white;
        label->fontSize = fontSize;
        addChild(label);
        return label;
    }
#endif
};

Vec position(int row, int col) {
    float x = col * 37 + 20;
    float y = row * 30 + 50;
    return Vec(x, y);
}

Vec labelPos(int row, int col) {
    float x = col * 37 + 20;
    float y = row * 30 + 30;
    return Vec(x, y);
}

void AdditiveWidget::addControls() {
    auto param = createParam<RoundBlackSnapKnob>(position(0,0), module, Comp::OCTAVE_PARAM);
    addParam(param);
    addLabel(labelPos(0,0), "oct");

    param = createParam<RoundBlackSnapKnob>(position(0,1), module, Comp::SEMI_PARAM);
    addParam(param);
    addLabel(labelPos(0,1), "semi");

}

rack::Model* modelAdditive = createModel<AdditiveModule, AdditiveWidget>("sqh-additive");