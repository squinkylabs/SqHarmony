
#pragma once


#include "ClockMult.h"
#include "Divider.h"
#include "FreqMeasure.h"
#include "OneShot.h"
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
    SchmidtTrigger _inputClockProc;
    OneShot _triggerOutOneShot;
    Divider divn;
};

template <class TBase>
inline void Multiplier<TBase>::_init() {
    divn.setup(32, [this]() {
        this->_stepn();
    });
    _triggerOutOneShot.setDelayMs(1);
}


template <class TBase>
inline void Multiplier<TBase>::_stepn() {

}

template <class TBase>
inline void Multiplier<TBase>::process(const typename TBase::ProcessArgs& args) {
    divn.step();
  
    const float rawClockIn = TBase::inputs[CK_INPUT].getVoltage();
    const bool clockIn = _inputClockProc.go(rawClockIn);
   const bool rawClockOut = _clockMult.run(clockIn);
    if (rawClockOut) {
        _triggerOutOneShot.set();
    }
    _triggerOutOneShot.step(args.sampleTime);
    const bool trigger = !_triggerOutOneShot.hasFired();
    const float clockOut = trigger ? cGateOutHi : cGateOutLow;
      

    TBase::outputs[CK_OUTPUT].setVoltage(clockOut);
}
