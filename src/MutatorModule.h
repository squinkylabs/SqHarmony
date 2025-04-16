
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
        this->configInput(Comp::INITIAL_VOLTAGE_INPUT, "Initial pitches");
        

        this->configParam(Comp::KEY_PARAM, 0, 11, 0, "Key signature root");
        // TODO: what is the real number of modes?
        this->configParam(Comp::MODE_PARAM, 0, (int) Scale::Scales::Chromatic, 0, "Key signature mode");
        this->configParam(Comp::ROW_LENGTH_PARAM, 2, 32, 8, "Length");

        this->configParam(Comp::NON_CENTERED_WEIGHT_STYLE_PARAM, 0, 1, .5, "force pitches to desired center");
        this->configParam(Comp::PITCH_RANGE_WEIGHT_STYLE_PARAM, 0, 1, .5, "spread pitches across desired range");
        this->configParam(Comp::LEAPS_WEIGHT_STYLE_PARAM, 0, 1, .5, "avoid melodic leaps");
        this->configParam(Comp::UNISON_WEIGHT_STYLE_PARAM, 0, 1, .5, "avoid repeating pitches");

        // 0 == all of them
        this->configParam(Comp::SLOTS_TO_CHANGE_PARAM, 0, 4, 1, "how many notes to change each time");
        this->configParam(Comp::ADJACENT_SLOTS_PARAM, 0, 1, 1, "when changing more than one note, should they be adjacent");        
    }

    void process(const ProcessArgs& args) override {
        comp->process(args);
    }
};