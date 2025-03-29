
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
    }

    void process(const ProcessArgs& args) override {
        comp->process(args);
    }
};