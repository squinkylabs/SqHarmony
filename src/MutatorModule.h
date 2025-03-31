
#pragma once

#include "Mutator.h"
#include "WidgetComposite.h"

using Comp = Mutator<WidgetComposite>;

class MutatorModule : public rack::engine::Module {
public:
    std::shared_ptr<Comp> comp = std::make_shared<Comp>(this);
    MutatorModule() {
        config(Comp::NUM_PARAMS, Comp::NUM_INPUTS, Comp::NUM_OUTPUTS, Comp::NUM_LIGHTS);

        this->configOutput(Comp::NOTES_OUTPUT, "Pitches");
        this->configInput(Comp::MUTATE_INPUT, "Mutate trigger");
        this->configInput(Comp::CENTER_VOLTAGE_INPUT, "Center voltage");

        this->configParam(Comp::KEY_PARAM, 0, 11, 0, "Key signature root");
        // TODO: what is the real number of modes?
        this->configParam(Comp::MODE_PARAM, 0, (int) Scale::Scales::Chromatic, 0, "Key signature mode");
        this->configParam(Comp::ROW_LENGTH_PARAM, 2, 32, 8, "Length");
    }

    void process(const ProcessArgs& args) override {
        comp->process(args);
    }
};