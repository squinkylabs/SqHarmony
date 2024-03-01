
#pragma once

#include "ClockShifter6.h"
#include "Divider.h"
#include "FreqMeasure.h"
#include "GateTrigger.h"
#include "ShiftCalc.h"

#include <cmath>
#include <vector>
#include <string>

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
        RIB_POSITIVE_BUTTON_PARAM,
        RIB_DURATION_PARAM, // This is is the "numerator", controlled by the widget currently called "Total"
        RIB_SPAN_PARAM,     // This is the denominator, controlled by that widget called "Dur"
        RIB_NEGATIVE_BUTTON_PARAM,
        SHIFT_RANGE_PARAM,
        NUM_PARAMS
    };
    enum InputIds {
        CK_INPUT,
        SHIFT_INPUT,
        RIB_POSITIVE_INPUT,
        RIB_NEGATIVE_INPUT,
        NUM_INPUTS
    };

    enum OutputIds {
        CK_OUTPUT,
        NUM_OUTPUTS
    };

    enum LightIds {
        RIB_POSITIVE_LIGHT,
        RIB_NEGATIVE_LIGHT,
        NUM_LIGHTS
    };

    static const std::vector<std::string> getRibDurationLabels() {
        return {"1/3", "1/2", "1", "2", "3"};
    }

    static float getRibDurationFromIndex(int index) {
        float ret = 1;
        switch (index) {
            case 0:
                ret = 1.f / 3.f;
                break;
            case 1:
                ret = 1.f / 2.f;
                break;
            case 2:
                ret = 1;
                break;
            case 3:
                ret = 2;
                break;
            case 4:
                ret = 3;
                break;
            default:
                assert(false);
        }
        return ret;
    }

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
    void _updateButtons();
    void _updatePoly();
    void _updateShiftAmount();

    ClockShifter6 _clockShifter[16];
    ShiftCalc _ribGenerator[16];

    // TODO: get rid of these initializers. There are just here to make some test pass.
    GateTrigger _inputClockProc[16];
    GateTrigger _positiveButtonProc;
    GateTrigger _negativeButtonProc;
    GateTrigger _ribPositiveTrigger[16];
    GateTrigger _ribNegativeTrigger[16];
    float _curShift[16] = {0};
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
inline void PhasePatterns<TBase>::_updateButtons() {
    //  TODO: for now, just do channel 1 for this indicator
    TBase::lights[RIB_POSITIVE_LIGHT].value = _ribGenerator[0].busyPositive() ? 10 : 0;
    TBase::lights[RIB_NEGATIVE_LIGHT].value = _ribGenerator[0].busyNegative() ? 10 : 0;
    _positiveButtonProc.go(TBase::params[RIB_POSITIVE_BUTTON_PARAM].value);
    _negativeButtonProc.go(TBase::params[RIB_NEGATIVE_BUTTON_PARAM].value);

    if (_numRibsGenerators < 1) {
        return;
    }

    // This loop assumes there are at least as many shifters as there are rib
    // units. Which is fair.
    const bool positiveButtonTriggered = _positiveButtonProc.trigger();
    const bool negativeButtonTriggered = _negativeButtonProc.trigger();
    for (int i = 0; i < _numRibsGenerators; ++i) {
        {
            const float rawPositiveRibTrigger = TBase::inputs[RIB_POSITIVE_INPUT].getVoltage(i);
            _ribPositiveTrigger[i].go(rawPositiveRibTrigger);
            const float rawNegativeRibTrigger = TBase::inputs[RIB_NEGATIVE_INPUT].getVoltage(i);
            _ribNegativeTrigger[i].go(rawNegativeRibTrigger);
        }

        const bool ribTriggeredPositive = _ribPositiveTrigger[i].trigger();
        const bool ribTriggeredNegative = _ribNegativeTrigger[i].trigger();

        const bool trigNegative = ribTriggeredNegative || negativeButtonTriggered;
        const bool triggered = trigNegative || ribTriggeredPositive || positiveButtonTriggered;

        if (triggered) {
            // If this channel isn't stable yet, skip it.
            if (!_clockShifter[i].freqValid()) {
                SQINFO("not triggering rib, no freq.");
                continue;
            }
            // SQINFO("will trigger rib for ch %d period %d", i, (_clockShifter[i].getPeriod()));
            
            const int period = _clockShifter[i].getPeriod();
            const int durationIndex = int(std::round(TBase::params[RIB_DURATION_PARAM].value));
            float duration = this->getRibDurationFromIndex(durationIndex);
            const float span = TBase::params[RIB_SPAN_PARAM].value;
            assert(span != 0);
            if (trigNegative) {
                duration = -duration;
            }
           
            _ribGenerator[i].trigger(period, duration, span);
        }
    }
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
                            _ribGenerator[ribIndex].get() +
                            .2 * TBase::inputs[SHIFT_INPUT].getVoltage(shiftCVIndex);       // .2 so 5 volts -> 1
       // _clockShifter[i].setShift(shift);
        _curShift[i] = shift;
    }

    // put channel 0 in the UI.
    TBase::params[COMBINED_SHIFT_INTERNAL_PARAM].value = globalShift + _ribGenerator[0].get();
}

