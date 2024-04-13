#pragma once

#include <cmath>
#include <string>
#include <tuple>
#include <vector>

#include "AudioMath.h"
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

// test perf sr =796
// H2: 3.4%, VCO1, 5.2
// don't poll the 6 guys:  2.4%
// and poly xpose 2.0
// no PES, 2.0
// back together, infrequent on 6: 2.4
// made other things infrequent, not something jumped. but is't now 2.7
// LFO 2hz -> Key CV 14%
// same at .2/ at .02 -> 4%
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
        ONLY_USE_DIATONIC_FOR_CV_PARAM,
        NUM_PARAMS
    };

    enum InputIds {
        XPOSE_INPUT,
        KEY_INPUT,
        MODE_INPUT,
        PITCH_INPUT,
        PES_INPUT,
        NUM_INPUTS
    };

    enum OutputIds {
        PITCH_OUTPUT,
        PES_OUTPUT,
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
    bool diatonicOnlyForCV() const { return TBase::params[ONLY_USE_DIATONIC_FOR_CV_PARAM].value > .5; }

    // Does not include the octave/
    int numNotesInCurrentScale();

    int numCurrentModesMax() const {
        return 1 + Scale::lastScale;
    }

private:
    GateTrigger _ButtonProcs[NUM_TRANPOSERS];
    ScaleQuantizerPtr _quantizer;
    ScaleQuantizer::OptionsPtr _quantizerOptions;

    CompositeUpdater<Harmony2<TBase>> _updater;
    CompositeUpdater<Harmony2<TBase>> _keyOutUpdater;
    CompositeUpdater<Harmony2<TBase>> _keyInUpdater;

    // Given the current state (ONLY_USE_DIATONIC_PARAM), how many modes are available for CV
    int _numCurrentModesForCV() const;

    void _init();
    void _serviceAllMiscInputs();
    void _serviceAllTranposers();
    void _servicePolyphony();
    void _serviceEnableButtons();
    void _serviceKeysigRootCV();
    void _serviceKeysigModeCV();
    void _serviceKeysigParams();
    void _serviceScaleInput();
    void _serviceScaleOutput();
};

template <class TBase>
int Harmony2<TBase>::_numCurrentModesForCV() const {
    return Scale::numScales(diatonicOnlyForCV());
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
        "+7",
        "+8",
        "+9",
        "+10",
        "+11",
    };
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

    _updater.set(this, getSubSampleFactor());
    _keyInUpdater.set(this, getSubSampleFactor());
    _keyOutUpdater.set(this, getSubSampleFactor());

    for (int i = 0; i < 6; ++i) {
        _updater.add(ParamIds(XPOSE_DEGREE1_PARAM + i), false);
        _updater.add(ParamIds(XPOSE_OCTAVE1_PARAM + i), false);
        _updater.add(ParamIds(XPOSE_ENABLEREQ1_PARAM + i), false);
    }

    _updater.add(KEY_PARAM);
    _updater.add(MODE_PARAM);
    _updater.add(SHARPS_FLATS_PARAM);
    _updater.add(ONLY_USE_DIATONIC_FOR_CV_PARAM);

    // for debugging, turn these 4 off
    _updater.add(XPOSE_INPUT, PolyMono::Poly, true, AudioMath::float2int12);
    _updater.add(KEY_INPUT, PolyMono::Mono, true, AudioMath::float2int12);
    _updater.add(MODE_INPUT, PolyMono::Mono, true, AudioMath::float2int12);
    _updater.add(PITCH_INPUT, PolyMono::Mono, true, AudioMath::float2int12);

    // Need to update scale out when either of these params change.
    _keyOutUpdater.add(KEY_PARAM, false);
    _keyOutUpdater.add(MODE_PARAM, false);
    _keyOutUpdater.add(PES_OUTPUT, false);  // monitor the output in case we are patched

    // Need to respond to scale input changes.
    // SQINFO("in h2, trying to add scale as infrequent");
    _keyInUpdater.add(PES_INPUT, PolyMono::Poly, false, nullptr);
}

template <class TBase>
inline void Harmony2<TBase>::_serviceAllMiscInputs() {
    _serviceEnableButtons();
    _serviceKeysigRootCV();
    _serviceKeysigModeCV();
    _serviceKeysigParams();
    _servicePolyphony();
}

template <class TBase>
inline void Harmony2<TBase>::_serviceKeysigParams() {
    // one version of a scale function - just rounds to nearest int.
    const int basePitch = int(std::round(TBase::params[KEY_PARAM].value));
    const auto mode = Scale::Scales(int(std::round(TBase::params[MODE_PARAM].value)));
    // SQINFO("in service keysig params. base=%d mode=%d", basePitch, int(mode));

    assert(basePitch < 12);
    _quantizerOptions->scale->set(basePitch, mode);
}

