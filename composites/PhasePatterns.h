
#pragma once

#include "ClockShifter3.h"
#include "SchmidtTrigger.h"

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
        NUM_PARAMS
    };
    enum InputIds {
        CK_INPUT,
        NUM_INPUTS
    };

    enum OutputIds {
        CK_OUTPUT,
        NUM_OUTPUTS
    };

    enum LightIds {
        NUM_LIGHTS
    };

    PhasePatterns(Module* module) : TBase(module) {
        init();
    }
    PhasePatterns() : TBase() {
        init();
    }

    void process(const typename TBase::ProcessArgs& args) override;

private:
    void init();

    ClockShifter3 _clockShifter;
    SchmidtTrigger _inputProc;
};

template <class TBase>
inline void PhasePatterns<TBase>::init() {
    // TODO: initialization goes here.
}

template <class TBase>
inline void PhasePatterns<TBase>::process(const typename TBase::ProcessArgs& args) {
    const float shift = TBase::params[SHIFT_PARAM].value;
    _clockShifter.setShift(shift);

    const float rawClockIn =  TBase::inputs[CK_INPUT].getVoltage();
    const bool clockIn = _inputProc.go(rawClockIn);
    const bool rawClockOut = _clockShifter.run(clockIn);
    const float clockOut = rawClockOut ? cGateOutHi : cGateOutLow;
    TBase::outputs[CK_OUTPUT].setVoltage(clockOut);
}

