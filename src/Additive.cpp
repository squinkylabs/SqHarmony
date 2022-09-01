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
    this->configParam(Comp::OCTAVE_PARAM, -4, 4, 0, "Octave");
    this->configParam(Comp::SEMI_PARAM, -12, 12, 0, "Semitone");
    this->configParam(Comp::FINE_PARAM, -1, 1, 0, "fine tune", " semitones");
    this->configParam(Comp::STRETCH_PARAM, -1, 1, 0, "stretch tuning");

    this->configParam(Comp::EVENOFFSET_PARAM, -1, 1, 0, "pitch offset even harmonics", " semitones");
    this->configParam(Comp::ODDOFFSET_PARAM, -1, 1, 0, "pitch offset odd harmonics", " semitones");

    this->configParam(Comp::EVENLEVEL_PARAM, 0, 1, 1, "even harmonic level");
    this->configParam(Comp::ODDLEVEL_PARAM, 0, 1, 1, "odd harmonic level");
    this->configParam(Comp::SLOPE_PARAM, 0, 1, 1, "harmonic roll-off");

    for (unsigned i = 0; i < Comp::numHarmonics; ++i) {
        char buffer[256];
        sprintf(buffer, "harmonic %d level", i);
        this->configParam(Comp::H0_PARAM + i, 0, 1, 1, buffer);
    }
}

//---------------------------------
class AdditiveWidget : public ModuleWidget {
public:
    AdditiveWidget(AdditiveModule*& module) {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/blank_panel.svg")));
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
        // NVGcolor white = nvgRGB(0xe0, 0xe0, 0xe0);
        NVGcolor black = nvgRGB(0, 0, 0);
        Label* label = new Label();
        auto adjustedPos = v;
        adjustedPos.x -= 1.5f * str.size();
        label->box.pos = adjustedPos;
        label->text = str;
        label->color = black;
        label->fontSize = fontSize;
        addChild(label);
        return label;
    }
#endif
};

const float columnSpace = 10;
const int dx = 40;
const int dy = 60;
const float labelDy = 22;
const float y0 = 60;
const float x0 = 14;

Vec position(int row, int col) {
    float x = col * dx + x0;
    if (col > 2) {
        x += columnSpace;
    }
    float y = (row * dy) + y0;
    return Vec(x, y);
}

Vec labelPos(int row, int col) {
    float x = col * dx + x0;
    if (col > 2) {
        x += columnSpace;
    }
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
    ParamWidget* param = createParam<RoundBlackSnapKnob>(position(0, 0), module, Comp::OCTAVE_PARAM);
    addParam(param);
    addLabel(labelPos(0, 0), "oct");

    param = createParam<RoundBlackSnapKnob>(position(0, 1), module, Comp::SEMI_PARAM);
    addParam(param);
    addLabel(labelPos(0, 1), "semi");

    param = createParam<RoundBlackKnob>(position(0, 2), module, Comp::FINE_PARAM);
    addParam(param);
    addLabel(labelPos(0, 2), "fine");

    param = createParam<RoundBlackKnob>(position(1, 0), module, Comp::STRETCH_PARAM);
    addParam(param);
    addLabel(labelPos(1, 0), "strch");

    param = createParam<RoundBlackKnob>(position(1, 1), module, Comp::EVENOFFSET_PARAM);
    addParam(param);
    addLabel(labelPos(1, 1), "e shft");

    param = createParam<RoundBlackKnob>(position(1, 2), module, Comp::ODDOFFSET_PARAM);
    addParam(param);
    addLabel(labelPos(1, 2), "o shft");

    param = createParam<RoundBlackKnob>(position(0, 3), module, Comp::SLOPE_PARAM);
    addParam(param);
    addLabel(labelPos(0, 3), "slope");

    param = createParam<RoundBlackKnob>(position(0, 4), module, Comp::EVENLEVEL_PARAM);
    addParam(param);
    addLabel(labelPos(0, 4), "even");

    param = createParam<RoundBlackKnob>(position(0, 5), module, Comp::ODDLEVEL_PARAM);
    addParam(param);
    addLabel(labelPos(0, 5), "odd");

    const float yHarm = 160;
    const float xHarm0 = 18;
    const float dxHarm = 20;
    const float dyHarm = 24;

    for (unsigned i = 0; i < Comp::numHarmonics; ++i) {
        param = createParam<Trimpot>(Vec(xHarm0 + dxHarm * i, yHarm), module, Comp::H0_PARAM + i);
        addParam(param);

        param = createParam<Trimpot>(Vec(xHarm0 + dxHarm * i, yHarm+dyHarm), module, Comp::MA0_PARAM + i);
        addParam(param);

        param = createParam<Trimpot>(Vec(xHarm0 + dxHarm * i, yHarm+ 2* dyHarm), module, Comp::MB0_PARAM + i);
        addParam(param);

          param = createParam<Trimpot>(Vec(xHarm0 + dxHarm * i, yHarm+ 3* dyHarm), module, Comp::MC0_PARAM + i);
        addParam(param);

          param = createParam<Trimpot>(Vec(xHarm0 + dxHarm * i, yHarm+ 4* dyHarm), module, Comp::MD0_PARAM + i);
        addParam(param);

          param = createParam<Trimpot>(Vec(xHarm0 + dxHarm * i, yHarm+ 5* dyHarm), module, Comp::ME0_PARAM + i);
        addParam(param);
    }
}

rack::Model* modelAdditive = createModel<AdditiveModule, AdditiveWidget>("sqh-additive");