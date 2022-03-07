
#pragma once

#include "Harmony.h"
#include "WidgetComposite.h"
#include "plugin.hpp"

using Comp = Harmony<WidgetComposite>;

class Harmony1Module : public Module {
public:
    std::shared_ptr<Comp> comp = std::make_shared<Comp>(this);

    Harmony1Module() {
        config(Comp::NUM_PARAMS, Comp::NUM_INPUTS, Comp::NUM_OUTPUTS, Comp::NUM_LIGHTS);
        // configParam(PITCH_PARAM, 0, 10, 4, "Initial Pitch");
        this->configParam(Comp::SCORE_COLOR_PARAM, 0, 1, 0, "Invert Score colors");
        this->configParam(Comp::SCORE_GLOW_PARAM, 0, 1, 0, "Score Glow");
        this->configParam(Comp::SCHEMA_PARAM, 0, 1, 0, "hidden schema");
        this->configParam(Comp::KEY_PARAM, 0, 0, 0, "Key: not implemented");
        this->configParam(Comp::MODE_PARAM, 0, 0, 0, "Mode: not implemented");


        this->configSwitch(Comp::INVERSION_PREFERENCE_PARAM, 0, 2, 0, "Inversion preference", {"None", "A little", "A lot"});
        this->configSwitch(Comp::CENTER_PREFERENCE_PARAM, 0, 2, 0, "Centered preference", {"None", "A little", "A lot"});
        this->configSwitch(Comp::NNIC_PREFERENCE_PARAM, 0, 1, 1, "No Notes in Common rule", {"Disable", "enabled"});


        this->configOutput(Comp::BASS_OUTPUT, "Bass voice pitch");
        this->configOutput(Comp::TENOR_OUTPUT, "Tenor voice pitch");
        this->configOutput(Comp::ALTO_OUTPUT, "Alto voice pitch");
        this->configOutput(Comp::SOPRANO_OUTPUT, "Soprano voice pitch");

        this->configInput(Comp::CV_INPUT, "Chord root scale degree");
    }

    using Chord = Comp::Chord;
    bool isChordAvailable() const { return comp->isChordAvailable(); }
    Chord getChord() { return comp->getChord(); }

    void process(const ProcessArgs& args) override {
        comp->process(args);
    }
};
