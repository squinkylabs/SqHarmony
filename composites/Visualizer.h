#pragma once

#include "ChordRecognizer.h"
#include "Divider.h"
#include "FloatNote.h"
#include "KeysigOld.h"
#include "NoteConvert.h"
#include "Options.h"
#include "PESConverter.h"
#include "Scale.h"
#include "ScaleQuantizer.h"
// #include "SqLog.h"

namespace rack {
namespace engine {
struct Module;
}
}  // namespace rack
using Module = ::rack::engine::Module;

template <class TBase>
class Visualizer : public TBase {
public:
    friend class TestX;
    enum ParamIds {
        TYPE_PARAM,       // For reporting back the identified chord type.
        ROOT_PARAM,       // For reporting back the identified chord root.
        CHANGE_PARAM,     // For reporting back the identified chord has changed.
        INVERSION_PARAM,  // For reporting back the identified chord inversion.

        KEY_PARAM,   // For controlling the key signature root.
        MODE_PARAM,  // For controlling the key signature mode.
        SHARPS_FLATS_PARAM,
        NUM_PARAMS
    };
    enum InputIds {
        CV_INPUT,
        GATE_INPUT,
        PES_INPUT,
        NUM_INPUTS
    };

    enum OutputIds {
        PES_OUTPUT,
        ROOT_OUTPUT,
        RECOGNIZED_OUTPUT,
        NUM_OUTPUTS
    };

    enum LightIds {
        PES_INVALID_LIGHT,
        NUM_LIGHTS
    };

    Visualizer(Module* module) : TBase(module) {
        _init();
    }

    Visualizer() : TBase() {
        _init();
    }

    void process(const typename TBase::ProcessArgs& args) override;
    static int getSubSampleFactor() { return 32; }

    ConstScalePtr getScale() const {
        //  SQINFO("in get scale co = %p", _chordOptions.get());
        //   SQINFO("in get scale ks = %p", _chordOptions->keysig.get());

        return _chordOptions->keysig->getUnderlyingScale();
    }

    // std::tuple<const int*, unsigned> getQuantizedPitchesAndChannels() const {
    const SqArray<int, 16>& getQuantizedPitches() const {
        // return std::make_tuple(_quantizedInputPitches.getDirectPtrAt(0), _outputChannels);
        return _activeQuantizedPitches;
    }

private:
    void _init();
    void _stepn();
    void _processInput();
    void _lookForKeysigChange();
    void _servicePES();
    void _getCurrentInput(SqArray<int, 16>&);

    Divider _divn;

    // quantized pitches and number of valid entries.
    //  SqArray<int, 16> _quantizedInputPitches;
    SqArray<int, 16> _activeQuantizedPitches;
    //  unsigned _outputChannels = 0;
    ScaleQuantizerPtr _quantizer;
    OptionsPtr _chordOptions;
};

template <class TBase>
inline void Visualizer<TBase>::_init() {
    //   _quantizedInputPitches.allowRandomAccess();         // so we remain compatible with the stuff from before SqArray.
    _divn.setup(getSubSampleFactor(), [this]() {
        this->_stepn();
    });

    ScaleQuantizer::OptionsPtr _quantizerOptions;
    _quantizerOptions = std::make_shared<ScaleQuantizer::Options>();
    _quantizerOptions->scale = std::make_shared<Scale>();
    _quantizerOptions->scale->set(MidiNote(MidiNote::C), Scale::Scales::Chromatic);
    _quantizer = std::make_shared<ScaleQuantizer>(_quantizerOptions);

    // Chord options get cmag
    auto keysig = std::make_shared<KeysigOld>(Roots::C);
    auto style = std::make_shared<Style>();
    _chordOptions = std::make_shared<Options>(keysig, style);
}

template <class TBase>
inline void Visualizer<TBase>::_stepn() {
    _lookForKeysigChange();
    _processInput();
    _servicePES();
}

