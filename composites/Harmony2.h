#pragma once

#include <cmath>
#include <string>
#include <tuple>
#include <vector>

#include "Divider.h"
#include "FloatNote.h"
#include "GateTrigger.h"
#include "NoteConvert.h"
#include "ScaleNote.h"
#include "ScaleQuantizer.h"
#include "SqLog.h"

#define NUM_TRANPOSERS 6

namespace rack {
namespace engine {
struct Module;
}
}  // namespace rack
using Module = ::rack::engine::Module;

template <class TBase>
class Harmony2 : public TBase {
public:
    Harmony2(Module* module) : TBase(module) {
        this->_init();
    }

    Harmony2() : TBase() {
        this->_init();
    }

    static std::vector<std::string> getTransposeOctaveLabels();
    static std::vector<std::string> getTransposeDegreeLabels();

    enum ParamIds {
        XPOSE_DEGREE1_PARAM,
        XPOSE_DEGREE2_PARAM,
        XPOSE_DEGREE3_PARAM,
        XPOSE_DEGREE4_PARAM,
        XPOSE_DEGREE5_PARAM,
        XPOSE_DEGREE6_PARAM,
        XPOSE_OCTAVE1_PARAM,
        XPOSE_OCTAVE2_PARAM,
        XPOSE_OCTAVE3_PARAM,
        XPOSE_OCTAVE4_PARAM,
        XPOSEO_OCTAVE5_PARAM,
        XPOSE_OCTAVE6_PARAM,
        XPOSE_ENABLE1_PARAM,
        XPOSE_ENABLE2_PARAM,
        XPOSE_ENABLE3_PARAM,
        XPOSE_ENABLE4_PARAM,
        XPOSE_ENABLE5_PARAM,
        XPOSE_ENABLE6_PARAM,
        XPOSE_TOTAL1_PARAM,
        XPOSE_TOTAL2_PARAM,
        XPOSE_TOTAL3_PARAM,
        XPOSE_TOTAL4_PARAM,
        XPOSE_TOTAL5_PARAM,
        XPOSE_TOTAL6_PARAM,
        XPOSE_ENABLEREQ1_PARAM,
        XPOSE_ENABLEREQ2_PARAM,
        XPOSE_ENABLEREQ3_PARAM,
        XPOSE_ENABLEREQ4_PARAM,
        XPOSE_ENABLEREQ5_PARAM,
        XPOSE_ENABLEREQ6_PARAM,
        KEY_PARAM,
        MODE_PARAM,
        SHARPS_FLATS_PARAM,
        ONLY_USE_DIATONIC_PARAM,
        NUM_PARAMS
    };

    enum InputIds {
        //    DEBUG_IN,
        XPOSE_INPUT,
        KEY_INPUT,
        MODE_INPUT,
        PITCH_INPUT,
        NUM_INPUTS
    };

    enum OutputIds {
        PITCH_OUTPUT,
        NUM_OUTPUTS
    };

    enum LightIds {
        XPOSE_ENABLE1_LIGHT,
        XPOSE_ENABLE2_LIGHT,
        XPOSE_ENABLE3_LIGHT,
        XPOSE_ENABLE4_LIGHT,
        XPOSE_ENABLE5_LIGHT,
        XPOSE_ENABLE6_LIGHT,
        NUM_LIGHTS
    };

    void process(const typename TBase::ProcessArgs& args) override;

    static int getSubSampleFactor() { return 32; }

    // TODO: there all need to be dynamic
    // these will change when we implement ONLY_USE_DIATONIC_PARAM
    bool diatonicOnly() { return TBase::params[ONLY_USE_DIATONIC_PARAM].value > .5; }

    int numCurrentModes();
    // Does not include the octave/
    int numNotesInCurrentScale();

private:
    Divider _divn;
    GateTrigger _ButtonProcs[NUM_TRANPOSERS];
    ScaleQuantizerPtr _quantizer;
    ScaleQuantizer::OptionsPtr _quantizerOptions;

