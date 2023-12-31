
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

private:
    void _init();
    void _stepn();
    void _updateButton();
    void _updateShiftAmount();

    ClockShifter4 _clockShifter;
    ShiftCalc _shiftCalculator;
    GateTrigger _inputClockProc;
    GateTrigger _buttonProc;
    Divider divn;
};

template <class TBase>
inline void PhasePatterns<TBase>::_init() {
    divn.setup(32, [this]() {
        this->_stepn();
    });
}

template <class TBase>
inline void PhasePatterns<TBase>::_updateButton() {
    TBase::lights[RIB_LIGHT].value = _shiftCalculator.busy() ? 10 : 0;
    _buttonProc.go(TBase::params[RIB_BUTTON_PARAM].value);
    if (!_buttonProc.trigger()) {
        return;
    }
    if (!_clockShifter.freqValid()) {
        return;
    }

    _shiftCalculator.trigger(_clockShifter.getPeriod());
}

template <class TBase>
inline void PhasePatterns<TBase>::_updateShiftAmount() {
    float shift = TBase::params[SHIFT_PARAM].value;
    shift += TBase::inputs[SHIFT_INPUT].value;
    shift += _shiftCalculator.get();
    _clockShifter.setShift(shift);
    TBase::params[COMBINED_SHIFT_INTERNAL_PARAM].value = shift;
}

template <class TBase>
inline void PhasePatterns<TBase>::_stepn() {
    _updateShiftAmount();
    _updateButton();
    _updateShiftAmount();
}

template <class TBase>
inline void PhasePatterns<TBase>::process(const typename TBase::ProcessArgs& args) {
    divn.step();
    _shiftCalculator.go();
    const float rawClockIn = TBase::inputs[CK_INPUT].getVoltage();
    _inputClockProc.go(rawClockIn);
   // const bool clockIn = _inputClockProc.go(rawClockIn);
    const bool rawClockOut = _clockShifter.process(_inputClockProc.trigger(), _inputClockProc.gate());
    const float clockOut = rawClockOut ? cGateOutHi : cGateOutLow;
    TBase::outputs[CK_OUTPUT].setVoltage(clockOut);
}
