#include "Divider.h"
#include "FloatNote.h"
// #include "GateTrigger.h"
#include "MelodyEvaluator.h"
#include "MelodyGenerator.h"
#include "NoteConvert.h"
#include "SeqClock.h"

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
                                //  ADJACENT_SLOTS_PARAM,   // 0 = spread out, 1 = adjacent

        ADJACENCY_STYLE_PARAM,  // See enum SlotSelectionMethod
        PITCH_RANGE_STYLE_PARAM,
        SCHEMA_PARAM,
        CONSONANT_WEIGHT_PARAM,
        DESIRED_CENTER_PARAM,
        NUM_PARAMS
    };

    enum InputIds {
        MUTATE_INPUT,
        CENTER_VOLTAGE_INPUT,
        INITIAL_VOLTAGE_INPUT,
        REINIT_INPUT,
        xDEBUG_EVAL_INPUT,
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

    //  GateTrigger _mutateTrigger;
    SeqClock _seqClock;

    MelodyRow _theNoteData;
    //  Scale _theScale;
    MelodyMutateState _theState;
    MelodyMutateStyle _theStyle;

    std::function<double(double)> _audioCurve;
    bool _initialized = false;

    // void _pollDebug();
    void _evalDebug();
    void _reInitRow();
    GateTrigger _debugEvalProc;
};

template <class TBase>
inline void Mutator<TBase>::_init() {
    MidiNote base(MidiNote::C);
    _theStyle.scale.set(base, Scale::Scales::Major);
    //  _theScale.set(base, Scale::Scales::Major);
    // SQINFO("init, base=%d wasSet = %d", _theScale.base().get(), _theScale.getWasSet());
    //  assert(false);

    _theNoteData.init(8, _theStyle.scale);

    _audioCurve = AudioMath::makeFunc_AudioTaper(-18);

    // SQINFO("BGF: init");
    _divn.setup(4, [this]() {
        this->_stepn();
    });
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
    int desiredLen = TBase::params[ROW_LENGTH_PARAM].value;
    desiredLen = std::min(desiredLen, 16);
    desiredLen = std::max(desiredLen, 1);

    assert(desiredLen > 0);
    assert(desiredLen < 17);
    if (desiredLen != (int)_theNoteData.getSize()) {
        _theNoteData.setSize(desiredLen);
        // SQINFO("setting length to %d raw=%f", desiredLen, TBase::params[ROW_LENGTH_PARAM].value);
        TBase::outputs[NOTES_OUTPUT].setChannels(_theNoteData.getSize());
    }

    if (!_initialized) {
        _reInitRow();

#if 0
        const int channels = TBase::inputs[INITIAL_VOLTAGE_INPUT].channels;

        //  SQINFO("r-init chan count=%d", TBase::inputs[INITIAL_VOLTAGE_INPUT].channels);
        //   assert(_theNoteData.getSize() == channels);
        for (int i = 0; i < int(_theNoteData.getSize()); ++i) {
            const float v = (i < channels) ? TBase::inputs[INITIAL_VOLTAGE_INPUT].getVoltage(i) : 0;
            // SQINFO("re-init output ch %d to %f based on note data len %d", i, v, (unsigned) _theNoteData.getSize());
            FloatNote fn(v);
            MidiNote midiNote;
            NoteConvert::f2m(midiNote, fn);
            _theNoteData.setNote(i, midiNote);
        }
#endif

        _initialized = true;
    }

    MidiNote root(MidiNote::C + TBase::params[KEY_PARAM].value);
    _theStyle.scale.set(root, Scale::Scales(TBase::params[MODE_PARAM].value));
    //_pollDebug();

    auto centerPort = TBase::inputs[CENTER_VOLTAGE_INPUT];
    float centerV = centerPort.isConnected() ? centerPort.value : 0;
    centerV += TBase::params[DESIRED_CENTER_PARAM].value;
    _theStyle.centerVoltage = centerV;
    // SQINFO("CenterV = %f", centerV);

    _theStyle.numToMutate = int(std::round(TBase::params[SLOTS_TO_CHANGE_PARAM].value));
    const int slotSelectionMethodInt = int(std::round(TBase::params[ADJACENCY_STYLE_PARAM].value));
    _theStyle.slotSelectionMethod = SlotSelectionMethod(slotSelectionMethodInt);
}

