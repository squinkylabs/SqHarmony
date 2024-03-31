#pragma once

#include <cmath>
#include <string>
#include <tuple>
#include <vector>

#include "CompositeUpdater.h"
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
        XPOSE_OCTAVE5_PARAM,
        XPOSE_OCTAVE6_PARAM,
        XPOSE_ENABLE1_PARAM,
        XPOSE_ENABLE2_PARAM,
        XPOSE_ENABLE3_PARAM,
        XPOSE_ENABLE4_PARAM,
        XPOSE_ENABLE5_PARAM,
        XPOSE_ENABLE6_PARAM,
        XPOSE_TOTAL1_PARAM,  // units 1/12 volt = 1 step, same as XPOSE CX
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
        XPOSE_INPUT,
        KEY_INPUT,
        MODE_INPUT,
        PITCH_INPUT,
        XSCALE_INPUT,
        NUM_INPUTS
    };

    enum OutputIds {
        PITCH_OUTPUT,
        XSCALE_OUTPUT,
        NUM_OUTPUTS
    };

    enum LightIds {
        XPOSE_ENABLE1_LIGHT,
        XPOSE_ENABLE2_LIGHT,
        XPOSE_ENABLE3_LIGHT,
        XPOSE_ENABLE4_LIGHT,
        XPOSE_ENABLE5_LIGHT,
        XPOSE_ENABLE6_LIGHT,
        XSCALE_INVALID_LIGHT,
        NUM_LIGHTS
    };

    void process(const typename TBase::ProcessArgs& args) override;

    static int getSubSampleFactor() { return 32; }

    // TODO: there all need to be dynamic
    // these will change when we implement ONLY_USE_DIATONIC_PARAM
    bool diatonicOnly() { return TBase::params[ONLY_USE_DIATONIC_PARAM].value > .5; }

    // Given the current state (ONLY_USE_DIATONIC_PARAM), how many modes are available.
    int numCurrentModes();
    // Does not include the octave/
    int numNotesInCurrentScale();

private:
    GateTrigger _ButtonProcs[NUM_TRANPOSERS];
    ScaleQuantizerPtr _quantizer;
    ScaleQuantizer::OptionsPtr _quantizerOptions;

    CompositeUpdater<Harmony2<TBase>> _updater;
    CompositeUpdater<Harmony2<TBase>> _keyOutUpdater;
    CompositeUpdater<Harmony2<TBase>> _keyInUpdater;

    void _init();
    void _stepn();
    void _old_process();
    void _servicePolyphony();
    void _serviceEnableButtons();
    void _serviceKeysigRootCV();
    void _serviceKeysigModeCV();
    void _serviceKeysigParam();
    void _serviceTranspose();
    void _serviceTranspose(int channel, int& outputChannel);
    void _serviceScaleInput();
    void _serviceScaleOutput();
};

template <class TBase>
int Harmony2<TBase>::numCurrentModes() {
    return diatonicOnly() ? 7 : 13;
}
// Includes the octave
template <class TBase>
int Harmony2<TBase>::numNotesInCurrentScale() {
    const Scale::Scales curMode = Scale::Scales(int(TBase::params[MODE_PARAM].value));
    return Scale::numNotesInScale(curMode);
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
    _quantizerOptions = std::make_shared<ScaleQuantizer::Options>();
    _quantizerOptions->scale = std::make_shared<Scale>();
    _quantizerOptions->scale->set(MidiNote::C, Scale::Scales::Major);
    _quantizer = std::make_shared<ScaleQuantizer>(_quantizerOptions);

    _updater.set(this);
    _keyInUpdater.set(this);
    _keyOutUpdater.set(this);

    for (int i = 0; i < 6; ++i) {
        _updater.add(ParamIds(XPOSE_DEGREE1_PARAM + i));
        _updater.add(ParamIds(XPOSE_OCTAVE1_PARAM + i));
        _updater.add(ParamIds(XPOSE_ENABLEREQ1_PARAM + i));
    }

    _updater.add(KEY_PARAM);
    _updater.add(MODE_PARAM);
    _updater.add(SHARPS_FLATS_PARAM);
    _updater.add(ONLY_USE_DIATONIC_PARAM);

    _updater.add(XPOSE_INPUT, false);
    _updater.add(KEY_INPUT, true);
    _updater.add(MODE_INPUT, true);
    _updater.add(PITCH_INPUT, true);

    // Need to update scale out when either of these params change.
    _keyOutUpdater.add(KEY_PARAM);
    _keyOutUpdater.add(MODE_PARAM);

    // Need to respond to scale input changes.
    _keyInUpdater.add(XSCALE_INPUT, false);
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

    assert(basePitch < 12);
    _quantizerOptions->scale->set(basePitch, mode);
}

template <class TBase>
inline void Harmony2<TBase>::_serviceKeysigRootCV() {
    if (TBase::inputs[KEY_INPUT].channels == 0) {
        return;
    }

    const int semisPerOctave = 12;
    float newKeyF = TBase::inputs[KEY_INPUT].getVoltage(0);
    newKeyF -= std::floor(newKeyF);
    int newKeyScaledAndRounded = int(std::round(semisPerOctave * newKeyF));
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
        assert(TBase::params[KEY_PARAM].value < 11.5 && TBase::params[KEY_PARAM].value >= 0);
    }
}

