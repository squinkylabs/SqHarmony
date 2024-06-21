
#pragma once

#include "Visualizer.h"
#include "WidgetComposite.h"

using Comp = Visualizer<WidgetComposite>;

class VisualizerModule : public rack::engine::Module {
public:
    std::shared_ptr<Comp> comp = std::make_shared<Comp>(this);
    VisualizerModule() {
        config(Comp::NUM_PARAMS, Comp::NUM_INPUTS, Comp::NUM_OUTPUTS, Comp::NUM_LIGHTS);
        // addParams();
        this->configParam(Comp::KEY_PARAM, 0, 11, 0, "Key Root");
        this->configParam(Comp::MODE_PARAM, 0, 6, 0, "Diatonic Mode");
    }
    void process(const ProcessArgs& args) override {
        comp->process(args);
    }

    ConstScalePtr getScale() const { return comp->getScale(); }
    std::tuple<const int *, unsigned> getQuantizedPitchesAndChannels() const { return comp->getQuantizedPitchesAndChannels(); }

    unsigned getChangeParam() {
        return unsigned(APP->engine->getParamValue(this, Comp::CHANGE_PARAM));
    }
};