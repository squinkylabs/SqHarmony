
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
        this->configParam(Comp::SHARPS_FLATS_PARAM, 0, 3, 0, "Sharps/Flats");

        this->configOutput(Comp::PES_OUTPUT, "Scale (PES)");

        this->configInput(Comp::CV_INPUT, "Visualizer Pitch");
        this->configInput(Comp::GATE_INPUT, "Optional Gate");
        this->configInput(Comp::PES_INPUT, "Scale (PES)");
    }

    void process(const ProcessArgs& args) override {
        comp->process(args);
    }

    ConstScalePtr getScale() const { return comp->getScale(); }
    std::tuple<const int*, unsigned> getQuantizedPitchesAndChannels() const { return comp->getQuantizedPitchesAndChannels(); }

    unsigned getChangeParam() {
        return unsigned(APP->engine->getParamValue(this, Comp::CHANGE_PARAM));
    }

    std::shared_ptr<Comp> getComp() const {
        return comp;
    }
};