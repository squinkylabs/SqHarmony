#include "Divider.h"
#include "FloatNote.h"
#include "GateTrigger.h"
#include "MelodyEvaluator.h"
#include "MelodyGenerator.h"
#include "NoteConvert.h"

namespace rack {
namespace engine {
struct Module;
}
}  // namespace rack
using Module = ::rack::engine::Module;

template <class TBase>
class Mutator : public TBase {
public:
    Mutator(Module* module) : TBase(module) {
        _init();
    }
    Mutator() : TBase() {
        _init();
    }

    enum ParamIds {
        ROW_LENGTH_PARAM,
        KEY_PARAM,
        MODE_PARAM,

        NON_CENTERED_WEIGHT_STYLE_PARAM,
        PITCH_RANGE_WEIGHT_STYLE_PARAM,
        LEAPS_WEIGHT_STYLE_PARAM,
        UNISON_WEIGHT_STYLE_PARAM,

        SLOTS_TO_CHANGE_PARAM,  // 0 == all of them
        ADJACENT_SLOTS_PARAM,   // 0 = spread out, 1 = adjacent
        PITCH_RANGE_STYLE_PARAM,
        NUM_PARAMS
    };

    enum InputIds {
        MUTATE_INPUT,
        CENTER_VOLTAGE_INPUT,
        INITIAL_VOLTAGE_INPUT,
        DEBUG_REINIT_INPUT,
        DEBUG_EVAL_INPUT,
        NUM_INPUTS
    };

    enum OutputIds {
        NOTES_OUTPUT,
        NUM_OUTPUTS
    };

    enum LightIds {
        NUM_LIGHTS
    };

    void process(const typename TBase::ProcessArgs& args) override;

private:
    void _init();
    void _stepn();
    void _processTrigger();

    Divider _divn;

    GateTrigger _mutateTrigger;

    MelodyRow _theNoteData;
    Scale _theScale;
    MelodyMutateState _theState;
    MelodyMutateStyle _theStyle;

    std::function<double(double)> _audioCurve;
    bool _initialized = false;

    void _pollDebug();
    void _evalDebug();
    GateTrigger _debugReinitProc;
    GateTrigger _debugEvalProc;
};

template <class TBase>
inline void Mutator<TBase>::_init() {
    MidiNote base(MidiNote::C);
    _theScale.set(base, Scale::Scales::Major);
    _theNoteData.init(8, _theScale);

    _audioCurve = AudioMath::makeFunc_AudioTaper(-18);

    // SQINFO("BGF: init");
    _divn.setup(4, [this]() {
        this->_stepn();
    });
}

template <class TBase>
inline void Mutator<TBase>::_pollDebug() {
    _debugReinitProc.go(TBase::inputs[DEBUG_REINIT_INPUT].getVoltage(0));
    if (_debugReinitProc.trigger()) {
        SQINFO("re-init");
        _initialized = false;
    }

    _debugEvalProc.go(TBase::inputs[DEBUG_EVAL_INPUT].getVoltage(0));
    if (_debugEvalProc.trigger()) {
        SQINFO("eval");
        _evalDebug();
    }
}

template <class TBase>
inline void Mutator<TBase>::_evalDebug() {
    SQINFO("enter debug eval ------");
    // MelodyEvaluator::getPenalty(_theNoteData, _theStyle, true);
    const std::string s = MelodyEvaluator::toString(_theNoteData, _theStyle);
    SQINFO("eval debug %s", s.c_str());
}

template <class TBase>
inline void Mutator<TBase>::_stepn() {
    if (!_initialized) {
        const int channels = TBase::inputs[INITIAL_VOLTAGE_INPUT].channels;

        SQINFO("r-init chan=%d", TBase::inputs[INITIAL_VOLTAGE_INPUT].channels);
        for (int i = 0; i < int(_theNoteData.getSize()); ++i) {
            const float v = (i < channels) ? TBase::inputs[INITIAL_VOLTAGE_INPUT].getVoltage(i) : 0;
            SQINFO("re-init ch %d to %f", i, v);
            FloatNote fn(v);
            MidiNote midiNote;
            NoteConvert::f2m(midiNote, fn);
            _theNoteData.setNote(i, midiNote);
        }

        TBase::outputs[NOTES_OUTPUT].setChannels(_theNoteData.getSize());
        _initialized = true;
    }

    int desiredLen = TBase::params[ROW_LENGTH_PARAM].value;
    desiredLen = std::min(desiredLen, 16);
    desiredLen = std::max(desiredLen, 1);

    assert(desiredLen > 0);
    assert(desiredLen < 17);
    if (desiredLen != _theNoteData.getSize()) {
        _theNoteData.setSize(desiredLen);
        SQINFO("setting length to %d", desiredLen);
        TBase::outputs[NOTES_OUTPUT].setChannels(_theNoteData.getSize());
    }

    // currentRoot = currrentScale
    MidiNote root(MidiNote::C + TBase::params[KEY_PARAM].value);
    _theScale.set(root, Scale::Scales(TBase::params[MODE_PARAM].value));
    // SQINFO("root = %d  param=%f scale param=%f", root.get(), TBase::params[KEY_PARAM].value, TBase::params[MODE_PARAM].value);
    _pollDebug();

    //  std::pair<const MidiNote, Scale::Scales> currentScale = _theScale.get();
    // SQINFO("scale = %d %d", currentScale.first.get(), int(currentScale.second));

    auto centerPort = TBase::inputs[CENTER_VOLTAGE_INPUT];
    float centerV = centerPort.isConnected() ? centerPort.value : 0;
    _theStyle.centerVoltage = centerV;

    _theStyle.numToMutate = int(std::round(TBase::params[SLOTS_TO_CHANGE_PARAM].value));
    _theStyle.mutateAdjacent = bool(std::round(TBase::params[ADJACENT_SLOTS_PARAM].value));
}

