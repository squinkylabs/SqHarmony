#pragma once

#include "ChordRecognizer.h"
#include "Divider.h"
#include "KeysigOld.h"
#include "Options.h"
#include "PESConverter.h"
#include "Scale.h"
#include "ScaleQuantizer.h"
#include "SqLog.h"

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

    std::tuple<const int*, unsigned> getQuantizedPitchesAndChannels() const {
        return std::make_tuple(_quantizedInputPitches, _outputChannels);
    }

private:
    void _init();
    void _stepn();
    void _processInput();
    void _lookForKeysigChange();
    void _servicePES();

    Divider _divn;

    // quantized pitches and number of valid entries.
    int _quantizedInputPitches[16] = {0};
    unsigned _outputChannels = 0;
    ScaleQuantizerPtr _quantizer;
    OptionsPtr _chordOptions;
};

template <class TBase>
inline void Visualizer<TBase>::_init() {
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
inline void Visualizer<TBase>::_processInput() {
    // Read in the CV from the input port.
    //  bool gates[16] = {0};
    bool wasChange = false;
    auto& inputPort = TBase::inputs[CV_INPUT];
    auto& gatePort = TBase::inputs[GATE_INPUT];

    const unsigned gateChannels = gatePort.isConnected() ? gatePort.getChannels() : 0;
    const unsigned cvChannels = inputPort.getChannels();
    unsigned outputChannel = 0;
    // Loop throught all the input CV that are valid, building up a list of quantized pitches.
    for (unsigned inputChannel = 0; inputChannel < cvChannels; ++inputChannel) {
        const float f = inputPort.getVoltage(inputChannel);

        //
        bool gate;
        if (!gatePort.isConnected()) {
            gate = true;                // if the port isn't connected, the all gates assumed "on"
        } else {
            gate = (inputChannel > gateChannels) ? false : gatePort.getVoltage(inputChannel) > 5;
        }

        if (gate) {
            const MidiNote mn = _quantizer->run(f);
            const int iNote = mn.get();
            if (_quantizedInputPitches[outputChannel] != iNote) {
                _quantizedInputPitches[outputChannel] = iNote;
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
    for (int i = outputChannel; i < 16; ++i) {
        _quantizedInputPitches[i] = 0;
    }

    if (!wasChange) {
        return;
    }

    // Now put the new chord into the params.
    const auto chord = ChordRecognizer::recognize(_quantizedInputPitches, _outputChannels);
    TBase::params[TYPE_PARAM].value = int(ChordRecognizer::typeFromInfo(chord));
    TBase::params[ROOT_PARAM].value = ChordRecognizer::pitchFromInfo(chord);

    // And signal a change.
    TBase::params[INVERSION_PARAM].value = int(ChordRecognizer::inversionFromInfo(chord));
    TBase::params[CHANGE_PARAM].value += 1;
    if (TBase::params[CHANGE_PARAM].value >= 100) {
        TBase::params[CHANGE_PARAM].value = 0;
    }
}

template <class TBase>
inline void Visualizer<TBase>::process(const typename TBase::ProcessArgs& args) {
    _divn.step();
}

template <class TBase>
inline void Visualizer<TBase>::_servicePES() {
    // This is all fake.
    auto pes = PESConverter::convertToPES(TBase::inputs[PES_INPUT]);
    PESConverter::outputPES(TBase::outputs[PES_OUTPUT], pes);
    assert(false);
}

template <class TBase>
inline void Visualizer<TBase>::_lookForKeysigChange() {
    // This for PES support
    // _pollPESInput();
    const int basePitch = int(std::round((Visualizer<TBase>::params[KEY_PARAM].value)));
    const auto mode = Scale::Scales(int(std::round(Visualizer<TBase>::params[MODE_PARAM].value)));
    // const auto newSetting = std::make_pair(basePitch, mode);

    const auto current = _chordOptions->keysig->get();
    const int currentPitch = current.first.get();
    if ((current.second != mode) || (currentPitch != basePitch)) {
        // this form Hramony for efficiency
        //  _mustUpdate = true;
        SQINFO("in look for change, see new mode = %d", int(mode));
        _chordOptions->keysig->set(MidiNote(basePitch), mode);
        //  quantizerOptions->scale->set(MidiNote(basePitch), mode);
    }
}