    void _init();
    void _stepn();
    void _servicePolyphony();
    void _serviceEnableButtons();
    void _serviceKeysigRootCV();
    void _serviceKeysigModeCV();
    void _serviceKeysigParam();
    void _serviceTranspose();
    void _serviceTranspose(int channel, int& outputChannel);
};

template <class TBase>
int Harmony2<TBase>::numCurrentModes() {
    return 12;
}
// Includes the octave
template <class TBase>
int Harmony2<TBase>::numNotesInCurrentScale() {
    const bool onlyDiatonic = diatonicOnly();
    if (onlyDiatonic) {
        return 8;       // all the diatonics have 7 notes + octave
    }

    return 8;
}

template <class TBase>
inline std::vector<std::string> Harmony2<TBase>::getTransposeDegreeLabels() {
    return {
        "0",
        "+1",
        "+2",
        "+3",
        "+4",
        "+5",
        "+6",
        "+7"};
}

template <class TBase>
inline std::vector<std::string> Harmony2<TBase>::getTransposeOctaveLabels() {
    return {
        "-2",
        "-1",
        "0",
        "+1",
        "+2",
    };
}

template <class TBase>
inline void Harmony2<TBase>::_init() {
    _divn.setup(Harmony2<TBase>::getSubSampleFactor(), [this]() {
        this->_stepn();
    });

    _quantizerOptions = std::make_shared<ScaleQuantizer::Options>();
    _quantizerOptions->scale = std::make_shared<Scale>();
    _quantizerOptions->scale->set(MidiNote::C, Scale::Scales::Major);
    _quantizer = std::make_shared<ScaleQuantizer>(_quantizerOptions);
}

template <class TBase>
inline void Harmony2<TBase>::_stepn() {
    _serviceEnableButtons();

    _serviceKeysigRootCV();
    _serviceKeysigModeCV();
    _serviceKeysigParam();
    _servicePolyphony();
}

template <class TBase>
inline void Harmony2<TBase>::_serviceKeysigParam() {
    const int basePitch = int(std::round(TBase::params[KEY_PARAM].value));
    const auto mode = Scale::Scales(int(std::round(TBase::params[MODE_PARAM].value)));
    // really only need to do this on a change.
    _quantizerOptions->scale->set(basePitch, mode);
    // SQINFO("just set ks base=%d", basePitch);
}

template <class TBase>
inline void Harmony2<TBase>::_serviceKeysigRootCV() {
    if (TBase::inputs[KEY_INPUT].channels == 0) {
        return;
    }

    const float newKeyF = TBase::inputs[KEY_INPUT].getVoltage(0);
    int newKeyScaledAndRounded = int(std::round(numCurrentModes() * newKeyF)) % numCurrentModes();
    if (newKeyScaledAndRounded < 0) {
        newKeyScaledAndRounded += numCurrentModes();
    }

    const Scale* oldKey = _quantizerOptions->scale.get();
    const auto x = oldKey->get();

    const MidiNote oldRoot = std::get<0>(x);
    FloatNote oldRootFloat;
    NoteConvert::m2f(oldRootFloat, oldRoot);
    const float oldKeyCV = oldRootFloat.get() + 6;  // I don't remember what this offset is...
    const float oldKeyVSCaled = oldKeyCV * numCurrentModes();
    const int oldKeyScaledAndRounded = int(std::round(oldKeyVSCaled));
    if (oldKeyScaledAndRounded != newKeyScaledAndRounded) {
        TBase::params[KEY_PARAM].value = newKeyScaledAndRounded;
    }
}

template <class TBase>
inline void Harmony2<TBase>::_serviceKeysigModeCV() {
    if (TBase::inputs[MODE_INPUT].channels == 0) {
        return;
    }

    SQINFO("How do we want to scale ksig input?");
    const int degreesPerVolt = numNotesInCurrentScale();
    const float newModeF = TBase::inputs[MODE_INPUT].getVoltage(0);
    int newModeScaledAndRounded = int(std::round(degreesPerVolt * newModeF)) % degreesPerVolt;
    if (newModeScaledAndRounded < 0) {
        newModeScaledAndRounded += degreesPerVolt;
    }

    //  scale->get   std::pair<const MidiNote, Scales> get() const;
    const Scale* oldKey = _quantizerOptions->scale.get();
    const auto oldScale = oldKey->get();

    const Scale::Scales oldMode = std::get<1>(oldScale);
    if (newModeScaledAndRounded != int(oldMode)) {
        TBase::params[MODE_PARAM].value = newModeScaledAndRounded;
    }
}

