
#pragma once

#include "ClockMult.h"
#include "Divider.h"
#include "FreqMeasure.h"
#include "GateTrigger.h"
#include "OneShot.h"


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
        RATCHET_1_COUNT_PARAM,
        NUM_PARAMS
    };
    enum InputIds {
        CK_INPUT,
        //   SHIFT_INPUT,
        RATCHET_1_TRIGGER_INPUT,
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
    //   void _updateButton();
    void _updateMultAmount();

    ClockMult _clockMult;
    GateTrigger _inputClockProc;
    OneShot _triggerOutOneShot;
    Divider divn;
};

template <class TBase>
inline void Multiplier<TBase>::_init() {
    divn.setup(32, [this]() {
        this->_stepn();
    });
    _triggerOutOneShot.setDelayMs(2.f);
}

template <class TBase>
inline void Multiplier<TBase>::_updateMultAmount() {
    float mult = TBase::params[MULT_PARAM].value;
    _clockMult.setMul(mult);
    // SQINFO("set mult %f", mult);
}

template <class TBase>
inline void Multiplier<TBase>::_stepn() {
    _updateMultAmount();
}

template <class TBase>
inline void Multiplier<TBase>::process(const typename TBase::ProcessArgs& args) {
    divn.step();

    const float rawClockIn = TBase::inputs[CK_INPUT].getVoltage();
    _inputClockProc.go(rawClockIn);
    const bool triggerIn = _inputClockProc.trigger();
    const bool rawClockOut = _clockMult.run(triggerIn);
    if (rawClockOut) {
        _triggerOutOneShot.set();
    }
    _triggerOutOneShot.step(args.sampleTime);
    const bool trigger = !_triggerOutOneShot.hasFired();
    const float clockOut = trigger ? cGateOutHi : cGateOutLow;

    TBase::outputs[CK_OUTPUT].setVoltage(clockOut);
}
