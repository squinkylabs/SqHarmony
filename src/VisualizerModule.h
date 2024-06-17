
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
    }
    void process(const ProcessArgs& args) override {
        comp->process(args);
    }

    ConstScalePtr getScale() const { return comp->getScale(); }
};