template <class TBase>
inline void Mutator<TBase>::_reInitRow() {
    TBase::outputs[NOTES_OUTPUT].setChannels(_theNoteData.getSize());
    SQINFO("set num output channels to %d %d ", (int)_theNoteData.getSize(), (int)TBase::outputs[NOTES_OUTPUT].channels);
    const unsigned inputChannels = unsigned(TBase::inputs[INITIAL_VOLTAGE_INPUT].getChannels());
    const unsigned rowSize = _theNoteData.getSize();

    for (unsigned int i = 0; i < rowSize; ++i) {
        const float v = (i < inputChannels) ? TBase::inputs[INITIAL_VOLTAGE_INPUT].getVoltage(i) : 0;
        // SQINFO("re-init output ch %d to %f based on note data len %d", i, v, (unsigned) _theNoteData.getSize());
        FloatNote fn(v);
        MidiNote midiNote;
        NoteConvert::f2m(midiNote, fn);
        _theNoteData.setNote(i, midiNote);
        //   SQINFO("write output %d = %f", i, v);
        TBase::outputs[NOTES_OUTPUT].setVoltage(v, i);
        //   SQINFO("read output = %f", TBase::outputs[NOTES_OUTPUT].getVoltage(i));
    }
}

template <class TBase>
inline void Mutator<TBase>::process(const typename TBase::ProcessArgs& args) {
    _divn.step();
    const float mutateInput = TBase::inputs[MUTATE_INPUT].getVoltage(0);
    const float reInitInput = TBase::inputs[REINIT_INPUT].getVoltage(0);

    // TODO: use schmidt on reset?
    const auto clockResult = _seqClock.updateOnce(mutateInput, true, reInitInput);
    if (clockResult.didReset) {
        _reInitRow();
    } else if (clockResult.didClock) {
        // SQINFO("composite::_processtrigger input=%f", mutateInput);
        _processTrigger();
    }

    //   SQINFO("at M214 %f", TBase::outputs[NOTES_OUTPUT].getVoltage(2));
    for (size_t i = 0; i < _theNoteData.getSize(); ++i) {
        //  SQINFO("set volt(1.3, %llu)", i);

        FloatNote floatNote;
        NoteConvert::m2f(floatNote, _theNoteData.getNote(i));
        ///   SQINFO("M220 set note %d to %f", i, floatNote.get());
        TBase::outputs[NOTES_OUTPUT].setVoltage(floatNote.get(), i);

        // const float q =TBase::outputs[NOTES_OUTPUT].getVoltage(i);
        //  SQINFO("proc: just set output %llu to %f, is %f", i, floatNote.get(), q);
        //  SQINFO("addr of port = %p", &TBase::outputs[NOTES_OUTPUT]);
    }
    //  SQINFO("at M226 %f", TBase::outputs[NOTES_OUTPUT].getVoltage(2));
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

    // SQINFO("enter process trigger----");

    _theStyle.leapsWeight = 4 * _audioCurve(TBase::params[LEAPS_WEIGHT_STYLE_PARAM].value);
    _theStyle.unisonWeight = 4 * _audioCurve(TBase::params[UNISON_WEIGHT_STYLE_PARAM].value);
    _theStyle.pitchRangeWeight = 4 * _audioCurve(TBase::params[PITCH_RANGE_WEIGHT_STYLE_PARAM].value);
    _theStyle.idealPitchRange2 = TBase::params[PITCH_RANGE_STYLE_PARAM].value;
    _theStyle.nonCenteredWeight = 4 * _audioCurve(TBase::params[NON_CENTERED_WEIGHT_STYLE_PARAM].value);
    _theStyle.dissonantWeight = 4 * _audioCurve(TBase::params[CONSONANT_WEIGHT_PARAM].value);
#if 0
    SQINFO("process trgger set to %f %f %f %f",
           _theStyle.leapsWeight,
           _theStyle.unisonWeight,
           _theStyle.pitchRangeWeight,
           _theStyle.nonCenteredWeight);
    SQINFO("from %f", TBase::params[NON_CENTERED_WEIGHT_STYLE_PARAM].value);
#endif
#if 0

    SQINFO("process trigger set weight %s", MelodyEvaluator::toString(_theNoteData, _theStyle).c_str());
    //   SQINFO("style params = %s", _theStyle.toString().c_str());
    //   SQINFO("%s", MelodyEvaluator::toString(_theNoteData, _theStyle).c_str());
#endif
    MelodyGenerator::mutate(_theNoteData, _theState, _theStyle);
    //  SQINFO("notes: %s", _theNoteData.print().c_str());

    // SQINFO("exit process trigger----");
}
