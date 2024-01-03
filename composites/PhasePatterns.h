
#pragma once

#include "ClockShifter4.h"
#include "Divider.h"
#include "FreqMeasure.h"
#include "GateTrigger.h"
#include "ShiftCalc.h"

namespace rack {
namespace engine {
struct Module;
}
}  // namespace rack

using Module = ::rack::engine::Module;
template <class TBase>
class PhasePatterns : public TBase {
public:
    enum ParamIds {
        SCHEMA_PARAM,
        SHIFT_PARAM,
        COMBINED_SHIFT_INTERNAL_PARAM,
        RIB_BUTTON_PARAM,
        NUM_PARAMS
    };
    enum InputIds {
        CK_INPUT,
        SHIFT_INPUT,
        RIB_INPUT,
        NUM_INPUTS
    };

    enum OutputIds {
        CK_OUTPUT,
        NUM_OUTPUTS
    };

    enum LightIds {
        RIB_LIGHT,
        NUM_LIGHTS
    };

    PhasePatterns(Module* module) : TBase(module) {
        _init();
    }
    PhasePatterns() : TBase() {
        _init();
    }

    void process(const typename TBase::ProcessArgs& args) override;

    static int getSubSampleFactor() { return 32; }

private:
    void _init();
    void _stepn();
    void _updateButton();
    void _updatePoly();
    void _updateShiftAmount();

    ClockShifter4 _clockShifter[16];
    ShiftCalc _shiftCalculator[16];

    // TODO: get rid of these initializers. There are just here to make some test pass.
    GateTrigger _inputClockProc[16] = {false, false, false, false,
                                       false, false, false, false,
                                       false, false, false, false,
                                       false, false, false, false};
    GateTrigger _buttonProc;
    Divider divn;

    int _numInputClocks = 0;
    int _numRibsGenerators = 0;
    int _numOutputClocks = 0;
};

template <class TBase>
inline void PhasePatterns<TBase>::_init() {
    divn.setup(this->getSubSampleFactor(), [this]() {
        this->_stepn();
    });
}

template <class TBase>
inline void PhasePatterns<TBase>::_updateButton() {
    // TODO: for now, just do channel 1 for this
    TBase::lights[RIB_LIGHT].value = _shiftCalculator[0].busy() ? 10 : 0;
    _buttonProc.go(TBase::params[RIB_BUTTON_PARAM].value);
    if (!_buttonProc.trigger()) {
        return;
    }
    if (!_clockShifter[0].freqValid()) {
        return;
    }

    // TODO: this is totally wrong for poly
    _shiftCalculator[0].trigger(_clockShifter[0].getPeriod());
}

template <class TBase>
inline void PhasePatterns<TBase>::_updateShiftAmount() {
    float shift = TBase::params[SHIFT_PARAM].value;
    shift += TBase::inputs[SHIFT_INPUT].value;
    shift += _shiftCalculator[0].get();
    _clockShifter[0].setShift(shift);
    TBase::params[COMBINED_SHIFT_INTERNAL_PARAM].value = shift;
}

template <class TBase>
inline void PhasePatterns<TBase>::_updatePoly() {
    const bool conn = TBase::outputs[CK_OUTPUT].isConnected();
    if (!conn) {
        return;
    }
    int numOutputs = 1;
    _numInputClocks = TBase::inputs[CK_INPUT].channels;
    _numRibsGenerators = TBase::inputs[RIB_INPUT].channels;
    _numRibsGenerators = std::max(1, _numRibsGenerators);

    numOutputs = std::max(numOutputs, _numInputClocks);
    numOutputs = std::max(numOutputs, _numRibsGenerators);
    numOutputs = std::max(numOutputs, int(TBase::inputs[SHIFT_INPUT].channels));

    _numOutputClocks = numOutputs;
    TBase::outputs[CK_OUTPUT].setChannels(numOutputs);
    SQINFO("out=%d, ic=%d, rib=%d", _numOutputClocks, _numInputClocks, _numRibsGenerators);
}

template <class TBase>
inline void PhasePatterns<TBase>::_stepn() {
    _updatePoly();
    _updateShiftAmount();
    _updateButton();
    _updateShiftAmount();
}

template <class TBase>
inline void PhasePatterns<TBase>::process(const typename TBase::ProcessArgs& args) {
    divn.step();

    // First process all the input clock channels. They retain output, and don't have any
    // dependencies, so they are easy. Also update all the RIB ramp generators
    for (int i = 0; i < _numInputClocks; ++i) {
        const float rawClockIn = TBase::inputs[CK_INPUT].getVoltage(i);
        _inputClockProc[i].go(rawClockIn);
        _shiftCalculator[i].go();
    }

    bool monoClock = false;
    // this is the only case we can handle right now
    if (_numOutputClocks > 1) {
        if (_numInputClocks != _numOutputClocks) {
            // SQINFO("unequal clock case, nimp");
            monoClock = true;
        }
    }
    for (int i = 0; i < _numOutputClocks; ++i) {
        const int clockInputIndex = monoClock ? 0 : i;
        const bool rawClockOut = _clockShifter[clockInputIndex].process(
            _inputClockProc[clockInputIndex].trigger(),
            _inputClockProc[clockInputIndex].gate());
        const float clockOut = rawClockOut ? cGateOutHi : cGateOutLow;
        TBase::outputs[CK_OUTPUT].setVoltage(clockOut, i);
    }
}
