#pragma once

#include "ChordRecognizer.h"
#include "Divider.h"
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
        TYPE_PARAM,
        ROOT_PARAM,
        CHANGE_PARAM,
        INVERSION_PARAM,
        NUM_PARAMS
    };
    enum InputIds {
        CV_INPUT,
        NUM_INPUTS
    };

    enum OutputIds {
        NUM_OUTPUTS
    };

    enum LightIds {
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

private:
    void _init();
    void _stepn();
    void _processInput();

    Divider _divn;
    int _inputPitches[16] = {0};
    int _inputChannels = 0;
    ScaleQuantizerPtr _quantizer;
  
};

template <class TBase>
inline void Visualizer<TBase>::_init() {
    _divn.setup(getSubSampleFactor(), [this]() {
        this->_stepn();
    });

    ScaleQuantizer::OptionsPtr _quantizerOptions;
    _quantizerOptions = std::make_shared<ScaleQuantizer::Options>();
    _quantizerOptions->scale = std::make_shared<Scale>();
    _quantizerOptions->scale->set(MidiNote::C, Scale::Scales::Chromatic);
    _quantizer = std::make_shared<ScaleQuantizer>(_quantizerOptions);
}

template <class TBase>
inline void Visualizer<TBase>::_stepn() {
    _processInput();
}

template <class TBase>
inline void Visualizer<TBase>::_processInput() {

    // Read in the CV from the input port.
    bool wasChange = false;
    auto& inputPort = TBase::inputs[CV_INPUT];
    const int channels = inputPort.getChannels();
    for (int i = 0; i < channels; ++i) {
        const float f = inputPort.getVoltage(i);
        const MidiNote mn = _quantizer->run(f);
        const int iNote = mn.get();
        if (_inputPitches[i] != iNote) {
            _inputPitches[i] = iNote;
            wasChange = true;
            SQINFO("set was change 89");
        }
    }
    if (channels != _inputChannels) {
        wasChange = true;
        SQINFO("set change 94 ");
        _inputChannels = channels;
    }
    for (int i = channels; i < 16; ++i) {
        _inputPitches[i] = 0;
    }

    if (!wasChange) {
        return;
    }
    const auto chord = ChordRecognizer::recognize(_inputPitches, _inputChannels);

    TBase::params[CHANGE_PARAM].value += 1;
    if (TBase::params[CHANGE_PARAM].value >= 100) {
        TBase::params[CHANGE_PARAM].value = 0;
    }
    TBase::params[TYPE_PARAM].value = int(ChordRecognizer::typeFromInfo(chord));
    TBase::params[ROOT_PARAM].value = ChordRecognizer::pitchFromInfo(chord);
    TBase::params[INVERSION_PARAM].value =  int(ChordRecognizer::inversionFromInfo(chord));
}

template <class TBase>
inline void Visualizer<TBase>::process(const typename TBase::ProcessArgs& args) {
    _divn.step();
}