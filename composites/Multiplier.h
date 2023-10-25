
#pragma once


#include "ClockMult.h"
#include "Divider.h"
#include "FreqMeasure.h"
#include "SchmidtTrigger.h"


namespace rack {
namespace engine {
struct Module;
}
}  // namespace rack

using Module = ::rack::engine::Module;
template <class TBase>
class Multiplier : public TBase {
public:
    enum ParamIds {
        SCHEMA_PARAM,
        MULT_PARAM,
        NUM_PARAMS
    };
    enum InputIds {
        CK_INPUT,
     //   SHIFT_INPUT,
        NUM_INPUTS
    };

    enum OutputIds {
        CK_OUTPUT,
        NUM_OUTPUTS
    };

    enum LightIds {
     //   RIB_LIGHT,
        NUM_LIGHTS
    };

    Multiplier(Module* module) : TBase(module) {
        _init();
    }
    Multiplier() : TBase() {
        _init();
    }

    void process(const typename TBase::ProcessArgs& args) override;

private:
    void _init();
    void _stepn();
    void _updateButton();
    void _updateShiftAmount();

    ClockMult _clockMult;
    // ShiftCalc _shiftCalculator;
    SchmidtTrigger _inputClockProc;
    // GateTrigger _buttonProc;
    Divider divn;
};

template <class TBase>
inline void Multiplier<TBase>::_init() {
    divn.setup(32, [this]() {
        this->_stepn();
    });
}


template <class TBase>
inline void Multiplier<TBase>::_stepn() {

}

template <class TBase>
inline void Multiplier<TBase>::process(const typename TBase::ProcessArgs& args) {
    divn.step();
   // _shiftCalculator.go();
    const float rawClockIn = TBase::inputs[CK_INPUT].getVoltage();
    const bool clockIn = _inputClockProc.go(rawClockIn);
   const bool rawClockOut = _clockMult.run(clockIn);
    const float clockOut = rawClockOut ? cGateOutHi : cGateOutLow;
    TBase::outputs[CK_OUTPUT].setVoltage(clockOut);
}
