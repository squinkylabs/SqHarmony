
#pragma once

#include <cmath>
#include <string>
#include <vector>

#include "ClockShifter6.h"
#include "DifferentialClockCounter.h"
#include "Divider.h"
#include "FreqMeasure.h"
#include "GateTrigger.h"
#include "ShiftCalc.h"

// #define _DCC        // diagnostic

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
        RIB_DURATION_PARAM,  // This is is the "numerator", controlled by the widget currently called "Total"
        RIB_SPAN_PARAM,      // This is the denominator, controlled by that widget called "Dur"
        RIB_NEGATIVE_BUTTON_PARAM,
        SHIFT_RANGE_PARAM,
        USE_ADVANCED_UI_PARAM,
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

    static float indexToValueRibDuration(int index);
    static int valueToIndexRibDuration(float value);
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
#ifdef _DCC
        _dcc._mode = DifferentialClockCounter::Mode::MeasureRefInput;
#endif
    }
    PhasePatterns() : TBase() {
        _init();
#ifdef _DCC
        _dcc._mode = DifferentialClockCounter::Mode::MeasureRefInput;
#endif
    }

    void process(const typename TBase::ProcessArgs& args) override;

    static int getSubSampleFactor() { return 32; }

    float _getShift(unsigned channel) const {
        assert(channel < 16);
        return _curShift[channel];
    }

    void onReset() override;

private:
    void _init();
    void _stepn();
    void _updateButtons();
    void _updatePoly();
    void _updateShiftAmount();

    ClockShifter6 _clockShifter[16];
    ShiftCalc _ribGenerator[16];

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

#ifdef _DCC
    DifferentialClockCounter _dcc;  // for debugging
#endif
    float _lastShift = 0;
};

template <class TBase>
inline void PhasePatterns<TBase>::onReset() {
    // SQINFO("ON RESET");
    for (int i = 0; i < 16; ++i) {
        _ribGenerator[i].reset();
        _curShift[i] = 0;
    }
}

template <class TBase>
inline float PhasePatterns<TBase>::indexToValueRibDuration(int index) {
    float ret = 0;
    switch (index) {
        case 0:
            ret = 1.f / 3.f;
            break;
        case 1:
            ret = 1.f / 2.f;
            break;
        case 2:
            ret = 1.f;
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

template <class TBase>
inline int PhasePatterns<TBase>::valueToIndexRibDuration(float value) {
    if (value > 1.5f) {
        return (value > 2.5) ? 4 : 3;
    }
    if (value < 1.f / 2.5f) {
        return 0;
    }
    return (value < .7) ? 1 : 2;
}

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
                continue;
            }

            const int period = _clockShifter[i].getPeriod();
            float duration = TBase::params[RIB_DURATION_PARAM].value;
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
    const int sens = int(std::round(TBase::params[SHIFT_RANGE_PARAM].value));
    float shiftMult = 1;
    switch (sens) {
        case 0:
            shiftMult = 1;
            break;
        case 1:
            shiftMult = 10;
            break;
        case 2:
            shiftMult = 100;
            break;
        default:
            assert(0);
    }

    const float globalShift = TBase::params[SHIFT_PARAM].value;
    const bool ribsPoly = _numRibsGenerators > 1;
    const bool shiftCVPoly = _numShiftInputs > 1;

    for (int i = 0; i < _numOutputClocks; ++i) {
        const int ribIndex = ribsPoly ? i : 0;
        const int shiftCVIndex = shiftCVPoly ? i : 0;
        float shift = globalShift +
                      _ribGenerator[ribIndex].get() +
                      .1 * TBase::inputs[SHIFT_INPUT].getVoltage(shiftCVIndex);  // .2 so 5 volts -> 1
        shift *= shiftMult;
        // if ((shift != _lastShift) && (i == 0)) {
        //     SQINFO("shift=%f, mult=%f glob=%f rib=%f cv = %f",
        //         shift,
        //         shiftMult,
        //         globalShift,
        //         _ribGenerator[0].get(),
        //         TBase::inputs[SHIFT_INPUT].getVoltage(0 ));
        //     SQINFO("delta = %e", (_lastShift - shift));
        //     _lastShift = shift;

        // }
        //  SQINFO("shifter[%d] set to %f", i, shift);
        _curShift[i] = std::max(shift, 0.f);
    }

    // put channel 0 in the UI.
    TBase::params[COMBINED_SHIFT_INTERNAL_PARAM].value = _curShift[0];

    // TBase::params[COMBINED_SHIFT_INTERNAL_PARAM].value = shiftMult * (globalShift + _ribGenerator[0].get()) + _curShift[0];
    //   SQINFO("shift internal set to %f from mult=%f glb=%f rib=%f",
    //      TBase::params[COMBINED_SHIFT_INTERNAL_PARAM].value,
    //      shiftMult,
    //      globalShift,
    //       _ribGenerator[0].get());
}

template <class TBase>
inline void PhasePatterns<TBase>::_updatePoly() {
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
}

template <class TBase>
inline void PhasePatterns<TBase>::_stepn() {
    _updatePoly();
    _updateShiftAmount();
    _updateButtons();
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

#ifdef _DCC
    _dcc.process(TBase::inputs[CK_INPUT].getVoltage(0), TBase::outputs[CK_OUTPUT].getVoltage(0));
#endif
    //_dcc.dump();
}
