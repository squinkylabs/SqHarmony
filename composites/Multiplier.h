
#pragma once

#include "ClockMult.h"
#include "Divider.h"
#include "FreqMeasure.h"
#include "GateTrigger.h"
#include "OneShot.h"
#include "Ratchet.h"

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
    void _updateRatchet1Count();

    ClockMult _clockMult;
    GateTrigger _inputClockProc;
    OneShot _triggerOutOneShot;
    Divider divn;

    GateTrigger _ratchet1Trigger;
    Ratchet _ratchet1;
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
inline void Multiplier<TBase>::_updateRatchet1Count() {
    float count = TBase::params[RATCHET_1_COUNT_PARAM].value;
    _ratchet1.setCount(int(count));
}

template <class TBase>
inline void Multiplier<TBase>::_stepn() {
    _updateMultAmount();
    _updateRatchet1Count();
}

template <class TBase>
inline void Multiplier<TBase>::process(const typename TBase::ProcessArgs& args) {
    divn.step();

     {
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

    {
        const bool inputClockTrigger = _inputClockProc.trigger();
        if (inputClockTrigger) {
            SQINFO("got an input trigger");
        }
        // process the external triggers for ratchet 1
        const float rawRatchet1Trigger = TBase::inputs[RATCHET_1_TRIGGER_INPUT].getVoltage();
        _ratchet1Trigger.go(rawRatchet1Trigger);
        if (_ratchet1Trigger.trigger()) {
            SQINFO("setting a ratchet trigger");
            _ratchet1.trigger();
        }
        const int count = _ratchet1.run(inputClockTrigger);
        if (count != 1) {
            SQINFO("got count %d, will set on mult", count);
           // _ratchet1.setCount(count);
           _clockMult.setMul(count);
        }
    } 
}