template <class TBase>
inline void Mutator<TBase>::process(const typename TBase::ProcessArgs& args) {
    _divn.step();
    const float mutateInput = TBase::inputs[MUTATE_INPUT].getVoltage(0);
    _mutateTrigger.go(mutateInput);
    if (_mutateTrigger.trigger()) {
        // SQINFO("trigger");
        _processTrigger();
    }

    // SQINFO("x %llu", _theNoteData.getSize());

    for (size_t i = 0; i < _theNoteData.getSize(); ++i) {
        //  SQINFO("set volt(1.3, %llu)", i);

        FloatNote floatNote;
        NoteConvert::m2f(floatNote, _theNoteData.getNote(i));
        TBase::outputs[NOTES_OUTPUT].setVoltage(floatNote.get(), i);

        // const float q =TBase::outputs[NOTES_OUTPUT].getVoltage(i);
        //  SQINFO("proc: just set output %llu to %f, is %f", i, floatNote.get(), q);
        //  SQINFO("addr of port = %p", &TBase::outputs[NOTES_OUTPUT]);
    }
}

template <class TBase>
inline void Mutator<TBase>::_processTrigger() {
    //   SQINFO("process trigger");
    //    SQINFO("style params are %s",
    //         TBase::params[NON_CENTERED_WEIGHT_STYLE_PARAM].value,
    //         TBase::params[PITCH_RANGE_WEIGHT_STYLE_PARAM].value,
    //         TBase::params[LEAPS_WEIGHT_STYLE_PARAM].value,
    //         TBase::params[UNISON_WEIGHT_STYLE_PARAM].value
    //     );
    //    SQINFO("after proc %f %f %f %f",
    //        _audioCurve(TBase::params[NON_CENTERED_WEIGHT_STYLE_PARAM].value),
    //        _audioCurve(TBase::params[PITCH_RANGE_WEIGHT_STYLE_PARAM].value),
    //        _audioCurve(TBase::params[LEAPS_WEIGHT_STYLE_PARAM].value),
    //        _audioCurve(TBase::params[UNISON_WEIGHT_STYLE_PARAM].value)
    //    );
    //    SQINFO("for .5 is %f", _audioCurve(.5));

    SQINFO("enter process trigger----");

    _theStyle.leapsWeight = 4 * _audioCurve(TBase::params[LEAPS_WEIGHT_STYLE_PARAM].value);
    _theStyle.unisonWeight = 4 * _audioCurve(TBase::params[UNISON_WEIGHT_STYLE_PARAM].value);
    _theStyle.pitchRangeWeight = 4 * _audioCurve(TBase::params[PITCH_RANGE_WEIGHT_STYLE_PARAM].value);
    _theStyle.idealPitchRange2 = TBase::params[PITCH_RANGE_STYLE_PARAM].value;
    _theStyle.nonCenteredWeight = 4 * _audioCurve(TBase::params[NON_CENTERED_WEIGHT_STYLE_PARAM].value);
    SQINFO("process trgger set to %f %f %f %f",
           _theStyle.leapsWeight,
           _theStyle.unisonWeight,
           _theStyle.pitchRangeWeight,
           _theStyle.nonCenteredWeight);

    SQINFO("process trigger set weight %s", MelodyEvaluator::toString(_theNoteData, _theStyle).c_str());
    //   SQINFO("style params = %s", _theStyle.toString().c_str());
    //   SQINFO("%s", MelodyEvaluator::toString(_theNoteData, _theStyle).c_str());

    MelodyGenerator::mutate(_theNoteData, _theScale, _theState, _theStyle);
    //  SQINFO("notes: %s", _theNoteData.print().c_str());

    SQINFO("exit process trigger----");
}