template <class TBase>
inline void Harmony2<TBase>::_serviceKeysigModeCV() {
    if (TBase::inputs[MODE_INPUT].channels == 0) {
        return;
    }

    const int _inumCurrentModes = numCurrentModes();
    const float newModeF = TBase::inputs[MODE_INPUT].getVoltage(0);
    const int newModeI = int(std::round(12 * newModeF));

    int newModeScaledAndRounded = int(std::round(newModeI)) % _inumCurrentModes;
    if (newModeScaledAndRounded < 0) {
        newModeScaledAndRounded += _inumCurrentModes;
    }
    assert(newModeScaledAndRounded >= 0);
    assert(newModeScaledAndRounded <= _inumCurrentModes);

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

template <class TBase>
inline void Harmony2<TBase>::_serviceScaleInput() {
    SQINFO("scale in changed");

    //static std::tuple<bool, MidiNote, Scales> convert(const Role* noteRole);
    auto &input = TBase::inputs[XSCALE_INPUT];
    Scale::Role roles[13];
    for (int i=0; i<12; ++i) {
        float v = input.getVoltage(i);
        Scale::Role role;
        if (v < 4) {
            role = Scale::Role::NotInScale;
        } else if (v < 9) {
            role = Scale::Role::InScale;
        } else {
            role = Scale::Role::Root;
        }
        roles[i] = role;
    }
    roles[12] = Scale::Role::End;

    const auto scaleConverted = Scale::convert(roles);
    Scale::_dumpRoles("derived roles", roles);
    if (std::get<0>(scaleConverted) == false) {
        SQINFO("bad scale");
        TBase::lights[XSCALE_INVALID_LIGHT].value = 8;
    } else {
        TBase::lights[XSCALE_INVALID_LIGHT].value = 0;
        SQINFO("good scale, %d, %d", std::get<1>(scaleConverted).get(), int(std::get<2>(scaleConverted)));
    }

}

template <class TBase>
inline void Harmony2<TBase>::_serviceScaleOutput() {
    auto &output = TBase::outputs[XSCALE_OUTPUT];
    if (output.isConnected()) {
        output.channels = 12;
    }
    auto scale = _quantizerOptions->scale;
    std::pair<const MidiNote, Scale::Scales> settings = scale->get();;
    const Scale::RoleArray roleArray = Scale::convert(settings.first, settings.second);
    for (int i=0; i<12; ++i) {
        float v = 0;
        switch(roleArray.data[i]) {
            case Scale::Role::InScale:
                v = 8;
                break;
            case Scale::Role::Root:
                v = 10;
                break;
            case Scale::Role::NotInScale:
            case Scale::Role::End:
                break;
            default:
                assert(false);
        }
        output.setVoltage(v, i);
    }
}

template <class TBase>
inline void Harmony2<TBase>::process(const typename TBase::ProcessArgs& args) {
    bool changed = _updater.poll();
    if (changed) {
        _stepn();
        _old_process();
    }
    changed = _keyInUpdater.poll();
    if (changed) {
        _serviceScaleInput();
    }

    changed = _keyOutUpdater.poll();
    if (changed) {
        _serviceScaleOutput();
    }
}

template <class TBase>
inline void Harmony2<TBase>::_old_process() {
    // assert(TBase::params[KEY_PARAM].value < 11.5);
    // _divn.step();
    assert(TBase::params[KEY_PARAM].value < 11.5);
    const float input = TBase::inputs[PITCH_INPUT].getVoltage(0);
    MidiNote quantizedInput = _quantizer->run(input);

    ScaleNote scaleNote;
    NoteConvert::m2s(scaleNote, *_quantizerOptions->scale, quantizedInput);

    int channel = 0;
    const bool polyXposeCV = TBase::inputs[XPOSE_INPUT].channels > 1;
    float xposeCV = TBase::inputs[XPOSE_INPUT].getVoltage(0);
    for (int bank = 0; bank < NUM_TRANPOSERS; ++bank) {
        const bool bankEnabled = TBase::params[XPOSE_ENABLE1_PARAM + bank].value > 5;
        if (bankEnabled) {
            if (polyXposeCV) {
                xposeCV = TBase::inputs[XPOSE_INPUT].getVoltage(channel);
            }
            const int xposeCVSteps = int(std::round(xposeCV * 12));
            const int xposeBaseSteps = int(TBase::params[XPOSE_DEGREE1_PARAM + bank].value);
            const int xposeSteps = xposeBaseSteps + xposeCVSteps;

            TBase::params[XPOSE_TOTAL1_PARAM + bank].value = float(xposeSteps) / 12.f;  // report back what we did.
            const int xposeOctaves = int(TBase::params[XPOSE_OCTAVE1_PARAM + bank].value) - 2;
            ScaleNote noteForBank = scaleNote;
            noteForBank.transposeDegree(xposeSteps);

            FloatNote f;
            NoteConvert::s2f(f, *_quantizerOptions->scale, noteForBank);
            const float cv = f.get() + float(xposeOctaves);
            TBase::outputs[PITCH_OUTPUT].setVoltage(cv, channel);
            channel++;
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