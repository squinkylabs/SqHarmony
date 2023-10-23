
#pragma once

#include "ClockShifter3.h"
#include "Divider.h"
#include "FreqMeasure.h"
#include "GateTrigger.h"
#include "SchmidtTrigger.h"
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

    ClockShifter3 _clockShifter;
    ShiftCalc _shiftCalculator;
    SchmidtTrigger _inputClockProc;
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
       _buttonProc.go(TBase::params[RIB_BUTTON_PARAM].value);
    if (!_buttonProc.trigger()) {
        return;
    }
    SQINFO("button trigger");
    auto const freqMeasure = _clockShifter.getFreqMeasure();
    if (!freqMeasure.freqValid()) {
        SQINFO("unstable");
        return;
    }
    _shiftCalculator.trigger(freqMeasure.getPeriod());
}

template <class TBase>
inline void PhasePatterns<TBase>::_stepn() {
    const float shift = TBase::params[SHIFT_PARAM].value;
    _clockShifter.setShift(shift);

   _updateButton();
}

template <class TBase>
inline void PhasePatterns<TBase>::process(const typename TBase::ProcessArgs& args) {
    divn.step();
    const float rawClockIn = TBase::inputs[CK_INPUT].getVoltage();
    const bool clockIn = _inputClockProc.go(rawClockIn);
    const bool rawClockOut = _clockShifter.run(clockIn);
    const float clockOut = rawClockOut ? cGateOutHi : cGateOutLow;
    TBase::outputs[CK_OUTPUT].setVoltage(clockOut);
}