template <class TBase>
inline void Harmony2<TBase>::_serviceEnableButtons() {
    for (int i = 0; i < NUM_TRANPOSERS; ++i) {
        _ButtonProcs[i].go(TBase::params[XPOSE_ENABLEREQ1_PARAM + i].value);
        if (_ButtonProcs[i].trigger()) {
            // Toggle the value
            TBase::params[XPOSE_ENABLE1_PARAM + i].value = TBase::params[XPOSE_ENABLE1_PARAM + i].value < 5 ? 10 : 0;
        }
        TBase::lights[XPOSE_ENABLE1_LIGHT + i].value = TBase::params[XPOSE_ENABLE1_PARAM + i].value;
    }
}

template <class TBase>
inline void Harmony2<TBase>::_servicePolyphony() {
    int numEnabled = 0;
    for (int i = 0; i < NUM_TRANPOSERS; ++i) {
        if (TBase::params[XPOSE_ENABLE1_PARAM + i].value > 5) {
            ++numEnabled;
        }
    }
    TBase::outputs[PITCH_OUTPUT].channels = numEnabled;
}

// static int count = 0;
template <class TBase>
inline void Harmony2<TBase>::process(const typename TBase::ProcessArgs& args) {
    _divn.step();
    float input = TBase::inputs[PITCH_INPUT].getVoltage(0);
    MidiNote quantizedInput = _quantizer->run(input);

    ScaleNote scaleNote;
    NoteConvert::m2s(scaleNote, *_quantizerOptions->scale, quantizedInput);

    int outputChannel = 0;
    for (int bank = 0; bank < NUM_TRANPOSERS; ++bank) {
        const bool bankEnabled = TBase::params[XPOSE_ENABLE1_PARAM + bank].value > 5;
        if (bankEnabled) {
            const int xposeSteps = int(TBase::params[XPOSE_DEGREE1_PARAM + bank].value);
            const int xposeOctaves = int(TBase::params[XPOSE_OCTAVE1_PARAM + bank].value) - 2;
            ScaleNote noteForBank = scaleNote;
            noteForBank.transposeDegree(xposeSteps);

            FloatNote f;
            //    NoteConvert::m2f(f, quantizedInput);
            NoteConvert::s2f(f, *_quantizerOptions->scale, noteForBank);
            const float cv = f.get() + float(xposeOctaves);
            TBase::outputs[PITCH_OUTPUT].setVoltage(cv, outputChannel);
            outputChannel++;
        }
    }
}

template <class TBase>
void Harmony2<TBase>::_serviceTranspose() {
    int outputChannel = 0;
    for (int i = 0; i < NUM_TRANPOSERS; ++i) {
        _serviceTranspose(i, outputChannel);
    }
}

template <class TBase>
void Harmony2<TBase>::_serviceTranspose(int channel, int& outputChannel) {
    const bool enabled = TBase::params[XPOSE_ENABLE1_PARAM + channel].value > 5;
    if (!enabled) {
        return;
    }

    float input = TBase::inputs[PITCH_INPUT].getVoltage(0);
    MidiNote quantizedInput = _quantizer->run(input);

    const int xposeSteps = int(TBase::params[XPOSE_DEGREE1_PARAM].value);
    ScaleNote scaleNote;
    NoteConvert::m2s(scaleNote, *_quantizerOptions->scale, quantizedInput);
    scaleNote.transposeDegree(xposeSteps);
    NoteConvert::s2m(quantizedInput, *_quantizerOptions->scale, scaleNote);

    FloatNote f;
    NoteConvert::m2f(f, quantizedInput);
    TBase::outputs[PITCH_OUTPUT].setVoltage(f.get(), outputChannel);
    outputChannel++;
}