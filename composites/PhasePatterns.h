
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
    friend class TestX;
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
    GateTrigger _ribTrigger[16];
    Divider divn;

    int _numInputClocks = 0;
    int _numRibsGenerators = 0;
    int _numOutputClocks = 0;
    int _numShiftInputs = 0;
};

template <class TBase>
inline void PhasePatterns<TBase>::_init() {
    divn.setup(this->getSubSampleFactor(), [this]() {
        this->_stepn();
    });
}

template <class TBase>
inline void PhasePatterns<TBase>::_updateButton() {
    SQINFO("update button");
    // TODO: for now, just do channel 1 for this
    TBase::lights[RIB_LIGHT].value = _shiftCalculator[0].busy() ? 10 : 0;
    _buttonProc.go(TBase::params[RIB_BUTTON_PARAM].value);

    if (_numRibsGenerators < 1) {
        SQINFO("skip rib process, no input");
        return;
    }

    // This loop assumes there are at least as many shifters as there are rib
    // units. Which is fair.
    const bool buttonTriggered = _buttonProc.trigger();
    for (int i = 0; i < _numRibsGenerators; ++i) {
        const float rawRibTrigger = TBase::inputs[RIB_INPUT].getVoltage(i);
        _ribTrigger[i].go(rawRibTrigger);
        const bool ribTriggered = _ribTrigger[i].trigger();
        if (buttonTriggered || ribTriggered) {
            // If this channel isn't stable yet, skip it.
            if (!_clockShifter[i].freqValid()) {
                continue;
            }
            SQINFO("will trigger rib for ch %d period %d", i, (_clockShifter[i].getPeriod()));
            _shiftCalculator[i].trigger(_clockShifter[i].getPeriod());
        }
    }

#if 0
    // mono only - won't work with poly
    const float rawRibTrigger = TBase::inputs[RIB_INPUT].getVoltage(0);
    _ribTrigger[0].go(rawRibTrigger);
    SQINFO("in btn, raw rib = %f trig=%d", rawRibTrigger, _ribTrigger[0].trigger());

    if (_numRibsGenerators != 1) SQINFO("ribs is not poly yet %d", _numRibsGenerators);
    if (!_buttonProc.trigger() && !_ribTrigger[0].trigger()) {
        return;
    }

    for (int i = 0; i < _numOutputClocks; ++i) {
        if (!_clockShifter[i].freqValid()) {
            return;
        }
      
    }
    for (int i = 0; i < _numOutputClocks; ++i) {
        SQINFO("in loop busy=%d, will trig %d", _shiftCalculator[0].busy(), i);
        // TODO: this is totally wrong for poly
        _shiftCalculator[i].trigger(_clockShifter[0].getPeriod());
    }
#endif
}

template <class TBase>
inline void PhasePatterns<TBase>::_updateShiftAmount() {
    // TODO: this code assumes that the shift input is mono
    const float globalShift = TBase::params[SHIFT_PARAM].value;
    const bool ribsPoly = _numRibsGenerators > 1;
    const bool shiftCVPoly = _numShiftInputs > 1;

 
    for (int i = 0; i < _numOutputClocks; ++i) {
        const int ribIndex = ribsPoly ? i : 0;
        const int shiftCVIndex = shiftCVPoly ? i : 0;
        const float shift = globalShift +
                            _shiftCalculator[ribIndex].get() +
                            TBase::inputs[SHIFT_INPUT].getVoltage(shiftCVIndex);
        _clockShifter[i].setShift(shift);
    }

    // put channel 0 in the UI.
    TBase::params[COMBINED_SHIFT_INTERNAL_PARAM].value = globalShift + _shiftCalculator[0].get();
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

    _numShiftInputs = TBase::inputs[SHIFT_INPUT].channels;

    SQINFO("out=%d, ic=%d, rib=%d shiftInputs=%d", _numOutputClocks, _numInputClocks, _numRibsGenerators, _numShiftInputs);
}

template <class TBase>
inline void PhasePatterns<TBase>::_stepn() {
    //   SQINFO("stepn");
    _updatePoly();
    _updateShiftAmount();
    _updateButton();
    _updateShiftAmount();
}

template <class TBase>
inline void PhasePatterns<TBase>::process(const typename TBase::ProcessArgs& args) {
    //  SQINFO("process");
    divn.step();
    //  SQINFO("process2");

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
    // SQINFO("mono clock = %d", monoClock);
    for (int i = 0; i < _numOutputClocks; ++i) {
        const int clockInputIndex = monoClock ? 0 : i;
        const bool rawClockOut = _clockShifter[clockInputIndex].process(
            _inputClockProc[clockInputIndex].trigger(),
            _inputClockProc[clockInputIndex].gate());
        const float clockOut = rawClockOut ? cGateOutHi : cGateOutLow;
        TBase::outputs[CK_OUTPUT].setVoltage(clockOut, i);
    }
}