template <class TBase>
inline void Visualizer<TBase>::_getCurrentInput(SqArray<int, 16>& out) {
    auto& inputPort = TBase::inputs[CV_INPUT];
    auto& gatePort = TBase::inputs[GATE_INPUT];
    const unsigned gateChannels = gatePort.isConnected() ? gatePort.getChannels() : 0;
    const unsigned cvChannels = inputPort.getChannels();

    for (unsigned inputChannel = 0; inputChannel < cvChannels; ++inputChannel) {
        const float f = inputPort.getVoltage(inputChannel);

        //
        bool gate;
        if (!gatePort.isConnected()) {
            gate = true;  // if the port isn't connected, the all gates assumed "on"
        } else {
            gate = (inputChannel > gateChannels) ? false : gatePort.getVoltage(inputChannel) > 5;
        }

        if (gate) {
            const MidiNote mn = _quantizer->run(f);
            const int iNote = mn.get();

            // add the quantized note
            out.putAt(out.numValid(), iNote);
        }
    }
}

template <class TBase>
inline void Visualizer<TBase>::_processInput() {
    SqArray<int, 16> _newQuantizedPitches;
    _getCurrentInput(_newQuantizedPitches);
    bool changed = false;
    if (_newQuantizedPitches.numValid() != _activeQuantizedPitches.numValid()) {
        changed = true;
        //SQINFO("changed on num changed active=%d new=%d", _activeQuantizedPitches.numValid(), _newQuantizedPitches.numValid());
    } else {
        for (unsigned i = 0; i < _newQuantizedPitches.numValid(); ++i) {
            if (_newQuantizedPitches.getAt(i) != _activeQuantizedPitches.getAt(i)) {
                changed = true;
                //SQINFO("changed on entry pitch");
            }
        }
    }

    if (!changed) {
        return;
    }

    _activeQuantizedPitches.clear();
    // copy over the new ones
    for (unsigned i = 0; i < _newQuantizedPitches.numValid(); ++i) {
        _activeQuantizedPitches.putAt(i, _newQuantizedPitches.getAt(i));
    }

    // Now put the new chord into the params.
    const auto chord = ChordRecognizer::recognize(_activeQuantizedPitches);
    TBase::params[TYPE_PARAM].value = int(chord.type);
    TBase::params[ROOT_PARAM].value = chord.pitch;

    // And signal a change.
    //SQINFO("signal change. now %f", TBase::params[CHANGE_PARAM].value);
    TBase::params[INVERSION_PARAM].value = int(chord.inversion);
    TBase::params[CHANGE_PARAM].value += 1;
    if (TBase::params[CHANGE_PARAM].value >= 100) {
        TBase::params[CHANGE_PARAM].value = 0;
    }

    if (chord.type == ChordRecognizer::Type::Unrecognized) {
        TBase::outputs[RECOGNIZED_OUTPUT].value = 0.f;
    } else {
        // this works - the index is into the inputs...
        const int rootIndex = chord.identifiedPitches.getAt(0).index;
        const int rootMIDIPitch = _activeQuantizedPitches.getAt(rootIndex);
        FloatNote fn;
        MidiNote mn(rootMIDIPitch);
        NoteConvert::m2f(fn, mn);
        TBase::outputs[ROOT_OUTPUT].value = fn.get();
        TBase::outputs[RECOGNIZED_OUTPUT].value = 10.f;
    }
}

