#pragma once


#include "ScaleNote.h"

#include <string>
#include <vector>

#include "Divider.h"
#include "FloatNote.h"
#include "GateTrigger.h"
#include "NoteConvert.h"

#include "SqLog.h"
#include "ScaleQuantizer.h"

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
        XPOSE_DEGREE_2_PARAM,
        XPOSE_DEGREE_3_PARAM,
        XPOSE_DEGREE_4_PARAM,
        XPOSE_DEGREE_5_PARAM,
        XPOSE_DEGREE_6_PARAM,
        XPOSE_OCTAVE1_PARAM,
        XPOSE_OCTAVE_2_PARAM,
        XPOSE_OCTAVE_3_PARAM,
        XPOSE_OCTAVE_4_PARAM,
        XPOSEO_OCTAVE_5_PARAM,
        XPOSE_OCTAVE_6_PARAM,
        XPOSE_ENABLE1_PARAM,
        XPOSE_ENABLE_2_PARAM,
        XPOSE_ENABLE_3_PARAM,
        XPOSE_ENABLE_4_PARAM,
        XPOSE_ENABLE_5_PARAM,
        XPOSE_ENABLE_6_PARAM,
        XPOSE_TOTAL1_PARAM,
        XPOSE_TOTAL_2_PARAM,
        XPOSE_TOTAL_3_PARAM,
        XPOSE_TOTAL_4_PARAM,
        XPOSE_TOTAL_5_PARAM,
        XPOSE_TOTAL_6_PARAM,
        XPOSE_ENABLEREQ1_PARAM,
        XPOSE_ENABLEREQ2_PARAM,
        XPOSE_ENABLEREQ3_PARAM,
        XPOSE_ENABLEREQ4_PARAM,
        XPOSE_ENABLEREQ5_PARAM,
        XPOSE_ENABLEREQ6_PARAM,
        KEY_PARAM,
        MODE_PARAM,
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

private:
    Divider _divn;
    GateTrigger _ButtonProcs[NUM_TRANPOSERS];
    ScaleQuantizerPtr _quantizer;
    ScaleQuantizer::OptionsPtr _quantizerOptions;

    void _init();
    void _stepn();
    void _serviceEnableButtons();
    void _serviceKeysigInput();
};

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
    _divn.setup(32, [this]() {
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
    _serviceKeysigInput();
}

template <class TBase>
inline void Harmony2<TBase>::_serviceKeysigInput() {
    const int basePitch = int(std::round(TBase::params[KEY_PARAM].value));
    const auto mode = Scale::Scales(int(std::round(TBase::params[MODE_PARAM].value)));
    _quantizerOptions->scale->set(basePitch, mode);
}

template <class TBase>
inline void Harmony2<TBase>::_serviceEnableButtons() {

    if (TBase::params[XPOSE_ENABLEREQ1_PARAM].value > 5) SQINFO("pressed");
    for (int i = 0; i < NUM_TRANPOSERS; ++i) {
        _ButtonProcs[i].go(TBase::params[XPOSE_ENABLEREQ1_PARAM + i].value);
        if (_ButtonProcs[i].trigger()) {
            // SQINFO("trigger");
            // Toggle the value
            TBase::params[XPOSE_ENABLE1_PARAM + i].value = TBase::params[XPOSE_ENABLE1_PARAM + i].value < 5 ? 10 : 0;
        }
        TBase::lights[XPOSE_ENABLE1_LIGHT + i].value = TBase::params[XPOSE_ENABLE1_PARAM + i].value;
    }
  //  SQINFO("light =%f", TBase::lights[XPOSE_ENABLE1_LIGHT].value);
}

static int count=0;
template <class TBase>
inline void Harmony2<TBase>::process(const typename TBase::ProcessArgs& args) {
    _divn.step();
    float input = TBase::inputs[PITCH_INPUT].getVoltage(0);
    MidiNote quantizedInput = _quantizer->run(input);
    

    const int xposeSteps = int(TBase::params[XPOSE_DEGREE1_PARAM].value);
    ScaleNote scaleNote;
    NoteConvert::m2s(scaleNote, *_quantizerOptions->scale, quantizedInput);
    scaleNote.transposeDegree(xposeSteps);
    NoteConvert::s2m(quantizedInput, *_quantizerOptions->scale, scaleNote);

    FloatNote f;
    NoteConvert::m2f(f, quantizedInput);
 

     if (count == 0) {
        SQINFO("\ninput = %f quantized = %d", input, quantizedInput.get());
        SQINFO("xoseSteps %d final CV=%f", xposeSteps, f.get());
    }
    ++count;
    if (count > 80000) {
        count = 0;
    }
}