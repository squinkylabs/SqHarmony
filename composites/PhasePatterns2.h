
#pragma once

#include "ClockShifter4.h"
#include "Divider.h"
#include "FreqMeasure.h"
#include "GateTrigger.h"
#include "PolyClockShifter.h"
#include "ShiftCalc.h"

namespace rack {
namespace engine {
struct Module;
}
}  // namespace rack

using Module = ::rack::engine::Module;
template <class TBase>
class PhasePatterns2 : public TBase {
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

    PhasePatterns2(Module* module) : TBase(module) {
        _init();
    }
    PhasePatterns2() : TBase() {
        _init();
    }

    void process(const typename TBase::ProcessArgs& args) override;

private:
    void _init();
    void _stepn();
    void _updateButton();
    void _updateShiftAmount();

    void _fillInfo(PolyClockShifter::PortInfo& info);

    // ClockShifter4 _clockShifter;
    // ShiftCalc _shiftCalculator;
    // GateTrigger _inputClockProc;
    PolyClockShifter _shifter;
    GateTrigger _buttonProc;
    Divider divn;
};

template <class TBase>
inline void PhasePatterns2<TBase>::_init() {
    divn.setup(32, [this]() {
        this->_stepn();
    });
}

template <class TBase>
inline void PhasePatterns2<TBase>::_updateButton() {
    // TBase::lights[RIB_LIGHT].value = _shiftCalculator.busy() ? 10 : 0;
    // _buttonProc.go(TBase::params[RIB_BUTTON_PARAM].value);
    // if (!_buttonProc.trigger()) {
    //     return;
    // }
    // if (!_clockShifter.freqValid()) {
    //     return;
    // }

    //_shiftCalculator.trigger(_clockShifter.getPeriod());
}

template <class TBase>
inline void PhasePatterns2<TBase>::_updateShiftAmount() {
    // float shift = TBase::params[SHIFT_PARAM].value;
    // shift += TBase::inputs[SHIFT_INPUT].value;
    // shift += _shiftCalculator.get();
    // _clockShifter.setShift(shift);
    // TBase::params[COMBINED_SHIFT_INTERNAL_PARAM].value = shift;
}

template <class TBase>
inline void PhasePatterns2<TBase>::_stepn() {
    _updateShiftAmount();
    _updateButton();
    _updateShiftAmount();
    
    PolyClockShifter::PortInfo info;
    _fillInfo(info);
    _shifter.runEveryBlock(info);
}

template <class TBase>
inline void PhasePatterns2<TBase>::_fillInfo(PolyClockShifter::PortInfo& info) {
    Port* const _clockOutput = &TBase::outputs[0];
    Port* const _clockInput = &TBase::inputs[0];
    Port* const _ribsTrigger = &TBase::inputs[1];
    Port* const _shiftAmount = &TBase::inputs[2];

    info.clockInput = _clockInput;
    info.clockOutput = _clockOutput;
    info.ribsTrigger = _ribsTrigger;
    info.shiftAmount = _shiftAmount;
}
template <class TBase>
inline void PhasePatterns2<TBase>::process(const typename TBase::ProcessArgs& args) {
    divn.step();
    PolyClockShifter::PortInfo info;
    _fillInfo(info);
    _shifter.runEverySample(info);
    //     _shiftCalculator.go();
    //     const float rawClockIn = TBase::inputs[CK_INPUT].getVoltage();
    //     _inputClockProc.go(rawClockIn);
    //    // const bool clockIn = _inputClockProc.go(rawClockIn);
    //     const bool rawClockOut = _clockShifter.process(_inputClockProc.trigger(), _inputClockProc.gate());
    //     const float clockOut = rawClockOut ? cGateOutHi : cGateOutLow;
    //     TBase::outputs[CK_OUTPUT].setVoltage(clockOut);
}