template <class TBase>
inline void Harmony2<TBase>::_serviceKeysigRootCV() {
    if (TBase::inputs[KEY_INPUT].channels == 0) {
        return;
    }

    // Here is a scale function scales and wraps 0..semisPerOctave-1
    const int semisPerOctave = 12;
    float newKeyF = TBase::inputs[KEY_INPUT].getVoltage(0);
    newKeyF -= std::floor(newKeyF);
    int newKeyScaledAndRounded = int(std::round(semisPerOctave * newKeyF));
    while (newKeyScaledAndRounded < 0) {
        newKeyScaledAndRounded += semisPerOctave;
    }
    while (newKeyScaledAndRounded >= semisPerOctave) {
        newKeyScaledAndRounded -= semisPerOctave;
    }

    const Scale* oldKey = _quantizerOptions->scale.get();
    const auto x = oldKey->get();
    const MidiNote oldRoot = std::get<0>(x);
    FloatNote oldRootFloat;
    NoteConvert::m2f(oldRootFloat, oldRoot);
    const float oldKeyCV = oldRootFloat.get() + 6;  // I don't remember what this offset is...
    const float oldKeyVSCaled = oldKeyCV * _numCurrentModesForCV();
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

    // Here is a scale function that multiplies by 12, but wraps in the number of current modes
    // number of modes is just a function of ONLY_USE_DIATONIC_PARAM
    const int _inumCurrentModes = _numCurrentModesForCV();
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
    // SQINFO("service ks input");
    auto& input = TBase::inputs[PES_INPUT];
    if (input.channels < 1) {
        return;  // unconnected
    }
    if (input.channels < 12) {
        // wrong number of channels - error
        TBase::lights[XSCALE_INVALID_LIGHT].value = 8;
        return;
    }
    Scale::Role roles[13];
    bool haveAddedRoot = false;
    for (int i = 0; i < 12; ++i) {
        float v = input.getVoltage(i);
        Scale::Role role;
        if (v < 4) {
            role = Scale::Role::NotInScale;
        } else if (v < 9) {
            role = Scale::Role::InScale;
        } else {
            // only add the first root. Some modules send all note as 10V.
            role = haveAddedRoot ? Scale::Role::InScale : Scale::Role::Root;
            haveAddedRoot = true;
        }
        roles[i] = role;
    }
    roles[12] = Scale::Role::End;

    const auto scaleConverted = Scale::convert(roles, false);
    if (std::get<0>(scaleConverted) == false) {
        TBase::lights[XSCALE_INVALID_LIGHT].value = 8;
    } else {
        TBase::lights[XSCALE_INVALID_LIGHT].value = 0;
        // SQINFO("good scale, %d, %d (mode)", std::get<1>(scaleConverted).get(), int(std::get<2>(scaleConverted)));
        // SQINFO("servicing input caused us to update the KEY_PARAM %d and MODE_PARAM %d",
        //     std::get<1>(scaleConverted).get(),
        //     int(std::get<2>(scaleConverted)));
        TBase::params[KEY_PARAM].value = std::get<1>(scaleConverted).get();
        TBase::params[MODE_PARAM].value = int(std::get<2>(scaleConverted));
    }
}

template <class TBase>
inline void Harmony2<TBase>::_serviceScaleOutput() {
    // SQINFO("_serviceScaleOutput");
    auto& output = TBase::outputs[PES_OUTPUT];
    if (output.isConnected()) {
        output.channels = 12;
    }

    const auto note = MidiNote::C + int(std::round(TBase::params[KEY_PARAM].value));
    const auto mode = Scale::Scales(int(std::round(TBase::params[MODE_PARAM].value)));
    const Scale::RoleArray roleArray = Scale::convert(note, mode);
    for (int i = 0; i < 12; ++i) {
        float v = 0;
        switch (roleArray.data[i]) {
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
        _serviceAllMiscInputs();
        _serviceAllTranposers();
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
inline void Harmony2<TBase>::_serviceAllTranposers() {
    ScalePtr scale = this->_quantizerOptions->scale;
    const Scale::Scales mode = std::get<1>(scale.get()->get());
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
            // here is a scale function that's just f * 12;
            const int xposeCVSteps = int(std::round(xposeCV * 12));
            const int xposeBaseSteps = int(TBase::params[XPOSE_DEGREE1_PARAM + bank].value);
            const int xposeSteps = xposeBaseSteps + xposeCVSteps;

            //  SQINFO("xpose steps = %d", xposeSteps);

            TBase::params[XPOSE_TOTAL1_PARAM + bank].value = float(xposeSteps) / 12.f;  // report back what we did.
            const int xposeOctaves = int(TBase::params[XPOSE_OCTAVE1_PARAM + bank].value) - 2;
            ScaleNote noteForBank = scaleNote;
            // SQINFO("note for bank deg = %d", noteForBank.getDegree());
            noteForBank.transposeDegree(xposeSteps, Scale::numNotesInScale(mode));
            //  SQINFO("after note=%d",  noteForBank.getDegree());

            FloatNote f;
            NoteConvert::s2f(f, *_quantizerOptions->scale, noteForBank);
            //   SQINFO("f=%f oct=%d", f.get(), xposeOctaves);
            const float cv = f.get() + float(xposeOctaves);
            TBase::outputs[PITCH_OUTPUT].setVoltage(cv, channel);
            //  SQINFO("out(%d) = %f in old_process", channel, cv);
            channel++;
        }
    }
}
