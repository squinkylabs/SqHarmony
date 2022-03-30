
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


     /*           INVERSION_PREFERENCE_PARAM,
        CENTER_PREFERENCE_PARAM,
        NNIC_PREFERENCE_PARAM,
        RETRIGGER_CV_AND_NOTE_PARAM, */
        this->configParam(Comp::SCORE_COLOR_PARAM, 0, 1, 0, "Invert Score colors");
        this->configParam(Comp::SCORE_GLOW_PARAM, 0, 1, 0, "Score Glow");
        this->configParam(Comp::SCHEMA_PARAM, 0, 1, 0, "hidden schema");
        this->configParam(Comp::KEY_PARAM, 0, 11, 0, "Key Root");
        this->configParam(Comp::MODE_PARAM, 0, 6, 0, "Diatonic Mode");
        this->configParam(Comp::RETRIGGER_CV_AND_NOTE_PARAM, 0, 1, 1, "Retrigger CV and Note");



        this->configSwitch(Comp::INVERSION_PREFERENCE_PARAM, 0, 2, 0, "Inversion preference", {"DONT_CARE", "DISCOURAGE_CONSECUTIVE", "DISCOURAGE"});
        this->configSwitch(Comp::CENTER_PREFERENCE_PARAM, 0, 2, 0, "Centered preference", {"None", "ENCOURAGE_CENTER", "NARROW_RANGE"});
        this->configSwitch(Comp::NNIC_PREFERENCE_PARAM, 0, 1, 1, "No Notes in Common rule", {"Disable", "enabled"});


        this->configOutput(Comp::BASS_OUTPUT, "Bass voice pitch");
        this->configOutput(Comp::TENOR_OUTPUT, "Tenor voice pitch");
        this->configOutput(Comp::ALTO_OUTPUT, "Alto voice pitch");
        this->configOutput(Comp::SOPRANO_OUTPUT, "Soprano voice pitch");

        this->configInput(Comp::CV_INPUT, "Chord root scale degree");
        this->configInput(Comp::TRIGGER_INPUT, "Trigger");
    }

    using Chord = Comp::Chord;
    bool isChordAvailable() const { return comp->isChordAvailable(); }
    Chord getChord() { return comp->getChord(); }
    ConstScalePtr getScale() const { return comp->getScale(); }

    void process(const ProcessArgs& args) override {
        comp->process(args);
    }
};