#if 0  // old wa
template <class TBase>
inline void Visualizer<TBase>::_processInput() {
    // Read in the CV from the input port.
    //  bool gates[16] = {0};
    bool wasChange = false;
    auto& inputPort = TBase::inputs[CV_INPUT];
    auto& gatePort = TBase::inputs[GATE_INPUT];

    const unsigned gateChannels = gatePort.isConnected() ? gatePort.getChannels() : 0;
    const unsigned cvChannels = inputPort.getChannels();
    unsigned outputChannel = 0;

    if (cvChannels != _quantizedInputPitches.numValid()) {
        _quantizedInputPitches.clear();  // clear them all out
    }
    // Loop through all the input CV that are valid, building up a list of quantized pitches.
    for (unsigned inputChannel = 0; inputChannel < cvChannels; ++inputChannel) {
        const float f = inputPort.getVoltage(inputChannel);

        //
        bool gate;
        if (!gatePort.isConnected()) {
            gate = true;  // if the port isn't connected, the all gates assumed "on"
        } else {
            gate = (inputChannel > gateChannels) ? false : gatePort.getVoltage(inputChannel) > 5;
        }

        if (gate) {
            const MidiNote mn = _quantizer->run(f);
            const int iNote = mn.get();
            // SQINFO("will try to gate in a pitch on channel %d, numvalid=%d", outputChannel, _quantizedInputPitches.numValid());
            // If we are adding one past the last valid one.
            // OR we are changing one already valid
            if ((outputChannel == _quantizedInputPitches.numValid()) ||
                ((_quantizedInputPitches.numValid() > outputChannel) &&
                 (_quantizedInputPitches.getAt(outputChannel) != iNote))) {
                    
                _quantizedInputPitches.putAt(outputChannel, iNote);
                wasChange = true;
            }
            outputChannel++;
        }
    }
    if (outputChannel != _outputChannels) {
        wasChange = true;
        _outputChannels = cvChannels;
    }

    // Zero out all the pitches above our range.
    // for (int i = outputChannel; i < 16; ++i) {
    for (unsigned i = outputChannel; i < _quantizedInputPitches.numValid(); ++i) {
        _quantizedInputPitches.putAt(i, 0);
    }

    if (!wasChange) {
        return;
    }
    assert(_outputChannels >= _quantizedInputPitches.numValid());

    // Now put the new chord into the params.
    const auto chord = ChordRecognizer::recognize(_quantizedInputPitches);
    TBase::params[TYPE_PARAM].value = int(chord.type);
    TBase::params[ROOT_PARAM].value = chord.pitch;

    // And signal a change.
    TBase::params[INVERSION_PARAM].value = int(chord.inversion);
    TBase::params[CHANGE_PARAM].value += 1;
    if (TBase::params[CHANGE_PARAM].value >= 100) {
        TBase::params[CHANGE_PARAM].value = 0;
    }

    if (chord.type == ChordRecognizer::Type::Unrecognized) {
        TBase::outputs[RECOGNIZED_OUTPUT].value = 0.f;
    } else {
        // this works - the index is into the inputs...
        const int rootIndex = chord.identifiedPitches.getAt(0).index;
        const int rootMIDIPitch = _quantizedInputPitches.getAt(rootIndex);
        FloatNote fn;
        MidiNote mn(rootMIDIPitch);
        NoteConvert::m2f(fn, mn);
        TBase::outputs[ROOT_OUTPUT].value = fn.get();
        TBase::outputs[RECOGNIZED_OUTPUT].value = 10.f;
    }
}
#endif

template <class TBase>
inline void Visualizer<TBase>::process(const typename TBase::ProcessArgs& args) {
    _divn.step();
}

template <class TBase>
inline void Visualizer<TBase>::_servicePES() {
    // SQINFO("service pes 180");
    //  Parse out the incoming PES.
    auto pesInput = TBase::inputs[PES_INPUT];
    auto pes = PESConverter::convertToPES(pesInput, false);
    // SQINFO("service pes 183");
    if (!pes.valid) {
        TBase::lights[PES_INVALID_LIGHT].value = pesInput.isConnected() ? 8 : 0;
        pes.valid = true;
        // ok, get valid pes now so we can send it out.
        pes.keyRoot = MidiNote::C + int(std::round(TBase::params[KEY_PARAM].value));
        pes.mode = Scale::Scales(int(std::round(TBase::params[MODE_PARAM].value)));
        // SQINFO("service pes 184");
    } else {
        // Just
        TBase::lights[PES_INVALID_LIGHT].value = 0;
        TBase::params[KEY_PARAM].value = pes.keyRoot;
        TBase::params[MODE_PARAM].value = int(pes.mode);
    }
    PESConverter::outputPES(TBase::outputs[PES_OUTPUT], pes);
}

template <class TBase>
inline void Visualizer<TBase>::_lookForKeysigChange() {
    const int basePitch = int(std::round((Visualizer<TBase>::params[KEY_PARAM].value)));
    const auto mode = Scale::Scales(int(std::round(Visualizer<TBase>::params[MODE_PARAM].value)));

    const auto current = _chordOptions->keysig->get();
    const int currentPitch = current.first.get();
    if ((current.second != mode) || (currentPitch != basePitch)) {
        _chordOptions->keysig->set(MidiNote(basePitch), mode);
    }
}