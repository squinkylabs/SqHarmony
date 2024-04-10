
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

        this->configParam(Comp::SCORE_COLOR_PARAM, 0, 1, 1, "Invert Score colors");
        this->configParam(Comp::SCORE_GLOW_PARAM, 0, 1, 0, "Score Glow");
        this->configParam(Comp::SCHEMA_PARAM, 0, 1, 0, "hidden schema");

        class ModeParam : public ParamQuantity {
        public:
            std::string getDisplayValueString() override {
                int value = int((std::round(getValue())));
                return ((value >= 0) && value < int(labels.size())) ? labels[value] : "";
            }

        private:
            std::vector<std::string> labels = Scale::getScaleLabels(true);
        };

        class RootParam : public ParamQuantity {
        public:
            std::string getDisplayValueString() override {
                // TODO: replace with new poll
                const bool useFlats = module->params[Comp::SHARPS_FLATS_PARAM].value > .5;
                const auto labels = Scale::getRootLabels(useFlats);
                const int value = int((std::round(getValue())));
                return ((value >= 0) && value < int(labels.size())) ? labels[value] : "";
            }
        };

        this->configParam<RootParam>(Comp::KEY_PARAM, 0, 11, 0, "Key Root");
        this->configParam<ModeParam>(Comp::MODE_PARAM, 0, 6, 0, "Diatonic Mode");
        this->configParam(Comp::RETRIGGER_CV_AND_NOTE_PARAM, 0, 1, 1, "Retrigger CV and Note");
        this->configParam(Comp::HISTORY_SIZE_PARAM, 0, 4, 0, "History Size");
        this->configParam(Comp::TRANSPOSE_STEPS_PARAM, -7, 7, 0, "Transpose degrees");
        this->configParam(Comp::TRIGGER_DELAY_PARAM, 0, 1, 1, "Trigger delay");
        this->configParam(Comp::SHARPS_FLATS_PARAM, 0, 1, 0, "Use Flats");

        this->configSwitch(Comp::INVERSION_PREFERENCE_PARAM, 0, 2, 0, "Inversion preference", {"Don't care", "Discourage Consecutive", "Discourage Inversions"});
        this->configSwitch(Comp::CENTER_PREFERENCE_PARAM, 0, 2, 2, "Centered preference", {"None (wide)", "Encourage Center", "Narrow (vocal range)"});
        this->configSwitch(Comp::NNIC_PREFERENCE_PARAM, 0, 1, 1, "No Notes in Common rule", {"Disabled", "Enabled"});

        this->configOutput(Comp::BASS_OUTPUT, "Bass voice pitch");
        this->configOutput(Comp::TENOR_OUTPUT, "Tenor voice pitch");
        this->configOutput(Comp::ALTO_OUTPUT, "Alto voice pitch");
        this->configOutput(Comp::SOPRANO_OUTPUT, "Soprano voice pitch");

        this->configInput(Comp::CV_INPUT, "Chord root scale degree");
        this->configInput(Comp::TRIGGER_INPUT, "Trigger (affected by menu settings)");
        this->configInput(Comp::PES_INPUT, "External Polyphonic Scale");
    }

    using Chord = Comp::Chord;
    bool isChordAvailable() const { return comp->isChordAvailable(); }
    Chord getChord() { return comp->getChord(); }
    ConstScalePtr getScale() const { return comp->getScale(); }

    void process(const ProcessArgs& args) override {
        comp->process(args);
    }
};