template <class TBase>
inline void PhasePatterns<TBase>::_updatePoly() {
    const bool conn = TBase::outputs[CK_OUTPUT].isConnected();
    if (!conn) {
        return;
    }

    int numOutputs = 1;
    _numInputClocks = TBase::inputs[CK_INPUT].channels;
    _numRibsGenerators = int(TBase::inputs[RIB_POSITIVE_INPUT].channels);
    _numRibsGenerators = std::max(
        _numRibsGenerators, 
        int(TBase::inputs[RIB_NEGATIVE_INPUT].channels));
    _numRibsGenerators = std::max(1, _numRibsGenerators);

    numOutputs = std::max(numOutputs, _numInputClocks);
    numOutputs = std::max(numOutputs, _numRibsGenerators);
    numOutputs = std::max(numOutputs, int(TBase::inputs[SHIFT_INPUT].channels));

    _numOutputClocks = numOutputs;
    TBase::outputs[CK_OUTPUT].setChannels(numOutputs);

    _numShiftInputs = TBase::inputs[SHIFT_INPUT].channels;

    //  SQINFO("updatePoly: out=%d, ic=%d, rib=%d shiftInputs=%d", _numOutputClocks, _numInputClocks, _numRibsGenerators, _numShiftInputs);
}

template <class TBase>
inline void PhasePatterns<TBase>::_stepn() {
    //   SQINFO("stepn");
    _updatePoly();
    _updateShiftAmount();
    _updateButtons();
    _updateShiftAmount();
}

template <class TBase>
inline void PhasePatterns<TBase>::process(const typename TBase::ProcessArgs& args) {
    //  SQINFO("process");
    divn.step();
    // SQINFO("process2");

    // const bool monoClocks = _n_numInputClock <= 1;
    // First process all the input clock channels. They retain output, and don't have any
    // dependencies, so they are easy. Also update all the RIB ramp generators
    for (int i = 0; i < _numInputClocks; ++i) {
        const float rawClockIn = TBase::inputs[CK_INPUT].getVoltage(i);
        _inputClockProc[i].go(rawClockIn);
    }

    for (int i = 0; i < _numRibsGenerators; ++i) {
        _ribGenerator[i].go();
    }

    const bool monoClock = _numInputClocks <= 1;
    for (int i = 0; i < _numOutputClocks; ++i) {
        const int clockInputIndex = monoClock ? 0 : i;
        const bool rawClockOut = _clockShifter[i].process(
            _inputClockProc[clockInputIndex].gate(),
            _curShift[i]);
        const float clockOut = rawClockOut ? cGateOutHi : cGateOutLow;
        TBase::outputs[CK_OUTPUT].setVoltage(clockOut, i);
    }
}
